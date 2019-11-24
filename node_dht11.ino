#include <ESP8266WiFi.h>                                                    // esp8266 library
#include <FirebaseArduino.h>                                                // firebase library
#include <DHT.h>                                                            // dht11 temperature and humidity sensor library

#define FIREBASE_HOST "clima-c2b2a.firebaseio.com"                          // the project name address from firebase id
#define FIREBASE_AUTH "wwN8YOoWFlh2pjdRxWpU7DZXGa9Y2V88c8mAmIJs"            // the secret key generated from firebase

#define WIFI_SSID "AndroidAP3869"                                             // input your home or public wifi name 
#define WIFI_PASSWORD "inovacao2019"                                    //password of wifi ssid

#define LEDPIN D1
#define DHTPIN D5                                                           // what digital pin we're connected to
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);       
WiFiServer server(80);    
                                      

void setup() {
  Serial.begin(9600);
  delay(1000);                
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Conectando");
  Serial.print(WIFI_SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("Conectado: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                              // connect to firebase
  dht.begin();          //Start reading dht sensor
  server.begin();
  Serial.println("Servidor iniciado");
  pinMode(LEDPIN, OUTPUT);
}

void loop() { 
  digitalWrite(LEDPIN, HIGH);
  float h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  float t = dht.readTemperature();                                           // Read temperature as Celsius (the default)
    
  if (isnan(h) || isnan(t)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Falha ao ler o sensor DHT!"));
    return;
  }

  // Pisca o led.
  digitalWrite(LEDPIN, LOW); // apaga o LED
  delay(1000); // aguarda 1 segundo
  digitalWrite(LEDPIN, HIGH); // acende o LED
  delay(1000); // aguarda 1 segundo
  digitalWrite(LEDPIN, LOW); // apaga o LED
  delay(1000); // aguarda 1 segundo
  digitalWrite(LEDPIN, HIGH); // acende o LED
  
  Serial.print("Humidade: ");  Serial.print(h);
  String fireHumid = String(h) + String("%");                                         //convert integer humidity to string humidity 
  Serial.print("%  Temperatura: ");  Serial.print(t);  Serial.println("°C ");
  String fireTemp = String(t) + String("°C");                                                     //convert integer temperature to string temperature
   
  Firebase.pushFloat("/DHT11/Humidade", h);                                  //setup path and send readings
  Firebase.pushFloat("/DHT11/Temperatura", t);                                //setup path and send readings

  WiFiClient client = server.available();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Refresh: 10");  // update the page after 10 sec
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html><head>");
  client.println("<tittle>CLima - IoT</tittle>");
  client.println("<style>html { font-family: Cairo; display: block; margin: 0px auto; text-align: center;color: #333333; background-color: #ccffb3;}");
  client.println("body{margin-top: 50px;}");
  client.println("h1 {margin: 50px auto 30px; font-size: 50px; text-align: center;}");
  client.println(".side_adjust{display: inline-block;vertical-align: middle;position: relative;}");
  client.println(".text1{font-weight: 180; padding-left: 15px; font-size: 50px; width: 170px; text-align: left; color: #3498db;}");
  client.println(".data1{font-weight: 180; padding-left: 80px; font-size: 50px;color: #3498db;}");
  client.println(".text2{font-weight: 180; font-size: 50px; width: 170px; text-align: left; color: #ff6600;}");
  client.println(".data2{font-weight: 180; padding-left: 150px; font-size: 50px;color: #ff6600;}");
  client.println(".text3{font-weight: 180; padding-left: 15px; font-size: 50px; width: 170px; text-align: left; color: #0066ff;}");
  client.println(".data3{font-weight: 180; padding-left: 80px; font-size: 50px;color: #0066ff;}");
  client.println(".data{padding: 10px;}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<div id=\"webpage\">");   
  client.println("<h1>Clima - IoT</h1>");
  client.println("<div class=\"data\">");
  client.println("<div class=\"side_adjust text1\">Humididade:</div>");
  client.println("<div class=\"side_adjust data1\">");
  client.print(h);
  client.println("<div class=\"side_adjust text1\">%</div>");
  client.println("</div>");
  client.println("<div class=\"data\">");
  client.println("<div class=\"side_adjust text2\">Temperatura:</div>");
  client.println("<div class=\"side_adjust data2\">");
  client.print(t);
  client.println("</div>");
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");

  delay(60000); // espera 1 minuto
}
