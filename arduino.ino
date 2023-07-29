#include "WiFi.h"
#include "HackPublisher.h"
#include <Wire.h>
#include <NewPing.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AM232X.h"
#include <Adafruit_NeoPixel.h>

AM232X AM2320;

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define MQ135_PIN A4
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define YELLOW_THRESHOLD 300
#define RED_THRESHOLD 1500
#define PIN 19
#define NUMPIXELS 12

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 250;
const int trigPin = 4;
const int echoPin = 5;
const int buzzer = 18;
NewPing sonar(trigPin, echoPin);
HackPublisher publisher("theCircus");
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  WiFi.begin("ASUS-F8", "K33pi7$@f3%");
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print('.');
    delay(1000);
  }
  publisher.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  if (! AM2320.begin() )
  {
    Serial.println("Sensor not found");
    while (1);
  }
  AM2320.wakeUp();
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  Wire.begin();
  pinMode(buzzer, OUTPUT);
  pixels.begin();
}

void loop() {
  int mq135_value = analogRead(MQ135_PIN);  // read the value from the MQ135
  //Display the value from the MQ135 sensor and appropriate warning
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Air Quality:");
  oled.setCursor(0,10);
  oled.println(mq135_value);
  if(mq135_value > RED_THRESHOLD) {
    oled.setCursor(0, 20);
    oled.println("WARNING: HIGH GAS LVL");
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
  } else if(mq135_value > YELLOW_THRESHOLD) {
    oled.setCursor(0, 20);
    oled.println("MODERATE GAS LEVEL");
  } else {
    oled.setCursor(0, 20);
    oled.println("Air quality is good");
  }


  int distance = sonar.ping_cm();
  oled.setCursor(0, 30);
  oled.print("Distance: ");
  oled.print(distance);
  oled.println(" cm");

int status = AM2320.read();
oled.setCursor(0, 40);
  float humid = AM2320.getHumidity();
  float temperature = AM2320.getTemperature();
  if(humid < 45){
    for(int i = 0; i < NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    }
  if(humid > 70){
    for(int i = 0; i < NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
  }
  }else{
    for(int i = 0; i < NUMPIXELS; i++){
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    }
  }
  pixels.show();
  oled.print("Humidity(%): ");
  oled.print(humid,1);
  oled.setCursor(0, 50);
  oled.print("Temperature(C): ");
  oled.print(temperature,1);


  publisher.store("ultrasonic", distance);
  publisher.store("temperature", (int)temperature);
  publisher.store("airquality", mq135_value);
  publisher.store("humidity", (int)humid);
  publisher.send();
  oled.display();
  delay(1000);
  // put your main code here, to run repeatedly:

}
