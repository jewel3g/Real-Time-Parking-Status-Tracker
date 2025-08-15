#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// WiFi credentials
#define WIFI_SSID "Anayet ullah"
#define WIFI_PASSWORD "12342345"

// Firebase
#define FIREBASE_HOST "diu-152332654.firebaseio.com"
#define FIREBASE_AUTH "ritDfQAuAIGV62V9TBaMzfvQ2JS3ZIBWIsD2hTMc"

// Parking slot pins
const int s1Pin = 14; // D5
const int s2Pin = 12; // D6
const int s3Pin = 13; // D7

int s1, s2, s3;

void setup() {
  pinMode(s1Pin, INPUT);
  pinMode(s2Pin, INPUT);
  pinMode(s3Pin, INPUT);

  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Database Test");
  delay(1000);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1000);

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Upload parking status to Firebase
void uploadStatus() {
  String status = "";

  // Read slot values
  s1 = digitalRead(s1Pin);
  s2 = digitalRead(s2Pin);
  s3 = digitalRead(s3Pin);

  if (s1 && s2 && s3) status = "HouseFull";
  else if (s1 && s2 && !s3) status = "Slot 1 & 2 Full, 3 free";
  else if (s1 && !s2 && s3) status = "Slot 1 & 3 Full, 2 free";
  else if (!s1 && s2 && s3) status = "Slot 2 & 3 Full, 1 free";
  else if (s1 && !s2 && !s3) status = "Slot 1 Full, 2 & 3 free";
  else if (!s1 && s2 && !s3) status = "Slot 2 Full, 1 & 3 free";
  else if (!s1 && !s2 && s3) status = "Slot 3 Full, 1 & 2 free";
  else status = "All slots free";

  // Update Firebase
  Firebase.setString("Status", status);

  if (Firebase.failed()) {
    Serial.print("Firebase update failed: ");
    Serial.println(Firebase.error());
    lcd.setCursor(0, 1);
    lcd.print("Error updating DB");
    delay(1000);
    return;
  }

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parking Status:");
  lcd.setCursor(0, 1);
  lcd.print(status);
  delay(1000);
}

void loop() {
  uploadStatus();
  delay(2000); // Update every 2 seconds
}
