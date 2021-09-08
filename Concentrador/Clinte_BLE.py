# Importar libreria Bluepy
 
#from blynkLib import *
from bluepy import btle
import time



# Initialize Blynk
#blynk = Blynk('mTtJXry_Ey7skY8EpcoNR5uEfoE65hqZ')

class MyDelegate(btle.DefaultDelegate):
	def _init_(self):
		blte.DefaultDelegate._init_(self)
	def handleNotification(self, chandle, data):
		ch_data = data
		
		
DEVICE =          ['A001', 'A002', 'A003']
DIRECCION =       ['9C:9C:1F:CA:1B:22', '9C:9C:1F:CA:FD:0E', '9C:9C:1F:CA:15:32']
SERVICIO =        ["4fafc201-1fb5-459e-8fcc-c5c9c331914b", "bbfb7249-236c-4457-83dd-048cc359f257", "51b4700f-600a-4084-b895-382fd7480683"] 
CARACTERISTICA =  ["beb5483e-36e1-4688-b7f5-ea07361b26a8", "8f4dd464-fdcf-43f4-902b-049365f2d6df", "aff4d08d-c11f-4c42-b742-471accdd98b0"]
"""CARACTERISTICA2 = ["972dc46c-1eca-43a3-ae54-23839f0ad6c6", "8da693cc-a8da-414d-9be8-b0194f76ccad", "1901e103-4a76-4167-a12b-fc94f5873feb"]
CARACTERISTICA3 = ["1d6df975-d857-4478-8474-35d3272d753d", "c6ac75ea-f2a7-433e-8928-bee828905163", "4a347c6e-a280-4f82-b4e4-df39c372f46b"]"""




while True:
    #blynk.run()
    try:
        i=0
        for i in range(2):
            
            device = DEVICE[i]
            direccion = DIRECCION[i]
            servicio = SERVICIO[i]
            caracteristica = CARACTERISTICA[i]
            """caracteristica2 = CARACTERISTICA2[i]
            caracteristica3 = CARACTERISTICA3[i]"""
            print(device)
            print(direccion)
            
            p = btle.Peripheral(direccion)
            time.sleep(4)
            p.setDelegate(MyDelegate())
            
            svc = p.getServiceByUUID(servicio)
            ch = svc.getCharacteristics(caracteristica)[0]
            print(ch)
            ch_data = p.readCharacteristic(ch.valHandle)
            print(ch_data)
            #if ch_data =='ON':
                  #blynk.virtual_write(i+1, 1)
            #else:
                  #blynk.virtual_write(i+1, 0)
            
            """ch2 = svc.getCharacteristics(caracteristica2)[0]
            ch_data2 = p.readCharacteristic(ch2.valHandle)
            print(ch_data2)
            time.sleep(1)
            
            ch3 = svc.getCharacteristics(caracteristica3)[0]
            ch_data3 = p.readCharacteristic(ch3.valHandle)
            print(ch_data3)"""
            
            p.disconnect()
            time.sleep(10)
            
    except btle.BTLEDisconnectError as err:
        print("Dispositivo "+device+" desconectrado")
