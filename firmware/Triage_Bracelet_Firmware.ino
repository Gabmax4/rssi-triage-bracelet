#include <ESP8266WiFi.h>//Se incluye esta libreria para que pueda jalar la placa Wifi NodeMCU 
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WebServer serv;
ESP8266WiFiMulti WiFiMulti;

// Fingerprint for demo URL, expires on June 2, 2019, needs to be updated well before this date
// const uint8_t fingerprint[20] = {0x5A, 0xCF, 0xFE, 0xF0, 0xF1, 0xA6, 0xF4, 0x5F, 0xD2, 0x11, 0x11, 0xC6, 0x1D, 0x2F, 0x0E, 0xBC, 0x39, 0x8D, 0x50, 0xE0};


unsigned long contador_1h = 0;
int bandera_1h = 0;
unsigned long contador_2h = 0;
int bandera_2h = 0;
unsigned long contador_3h = 0;
int bandera_3h = 0;
unsigned long tiempo_mas = 0;
int bandera_todo_off = 0;
int bandera_arcoiris=0;
unsigned long contador_arcoiris = 0;

//OTA
bool ota_flag = true;
uint16_t time_elapsed = 0;

//Paso No.1: Seleccionar en Placa el NodeMCU 0.9 (esp-12 Module)
//Paso No.2: Seleccionar el Puerto.... en mi caso COM5 
//Paso No.3: Ingresar el ssid al que se conectara la placa Wifi
//Paso No.4: Ingresar Contraseña 
//Paso No.5: Subir Codigo a la placa Wifi
//Paso No.6: Abrir el puerto Serial y lo colocamos a 115200 bautios
//Paso No.7: En la placa WIFI presionar el boton de RESER
//Paso No.8: Se imprimira el IP el cual se pone en cualquier buscador o en el codigo
//paso No.9: Desconectar y alimantar de una fuente externa la placa Wifi 
//Paso No.10: Verlo jalando :P 

//CLAVE CITY JUMPER 
/*
const char* ssid = "Totalplay-E77F";  //AQUI SE INRESA LA ssid del modem
const char* password = "E77F3BE7PjYpUxaA";     //AQUI SE INGRESA LA contraseña del modem 
*/

IPAddress server_addr(185,176,43,90);  // IP of the MySQL *server* here
char user[] = "2801940_cityjumper";              // MySQL user login username
char password1[] = "parksolution";        // MySQL user login password


const char* ssid = "AND";  //AQUI SE INRESA LA ssid del modem
const char* password = "123456789";     //AQUI SE INGRESA LA contraseña del modem 

char INSERT_SQL[] = "INSERT INTO city_jumper.pulsera (status) values ('TRUE')";

//PONEMOS LOS PINES LOS CUALES VAMOS A USAR 
int ledROJO = 13;   // GPIO13---D7 de la placa NodeMCU
int ledVERDE = 4;  // GPIO16---D2 de la placa NodeMCU
int ledAZUL= 5;     // GPIO5---D1 de la placa NodeMCU
WiFiServer server(80); //SE PONE A UN SERVIDOR EN ESTE CASO 80

int valor_led_verde = LOW;         //ESTA VARIABLE NOS DIRA EN LA PAGUINA SI EL LED VERDE ESTA PRENDIDO
int valor_led_rojo = LOW;         //ESTA VARIABLE NOS DIRA EN LA PAGUINA SI EL LED VERDE ESTA PRENDIDO
int valor_led_azul = LOW;         //ESTA VARIABLE NOS DIRA EN LA PAGUINA SI EL LED VERDE ESTA PRENDIDO
int valor_led_amarillo = LOW;         //ESTA VARIABLE NOS DIRA EN LA PAGUINA SI EL LED VERDE ESTA PRENDIDO

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;




