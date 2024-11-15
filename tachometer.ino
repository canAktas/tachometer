#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int hallPin = A2;

int maxRPM = 200;
int goalRPM = 197;
int val = 0;
int hallSum = 0;
int sampleSize = 50;
int startupDelay = 2000;
int avgVal = 0;
int sigDiff = 50;
bool dropCheck = 0;
int preTime = 0;
int timePassed = 0;
int preVal = 0;
int RPM = -1;

String text;

int findRpm(int avgVal, int sigDiff, int sensorVal);
void displayRpms();
void drawProgressbar(int x,int y, int width,int height, int progress);

void setup() {
  Serial.begin(115200);
  digitalWrite(hallPin, HIGH);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  for(int i = 0; i < sampleSize; i++){
    hallSum += analogRead(hallPin);
    delay(startupDelay / sampleSize);
    }
    avgVal = hallSum / sampleSize;


  displayRpms();
  
}

void loop() {
  val = analogRead(hallPin);
  if(abs(val - avgVal) < 10){
    dropCheck = 1;
  }
  if(dropCheck){
    RPM = findRpm(avgVal, sigDiff, val);
    if(RPM != -1){
    displayRpms();

    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 18);
    // Display static text
    display.println(RPM);
    Serial.println(RPM);

    display.setCursor(64, 18);

    text = String(int(RPM * 241.270111708 / 1000))+"k";
    display.println(text);

    drawProgressbar(10, 46, 108, 18, RPM/(goalRPM/100));
    
    display.display(); 
  }
  
    delay(10);
  }
  
  
}

int findRpm(int avgVal, int sigDiff, int sensorVal){
  //Serial.println(sensorVal);
  if(abs(sensorVal - avgVal) > sigDiff && preVal == 0){
    dropCheck = 0;
    preVal = 1;
    timePassed = millis() - preTime;
    preTime = millis();
    delay(1000/(maxRPM/60));
    return 60 / (timePassed / 1000.0);
    
  }else if(sensorVal - avgVal > sigDiff && preVal == 1){
  }else{
    preVal = 0;
  }
  return -1;
}

void displayRpms(){
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("RPM1");
  display.setCursor(64, 0);

  display.println("RPM2");
  display.display();
}

void drawProgressbar(int x,int y, int width,int height, int progress)
{
   progress = progress > 100 ? 100 : progress; // set the progress value to 100
   progress = progress < 0 ? 0 :progress; // start the counting to 0-100
   float bar = ((float)(width-1) / 100) * progress;
   display.drawRect(x, y, width, height, WHITE);
   display.fillRect(x+2, y+2, bar , height-4, WHITE); // initailize the graphics fillRect(int x, int y, int width, int height)
}
