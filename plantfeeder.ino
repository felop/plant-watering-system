#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define EAU_HEIGHT   30
#define EAU_WIDTH    19

#define SOLEIL_HEIGHT   15
#define SOLEIL_WIDTH    33

int centerBarX = 55;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float lightning = 0;
float moistureLimit  = 0;
float maxWater  = 0;

int   moisture = 0;

float hour = 0;
int   offsetVal = 0;

const unsigned char bitmap_eau    [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 
  0xe0, 0x00, 0x00, 0xe0, 0x00, 0x01, 0xf0, 0x00, 0x03, 0xf8, 0x00, 0x03, 0xf8, 0x00, 0x07, 0xfc, 
  0x00, 0x0f, 0xfe, 0x00, 0x0f, 0xfe, 0x00, 0x1f, 0xff, 0x00, 0x1f, 0xff, 0x00, 0x37, 0xff, 0x80, 
  0x27, 0xff, 0x80, 0x27, 0xff, 0x80, 0x27, 0xff, 0xc0, 0x27, 0xff, 0xc0, 0x27, 0xff, 0x80, 0x33, 
  0xff, 0x80, 0x33, 0xff, 0x80, 0x1f, 0xff, 0x00, 0x0f, 0xfe, 0x00, 0x07, 0xfc, 0x00, 0x00, 0xf0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char bitmap_soleil [] PROGMEM = {
  0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x03, 
  0x3f, 0xff, 0x20, 0x00, 0x07, 0x3f, 0xfe, 0x38, 0x00, 0x1e, 0x1f, 0xfc, 0x1c, 0x00, 0x18, 0x07, 
  0xf0, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x60, 0x83, 
  0x00, 0x00, 0x00, 0x60, 0xc3, 0x00, 0x00, 0x00, 0xc0, 0xc1, 0x80, 0x00, 0x00, 0xc0, 0xc0, 0x80, 
  0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00
};

void setup() {
  Serial.begin(9600);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.display();
  display.clearDisplay();
}

void loop() {
  moisture = 100-analogRead(A3)/10.24;
  
  lightning = round(analogRead(0)/10.24)/100.0;
  moistureLimit  = round(analogRead(1)/102.4)/10.0;
  maxWater  = round(analogRead(2)/102.4/2.0)/10.0;
  hour = lightning*24;

  Serial.println(moisture);
  displayData(lightning,moistureLimit,maxWater);
  delay(20);
}








void displayData(float lightning, float moistureLimit, float maxWater) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);


  // Séparations
  display.drawLine(centerBarX,SCREEN_HEIGHT/2,SCREEN_WIDTH,SCREEN_HEIGHT/2,SSD1306_WHITE);
  display.drawLine(centerBarX,0,centerBarX,SCREEN_HEIGHT,SSD1306_WHITE);

  // 1 : Éclairage
  display.drawBitmap((centerBarX-SOLEIL_WIDTH)/2,0,
    bitmap_soleil, SOLEIL_WIDTH, SOLEIL_HEIGHT, 1);

  offsetVal = (1-round((round(lightning*24)-9.5)/29+0.5)) * 7;

  display.setTextSize(2);
  display.setCursor(9+offsetVal,25);
  display.println(String(round(lightning*24))+"h");

  display.setTextSize(1);
  display.setCursor(20,50);
  display.println(String(round(lightning*100))+"%");

  // 2 : taux d'humidité
  display.drawBitmap(centerBarX+4,0,
    bitmap_eau, EAU_WIDTH, EAU_HEIGHT, 1);

  display.setTextSize(2);
  display.setCursor(centerBarX+EAU_WIDTH+12,9);
  display.println(String(round(min(moistureLimit*100,90)))+"%");

  // 3 : arrosage max
  display.drawBitmap(centerBarX+4,SCREEN_HEIGHT-EAU_HEIGHT,
    bitmap_eau, EAU_WIDTH, EAU_HEIGHT, 1);

  display.setTextSize(2);
  display.setCursor(centerBarX+EAU_WIDTH+12,SCREEN_HEIGHT-23);
  display.println("."+String(round(maxWater*10))+"L");
  
  display.display();
}