void setup() {


  Serial.begin(115200); //Iniciamos la comunicacion Serial a 115200 bautios
  delay(10); //esperamos 10 milisegundos 

  //COLOCAMOS LOS TOSO LOS PINES COMO SALIDA Y LOS INICIALIZAMOS APAGADOS
  pinMode(ledROJO, OUTPUT);
  digitalWrite(ledROJO, LOW);
  pinMode(ledAZUL, OUTPUT);
  digitalWrite(ledAZUL, LOW);
  pinMode(ledVERDE, OUTPUT);
  digitalWrite(ledVERDE, LOW);
 
  // Coneccion a la red Wifi
  Serial.println();
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid); //Nos dice mediante el puerto serial a que Modem esta conectada la placa NodeMCU
  
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP( ssid, password);
  while (WiFi.status() != WL_CONNECTED) {//Mientras que no se conecte imprimira puntos suspensivos ...
    delay(500);                          //Tratara de conectarse cada 500 milisegundos
    Serial.print(".");                   //Imprime .
  }
  Serial.println("");                    //Hace un salto de linea
  Serial.println("WiFi CONECTADO");      //Si sale del while sicnifica que se pudo conectar a la red y se imrime WiFi conectado
 
  //Inicia el servidor que es la placa NodeMCU
  server.begin();
  Serial.println("Servidor en Marcha");
 
  //IMPRIME LA IP
  Serial.print("Utiliza este URL para conectarte: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP()); //imprime el IP
  Serial.println("/");
  Serial.print("configurado como como de: ");
  Serial.println(WiFi.getMode());
  Serial.println("Estado: ");
  WiFi.printDiag(Serial);

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password1))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);


 //OTA 
 ArduinoOTA.setHostname("ESP-PUTO!");
 ArduinoOTA.setPassword("pato");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

}
 
void loop() {
// HTTPS get()
if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint);
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://city-jumper.000webhostapp.com/confirm.php")) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          //Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }

  
  //OTA
if(ota_flag)
  {
    uint16_t time_start = millis();
    while(time_elapsed < 40000)
    {
      ArduinoOTA.handle();
      time_elapsed = millis()-time_start;
      delay(10);
    }
    ota_flag = false;
  }
  serv.handleClient();
  
  //PONE TODOS LOS LEDS APAGADOS
      if(bandera_todo_off == 1){
      bandera_1h=0;
      bandera_2h=0;
      bandera_3h=0;
      bandera_arcoiris=0;
      bandera_todo_off=0;
      valor_led_rojo = LOW;
      valor_led_azul = LOW;
      valor_led_verde = LOW;
    }
//CONTADOR PARA 1h
    if( bandera_1h==1){
    if( millis() - contador_1h >= 45000 + tiempo_mas){
          //PRENDO AMARILLO
          //verde + rojo = amarillo
          //NOTA el led verde ya esta prendido desde la instancia qH
      analogWrite(ledROJO, 1023);             //PRENDO EL ROJO
      valor_led_amarillo = HIGH;
      valor_led_rojo = HIGH;
      }
      else{
        analogWrite(ledVERDE, 1023);           //PRENDE VERDE
        analogWrite(ledAZUL, 0);           //PRENDE VERDE
        analogWrite(ledROJO, 0);           //PRENDE VERDE
        valor_led_verde=HIGH;
        valor_led_azul=LOW;
        valor_led_rojo=LOW;
        }
      if(millis() - contador_1h >= 60000 + tiempo_mas){
        //SOLO QUEDA EL ROJO PRENDIDO
        analogWrite(ledVERDE, 0);            //PRENDO EL VERDE
        analogWrite(ledROJO, 1023);            //APAGAR LED VERDE
        valor_led_verde = LOW;
        valor_led_rojo = HIGH;         //marcara en la paguina el valor del led rojo
        bandera_1h=0;
        }
  }

//CONTADOR PARA 2h
    if( bandera_2h==1){
    if( millis() - contador_2h >= 105000 + tiempo_mas){
          //PRENDO AMARILLO
          //verde + rojo = amarillo
          //NOTA el led verde ya esta prendido desde la instancia qH
      analogWrite(ledROJO, 1023);             //PRENDO EL ROJO
      valor_led_amarillo = HIGH;
      valor_led_rojo = HIGH;
      }
      else{
        analogWrite(ledVERDE, 1023);           //PRENDE VERDE
        analogWrite(ledAZUL, 0);           //PRENDE VERDE
        analogWrite(ledROJO, 0);           //PRENDE VERDE
        valor_led_verde=HIGH;
        valor_led_azul=LOW;
        valor_led_rojo=LOW;
        }
      if(millis() - contador_2h >= 120000 + tiempo_mas){
        //SOLO QUEDA EL ROJO PRENDIDO
        analogWrite(ledVERDE, 0);            //PRENDO EL VERDE
        analogWrite(ledROJO, 1023);            //APAGAR LED VERDE
        valor_led_verde = LOW;
        valor_led_rojo = HIGH;         //marcara en la paguina el valor del led rojo
        bandera_2h=0;
        }
  }

