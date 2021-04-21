#define BLYNK_PRINT Serial // Librería Blynk para mostrar por consola
#include <SPI.h>
#include <ESP8266WiFi.h> // Librería del NodeMCU ESP8266
#include <BlynkSimpleEsp8266.h> // Librería de Blynk par NodeMCU ESP8266
#include <DHT.h> // Librería del sensor DHT

#define BLYNK_MAX_SENDBYTES 256 // Definimos la cantidad máxima de caracteres que podemos enviar a través de Blynk
#define DHTPIN 4          // Definimos qué PIN del NodeMCU utilizaremos para la transmisión de datos del DHT
#define DHTTYPE DHT22   // Definimos qué modelo de sensor utilizaremos, en nuestro caso: DHT22

// Blynk nos da un token para asociar a nuestro proyecto:
char auth[] = "R-gXn2PUla6cR81PYKVeuDGElAzFYszq";

int contconexion = 0;
unsigned long previousMillis = 0;

// Credenciales para la conexión WiFi:
char ssid[] = "**********";  // Nombre de red
char pass[] = "**********";  // Contraseña de red


DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer; // Timer de Blynk

float h = dht.readHumidity();
float t = dht.readTemperature(); // dht.readTemperature(true) for Fahrenheit

String strt = String(t);
String strh = String(h);


//char *host = "185.27.134.102" ;
//String strhost = "ignacioesses.com.ar";
//String strurl = "/index.php?temperatura=" + strt + "&humedad=" + strh;
//String chipid = "";

bool correoEnviado = false; // Para no enviar un correo en cada lectura

// Envío de datos a PIN Virtual Blynk e impresión por consola Arduino:
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // dht.readTemperature(true) for Fahrenheit
    
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
   
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);  // Pin V5 Humedad
  Blynk.virtualWrite(V6, t);  // Pin V6 Temperatura

  if (t >= 29.0){ // Defino el valor de temperatura para enviar alerta
    
    if(!correoEnviado){
      String body = String("La temperatura de la cámara es ") + t + ", contactar a mantenimiento para revisar estado.";
      Blynk.email("ignacioesses@gmail.com", "********** URGENTE **********", body);
      Blynk.notify("ESP8266 Alert");
      correoEnviado = true;
      //Serial.println(correoEnviado); Para testear envío de correo.
  }
  }else{
    correoEnviado = false;
    //Serial.println(correoEnviado); Para testear envío de correo.
  }
}

//-------Función para Enviar Datos a la Base de Datos SQL--------

String enviardatos(String datos) {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  String strt = String(t);
  String strh = String(h);


  char *host = "185.27.134.102" ;
  String strhost = "ignacioesses.com.ar";
  String strurl = "/index.php?temperatura=" + strt + "&humedad=" + strh;
  String linea = "error";

  Serial.println(strhost + strurl);
  
  WiFiClient client;
  //strhost.toCharArray(host, 49);
  if (!client.connect(host, 80)) {
    Serial.println("Fallo de conexion");
    return linea;
  }

  client.print(String("GET ") + strurl + " HTTP/1.1" + "\r\n" + 
               "Host: " + strhost + "\r\n" +
               "Connection: keep-alive" + "\r\n" + 
               "Content-Length: " + datos.length() + "\r\n" +
               "Cache-Control: max-age=0" + "\r\n" + 
               "Origin: http://ignacioesses.com.ar" + "\r\n" + 
               "Upgrade-Insecure-Requests: 1" + "\r\n" + 
               "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.128 Safari/537.36" + "\r\n" + 
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" + 
               "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9" + "\r\n" + 
               "Referer: http://ignacioesses.com.ar/?i=1" + "\r\n" + 
               //"Accept-Encoding: gzip, deflate" + "\r\n" + 
               "Accept-Language: es-419,es;q=0.8" + "\r\n" + 
               "Cookie: _ga=GA1.3.975146639.1618700459; _gid=GA1.3.1085058457.1618700459; __test=ab85901b747734af2214530d9fd7f6d0" + "\r\n" +             
               "\r\n" + datos);           
  delay(10);             
  
  Serial.println("Enviando datos a SQL...");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Cliente fuera de tiempo!");
      client.stop();
      return linea;
    }
  }
  // Lee todas las lineas que recibe del servidor y las imprime por la terminal serial
  while(client.available()){
    linea = client.readStringUntil('\r');
    // Habilitar si quiero ver toda la respuesta del servidor:
    //Serial.print(linea);
  }  
  return linea;
}

void setup()
{
  Serial.begin(9600); // Estado de conexión en consola
  Blynk.begin(auth, ssid, pass); // Inicio Blynk

  dht.begin(); // Inicio sensor DHT

  timer.setInterval(10000, sendSensor); // Envía los datos cada 10 segundos - BLYNK

// Conexión WIFI
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  
}

// Ejecución contínua en función loop:
void loop()
{  
  Blynk.run(); // Inicialización de Blynk
  timer.run(); // Inicialización del timer

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 600000) { // Envía los datos cada 10 minutos - HOST
    previousMillis = currentMillis;
    //int analog = analogRead(17);
    //float temp = analog*0.322265625;
    //Serial.println(temp);
    enviardatos("temperatura=" + String (strt) + "&humedad=" + String(strh));
  }
}