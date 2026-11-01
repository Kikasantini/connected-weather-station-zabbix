// ====================================================== Web server :
// ------------------------------ Bibliothéques
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "MyWifi.h"


// ------------------------------ Constantes et variables
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);


// ====================================================== Boutons :
#include <PinButton.h>

PinButton changeLED(14); // D6
PinButton toggleLED(12); // D5

// ====================================================== DELs :
#define led0 0 // D3
#define led1 2 // D4
#define MAX 1

int lights_index = 0;

bool led0state = false; // false = éteinte, true = allumée
bool led1state = false; // false = éteinte, true = allumée


// ====================================================== BME 280 :
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

Adafruit_BME280 bme; // I2C

// VIN -> 3V3
// GND -> G
// SCL -> D1
// SDA -> D2

class BME280Meteo {
public:
  float humidite;
  float temperature;
  float pression;
  float altitude;

  void update() {
    humidite = bme.readHumidity();
    temperature = bme.readTemperature();
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    pression = bme.readPressure() / 100.0F;
  }

  String texteJSON() {
    String message = R"({"body":{"meteo":{"humidity": )";
    message += humidite;
    message += ",\"temperature\": ";
    message += temperature;
    message += ",\"altitude\": ";
    message += altitude;
    message += ",\"pressure\": ";
    message += pression;
    message += ",\"light\": ";
    message += lights_index;
    
    message += ",\"led0\": ";
    if(led0state == true){
      message += "\"ON\"";  
    }
    else{
      message += "\"OFF\"";  
    }
    
    message += ",\"led1\": ";
    if(led1state == true){
      message += "\"ON\"";  
    }
    else{
      message += "\"OFF\"";  
    }

    message += "}}}";
    return message;
  }

  String texteHTML(){
    String message;
    message = "Temp&eacute;rature = ";
    message += temperature;
    message += "&#x2103;<br />";
    message += "Humidit&eacute; relative = ";
    message += humidite;
    message += " %<br />";
    message += "Altitude = ";
    message += altitude;
    message += " m<br />";
    message += "Pression barom&eacute;trique = ";
    message += pression;
    message += " hPa<br />";

    return message;
  }
};

BME280Meteo* obj_meteo;

// ====================================================== Serveur - Méthodes :
void handleRoot() {
  server.send(200, "text/plain", "aaaaaaaaaaaaaaa\r\n");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void meteoJSON() {
  // Cette méthode affiche la string JSON dans la page
  obj_meteo->update();
  String r = obj_meteo->texteJSON();  
  server.send(200, "text/plain", r);
}

void meteoHTML() {
  // Cette méthode affiche la string des donnéss en format HTML dans la page
  obj_meteo->update();
  String r = obj_meteo->texteHTML();
  server.send(200, "text/html", r);
}

// ====================================================== Setup :
void setup(void) {
  Serial.begin(74880);
  
  // DELs
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  
  //Serveur
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Connexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/meteoJSON", meteoJSON);
  server.on("/meteoHTML", meteoHTML);
  server.onNotFound(handleNotFound);
    
  server.begin();
  Serial.println("HTTP server started");

  // BME 280
  unsigned status;
  status = bme.begin(0x76); // BME280 address

  obj_meteo = new BME280Meteo(); // l'obj_meteo est une instance de la classe BME280Meteo
}

// ====================================================== Boucle principal :
void loop(void) {
  // Serveur
  server.handleClient();
  MDNS.update();

  // Boutons
  changeLED.update();
  toggleLED.update();

  // À chaque fois que nous appuyons sur le bouton changeLED, 
  // il alterne entre les 2 DELs en changeante la valeur de la variable lights_index.
  if(changeLED.isSingleClick()){
    if(lights_index == 0){
      lights_index = 1;
    }
    else{
      lights_index = 0;
    }
  }

  // À chaque fois que nous appuyons sur le bouton toggleLED, le programme change l'état de la DEL selectionnée
  // et l'allume / l'éteint.
  if(toggleLED.isSingleClick()){
    if(lights_index == 0){
      led0state = !led0state;
      digitalWrite(led0, led0state);
    }
    if(lights_index == 1){
      led1state = !led1state;
      digitalWrite(led1, led1state);
    }
  }
  
}
