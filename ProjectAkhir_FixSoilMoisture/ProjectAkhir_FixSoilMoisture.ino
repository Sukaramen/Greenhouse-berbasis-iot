#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6w3bPeD46"
#define BLYNK_TEMPLATE_NAME "testing"
#define BLYNK_AUTH_TOKEN "_CO-rudTA30GljQspMFturVkK029qmO6"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h> //Library untuk DHT

char ssid[] = "xxxxx"; //nama hotspot yang digunakan
char pass[] = "xxxxx"; //password hotspot yang digunakan

#define DHTPIN D3 //deklarasi pin D3 untuk output dari DHT11
#define DHTTYPE DHT11 //Tipe DHT11
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int sensor_pin = A0; // Soil Sensor input at Analog PIN A0
int output_value ;
int kelembapan_tanah ;

#define relay1 D0     //RELAY 1
#define relay2 D5     //RELAY 2
#define relay3 D6     //RELAY 3

#include <Servo.h>
Servo servo1;
int pinHujan = D7;
int pinServo = D4;
int angle = 10;
int modeButton;
int hujan;
float t;
float h;

//kendali Relay 1 LAMPU
BLYNK_WRITE(V6)
{
  bool pinValue = param.asInt();
  if (pinValue == 1){
    digitalWrite(relay1, HIGH);
    Serial.println("Relay 1 menyala");
  } else {
    digitalWrite(relay1, LOW);
  }
}

//kendali Relay 2 POMPA
BLYNK_WRITE(V5)
{
  bool pinValue1 = param.asInt();
  if (pinValue1 == 1){
    digitalWrite(relay2, LOW);
    Serial.println("Relay 2 menyala");
  } else {
    digitalWrite(relay2, HIGH);
  }
}

//kendali Relay 3 KIPAS
BLYNK_WRITE(V4)
{
  bool pinValue2 = param.asInt();
  if (pinValue2 == 1){
    digitalWrite(relay3, HIGH);
    Serial.println("Relay 3 menyala");
  } else {
    digitalWrite(relay3, LOW);
  }
}

//kendali Atap
BLYNK_WRITE(V3)
{
  bool pinValue3 = param.asInt();
  if(pinValue3 == 1){
    servo1.write(180);
  } else {
    servo1.write(0);
  }
}

BLYNK_WRITE(V8){ //MANUAL MODE OR AUTOMATIC MODE//
  modeButton = param.asInt();
}
//function untuk pengiriman sensor
void sendDht()
{
  t = dht.readTemperature(); //pembacaan sensor
  h = dht.readHumidity();

  //menampilkan temperature pada Serial monitor
  Serial.print("% T: ");
  Serial.print(t); Serial.print("C     ");
  Serial.print("% H: ");
  Serial.print(h); Serial.println("% ");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t); lcd.print("C");
  Blynk.virtualWrite(V2, t);

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(h); lcd.print("%");
  Blynk.virtualWrite(V1, h);
  delay(2000);
  lcd.clear();
}

void sendMoisture() {
  output_value = analogRead(sensor_pin);
  kelembapan_tanah = (100-((output_value/1023.00)*100));
  lcd.setCursor(0, 0);
  lcd.print("phTanah: ");
  lcd.print(kelembapan_tanah);
  lcd.println("%");
  Serial.print("% H: ");
  Serial.print(kelembapan_tanah); Serial.println("% "); Serial.println("");
  Blynk.virtualWrite(V0, kelembapan_tanah);  
  
}

void sendRain() {
  hujan = digitalRead(pinHujan);
  lcd.setCursor(0, 1);
  lcd.println("Cuaca : ");
  if (hujan == 0) {
    Blynk.virtualWrite(V7, "Hujan");
    lcd.println("Hujan");
  }
  else if (hujan == 1) {
    Blynk.virtualWrite(V7, "Cerah");
    lcd.println("Cerah");
  }
  delay(2000);
  lcd.clear();
}

void setup()
{
  // Debug console
  Serial.begin(115200); //serial monitor menggunakan bautrate 9600
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); //memulai Blynk
  dht.begin(); //mengaktifkan DHT11
  timer.setInterval(1000L, sendDht); //Mengaktifkan timer untuk pengiriman data 1000ms
  servo1.attach(pinServo);
  servo1.write(angle);
  pinMode(sensor_pin, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("PROJEK AKHIR IoT");
  lcd.setCursor(0, 1);
  lcd.print("SMART GREENHOUSE");
  delay(3000);
  lcd.clear();
}

void loop()
{
  Blynk.run(); //menjalankan blynk
  timer.run(); //menjalankan timer
  sendMoisture();
  sendRain();
  if(modeButton == 0){
    Serial.println("Mode Masuk di Manual");
    return;
  } else if(modeButton == 1){
    Serial.println("Mode Masuk di Automatic");
  }
  otomasi();
}

void otomasi(){
  Serial.println("Masuk Mode Otomatis!");
  //Soil Moisture
  if (kelembapan_tanah < 20) {
    digitalWrite(relay2, LOW);
  }
  else {
    digitalWrite(relay2, HIGH);
  }
  //Otomasi Deteksi Hujan
  if (hujan == 0) {
    // servo menutup
    servo1.write(180);
  }else if (hujan == 1){
    // servo membuka
    servo1.write(0);
  }
  //Otomasi Kipas
  if (t > 27) {
    digitalWrite(relay3, HIGH);
    //digitalWrite(relay1, LOW);
    //lcd.print("Kipas ON, Lampu OFF");
  }
  else {
    digitalWrite(relay3, LOW);
    //digitalWrite(relay3, HIGH);
    //lcd.print("Kipas OFF, Lampu ON");
  }
  //delay(2000);
  //lcd.clear();
}
