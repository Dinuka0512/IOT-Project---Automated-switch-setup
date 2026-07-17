#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

//--WiFi
#define WIFI_SSID "Dnk"
#define WIFI_PASSWORD "(Dinuka0512)"

//--Firebase
#define API_KEY "AIzaSyB8YbWMEOLP3T9oqNibQOx3hXhcmXmzn9A"
#define DATABASE_URL "https://led-automated-switch-default-rtdb.firebaseio.com/"

//--Firebase Login
#define USER_EMAIL "Dinuka0512@gmail.com"
#define USER_PASSWORD "121212"

//--Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//--Hardware
const uint8_t lightPin = D1;

//--State
bool currentLightState = false;
bool lastLightState = false;

void setup() {
  Serial.begin(115200);

  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);

  connectToWiFi();
  setupFirebase();

  Serial.println("System Ready");
}

void loop() {
  if (!Firebase.ready()) {
    Serial.println("Firebase Not Ready");
    delay(1000);
    return;
  }

  readLightState();
  delay(5);
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase Started");
}

void readLightState() {
  if (Firebase.RTDB.getBool(&fbdo, "/devices/light")) {
    currentLightState = fbdo.boolData();

    if (currentLightState != lastLightState) {
      updateLight(currentLightState);
      lastLightState = currentLightState;
    }
  } else {
    Serial.print("Firebase Error: ");
    Serial.println(fbdo.errorReason());
  }
}

void updateLight(bool state) {
  digitalWrite(lightPin, state ? HIGH : LOW);

  Serial.print("Light: ");
  Serial.println(state ? "ON" : "OFF");
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
