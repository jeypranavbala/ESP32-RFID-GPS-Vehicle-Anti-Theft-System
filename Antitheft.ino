/*
  ESP32 Vehicle Anti-Theft System
  RFID + GPS + Email Alert

  Replace credentials below before use.
  Do NOT upload real credentials to GitHub.
*/

// ================= USER CONFIG =================
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465

#define sender_email "YOUR_EMAIL@gmail.com"
#define sender_password "YOUR_APP_PASSWORD"

#define Recipient_email "RECIPIENT_EMAIL@gmail.com"
#define Recipient_name ""
// ==============================================

#include <SPI.h>
#include <MFRC522.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

SMTPSession smtp;

// RFID and GPS Pins
#define SS_PIN 5
#define RST_PIN 4
#define BUTTON_PIN 25

MFRC522 rfid(SS_PIN, RST_PIN);
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

String authorizedTags[] = {"11 AA A1 7B", "D3 57 E7 29"};
bool ignitionOn = false;
bool validCardScanned = false;

void setup() {
    Serial.begin(115200);

    // Connect WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
    }
    Serial.println("\nWiFi connected");
    smtp.debug(1);

    // GPS + RFID
    SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
    SPI.begin();
    rfid.PCD_Init();

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Serial.println("🚗 Vehicle Anti-Theft System Ready!");
}

void loop() {
    bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

    if (buttonPressed && !ignitionOn) {
        if (validCardScanned) {
            Serial.println("🔑 Ignition ON - Authorized");
            ignitionOn = true;
        } else {
            Serial.println("🚨 ALERT! Ignition ON without RFID scan!");
            String location = getGPSLocation();
            sendEmailAlert("🚨 ALERT! Ignition ON without RFID scan!\nLocation: " + location);
            ignitionOn = true;
        }
        delay(500);
    } 
    else if (!buttonPressed && ignitionOn) {
        Serial.println("🔌 Ignition OFF");
        ignitionOn = false;
        validCardScanned = false;
        delay(500);
    }

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        String tagUID = getTagUID();
        Serial.print("🔍 RFID Detected: ");
        Serial.println(tagUID);

        if (isAuthorizedTag(tagUID)) {
            validCardScanned = true;
            Serial.println("✅ Valid Card - Ready to start");
        } else {
            Serial.println("⛔ ALERT! Unauthorized Card!");
            String location = getGPSLocation();
            sendEmailAlert("⛔ ALERT! Unauthorized Card Scanned!\nLocation: " + location);
        }
        rfid.PICC_HaltA();
    }
}

String getTagUID() {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
        uid.toUpperCase();
        if (i < rfid.uid.size - 1) uid += " ";
    }
    return uid;
}

bool isAuthorizedTag(String tagUID) {
    for (String tag : authorizedTags) {
        if (tagUID.equals(tag)) return true;
    }
    return false;
}

String getGPSLocation() {
    Serial.println("🛰️ Getting GPS location...");
    unsigned long start = millis();

    while (millis() - start < 5000) {
        while (SerialGPS.available() > 0) {
            if (gps.encode(SerialGPS.read())) {
                if (gps.location.isValid()) {
                    String loc = String(gps.location.lat(), 6) + ", " + String(gps.location.lng(), 6);
                    Serial.println("📍 Location: " + loc);
                    return loc;
                }
            }
        }
    }

    Serial.println("⚠️ GPS not available");
    return "GPS location not available";
}

void sendEmailAlert(String messageText) {
    ESP_Mail_Session session;

    session.server.host_name = SMTP_server;
    session.server.port = SMTP_Port;
    session.login.email = sender_email;
    session.login.password = sender_password;
    session.login.user_domain = "";

    SMTP_Message message;
    message.sender.name = "ESP32 Anti-Theft";
    message.sender.email = sender_email;
    message.subject = "Vehicle Alert!";
    message.addRecipient(Recipient_name, Recipient_email);
    message.text.content = messageText.c_str();
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    if (!smtp.connect(&session)) {
        Serial.println("SMTP Connection Error: " + smtp.errorReason());
        return;
    }

    if (!MailClient.sendMail(&smtp, &message)) {
        Serial.println("Email Send Error: " + smtp.errorReason());
    }
}
