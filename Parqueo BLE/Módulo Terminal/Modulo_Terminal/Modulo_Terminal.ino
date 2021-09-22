/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define trigPin  4
#define echoPin  5
#define umbral   10
#define Device                "A001"
#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"   //UUID SERVICIO
#define CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"   //UUID CARACTERISTICA 1

//Definición de variables
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t Estado_act;
uint16_t Con_id;

//Definición de objetos
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

//Definición de clases
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Dispositivo Conectado");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Dispositivo Desconectado");
      ESP.restart();
    }
};

void get_Estado(){
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
    Estado_act = 0;
  }
  else {
    Estado_act = 255;
  }  
  Serial.print("Distancia: ");
  Serial.println(distance);
  //Serial.println(Estado_act);  
}

void setup() {

  //Declaración de pines
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
   
  //Iniciar comunicación seria
  Serial.begin(115200);

  // Crear dispositivo BLE
  BLEDevice::init(Device);

  // Crear servidor BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Crear servicio
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Crear Característica para estado
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  
  // Create Descriptor para característica de Estado
  pCharacteristic->addDescriptor(new BLE2902());

  //Iniciar servicio
  pService->start();

  //Iniciar avisos
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Esperando por cliente para notificar...");
}  

void loop() {
    
    // notify changed value
    if (deviceConnected) {
      delay(8000);
      Con_id = pServer->getConnId();
      get_Estado();
      Serial.print("ID de conexión: ");
      Serial.println(Con_id);
      Serial.print("Estado: ");
      Serial.println(Estado_act);
      pCharacteristic->setValue((uint8_t*)&Estado_act, 4);
      pCharacteristic->notify();
      Serial.println("Notificación enviada");
      delay(200);
      //BLEDevice::stopAdvertising();
      //pServer->disconnect(Con_id);     
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        //pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
