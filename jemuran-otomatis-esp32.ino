#include <Stepper.h>

// Pin DO sensor hujan dan pin input modul ULN2003.
constexpr int pinHujan = 14;
constexpr int pinIN1 = 26;
constexpr int pinIN2 = 25;
constexpr int pinIN3 = 33;
constexpr int pinIN4 = 32;

// Sesuaikan jumlah langkah dengan panjang tarikan tali.
constexpr int langkahPerPutaran = 2048;
constexpr int langkahPerGerakan = langkahPerPutaran * 2;
constexpr int kecepatanRPM = 10;

// Uji dari sisi poros. Jika arah masuk terbalik, ubah nilai ini ke 1.
constexpr int arahMasuk = -1;
constexpr unsigned long waktuStabilMs = 500;

// Urutan argumen Stepper untuk rangkaian ini adalah IN1, IN3, IN2, IN4.
Stepper motor(langkahPerPutaran, pinIN1, pinIN3, pinIN2, pinIN4);

// Saat dinyalakan atau direset, kode mengasumsikan jemuran ada di luar.
bool jemuranDiLuar = true;
int bacaanTerakhir;
unsigned long waktuPerubahan;

void setup() {
  Serial.begin(115200);
  pinMode(pinHujan, INPUT);
  motor.setSpeed(kecepatanRPM);

  // Mulai menghitung waktu stabil dari pembacaan pertama.
  bacaanTerakhir = digitalRead(pinHujan);
  waktuPerubahan = millis();
}

void loop() {
  const int bacaan = digitalRead(pinHujan);

  // Ulangi hitungan waktu setiap kali nilai DO berubah.
  if (bacaan != bacaanTerakhir) {
    bacaanTerakhir = bacaan;
    waktuPerubahan = millis();
  }

  // Tunggu sampai bacaan bertahan selama minimal 500 ms.
  if (millis() - waktuPerubahan < waktuStabilMs) {
    return;
  }

  // LOW berarti basah. Tarik masuk jika jemuran masih di luar.
  if (bacaan == LOW && jemuranDiLuar) {
    Serial.println("Sensor basah. Menarik jemuran masuk.");
    motor.step(arahMasuk * langkahPerGerakan);
    jemuranDiLuar = false;
  }
  // HIGH berarti kering. Keluarkan jemuran dengan arah sebaliknya.
  else if (bacaan == HIGH && !jemuranDiLuar) {
    Serial.println("Sensor kering. Mengeluarkan jemuran.");
    motor.step(-arahMasuk * langkahPerGerakan);
    jemuranDiLuar = true;
  }
}
