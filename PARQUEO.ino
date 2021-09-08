#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
/*#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>*/


//Declaración de constantes
#define trigPin  4
#define echoPin  5
#define umbral   10
#define INTERVAL_READ 1000
#define Device              "A003"
#define SERVICE_UUID        "51b4700f-600a-4084-b895-382fd7480683"   //UUID SERVICIO
#define CARAC_ESTADO_UUID   "aff4d08d-c11f-4c42-b742-471accdd98b0"   //UUID CARACTERISTICA ESTADO

//Declaración de variables
unsigned long prvMillis;
bool rqsNotify = false;
bool deviceConnected = false;
bool oldDeviceConnected = false;
String Estado_act = "OFF";
String Estado_prev = "OFF";
double latitude = 0;
double longitude = 0;
static const int RXPin = 3, TXPin = 1;
static const uint32_t GPSBaud = 9600;
const unsigned int writeInterval = 2500; // write interval (in ms)

// objects
//TinyGPSPlus gps; // The TinyGPS++ object
//SoftwareSerial ss(RXPin, TXPin); // The serial connection to the GPS device

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
/*BLECharacteristic* pCharacteristic2 = NULL;
BLECharacteristic* pCharacteristic3 = NULL;*/

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      rqsNotify = false;
      prvMillis = millis();
      Serial.println("Device connected");
      
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      rqsNotify = false;
      Serial.println("Device disconnected");
    }
};

void Estado(){
  
  // Medición de distancia
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration*0.034/2; 
    
  //Indicación de estado
  if (distance < umbral){
    Estado_act = "ON";
  }
  else {
    Estado_act = "OFF";
  }
    
  //Habilitación para notificación
  rqsNotify = true;
  Serial.println(Estado_act);  
}  

// GPS displayInfo
/*void displayInfo() {
    if (gps.location.isValid()) 
    {
      latitude = (gps.location.lat());
      longitude = (gps.location.lng());
      Serial.println(latitude);
      Serial.println(longitude);
      delay(writeInterval);// delay 
    } else {
        Serial.println(F("INVALID"));
      }
}*/

void setup() {

  //Declaración de pines
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  // Inicio de comunicación serial
  Serial.begin(115200);
  
  // GPS baud rate
  //ss.begin(GPSBaud);

  // Iniciar Dispositivo BLE
  BLEDevice::init(Device);

  // Crear Servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Crear Servicio BLE
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Crear característca para ver estado de parqueo
  pCharacteristic = pService->createCharacteristic(
                      CARAC_ESTADO_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      //BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  /*pCharacteristic2 = pService->createCharacteristic(
                      CARAC_LAT_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic3 = pService->createCharacteristic(
                      CARAC_LNG_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );*/
                    
  // Añadir descriptores para caracteristicas
  pCharacteristic->addDescriptor(new BLE2902());
  /*pCharacteristic2->addDescriptor(new BLE2902());
  pCharacteristic3->addDescriptor(new BLE2902());*/
 
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
}



void loop() {
 
  // Captura de ubicación
  /*while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }*/

  //Captura de estado


  //Envío de notificación
  if (deviceConnected) {
    
    Estado();
    
    if(rqsNotify){
      
      rqsNotify = false;
      
      if (Estado_act == "OFF"){
         pCharacteristic->setValue("OFF");
         /*pCharacteristic2->setValue(latitude);
         pCharacteristic3->setValue(longitude);
         pCharacteristic->notify();
         pCharacteristic2->notify();
         pCharacteristic3->notify();*/
         delay(200);         
      }
      
       if (Estado_act == "ON"){
         pCharacteristic->setValue("ON");
         /*pCharacteristic2->setValue(latitude);
         pCharacteristic3->setValue(longitude);
         pCharacteristic->notify();
         delay(100);
         pCharacteristic2->notify();
         delay(100);
         pCharacteristic3->notify();*/
         delay(100);
      }
      
    }
  }

  //disconnecting
    if (!deviceConnected && oldDeviceConnected) {      
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        oldDeviceConnected = deviceConnected; 
        delay(1000);
        Serial.println("ESP reset");
        ESP.restart();   
    }
    
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
