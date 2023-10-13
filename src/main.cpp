#include "HX711.h"
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
#define SERIALBAUD 9600

HX711 scale;

#define WIFI_SSID "realme C15"
#define WIFI_PASSWORD "5301420020"
#define API_KEY "AIzaSyBkcVClw4nozmVk54g0MAYhWwjUCnTlZTA"
#define DATABASE_URL "https://babat-tb-35e6b-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

const float FACTOR = 6309.49;

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  Serial.begin(SERIALBAUD);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("LoadCell Data:");

  // Pembacaan LoadCell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(FACTOR);
  scale.tare();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.reconnectNetwork(true);

  fbdo.setBSSLBufferSize(4096, 1024);

  Serial.print("Sign up new user... ");

  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
    Serial.printf("%s\n", config.signer.signupError.message.c_str());

  Firebase.begin(&config, &auth);
}

void loop()
{
  delay(1000);
  if (Firebase.ready() && signupOK)
  {
    float berat = scale.get_units();
    if (Firebase.setFloat(fbdo, "LoadCell/berat", berat))
    {
      Serial.print("Berat: ");
      Serial.println(berat);

      // Hitung jumlah obat
      int jumlahObat = berat / 1.57;

      Serial.print("Jumlah Obat: ");
      Serial.println(jumlahObat);

      // Tampilkan berat dan jumlah obat pada LCD
      lcd.setCursor(0, 1);
      lcd.print("Berat: ");
      lcd.print(berat, 2);
      lcd.print(" g");
      lcd.setCursor(0, 2);
      lcd.print("Jumlah Obat: ");
      lcd.print(jumlahObat);

      // Kirim jumlah obat ke Firebase
      if (Firebase.setInt(fbdo, "LoadCell/jumlah_obat", jumlahObat))
      {
        Serial.print("Jumlah Obat sent to Firebase: ");
        Serial.println(jumlahObat);
      }
      else
      {
        Serial.println("Failed to send jumlah obat data to Firebase");
        Serial.println("Reason: " + fbdo.errorReason());
      }
    }
    else
    {
      Serial.println("Failed to send berat data");
      Serial.println("Reason: " + fbdo.errorReason());

      // Tampilkan pesan kesalahan pada LCD
      lcd.setCursor(0, 1);
      lcd.print("Failed to send data");
    }
  }

  // Tampilkan status koneksi pada LCD
  lcd.setCursor(0, 3);
  if (WiFi.status() == WL_CONNECTED)
  {
    lcd.print("Connected       ");
  }
  else
  {
    lcd.print("Not Connected   ");
  }
}
