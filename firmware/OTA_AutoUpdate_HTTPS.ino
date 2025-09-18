#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#ifndef APSSID
#define APSSID "Totalplay-19A2"
#define APPSK  "19A294AErC4fqSkj"
#endif

ESP8266WiFiMulti WiFiMulti;
WiFiServer server(80);
int value;
void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.print("WiFi Conectandose a : ");
  Serial.println( APSSID );
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);
  Serial.println();
  Serial.print("Connecting");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }

  
  Serial.println();

  Serial.println("WiFi Conectado Exitosamente!");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP() );

  server.begin();
  Serial.println("Servidor en marcha!");

  // Print the IP address
  Serial.print("Utilisa este link para conectarte: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
}

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void loop() {

  WiFiClient client= server.available();
  if (client)
  {
    //  Wait until the client sends some data
  Serial.println("HOLA NUEVO CLIENTE PUTO!");
  while(!client.available()){
  delay(1);
  }

   // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

   // Match the request
  if (request.indexOf("/Actualizate=1") != -1)  {     
    value = HIGH;
    client.stop();
  }
  if (request.indexOf("/Actualizate=0") != -1)  {
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("SI SE PUDO PERRO!");
  } else {
    client.print("ESTA MAMADA NO SIRVE");
  }
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
  }
  if (value == HIGH){
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "https://wifisolutions.herokuapp.com/public/version1.bin");
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
}
}
