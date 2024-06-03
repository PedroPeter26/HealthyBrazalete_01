#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PulseSensorPlayground.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Definir los pines para la comunicación I2C
const int mpu_sda_pin = 33;
const int mpu_scl_pin = 32;
const int oled_sda_pin = 4;
const int oled_scl_pin = 15;
const int mq3Pin = 39;
const int analogPin = 35; // Pin donde se conecta el termistor
const int PulseWire = 36; // Pin del sensor de pulso cardiaco
const int BUZZER_PIN = 18; // Pin del zumbador

// Coeficientes del termistor para la ecuación de Steinhart-Hart
const double A = 0.001129148;
const double B = 0.000234125;
const double C = 0.0000000876741;

// Tamaño pantalla
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Variable global para almacenar el tipo de mensaje recibido para la pantalla
int pantallaMessageType = 0; // Valor predeterminado, ninguna selección
String lastTemperatureMessage = ""; // Variable para almacenar el último mensaje de temperatura
String lastBPMMessage = ""; // Variable para almacenar el último mensaje de ritmo cardíaco
String lastClockMessage = ""; // Variable para almacenar el último mensaje del reloj


//Inicialización acelerómetro
MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);
double distancia = 0;

//Inicialización del sensor de pulso
int Threshold = 550;     // Determina qué señal "contar como un latido" y cuál ignorar.
PulseSensorPlayground pulseSensor;

//Consts acelerómetro
const int threshold = 13120;
int16_t prev_acc_z = 0;
int stepCount = 0;

// Timing variables
unsigned long previousMillisAccel = 0;
unsigned long previousMillisMQ3 = 0;
unsigned long previousMillisTermistor = 0;
unsigned long previousMillisPulseSensor = 0;
const long intervalAccel = 100;  // Intervalo para la función Acelerometro
const long intervalMQ3 = 5000;   // Intervalo para la función MQ3
const long intervalTermistor = 2000; // Intervalo para la función Termistor
const long intervalPulseSensor = 20;

// WiFi settings
const char* ssid = "DiositoPlus";
const char* password = "P3P3_xdm";

// MQTT Broker settings
const char* mqtt_broker = "143.198.135.231";
const char* mqtt_topic = "BrazaleteDistancia";
const char* mqtt_topic_umbral = "MetaDistancia"; // El nuevo tópico para el umbral
const char* pasos_topic = "BrazaletePasos";
const char* pasos_topic_umbral = "MetaPasos";
const char* temperatura_topic = "BrazaleteTemperatura";
const char* alcohol_topic = "BrazaleteAlcohol";
const char* ritmo_topic = "BrazaletePulso";
const char* reloj_topic = "Reloj";
const char* pantalla_topic = "BrazaletePantalla";
const int mqtt_port = 1883;

// Vars globales sensores
double temperature = 0.0;
int myBPM = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// Variable global para almacenar el valor del umbral
int umbralAlarma = 2500; // Inicializamos con un valor por defecto
int umbralPasos = 100;      // Inicializamos con un valor por defecto

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  for (unsigned int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  
  // Verificar si el mensaje es para el umbral
  if (String(topic) == mqtt_topic_umbral) {
    umbralAlarma = incoming.toInt();
  } else if (String(topic) == mqtt_topic) {
    int number = incoming.toInt();
    if (number > umbralAlarma) {
      activarAlarma();
    }
  }

  // Verificar si el mensaje es para el umbral de pasos
  if (String(topic) == pasos_topic_umbral) {
    umbralPasos = incoming.toInt();
  } else if (String(topic) == pasos_topic) {
    int number = incoming.toInt();
    if (number > umbralPasos) {
      activarAlarma();
    }
  }

  // Actualizar el tipo de mensaje para la pantalla
  if (String(topic) == pantalla_topic) {
    pantallaMessageType = incoming.toInt();
  }

  // Actualizar la variable correspondiente al tipo de información recibida
  if (String(topic) == temperatura_topic) {
    lastTemperatureMessage = incoming;
  } else if (String(topic) == ritmo_topic) {
    lastBPMMessage = incoming;
  } else if (String(topic) == reloj_topic) {
    lastClockMessage = incoming;
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(mqtt_topic);
      client.subscribe(mqtt_topic_umbral); // Suscribirse al tópico del umbral
      client.subscribe(pasos_topic);
      client.subscribe(pasos_topic_umbral);    // Suscribirse al tópico del umbral de pasos
      client.subscribe(pantalla_topic);
      client.subscribe(reloj_topic);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  Wire.begin(mpu_sda_pin, mpu_scl_pin);
  mpu.initialize();

  Wire1.begin(oled_sda_pin, oled_scl_pin);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se pudo inicializar la pantalla OLED"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(mq3Pin, INPUT);

  // Configure el objeto PulseSensor asignando nuestras variables.
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);

  // Asegúrate de que el objeto "pulseSensor" se creó y comenzó a detectar una señal.
  if (pulseSensor.begin()) {
    Serial.println("¡Creamos un objeto pulseSensor!");
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // Llama a la función Acelerometro cada intervaloAccel milisegundos
  if (currentMillis - previousMillisAccel >= intervalAccel) {
    previousMillisAccel = currentMillis;
    Acelerometro();
  }

  // Llama a la función MQ3 cada intervaloMQ3 milisegundos
  if (currentMillis - previousMillisMQ3 >= intervalMQ3) {
    previousMillisMQ3 = currentMillis;
    MedirMQ3();
  }

  // Llama a la función Termistor cada intervaloTermistor milisegundos
  if (currentMillis - previousMillisTermistor >= intervalTermistor) {
    previousMillisTermistor = currentMillis;
    MedirTermistor();
  }

  // Llama a la función PulseSensor cada intervaloPulseSensor milisegundos
  if (currentMillis - previousMillisPulseSensor >= intervalPulseSensor) {
    previousMillisPulseSensor = currentMillis;
    MedirPulseSensor();
  }

  // Actualiza la pantalla si hay nuevos mensajes disponibles
  if (client.connected()) {
    client.loop();
  }
  delay(100);

  if (!client.connected()) {
    reconnect();
  }
  // Actualiza la pantalla
  ActualizarPantalla();
}

void Acelerometro() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  int16_t delta_acc_z = abs(az - prev_acc_z);
  if (delta_acc_z > threshold) {
    stepCount++;
    distancia = distancia + 45; // Suponiendo que cada paso es de 45 cm
    Serial.print("Pasos: ");
    Serial.println(stepCount);
    Serial.print("Distancia: ");
    Serial.println(distancia);
    // Publicar distancia y pasos después de cada medición
    client.publish(mqtt_topic, String(distancia).c_str(), true);
    client.publish(pasos_topic, String(stepCount).c_str(), true);
  }

  prev_acc_z = az;
}