//CONTADOR PARA 3h
    if( bandera_3h==1){
    if( millis() - contador_3h >= 165000 + tiempo_mas){
          //PRENDO AMARILLO
          //verde + rojo = amarillo
          //NOTA el led verde ya esta prendido desde la instancia qH
      analogWrite(ledROJO, 1023);             //PRENDO EL ROJO
      valor_led_amarillo = HIGH;
      valor_led_rojo = HIGH;
      }
      else{
        analogWrite(ledVERDE, 1023);           //PRENDE VERDE
        analogWrite(ledAZUL, 0);           //PRENDE VERDE
        analogWrite(ledROJO, 0);           //PRENDE VERDE
        valor_led_verde=HIGH;
        valor_led_azul=LOW;
        valor_led_rojo=LOW;
        }
      if(millis() - contador_3h >= 180000 + tiempo_mas){
        //SOLO QUEDA EL ROJO PRENDIDO
        analogWrite(ledVERDE, 0);            //PRENDO EL VERDE
        analogWrite(ledROJO, 1023);            //APAGAR LED VERDE
        valor_led_verde = LOW;
        valor_led_rojo = HIGH;         //marcara en la paguina el valor del led rojo
        bandera_3h=0;
        }
  }

  ///FIESTA!!!!
  if(bandera_arcoiris==1){
    if(millis()- contador_arcoiris >= 250){
      contador_arcoiris=millis();
      analogWrite(ledVERDE, random(1,1024));
      analogWrite(ledROJO, random(1,1024));
      analogWrite(ledAZUL, random(1,1024));
    }
    }
 
  // Verifica si un cliente se ha conectado 
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Espera hasta que el cliente mande datos
  Serial.println("Nuevo Cliente");
  while(!client.available()){
    delay(1);
  }
 
  // Lee la primera linea de requisitos
  String request = client.readStringUntil('\r'); //Guarda lo que manda el cliente hasta que enceuntre un /¿
  Serial.println(request);                       //Imprime el requisito que se hizo
  client.flush();                                
 
  // Encaja el requisito que se fue realizado
  int i;                                        //Y se crea un contador para el ciclo for


//REQUISITO DE 1 HORA
if (request.indexOf("/LED=1hra") != -1)  {
    bandera_1h=1;
    contador_1h = millis();
    analogWrite(ledVERDE, 1023);           //PRENDE VERDE   
    valor_led_verde= HIGH;                   //AL PRENDER PRIMERO EL VERDE Y LUEGO EL ROJO SE HACE AMARILLO!!
  }
 

//REQUISITO DE 2 HORAS
if (request.indexOf("/LED=2hra") != -1)  {
    bandera_2h=1;
    contador_2h= millis();
    analogWrite(ledVERDE, 1023);           //PRENDE VERDE   
    valor_led_verde= HIGH;                   //AL PRENDER PRIMERO EL VERDE Y LUEGO EL ROJO SE HACE AMARILLO!!   
}
//REQUISITO DE 3 HORAS
if (request.indexOf("/LED=3hra") != -1)  {   
    bandera_3h=1;
    contador_3h = millis();
    analogWrite(ledVERDE, 1023);           //PRENDE VERDE   
    valor_led_verde= HIGH;                   //AL PRENDER PRIMERO EL VERDE Y LUEGO EL ROJO SE HACE AMARILLO!!
}

//REQUISITO DE 1 HORA ++
if (request.indexOf("/LED=1mas") != -1)  {
    tiempo_mas = tiempo_mas + 60000;
    
}
Serial.print(tiempo_mas);
//REQUISITO DE 2 HORA ++
if (request.indexOf("/LED=2mas") != -1)  {
    tiempo_mas = tiempo_mas + 120000;
    Serial.println(tiempo_mas);
}

//OTA
if(request.indexOf("/reinicio") != -1){
    delay(1000);
    ESP.restart();
  };

if(request.indexOf("/OTA") != -1){
    ota_flag = true;
    time_elapsed = 0;
  };

//REQUISITO DE 3 HORA ++
if (request.indexOf("/LED=3mas") != -1)  {
    tiempo_mas = tiempo_mas + 180000;
    
}
 
