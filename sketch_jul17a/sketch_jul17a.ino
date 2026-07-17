#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

// =================================
// Wi-Fi
// =================================

const char* ssid = "Dnk";
const char* password = "(Dinuka0512)";


// =================================
// Device Name
// =================================

const char* hostname = "robot";


// =================================
// Pin
// =================================

const int ledPin = D1;


// =================================
// Light State
// =================================

bool lightState = false;


// =================================
// Server
// =================================

WiFiServer server(80);


// =================================
// SETUP
// =================================

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println("Program Started");


  // Setup LED

  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);

  lightState = false;


  // Connect Wi-Fi

  connectToWiFi();


  // Start mDNS

  if (MDNS.begin(hostname)) {

    Serial.println("mDNS Started");

    Serial.print("URL: http://");

    Serial.print(hostname);

    Serial.println(".local");
  }


  // Start Server

  server.begin();

  server.setNoDelay(true);

  Serial.println("Server Started");
}


// =================================
// LOOP
// =================================

void loop() {

  MDNS.update();


  WiFiClient client = server.available();


  if (!client) {

    return;
  }


  handleClient(client);
}


// =================================
// CONNECT WIFI
// =================================

void connectToWiFi() {

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);


  Serial.print("Connecting to WiFi");


  while (WiFi.status() != WL_CONNECTED) {

    delay(100);

    Serial.print(".");
  }


  Serial.println();

  Serial.println("WiFi Connected");


  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());
}


// =================================
// HANDLE CLIENT
// =================================

void handleClient(WiFiClient& client) {


  // Wait for request

  unsigned long startTime = millis();


  while (!client.available()) {


    if (millis() - startTime > 1000) {

      client.stop();

      return;
    }


    yield();
  }


  // Read first line

  String request = client.readStringUntil('\r');


  Serial.println(request);


  // Process request

  processRequest(request);


  // Send response

  sendResponse(client);


  // Close connection

  delay(5);

  client.stop();
}


// =================================
// PROCESS REQUEST
// =================================

void processRequest(String request) {


  if (request.indexOf("GET /on") >= 0) {

    turnLightOn();
  }


  else if (request.indexOf("GET /off") >= 0) {

    turnLightOff();
  }


  else if (request.indexOf("GET /status") >= 0) {

    Serial.println("Status Requested");
  }
}


// =================================
// TURN ON
// =================================

void turnLightOn() {

  digitalWrite(ledPin, HIGH);

  lightState = true;

  Serial.println("Light ON");
}


// =================================
// TURN OFF
// =================================

void turnLightOff() {

  digitalWrite(ledPin, LOW);

  lightState = false;

  Serial.println("Light OFF");
}


// =================================
// SEND RESPONSE
// =================================

void sendResponse(WiFiClient& client) {


  client.println("HTTP/1.1 200 OK");

  client.println("Content-Type: text/plain");

  client.println("Access-Control-Allow-Origin: *");

  client.println("Connection: close");

  client.println();


  if (lightState) {

    client.print("ON");
  }


  else {

    client.print("OFF");
  }
}
