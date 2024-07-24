#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "heartRate.h"

static const unsigned char PROGMEM  logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E,  0x02, 0x10, 0x0C, 0x03, 0x10,              //Logo2 and Logo3 are two bmp pictures  that display on the OLED if called
0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40,  0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
0x02, 0x08, 0xB8, 0x04,  0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
0x00,  0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,  };

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F,  0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
0x20,  0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08,  0x03,
0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01,  0x80, 0x00, 0x14, 0x00,
0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40,  0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31,  0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
0x00, 0x60, 0xE0,  0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
0x01,  0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C,  0x00,
0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00,  0x00, 0x01, 0x80, 0x00  };


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0



int melody[] = {
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //1
  NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
  NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,4, //repeat from 1
  NOTE_E5,-4, NOTE_FS5,-4, NOTE_A5,16, NOTE_G5,16, NOTE_FS5,8,
  NOTE_D5,-4, NOTE_E5,-4, NOTE_A4,2,
  NOTE_A4,16, NOTE_A4,16, NOTE_B4,16, NOTE_D5,8, NOTE_D5,16,
  REST,4, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_D5,8, NOTE_E5,8, NOTE_CS5,-8,
  NOTE_B4,16, NOTE_A4,2, REST,4, 

  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,4, NOTE_A4,8, //7
  NOTE_A5,8, REST,8, NOTE_A5,8, NOTE_E5,-4, REST,4, 
  NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,8, REST,8,
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, REST,4,
   
  NOTE_D5,2, NOTE_E5,8, NOTE_FS5,8, NOTE_D5,8, //13
  NOTE_E5,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, NOTE_A4,4,
  REST,2, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8,
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,-8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //18
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,8, NOTE_A4,8, NOTE_A4,8, 
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,

  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  //23
  NOTE_E5,4, NOTE_D5,2, REST,4,
  REST,8, NOTE_B4,8, NOTE_D5,8, NOTE_B4,8, NOTE_D5,8, NOTE_E5,4, REST,8,
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4,
  REST,8, NOTE_A5,8, NOTE_A5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_D5,8,
  
  REST,8, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //29
  REST,8, NOTE_CS5,8, NOTE_B4,8, NOTE_A4,-4, REST,4,
  NOTE_B4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, NOTE_A4,4, REST,8,
  REST,8, NOTE_E5,8, NOTE_E5,8, NOTE_FS5,4, NOTE_E5,-4, 
  NOTE_D5,2, NOTE_D5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,4, 
  NOTE_E5,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,8, NOTE_A4,8, NOTE_A4,4,

  REST,-4, NOTE_A4,8, NOTE_B4,8, NOTE_CS5,8, NOTE_D5,8, NOTE_B4,8, //35
  REST,8, NOTE_E5,8, NOTE_FS5,8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_E5,-8, NOTE_E5,-8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

   NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //40
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
   
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8,  
  NOTE_E5,4, NOTE_D5,2, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_FS5,-8, NOTE_FS5,-8, NOTE_E5,-4, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16, //45
  
  NOTE_A5,4, NOTE_CS5,8, NOTE_D5,-8, NOTE_CS5,16, NOTE_B4,8, NOTE_A4,16, NOTE_B4,16, NOTE_D5,16, NOTE_B4,16,
  NOTE_D5,4, NOTE_E5,8, NOTE_CS5,-8, NOTE_B4,16, NOTE_A4,4, NOTE_A4,8, 

  NOTE_E5,4, NOTE_D5,2, REST,4
};


int tempo = 130;
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

unsigned long previousMillis = 0;
int thisNote = 0;
bool alarmActive = false;

String lastClockMessage = "00:00";

// Variable global para almacenar el valor del umbral
double umbralAlarma = 1000; // Inicializamos con un valor por defecto
int umbralPasos = 5;      // Inicializamos con un valor por defecto
int umbralRitmoCardiaco = 100;
int pantallaMessageType = 1; // Variable global para almacenar el tipo de mensaje recibido para la pantalla

// Son las variables que van a estar actualizandose a lo largo del codigo y que serán comparadas con su respectivo umbral
// Cuando se actualicen los contadores deben compararse con el umbral y si es igual o mayor a este aplica funcion de activar alarma
double distanciaContador = 0;
int pasosContador = 0;
int ritmoCardiacoContador = 0;