if (request.indexOf("/LED=TODOOFF") != -1)  {
    analogWrite(ledROJO, 0);
    analogWrite(ledAZUL, 0);
    analogWrite(ledVERDE, 0);
    valor_led_rojo = LOW;
    valor_led_azul = LOW;
    valor_led_verde = LOW;
    bandera_todo_off = 1;
    tiempo_mas = 0;
  }

  if (request.indexOf("/LED=VERDE") != -1)  {
    digitalWrite(ledVERDE, HIGH);
    valor_led_verde = HIGH;
  }
  
  if (request.indexOf("/LED=OFFV") != -1)  {
    analogWrite(ledVERDE, 0);
    valor_led_verde = LOW;
  }
 
  if (request.indexOf("/LED=ROJO") != -1)  {
    analogWrite(ledROJO, 1023);
    valor_led_rojo =HIGH;
  }
  if (request.indexOf("/LED=OFFR") != -1)  {
    analogWrite(ledROJO, 0);
    valor_led_rojo = LOW;
  }
 
  if (request.indexOf("/LED=AZUL") != -1)  {
    analogWrite(ledAZUL, 1023);
    valor_led_azul = HIGH;
  }
  if (request.indexOf("/LED=OFFA") != -1)  {
    analogWrite(ledAZUL, 0);
    valor_led_azul = LOW;
  }
   if (request.indexOf("/LED=AMARILLO") != -1)  { 
    analogWrite(ledROJO, 1023);
    analogWrite(ledVERDE, 1023);
    valor_led_amarillo = HIGH;
    valor_led_rojo = HIGH;
    valor_led_verde = HIGH;
  }
  if (request.indexOf("/LED=OFFAM") != -1)  {
    analogWrite(ledROJO, 0);
    analogWrite(ledVERDE, 0);
    valor_led_amarillo = LOW;
  }
  if (request.indexOf("/LED=ARCOIRIS") != -1)  {
    bandera_arcoiris = 1;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  //Devuelve la respuesta
  client.println("HTTP/1.1 200 OK");                //Imprime datos en el servidor como si fuera un cliente 
  client.println("Content-Type: text/html");
  client.println(""); //No olvides esta
  client.println("<!DOCTYPE HTML>");
  client.println("<html>"); //CODIGO PHP INICIALIZA EL PHP DE LA PAGUINA WEB
 
  client.print("Estado del Led VERDE: ");
 
  if(valor_led_verde == HIGH) {
    client.print("PRENDIDO");
  } else {
    client.print("APAGADO");
  }
  client.print("<br />");
  
  client.print("Estado del Led ROJO: ");
 
  if(valor_led_rojo == HIGH) {
    client.print("PRENDIDO");
  } else {
    client.print("APAGADO");
  }
  client.print("<br />");
  
  client.print("Estado del Led AZUL: ");
 
  if(valor_led_azul == HIGH) {
    client.print("PRENDIDO");
  } else {
    client.print("APAGADO");
  }
  client.print("<br />");
  
  client.print("El color Amarillos esta: ");
 
  if(valor_led_amarillo == HIGH) {
    client.print("PRENDIDO");
  } else {
    client.print("APAGADO");
  }
  
  client.println("<br><br>");
  client.println("<a href=\"/LED=VERDE\"\"><button>On </button></a>");
  client.println("<a href=\"/LED=OFFV\"\"><button>Off </button></a><br />");  
  client.println("<a href=\"/LED=ROJO\"\"><button>On </button></a>");
  client.println("<a href=\"/LED=OFFR\"\"><button>Off </button></a><br />"); 
  client.println("<a href=\"/LED=AZUL\"\"><button>On </button></a>");
  client.println("<a href=\"/LED=OFFA\"\"><button>Off </button></a><br />"); 
  client.println("<a href=\"/LED=AMARILLO\"\"><button>On </button></a>");
  client.println("<a href=\"/LED=OFFAM\"\"><button>PATOZO! </button></a><br />");
  client.println("<a href=\"/LED=1hra\"\"><button>1hra </button></a>");
  client.println("<a href=\"/LED=2hra\"\"><button>2hra </button></a>");
  client.println("<a href=\"/LED=3hra\"\"><button>3hra </button></a>");
  client.println("<a href=\"/LED=1mas\"\"><button>1hra + </button></a><br />");
  client.println("<a href=\"/LED=2mas\"\"><button>2hra + </button></a><br />");
  client.println("<a href=\"/LED=3mas\"\"><button>3hra + </button></a><br />");
  client.println("<a href=\"/LED=ARCOIRIS\"\"><button>FIESTA</button></a><br />");  
  client.println("<a href=\"/LED=TODOOFF\"\"><button>ALL OFF </button></a><br />");  
  
  client.println("</html>"); //TERMINA EL CODIGO PHP DE LA PAGUINA WEB
 
  delay(1);
  Serial.println("Cliente Desconectado");
  Serial.println("");
 
}
