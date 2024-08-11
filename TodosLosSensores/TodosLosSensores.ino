#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
 

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

const int PROGMEM melody[] = {
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,

  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,
  
  NOTE_E5,2,  NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,2,   NOTE_A4,2,
  NOTE_GS4,2,  NOTE_B4,4,  REST,8, 
  NOTE_E5,2,   NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,4,   NOTE_E5,4,  NOTE_A5,2,
  NOTE_GS5,2,
};


#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;


bool activo = false;

int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / 144;
int divider = 0, noteDuration = 0;

unsigned long previousMillis = 0;
int thisNote = 0;
bool alarmActive = false;

String lastClockMessage = "00:00";

double umbralAlarma = 1000; 
int umbralPasos = 5;      
int umbralRitmoCardiaco = 100;
int pantallaMessageType = 1; 

double distanciaContador = 0;
int pasosContador = 0;
bool ritmoCardiacoAlarmaActivo = true;

const int mpu_sda_pin = 33;
const int mpu_scl_pin = 32;
const int mq3Pin = 39;
const int analogPin = 35; // Pin donde se conecta el termistor
const int BUZZER_PIN = 18; // Pin del zumbador
const int max_scl_pin = 22;
const int max_sda_pin = 21;

MAX30105 particleSensor;
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;

int mq3Value = 0;


// Tamaño pantalla
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

MPU6050 mpu;
MPU6050 mpus[15];
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
unsigned long previousMillisPulseSensor = 0;
unsigned long previousMillisScreen = 0;
const long intervalAccel = 100;  
const long intervalSreen = 10000;
const long intervalMQ3 = 4000;
const long intervalPulseSensor = 20;
// Vars globales sensores
double temperature = 0.0;
int myBPM = 0;
String readString = "";

int numSensores = -1;
String sensores[15][2];

const int button1Pin = 16; 
const int button2Pin = 17;
bool button1State = LOW;
bool button2State = LOW;


void setSensor(String linea){
  activo = true;
  if(numSensores<=15){
    int firstIndex = 8;
    int lastColonIndex = linea.lastIndexOf(':');
    String id = linea.substring(firstIndex + 1, lastColonIndex);
    String lastPart = data.substring(lastColonIndex + 1);

    int pines[8] = {0};
    int index = 0;
    int startPos = 0;
    while (startPos < lastPart.length() && index < 8) {
      int endPos = lastPart.indexOf('-', startPos);
      
      if (endPos == -1) {  
        endPos = lastPart.length();
      }

      pines[index] = lastPart.substring(startPos, endPos).toInt();
      index++;
      
      startPos = endPos + 1;
    }

    bool agregar = true;
    for(int i = 0; i <= numSensores; i++){
      int firstColonIndex = sensores[i].indexOf(':');
      int secondColonIndex = sensores[i].indexOf(':', firstColonIndex + 1);
      if(id==sensores[i].substring(firstColonIndex + 1, secondColonIndex)){
        agregar = false;
        i = numSensores+1;
      }
    }
    if(agregar==true){
      String typeRequest = linea.substring(4, 7);
      typeRequest = typeRequest +":"+ id +":0";
      switch (typeRequest){

        case "RTC":

          max_sda_pin = pines[0];
          max_scl_pin = pines[1];

        case "ALC":

          pinMode(pines[0], INPUT);

        case "PSS":

          Wire.begin(pines[0], pines[1]);
          mpu.initialize();
          if (!mpu.testConnection()) {
            while (1);
          } 
          
          mpu_sda_pin = pines[0];
          mpu_scl_pin = pines[1];



      }
      sensores[numSensores+1] = {typeRequest, pines}; 
      numSensores = numSensores + 1;
    }
  }
}



