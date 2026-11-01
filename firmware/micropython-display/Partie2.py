# ================================================================= Bibliothèques :
import ujson
import time
import usocket
import machine
from machine import Pin, ADC
import SSD1306
import os

# ================================================================= Potentiomètre :
pot = ADC(0)

# ================================================================= Écran SSD1306 :
i2c = machine.I2C(machine.Pin(5), machine.Pin(4))
oled = SSD1306.SSD1306_I2C(128, 64, i2c,addr=0x3c)

# ================================================================= get JSON :

# configuration
INTERVAL = 1000 # fréquence de rafaîchissement en ms
GETtimeout = 1.8 # timeout en secondes pour le client HTTP 
HOST = "192.168.1.100" # replace with the ESP8266 Arduino server local IP

print("démarrage..", end = "")

#initialisation du timer
millis = time.ticks_ms()
millisPrec = millis

#initialisation prec json
JSONprec = ""

def getJSON(HOST):
    ''' la fonction prend une adresse IP et renvoie le corps de la réponse web à de l'URL "http://[adresse_IP]/json" '''
    global JSONprec #préserve la dernière valeur en cas d'erreur de connexion
    while True:
        
        try:
            s = usocket.socket()
            s.settimeout(GETtimeout)
            ai = usocket.getaddrinfo(HOST, 80)
            addr = ai[0][-1]
            s.connect(addr)
            s.write("GET /meteoJSON HTTP/1.0\r\n")
            s.write("Host: " + HOST + "\r\n\r\n\r\n")
            data = s.read(512)
            headers, body = data.split(b"\r\n\r\n", 1)
            JSONprec =  body
            break
        except Exception as e:
            s.close()
            print("error=", e, end=" ")
            body = JSONprec
            break
        
    s.close()
    return body

# ================================================================= Drawing LED :
def drawLED(x, y, state):

    oled.text(state, x-5, y+10, 1)
    
    oled.line(x, y, x, y+5, 1)
    oled.pixel(x+1, y-1, 1)
    oled.pixel(x+2, y-2, 1)
    oled.pixel(x+3, y-2, 1)
    oled.pixel(x+4, y-2, 1)
    oled.pixel(x+5, y-1, 1)
    oled.line(x+6, y, x+6, y+5, 1)
    oled.line(x-1, y+5, x+7, y+5, 1)
    oled.line(x-1, y+6, x+7, y+6, 1)

# ================================================================= Drawing Contrast :
def drawContrast(c):
    oled.text(str(c), 90, 50, 1)

# ================================================================= MAIN :

def main():
    
    isOff = False
    
    oled.fill(0)
    oled.show()
    
    # get JSON :
    global INTERVAL
    global millis
    global millisPrec


    while True:
        pot_value = pot.read()
        pot_converted = int(pot_value*255/1024)
        time.sleep(0.1)
        
        millis = time.ticks_ms()
        
        if millis > (millisPrec + INTERVAL):
            
            millisPrec = millis
            json = getJSON(HOST)
            print(json)
            
            # Désérialise le string JSON :
            meteo = ujson.loads(json)

            oled.fill(0)
            
            light = str(meteo["body"]["meteo"]["light"])
            
            oled.text("Temperat " + str(meteo["body"]["meteo"]["temperature"]), 0, 0)
            oled.text("Humidite " + str(meteo["body"]["meteo"]["humidity"]), 0, 10)
            oled.text("Altitute " + str(meteo["body"]["meteo"]["altitude"]), 0, 20)
            oled.text("Pressure " + str(meteo["body"]["meteo"]["pressure"]), 0, 30)
            drawLED(15, 47, str(meteo["body"]["meteo"]["led0"]))
            drawLED(45, 47, str(meteo["body"]["meteo"]["led1"]))
            
            oled.text(".", int(light)*30 + 15, 36)
            drawContrast(pot_converted)
            
            if pot_converted < 5:
                oled.poweroff()
                isOff = True
            else:
                if isOff == True:
                    oled.poweron()
                    isOff = False
                oled.contrast(pot_converted)    
            
            oled.show()
    # end of While
    # end of get JSON


if __name__ == "__main__":
    main()
