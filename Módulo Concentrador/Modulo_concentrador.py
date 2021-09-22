#Librería Bluepy para controlar dispositivos BLE
from bluepy import btle  
             
#Librería Blynk para enlace con platforma IOT Blynk
from blynklib import *  

#Librería time para menjo de retardos
import time 

#Inicializar Blynk
blynk = Blynk('PYkbrHBrV2oJ2G0KdVJRm3gL1rlepeYR')

#Declaración de parametros
DEVICES =          ['A001', 'A002', 'A003']
DIRECCION =       ['9C:9C:1F:CA:1B:22', '9C:9C:1F:CA:FD:0E', '9C:9C:1F:CA:15:32']
SERVICIO =        ["4fafc201-1fb5-459e-8fcc-c5c9c331914b", "bbfb7249-236c-4457-83dd-048cc359f257", "51b4700f-600a-4084-b895-382fd7480683"] 
CARACTERISTICA =  ["beb5483e-36e1-4688-b7f5-ea07361b26a8", "8f4dd464-fdcf-43f4-902b-049365f2d6df", "aff4d08d-c11f-4c42-b742-471accdd98b0"]

READ_PRINT_MSG = "[READ_VIRTUAL_PIN_EVENT] Pin: V{}"


class MyDelegate(btle.DefaultDelegate):
    def __init__(self, device):
        btle.DefaultDelegate.__init__(self)
        self.device = device
        # ... initialise here

    def handleNotification(self, cHandle, data):
        #print(cHandle)
        print("Notificacion recibida")
        print("Dispositivo Conectado: {}".format(self.device))
        print("Estado leído: {}".format(data[0]))
        
        blynk.connect()
        
        for i in range(len(DEVICES)):
            if self.device == DEVICES[i]:
                print(READ_PRINT_MSG.format(i+1))
                blynk.virtual_write(i+1, data[0])
                break
        
        blynk.disconnect()
        #time.sleep(1)




while True:
    
    for i in range(len(DEVICES)):
        flag1 = 0
        nty = 0
            
        while flag1 == 0:
            try:
                print("Conectando con: {}".format(DEVICES[i]))
                p = btle.Peripheral(DIRECCION[i])
                p.withDelegate(MyDelegate(DEVICES[i]))
                svc = p.getServiceByUUID(SERVICIO[i])
                ch = svc.getCharacteristics(CARACTERISTICA[i])[0]
                setup_data = b"\x01\x00"
                p.writeCharacteristic(ch.valHandle + 1, setup_data)
                    
                if p.waitForNotifications(12.0):
                    nty = 1;
                    print("Notificación procesada")
                        
                if nty == 0:
                    print("Notificación no recibida")
                    
                flag1 = 1
                    
            except btle.BTLEDisconnectError as err:
                print("Error de conexión con "+DEVICES[i])
                continue
                
        print("Desconexión de "+DEVICES[i])
        p.disconnect()
        time.sleep(1)