void EnviarDatos(String linea){
  if(linea != ""){
    String request = linea.substring(0, 3);
    if(request=="REA"){
      String message = getSensorValue(linea.substring(4));
      pTxCharacteristic->setValue(message); 
      pTxCharacteristic->notify();
      Serial.println(getSensorValue(linea.substring(4)));
    } 
    else if (request=="RLJ"){
      lastClockMessage = linea.substring(4);
      if(pantallaMessageType==4){
        ActualizarPantalla(); 
      }
    }
    else if (request=="UPA"){
      String typeRequest = linea.substring(4, 7);
      if (typeRequest == "RTC") {
        umbralRitmoCardiaco = linea.substring(8).toInt();
      } else if (typeRequest == "PSS") {
        umbralPasos = linea.substring(8).toInt();
        pasosContador = 0;
      } else if (typeRequest == "DST") {
        umbralAlarma = linea.substring(8).toFloat();
        distanciaContador = 0;
      }
      else if (typeRequest=="UPS"){
        pantallaMessageType = linea.substring(8).toInt();
        ActualizarPantalla(); 
      }
    }
    else if (request=="NEW"){
      setSensor(linea);
    } 
    else if(request == "DES"){
      activo = false;
      int firstColonIndex = linea.indexOf(':');
      int secondColonIndex = linea.indexOf(':', firstColonIndex + 1);
      String id1 = linea.substring(firstColonIndex + 1, secondColonIndex);
      String id2 = linea.substring(secondColonIndex+1);

      if(id1=="0"){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("No");
        display.println("registrado");
        display.println("Conecta a internet");
        display.display();
      } 
      else {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("No");
        display.println("registrado");
        display.println("Reloj: "+id1);
        display.println("Pesa: "+id2);
        display.display();
      } 
    }

    if(readString.indexOf('\n') != -1){
      EnviarDatosLn(readString);
    }
    
  }
  

}


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
 
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};
 
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();
      Serial.println(rxValue);
      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++){
        }
      }
      EnviarDatos(rxValue);
    }
};

void setup() {
  Serial.begin(9600);
  
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  Wire1.begin(max_sda_pin, max_scl_pin);
  if (!particleSensor.begin(Wire1)) 
  {
    while (1);
  }

  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); 
  particleSensor.setPulseAmplitudeGreen(0);

  //Activar sensor MPU6050

  Wire.begin(mpu_sda_pin, mpu_scl_pin);
  mpu.initialize();
  if (!mpu.testConnection()) {
    while (1);
  } 
  //Activar oled SSD1306

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  pinMode(BUZZER_PIN, OUTPUT);
  ActualizarPantalla(); 



  BLEDevice::init("ESP32");
 
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
 
  BLEService *pService = pServer->createService(SERVICE_UUID);
 
  pTxCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID_TX,
                                        BLECharacteristic::PROPERTY_NOTIFY
                                    );
                      
  pTxCharacteristic->addDescriptor(new BLE2902());
 
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                                             CHARACTERISTIC_UUID_RX,
                                            BLECharacteristic::PROPERTY_WRITE
                                        );
 
  pRxCharacteristic->setCallbacks(new MyCallbacks());
 
  pService->start();
 
  pServer->getAdvertising()->start();
  
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisAccel >= intervalAccel) {
    previousMillisAccel = currentMillis;
    Acelerometro();
  }
  if (currentMillis - previousMillisScreen >= intervalSreen) {
    previousMillisScreen = currentMillis;
    ActualizarPantalla();
  }
  if (currentMillis - previousMillisMQ3 >= intervalMQ3) {
    previousMillisMQ3 = currentMillis;
    MedirMQ3();
    MedirTermistor();
    if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
    } else if (!deviceConnected && oldDeviceConnected) {
      pServer->startAdvertising();
      oldDeviceConnected = deviceConnected;
    }
  }
  if (currentMillis - previousMillisPulseSensor >= intervalPulseSensor) {
    previousMillisPulseSensor = currentMillis;
    MedirPulseSensor();
    if (digitalRead(button1Pin) == LOW && button1State == HIGH) {
      if(pantallaMessageType<5){
        pantallaMessageType = pantallaMessageType + 1;
      }else{
        pantallaMessageType = 1;
      }
      ActualizarPantalla();
    }else if (digitalRead(button2Pin) == LOW && button2State == HIGH) {
      previousMillis = millis();
      thisNote = 0;
      alarmActive = false;
      noTone(BUZZER_PIN);
    }
    
    button1State = digitalRead(button1Pin);
    button2State = digitalRead(button2Pin);
  }
  activarAlarma();

}


void setSensorValue(int id, String value){
  for(int i = 0; i <= numSensores; i++){
    int firstColonIndex = sensores[i][0].indexOf(':');
    int secondColonIndex = sensores[i][0].indexOf(':', firstColonIndex + 1);

    if(id==sensores[i][0].substring(firstColonIndex + 1, secondColonIndex).toInt()){
      sensores[i][0]=sensores[i][0].substring(0,secondColonIndex+1)+value;
    } 
  }
}

void setSensorValue(String type, String value){
  for(int i = 0; i <= numSensores; i++){
    if(type==sensores[i][0].substring(0,3)){
      sensores[i][0]=sensores[i][0].substring(0,sensores[i][0].indexOf(':', sensores[i][0].indexOf(':') + 1))+":"+value;
    } 
  }
}