void MedirMQ3() {
  int mq3Value = analogRead(mq3Pin) - 800;
  Serial.print("Valor del sensor MQ3: ");
  Serial.println(mq3Value);
  // Publicar valor MQ3
  client.publish(alcohol_topic, String(mq3Value).c_str(), true);
}

void MedirTermistor() {
  int rawValue = analogRead(analogPin); // Lee el valor del ADC
  double voltage = (rawValue / 4095.0) * 3.3; // Convierte a voltaje
  double resistance = (3.3 * 10000 / voltage) - 10000; // Calcula la resistencia del NTC

  // Calcula la temperatura usando la ecuación de Steinhart-Hart
  temperature = (1.0 / (A + B * log(resistance) + C * pow(log(resistance), 3))) - 273.15;

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" C");
  // Publicar temperatura
  client.publish(temperatura_topic, String(temperature).c_str(), true);
}

void MedirPulseSensor() {
  if (pulseSensor.sawStartOfBeat()) {            // Constantemente prueba si "ocurrió un latido".
    myBPM = pulseSensor.getBeatsPerMinute(); // Llama a la función en nuestro objeto pulseSensor que devuelve BPM como un "int".
                                                  // "myBPM" ahora contiene este valor BPM.
    Serial.println("♥ ¡Un latido de corazón ocurrió!"); // Si la prueba es "verdadera", imprime un mensaje "¡un latido de corazón ocurrió!".
    Serial.print("BPM: ");                        // Imprime la frase "BPM: "
    Serial.println(myBPM);                        // Imprime el valor dentro de myBPM.
    // Publicar BPM
    client.publish(ritmo_topic, String(myBPM).c_str(), true);
  }
}

void activarAlarma() {
  tone(BUZZER_PIN, 6000);
  delay(1000);
  noTone(BUZZER_PIN);
  delay(600);
  tone(BUZZER_PIN, 6000);
  delay(400);
  noTone(BUZZER_PIN);
  delay(200);
  tone(BUZZER_PIN, 7000);
  delay(2000);
  noTone(BUZZER_PIN);
}

void ActualizarPantalla() {
  // Actualiza la pantalla con los valores almacenados
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Imprime la información correspondiente según el tipo de mensaje para la pantalla
  switch (pantallaMessageType) {
    case 1:
      // Imprimir distancia y pasos
      display.print("Distancia: ");
      display.println(distancia);
      display.print("Pasos: ");
      display.println(stepCount);
      break;
    case 2:
      // Imprimir ritmo cardíaco
        display.print("Ritmo: ");
        display.print(myBPM);
        display.println(" bpm");
      break;
    case 3:
      // Imprimir temperatura
        display.print("Temperatura: ");
        display.print(temperature);
        display.println(" C");
      break;
    case 4:
      // Imprimir reloj
      if (lastClockMessage != "") {
        display.println(lastClockMessage);
      }
      break;
    default:
      // No se ha seleccionado ningún tipo de mensaje para la pantalla
      display.println("Sin seleccionar");
      break;
  }

  display.display();
}
