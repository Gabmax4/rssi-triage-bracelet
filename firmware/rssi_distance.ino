//MIDA LA DISTANCIA SEGUN LA POTENCIA DE LA SEÑAL

#include <WiFi.h>

const char* ssid = "Totalplay-19A2";
const char* password = "19A294AErC4fqSkj";
 float rssi;
void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

 
  Serial.print("RSSI:");
  Serial.println(rssi);





}

void loop() {
  // put your main code here, to run repeatedly:
 rssi = WiFi.RSSI();
 Serial.print("RSSI:");
 Serial.println(WiFi.RSSI());

  double distancia= pow(10,(((-1*WiFi.RSSI())-20*log10(2.4E9)+147.55222)/20));
  double potencia=(pow(10,(WiFi.RSSI()/10))*0.001);
  double dB= 10*log10(potencia);
  double distancia2= pow(10,(((dB)-20*log10(2.4E9)+147.55222)/20));
  Serial.print(distancia,6);
  Serial.println("m     distancia mala/puta");
  Serial.print(potencia,6);
  Serial.println("W");
  Serial.print(dB);
  Serial.println("dB");  
    Serial.print(distancia2,6);
  Serial.println("m     distancia buena :) ");
 delay(1000);
}
