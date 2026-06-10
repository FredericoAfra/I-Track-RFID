/**********************************************************
* @file         esp32.ino
* @board        ESP32-S3 Dev Kit
* @components   MFRC522 RFID Reader + Firebase Integration
* @author       FredericoAfra
**********************************************************/

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPI.h>
#include <MFRC522.h>
#include "secrets.h"

// --- WI-FI CONFIGURATION ---
const char* WIFI_SSID = SECRET_SSID;
const char* WIFI_PASSWORD = SECRET_PASS;

// --- FIREBASE CONFIGURATION ---
const char* API_KEY = SECRET_API;
const char* DATABASE_URL = SECRET_URL;

// --- FIREBASE USER CREDENTIALS ---
const char* USER_EMAIL = SECRET_EMAIL;
const char* USER_PASSWORD = SECRET_UPASS;

// --- RFID RC522 PINS ---
#define SS_PIN  5
#define RST_PIN 21

// --- SERIAL COMMUNICATION WITH ARDUINO ---
#define RX2 16
#define TX2 17

const String SCAN_POINT = "armazem";

MFRC522 rfid(SS_PIN, RST_PIN);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Maps the servo position to the corresponding conveyor belt name
String positionToName(int position) {
  if (position == 1) return "conveyor 2";
  if (position == 2) return "conveyor 1";
  if (position == 3) return "dispatch";
  return "unknown";
}

void setup() {
  Serial.begin(115200);                      // PC Serial Monitor
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2); // Serial communication for Arduino Uno

  // Initialize SPI and RFID modules
  SPI.begin();
  rfid.PCD_Init();

  // Connect to Wi-Fi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected successfully!");

  // Configure Firebase Parameters
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  // Configure User Authentication
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  Serial.println("ESP32 Ready.");
}

void loop() {
  // --- RFID READING AND PROCESSING ---
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uidString = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      uidString += String(rfid.uid.uidByte[i], HEX);
    }
    uidString.toUpperCase();
    
    Serial.println("\n----------------------------------");
    Serial.println("RFID Tag read: " + uidString);

    // DEFAULT POSITION SETTING
    // If the tag is not registered on Firebase, the servo will adopt this fallback value:
    int servoPosition = 3; // Change to 1, 2, or 3 to set your factory default
    
    // Builds the dynamic path pointing directly to the tag value: /tags/TAG_VALUE/servo
    String tagPath = "/tags/" + uidString + "/servo";

    // VERIFY IF FIREBASE IS READY AND AUTHENTICATED BEFORE ACTION
    if (Firebase.ready()) {
      Serial.print("Sending last read tag to Firebase... ");
      // 1. Sends the current tag to the history log
      if (Firebase.RTDB.setString(&fbdo, "/rfid/last_tag", uidString)) {
        Serial.println("OK");
      } else {
        Serial.println("Failed: " + fbdo.errorReason());
      }

      Serial.print("Fetching configuration from Firebase (" + tagPath + ")... ");

      // 2. Queries the corresponding tag node
      if (Firebase.RTDB.getInt(&fbdo, tagPath)) {
        if (fbdo.dataType() == "int") {
          servoPosition = fbdo.intData();
          Serial.println("Registered! Position found: " + String(servoPosition));
        } else {
          Serial.println("Node found, but data is not an integer. Keeping default: " + String(servoPosition));
        }
      } else {
        // Displays the exact reason if the query fails or node doesn't exist
        Serial.println("Not registered or connection error. Reason: " + fbdo.errorReason());
        Serial.println("Applying default position: " + String(servoPosition));
      }

      // 3. Log the scan history
      Serial.print("Sending scan log to Firebase... ");
      FirebaseJson jsonScan;
      jsonScan.set("tag_id", uidString);
      jsonScan.set("point", SCAN_POINT);
      jsonScan.set("destination", positionToName(servoPosition));
      jsonScan.set("timestamp/.sv", "timestamp");
      if (Firebase.RTDB.pushJSON(&fbdo, "/scans", &jsonScan)) {
        Serial.println("OK");
      } else {
        Serial.println("Failed: " + fbdo.errorReason());
      }

      // 4. Update asset location node
      Serial.print("Updating asset location on Firebase... ");
      FirebaseJson jsonUpdate;
      jsonUpdate.set("position", SCAN_POINT);
      jsonUpdate.set("last_read/.sv", "timestamp");
      if (Firebase.RTDB.updateNode(&fbdo, "/tags/" + uidString, &jsonUpdate)) {
        Serial.println("OK");
      } else {
        Serial.println("Failed: " + fbdo.errorReason());
      }
    } else {
      Serial.println("ERROR: Firebase is not ready or SSL connection is still being established.");
      Serial.println("Applying default position: " + String(servoPosition));
    }

    // --- VALIDATION AND COMMAND TRANSMISSION TO ARDUINO UNO ---
    if (servoPosition >= 1 && servoPosition <= 3) {
      char command = '0' + servoPosition; // Converts int (1, 2, or 3) to char ('1', '2', or '3')
      Serial2.print(command); 
      Serial.print("Command sent to Arduino: ");
      Serial.println(command);
    } else {
      Serial.println("Warning: Invalid position ignored (must be between 1 and 3)");
    }
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  delay(200); // Small delay to maintain loop stability
}