// Definir los pines para la comunicación I2C
const int mpu_sda_pin = 33;
const int mpu_scl_pin = 32;
const int mq3Pin = 39;
const int analogPin = 35; // Pin donde se conecta el termistor
const int BUZZER_PIN = 18; // Pin del zumbador
const int max_scl_pin = 22;
const int max_sda_pin = 21;

// Constantes del sensor de BPM
MAX30105 particleSensor;
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;

int mq3Value = 0;

// Coeficientes del termistor para la ecuación de Steinhart-Hart
const double A = 0.001129148;
const double B = 0.000234125;
const double C = 0.0000000876741;

// Tamaño pantalla
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Inicialización acelerómetro
MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);
double distancia = 0;
int16_t ax, ay, az;

//Consts acelerómetro
const int threshold = 13120;
int16_t prev_acc_z = 0;
int stepCount = 0;

// Timing variables
unsigned long previousMillisAccel = 0;
unsigned long previousMillisMQ3 = 0;
unsigned long previousMillisTermistor = 0;
unsigned long previousMillisPulseSensor = 0;
unsigned long previousMillisData = 0;
const long intervalAccel = 100;  
const long intervalMQ3 = 5000;
const long intervalTermistor = 2000;
const long intervalPulseSensor = 20;
const long intervalData = 5000;

// Vars globales sensores
double temperature = 0.0;
int myBPM = 0;
String readString = "";

/*

Esta funcion será la que reciba los paquetes de datos (aun por ver de que tipo),
Puede recibir de varios tipos, se debe verificar de que tipo es y realizara las acciones necesarias como:

  - Cambiar umbral (distancia, pasos, temperatura).
   la informacion se debe componer de: umbral
   el contador del respectivo umbral debe reiniciarse al cambiar el umbral
   Ejemplo:
    {tipo: "distance_alarm", nuevo_umbral: "300"}

  - actualizar visualizacion de pantalla, se recibe un numero y actualiza pantallaMessageType
    Ejemplo:
     {tipo: "screen_type", type: "2"}

  - recibir datos de reloj
    Ejemplo: 
     {tipo: "watch", data: "12:00 Pz"}

  Realizar funcion cuando sepa el formato en el que se recibira la informacion

  

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

*/


class Sensor{
  public:
    int id = 0;
    String value = "0";

    Sensor(){
    }
    void setId(int i){
      id = i;
    }
    void setValue(String v){
      value = v;
    }
    String getValue(){
      return value;
    }
    int getId(){
      return id;
    }
};

int numSensoresRitmoCardiaco = -1;
int numSensoresTemperatura = -1;
int numSensoresPasos = -1;
int numSensoresDistancia = -1;
int numSensoresAlcohol = -1;

Sensor sensoresRitmoCardiaco[3];
Sensor sensoresTemperatura[3];
Sensor sensoresPasos[3];
Sensor sensoresDistancia[3];
Sensor sensoresAlcohol[3];


const int button1Pin = 16; 
const int button2Pin = 17;
bool button1State = LOW;
bool button2State = LOW;

// const int redPin = 13;
// const int greenPin = 12;
// const int bluePin = 14;

//const int intervalLed = 150;

//int redValue = 0;
//int greenValue = 0;
//int blueValue = 0;

//int colorIndex = 0;
//const int maxColorIndex = 255;

//unsigned long previousMillisLed = 0;

void setup() {
  Serial.begin(9600);
//  pinMode(bluePin, OUTPUT);
//  pinMode(greenPin, OUTPUT);
//  pinMode(redPin, OUTPUT);

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  //Activar sensor MAX30105
  
  Wire1.begin(max_sda_pin, max_scl_pin);
    if (!particleSensor.begin(Wire1)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 no se pudo conectar");
    while (1);
  }

  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); 
  particleSensor.setPulseAmplitudeGreen(0);



  //Activar sensor MPU6050

  Wire.begin(mpu_sda_pin, mpu_scl_pin);
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Error al conectar el MPU6050.");
    while (1);
  } else {
    Serial.println("MPU6050 conectado exitosamente.");
  }
  

      
  //Activar oled SSD1306

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se pudo inicializar la pantalla OLED"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(mq3Pin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  

    

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
  ActualizarPantalla();
    
  if (digitalRead(button1Pin) == LOW && button1State == HIGH) {
    if(pantallaMessageType<5){
      pantallaMessageType = pantallaMessageType + 1;
    }else{
      pantallaMessageType = 1;
    }
  }
  
  button1State = digitalRead(button1Pin);
  }
  if (currentMillis - previousMillisData >= intervalData) {
    previousMillisData = currentMillis;
    EnviarDatos();
  }

