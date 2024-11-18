#define BLYNK_TEMPLATE_ID "TMPL67-QqRfIR"
#define BLYNK_TEMPLATE_NAME "SENSOR"
#define BLYNK_AUTH_TOKEN "lsuwL5A1BMJ4jn6D1Wd9FYjaTRjAyyzv"

#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// Definisi tipe sensor DHT22
#define DHTTYPE DHT22  

// Inisialisasi sensor DHT pada GPIO 4
DHT dht(4, DHTTYPE);

// Pin untuk Servo dan Buzzer
#define BUZZER_PIN 19      // Pin Buzzer (GPIO 19)
#define SERVO_PIN 15       // Pin Servo Motor (GPIO 15)

// Pin untuk Ultrasonik HC-SR04
#define TRIG_PIN 5         // Pin Trig (GPIO 5)
#define ECHO_PIN 18        // Pin Echo (GPIO 18)

// Konfigurasi WiFi dan Blynk
char auth[] = "lsuwL5A1BMJ4jn6D1Wd9FYjaTRjAyyzv"; // Masukkan Auth Token dari aplikasi Blynk
char ssid[] = "Wokwi-GUEST";     // SSID yang benar
char pass[] = "";                // Tidak perlu password

// Inisialisasi Servo Motor
Servo servoMotor;

// Variabel untuk kontrol Blynk
bool buzzerState = false; // Status Buzzer
int servoAngle = 0;       // Sudut Servo Motor

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Inisialisasi DHT22
  dht.begin();
  
  // Inisialisasi pin untuk HC-SR04 dan Buzzer
  pinMode(TRIG_PIN, OUTPUT);  // GPIO 5 sebagai Trig
  pinMode(ECHO_PIN, INPUT);   // GPIO 18 sebagai Echo
  pinMode(BUZZER_PIN, OUTPUT);// GPIO 19 untuk Buzzer

  // Attach Servo Motor
  servoMotor.attach(SERVO_PIN);

  // Inisialisasi Blynk
  Blynk.begin(auth, ssid, pass);

  // Tampilkan pesan siap
  Serial.println("Rangkaian Siap!");
}

void loop() {
  // Membaca data dari sensor DHT22
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  
  // Membaca jarak menggunakan HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long durasi = pulseIn(ECHO_PIN, HIGH);
  float jarak = durasi * 0.034 / 2;

  // Kirim data ke Blynk
  Blynk.virtualWrite(V1, suhu);       // Kirim data suhu ke Virtual Pin V1
  Blynk.virtualWrite(V2, kelembapan); // Kirim data kelembapan ke Virtual Pin V2
  Blynk.virtualWrite(V3, jarak);      // Kirim data jarak ke Virtual Pin V3

  // Menampilkan data ke Serial Monitor
  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" °C, Kelembapan: ");
  Serial.print(kelembapan);
  Serial.print(" %, Jarak: ");
  Serial.print(jarak);
  Serial.println(" cm");

  // Kontrol Buzzer otomatis berdasarkan suhu
  if (suhu > 30) {
    tone(BUZZER_PIN, 1000);  // Buzzer berbunyi pada 1000 Hz
    Serial.println("Buzzer menyala karena suhu > 30°C.");
  } else {
    noTone(BUZZER_PIN);      // Matikan Buzzer
    Serial.println("Buzzer mati karena suhu <= 30°C.");
  }

  // Kontrol Servo berdasarkan jarak
  if (jarak > 50) {
    servoMotor.write(180);  // Servo bergerak ke 180 derajat jika jarak > 50 cm
    Serial.println("Servo bergerak ke 180 derajat (jarak > 50 cm).");
  } else {
    servoMotor.write(0);    // Servo bergerak ke 0 derajat jika jarak <= 50 cm
    Serial.println("Servo bergerak ke 0 derajat (jarak <= 50 cm).");
  }

  // Jalankan fungsi Blynk
  Blynk.run();
}

// Fungsi untuk kontrol Buzzer dari aplikasi Blynk
BLYNK_WRITE(V5) {
  buzzerState = param.asInt(); // Membaca nilai dari tombol Blynk (0 atau 1)
  if (buzzerState) {
    tone(BUZZER_PIN, 1000);  // Buzzer berbunyi pada 1000 Hz
    Serial.println("Buzzer dinyalakan (manual).");
  } else {
    noTone(BUZZER_PIN);      // Matikan Buzzer
    Serial.println("Buzzer dimatikan (manual).");
  }
}

// Fungsi untuk kontrol Servo Motor dari aplikasi Blynk
BLYNK_WRITE(V4) {
  servoAngle = param.asInt(); // Membaca nilai dari slider Blynk (0-180)
  servoMotor.write(servoAngle); // Atur sudut Servo
  Serial.print("Servo bergerak ke sudut: ");
  Serial.println(servoAngle);
}
