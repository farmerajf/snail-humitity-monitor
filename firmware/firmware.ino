#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

//#include <ESP8266WiFi.h>

ESP8266WebServer server(80);

DHT dht(2, DHT11);

void handleRoot();              // function prototypes for HTTP handlers
void handleNotFound();
void on();
void off();

void setup() {
  pinMode(D4, OUTPUT);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin("Wallaby", "kangaroo");
  digitalWrite(D4, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(D4, HIGH);

  MDNS.begin("esp8266");
  
  server.begin();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/json", HTTP_GET, handleJson);
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  dht.begin();
}

void loop() {
    server.handleClient(); 
}

void handleRoot() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String resp = String("<head><meta http-equiv=\"refresh\" content=\"10\"></head><center><h1>Temperature = " + String(t) + "</h1><br /><h1>Humidity = " + String(h) + "</h1></center>");
  Serial.println("received");
  server.send(200, "text/html", resp);   // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleJson() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  t = isnan(t) ? 0 : t;
  h = isnan(h) ? 0 : h;
  String resp = String("{ \"temperature\" : " + String(t) + ", \"humidity\" : " + String(h) + "}");
  Serial.println(resp);
  server.send(200, "text/json", resp);
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