//  if (currentMillis - previousMillisLed >= intervalLed) {
//    previousMillisLed = currentMillis;
//    cambiarColor();
//  }



  if (digitalRead(button2Pin) == LOW && button2State == HIGH) {
    previousMillis = millis();
    thisNote = 0;
    alarmActive = false;
    noTone(BUZZER_PIN);
  }
  
  button2State = digitalRead(button2Pin);

  activarAlarma();
  
}

//void cambiarColor(){
  
//    switch(colorIndex / 85) {
//      case 0:
//        redValue = 0;
//        greenValue = colorIndex * 3;
//        blueValue = 255 - colorIndex * 3;
//        break;
//      case 1:
//        redValue = (colorIndex - 85) * 3;
//        greenValue = 255 - (colorIndex - 85) * 3;
//        blueValue = 0;
//        break;
//      case 2:
//        redValue = 255 - (colorIndex - 170) * 3;
//        greenValue = 0;
//        blueValue = (colorIndex - 170) * 3;
//        break;
//    }
//
//    analogWrite(redPin, redValue);
//    analogWrite(greenPin, greenValue);
//    analogWrite(bluePin, blueValue);
    

//    colorIndex++;
//    if (colorIndex > maxColorIndex) {
//      colorIndex = 0;
//    }
//}

void printSensorValue(int id,String type){
  if(type=="TMP"){
    for(int i = 0; i <= numSensoresTemperatura; i++){
      if(sensoresTemperatura[i].id==id){
        Serial.println("TMP:" +String(id)+":"+ sensoresTemperatura[i].value);
      }
    }
  }
  else if(type=="RTC"){
    for(int i = 0; i <= numSensoresRitmoCardiaco; i++){
      if(sensoresRitmoCardiaco[i].id==id){
        Serial.println("RTC:" +String(id)+":"+ sensoresRitmoCardiaco[i].value);
      }
    }
  }
  else if(type=="ALC"){
    for(int i = 0; i <= numSensoresAlcohol; i++){
      if(sensoresAlcohol[i].id==id){
        Serial.println("ALC:" +String(id)+":"+ sensoresAlcohol[i].value);
      }
    }
  }
  else if(type=="PSS"){
    for(int i = 0; i <= numSensoresPasos; i++){
      if(sensoresPasos[i].id==id){
        Serial.println("PSS:" +String(id)+":"+ sensoresPasos[i].value);
      }
    }
  }
  else if(type=="DST"){
    for(int i = 0; i <= numSensoresDistancia; i++){
      if(sensoresDistancia[i].id==id){
        Serial.println("DST:" +String(id)+":"+ sensoresDistancia[i].value);
      }
    }
  }
}

void EnviarDatos(){
    readString = Serial.readStringUntil('\n');
    if(readString != ""){
      String request = readString.substring(0, 3);
      String typeRequest = readString.substring(5, 7); 
      if(request=="REA"){
        int id = readString.substring(9).toInt();
        printSensorValue(id,typeRequest);
      } else if (request=="NEW"){
        int id = readString.substring(9).toInt();
        if (typeRequest == "TMP") {
          if(numSensoresTemperatura+1<=3){
            sensoresTemperatura[numSensoresTemperatura+1].id = id; 
            numSensoresTemperatura = numSensoresTemperatura + 1;
          }
        } else if (typeRequest == "RTC") {
          if(numSensoresRitmoCardiaco+1<=3){
            sensoresRitmoCardiaco[numSensoresRitmoCardiaco+1].id = id; 
            numSensoresRitmoCardiaco = numSensoresRitmoCardiaco + 1;
          }
        } else if (typeRequest == "ALC") {
          if(numSensoresAlcohol+1<=3){
            sensoresAlcohol[numSensoresAlcohol+1].id = id; 
            numSensoresAlcohol = numSensoresAlcohol + 1;
          }
        } else if (typeRequest == "PSS") {
          if(numSensoresPasos+1<=3){
            sensoresPasos[numSensoresPasos+1].id = id; 
            numSensoresPasos = numSensoresPasos + 1;
          }
        } else if (typeRequest == "DST") {
          if(numSensoresDistancia+1<=3){
            sensoresDistancia[numSensoresDistancia+1].id = id; 
            numSensoresDistancia = numSensoresDistancia + 1;
          }
        }
      } else if (request=="UPA"){
        if (typeRequest == "RTC") {
          umbralRitmoCardiaco = readString.substring(9).toInt();
        } else if (typeRequest == "PSS") {
          umbralPasos = readString.substring(9).toInt();
        } else if (typeRequest == "DST") {
          umbralAlarma = readString.substring(9).toFloat();
        }
      }
    readString = "";

    }
  

}

