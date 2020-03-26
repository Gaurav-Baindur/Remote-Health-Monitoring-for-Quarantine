#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ADXL345.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include<SPI.h>
#include<SD.h>


int CS=15;
#define OLED_RESET 3    // 4 for Arduino Uno | 3 for NodeMCU
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

String apiKey = "G7JPCRLOV822HXKD";    
ADXL345 adxl;
const char *ssid =  "Katakuri";    
const char *pass =  "GAUrav4$";
const char* server = "api.thingspeak.com";

int temp= 0;
float chills= 0.00;

WiFiClient client;
 
void setup() 
{
  Serial.begin(115200);
  delay(10);
  adxl.powerOn();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(22,12);
  display.println("Angel Hack 2019");
  display.display();
  delay(500);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
    Serial.println("WiFi connected");
    pinMode(A0,INPUT);
    pinMode(5,INPUT);
    adxl.setActivityThreshold(75); //62.5mg per increment
    adxl.setInactivityThreshold(75); //62.5mg per increment
    adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
    //look of activity movement on this axes - 1 == on; 0 == off 
    adxl.setActivityX(1);
    adxl.setActivityY(1);
    adxl.setActivityZ(1);
 
    //look of inactivity movement on this axes - 1 == on; 0 == off
   adxl.setInactivityX(1);
   adxl.setInactivityY(1);
    adxl.setInactivityZ(1);
 
    //look of tap movement on this axes - 1 == on; 0 == off
    adxl.setTapDetectionOnX(0);
    adxl.setTapDetectionOnY(0);
    adxl.setTapDetectionOnZ(1);
 
   //set values for what is a tap, and what is a double tap (0-255)
   adxl.setTapThreshold(50); //62.5mg per increment
   adxl.setTapDuration(15); //625us per increment
   adxl.setDoubleTapLatency(80); //1.25ms per increment
   adxl.setDoubleTapWindow(200); //1.25ms per increment
  
   //set values for what is considered freefall (0-255)
   adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
   adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
   //setting all interrupts to take place on int pin 1
   //I had issues with int pin 2, was unable to reset it
   adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
   adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
   adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
   adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
   adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
   //register interrupt actions - 1 == on; 0 == off  
   adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
   adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
   adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
   adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
   adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
   yield();
   pinMode(CS,OUTPUT);
   if(!SD.begin(CS))
   {
    Serial.println("SD card not ready");
    return;
   }
  else
  {
    Serial.println("SD card is ready");
  }
  File dataFile=SD.open("CodieCon.txt",FILE_WRITE);
  String dataString="Body Temperature Chills Factor";
  if(dataFile)
  {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  else
  Serial.println("No file made");
  delay(1000);
}
 
void loop() 
{
  int val = analogRead(A0);
  temp = val*500/1023;
  int x,y,z;  
  adxl.readXYZ(&x, &y, &z); 
  Serial.println(temp);
  
  double xyz[3];
  double ax,ay,az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  Serial.print("X=");
  Serial.print(ax);
  Serial.println(" g");
  Serial.print("Y=");
  Serial.print(ay);
  Serial.println(" g");
  Serial.print("Z=");
  Serial.print(az);
  Serial.println(" g");
  Serial.println("**********************");
  delay(1000);
  chills=sqrt(pow(ax,2)+pow(ay,2));
  Serial.println(chills);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Body Temperature" );
  display.display();
  delay(500);
  display.println(temp);
  display.display();
  delay(1000);
  display.println("Chills Factor");
  display.display();
  display.println(chills);
  display.display(); 
  if (client.connect(server,80))   //   184.106.153.149
  {  
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(temp);
    postStr +="&field3=";
    postStr += String(chills);
    postStr += "\r\n\r\n";   
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" degrees Celcius, Chills Factor: ");
    Serial.print(chills);
    Serial.println("%. Send to Thingspeak.");          
  }
    client.stop();
    Serial.println("Waiting...");
    delay(10000);
    File dataFile=SD.open("CodieCon.txt",FILE_WRITE);
    String dataString=String(temp)+","+String(chills);
    if(dataFile)
    {
      dataFile.println(dataString);
      dataFile.close();
      Serial.println(dataString);
    }
    else
    Serial.println("No file made");
    delay(1000);                                           
}