String getSensorValue(String id){
  for(int i = 0; i <= numSensores; i++){
    int firstColonIndex = sensores[i][0].indexOf(':');
    int secondColonIndex = sensores[i][0].indexOf(':', firstColonIndex + 1);

    if(id==sensores[i][0].substring(firstColonIndex + 1, secondColonIndex)){
      return sensores[i][0];
    } 
  }
  return "";
}


String extractFirstLine(String &str) {
  int newlineIndex = str.indexOf('\n'); 
  
  if (newlineIndex == -1) {
    String firstLine = str;
    str = "";
    return firstLine;
  } else {
    String firstLine = str.substring(0, newlineIndex);
    
    str = str.substring(newlineIndex + 1);
    
    return firstLine;
  }
}


void EnviarDatosLn(String letReadString){
    String linea = extractFirstLine(letReadString);
    if(linea != ""){
      String request = linea.substring(0, 3);
      if(request=="REA"){
        Serial.println(getSensorValue(linea.substring(4)));
      } 
 
    else if (request=="NEW"){
      setSensor(linea);
    } 
      else if (request=="UPA"){
        String typeRequest = linea.substring(4, 7);
        if (typeRequest == "RTC") {
          umbralRitmoCardiaco = linea.substring(8).toInt();
        } else if (typeRequest == "PSS") {
          umbralPasos = linea.substring(8).toInt();
          pasosContador = 0;
        } else if (typeRequest == "DST") {
          umbralAlarma = linea.substring(8).toFloat();
          distanciaContador = 0;
        }
        else if (typeRequest=="UPS"){
          pantallaMessageType = linea.substring(8).toInt();
        }
      }
      else if (request=="RLJ"){
        lastClockMessage = linea.substring(4);
      }

      if(letReadString.indexOf('\n') != -1){
        EnviarDatosLn(letReadString);
      }
      
    }
  

}

void Acelerometro() {
  mpu.getAcceleration(&ax, &ay, &az);

  int16_t delta_acc_z = abs(az - prev_acc_z);
  if (delta_acc_z > threshold) {
    
    stepCount++;
    distancia += 0.45;

    setSensorValue("PSS",String(stepCount));
    setSensorValue("DST",String(distancia,2));
    pasosContador++;
    distanciaContador = distanciaContador + 0.45;
    
    ActualizarPantalla();
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
  for(int i = 0; i <= sensores; i++){
    if(sensores[i][0].substring(0,3)=="ALC"){

      mq3Value = analogRead(sensores[i][1][0]) - 800;

      int firstColonIndex = sensores[i][0].indexOf(':');
      int secondColonIndex = sensores[i][0].indexOf(':', firstColonIndex + 1);

      setSensorValue(sensores[i][0].substring(firstColonIndex + 1, secondColonIndex).toInt(),String(mq3Value));
    }
  }

}

void MedirTermistor() {
  
  for(int i = 0; i <= sensores; i++){
    if(sensores[i][0].substring(0,3)=="TMP"){
      const double A = 0.001129148;
      const double B = 0.000234125;
      const double C = 0.0000000876741;

      int rawValue = analogRead(sensores[i][1][0]); 
      double voltage = (rawValue / 4095.0) * 3.3; 
      double resistance = (3.3 * 10000 / voltage) - 10000; 
      temperature = (1.0 / (A + B * log(resistance) + C * pow(log(resistance), 3))) - 273.15;

      int firstColonIndex = sensores[i][0].indexOf(':');
      int secondColonIndex = sensores[i][0].indexOf(':', firstColonIndex + 1);

      setSensorValue(sensores[i][0].substring(firstColonIndex + 1, secondColonIndex).toInt(),String(temperature));
    }
  }
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

    setSensorValue("RTC",String(beatAvg));
      if(beatAvg <= umbralRitmoCardiaco){
        ritmoCardiacoAlarmaActivo = true;
      }

    if(ritmoCardiacoAlarmaActivo == true && beatAvg >= umbralRitmoCardiaco){
      iniciarAlarma();
      ritmoCardiacoAlarmaActivo = false;
    }
    }
  }
  
}

void iniciarAlarma() {
  if(alarmActive==false){
    alarmActive = true;
    previousMillis = millis();
    thisNote = 0;
  }
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
  // Actualiza la pantalla con los valores almacenados
  if(activo==true){
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
          display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       
          display.setTextSize(2);                                   
          display.setTextColor(WHITE);  
          display.setCursor(50,0);                
          display.println("BPM");             
          display.setCursor(50,18);                
          display.println(beatAvg);  
          display.display();
          
        if (checkForBeat(particleSensor.getIR()) == true)                       
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
            display.print("Temp: ");
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
    
    display.display();
  }
}