void Acelerometro() {
  mpu.getAcceleration(&ax, &ay, &az);

  int16_t delta_acc_z = abs(az - prev_acc_z);
  if (delta_acc_z > threshold) {
    stepCount++;
    distancia += 0.45; // Suponiendo que cada paso es de 45 cm

    sensoresPasos[0].value = String(stepCount);
    sensoresDistancia[0].value = String(distancia);
    pasosContador++;
    distanciaContador = distanciaContador + 0.45;

    if(pasosContador >= umbralPasos){
      iniciarAlarma();
      pasosContador = 0;
    }

    if(distanciaContador >= umbralAlarma){
      iniciarAlarma();
      distanciaContador = 0;
    }

  }

  prev_acc_z = az;
}

void MedirMQ3() {
  mq3Value = analogRead(mq3Pin) - 800;
  sensoresAlcohol[0].value = String(mq3Value);

}

void MedirTermistor() {
  int rawValue = analogRead(analogPin); // Lee el valor del ADC
  double voltage = (rawValue / 4095.0) * 3.3; // Convierte a voltaje
  double resistance = (3.3 * 10000 / voltage) - 10000; // Calcula la resistencia del NTC

  temperature = (1.0 / (A + B * log(resistance) + C * pow(log(resistance), 3))) - 273.15;

  sensoresTemperatura[0].value = String(temperature);

}

void MedirPulseSensor() {

    long irValue = particleSensor.getIR();
    if(checkForBeat(irValue) == true){
  
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; 
      rateSpot %= RATE_SIZE; 

      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;

      sensoresRitmoCardiaco[0].value = String(beatAvg);
      ritmoCardiacoContador = beatAvg;

    if(ritmoCardiacoContador >= umbralRitmoCardiaco){
      iniciarAlarma();
      ritmoCardiacoContador = 0;
    }
    }
  }
  
}

void iniciarAlarma() {
  alarmActive = true;
  previousMillis = millis();
  thisNote = 0;
  
}

void activarAlarma() {
  if (!alarmActive) return;

  unsigned long currentMillis = millis();

  if (thisNote < notes * 2) {
    if (currentMillis - previousMillis >= noteDuration) {
      thisNote += 2;

      if (thisNote < notes * 2) {
        divider = melody[thisNote + 1];
        if (divider > 0) {
          noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
          noteDuration = (wholenote) / abs(divider);
          noteDuration *= 1.5; 
        }
        
        //cambiarColor();
        tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);

        previousMillis = currentMillis;
      } else {
        alarmActive = false;
        noTone(BUZZER_PIN);
      }
    }
  }
}

void ActualizarPantalla() {
  long ir = particleSensor.getIR();
  // Actualiza la pantalla con los valores almacenados
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Imprime la información correspondiente según el tipo de mensaje para la pantalla
  if (ir < 9000 && pantallaMessageType != 5){
    display.println("Uso ");
    display.println("Inactivo ");
  }
  else{
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
        display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       
        display.setTextSize(2);                                   
        display.setTextColor(WHITE);  
        display.setCursor(50,0);                
        display.println("BPM");             
        display.setCursor(50,18);                
        display.println(beatAvg);  
        display.display();
        
      if (checkForBeat(ir) == true)                       
      {
        display.clearDisplay();                              
        display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);    
        display.setTextSize(2);                               
        display.setTextColor(WHITE);             
        display.setCursor(50,0);                
        display.println("BPM");             
        display.setCursor(50,18);                
        display.println(beatAvg); 
        display.display();
        tone(3,1000);        
        noTone(3);              

      }
        
        break;
      case 3:
        // Imprimir temperatura
          display.print("Temperatura: ");
          display.print(temperature);
          display.println(" C");
        break;
      case 4:
        // Imprimir reloj
          display.println(lastClockMessage);
        
        break;
      default:
        display.println("");
        break;
    }
  }
  display.display();
}
