#include <RTClib.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ezButton.h>

AsyncWebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define NEOPIN  12
#define NUMPIXELS 24 
#define CLOCK_INTERRUPT_PIN 2

const int LONG_PRESS_TIME  = 1000; // 1000 milliseconds
ezButton buttonUp(13);
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;

#define button 14
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;

byte WifiOnChar[] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};
byte WifiOffChar[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

int j = 0;
int dec = 0;
int bin[18];

const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

DateTime now;

int tag = 99;
int day = 0;
int hour = 0;
int minute = 0;
DateTime currentTime;
time_t epochTime;
struct tm *ptm;

double brightness = 255;
int count = 0;
bool light = true;

//unsigned long button_time = 0;  
//unsigned long last_button_time = 0; 

const char* ssid = "Test_Wlan5G";
const char* password = "hGz38jk5E";
const char* dns_name = "binarclock";

const char* newSSID = "input1";
const char* newPassword = "input2";
const char* PARAM_INPUT_3 = "input3";

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html><head>
  <meta name='apple-mobile-web-app-capable' content='yes' />
  <meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />
  </head>
  <body bgcolor = '#70706F'>
  <hr/><hr>
  <h1 style='color : #3AAA35;'><center> Binar-Clock V2.2 Control Center</center></h1>
  <hr/><hr>
  <br/><br>
  <center>

  <form action="/get">
    Clock Brightness: <input type="range" name="mySlider" value="demo">
    <button onclick="myFunction()">Try it</button>
  </form><br>

  LCD-Off
  <a href='/light1on'><button>Turn On </button></a>
  <a href='/light1off'><button>Turn Off </button></a><br/>
  <br/><br>

  Synchronize Time
  <a href='/syncTime'><button>Sync now </button></a><br/>
  <br/><br>

  <a href='/update'><button>Update Software</button></a><br/>
  <br/><br>

  <form action='/get'>
    input1: <input type='text' name='input1'>
    <input type='submit' value='Submit'>
  </form><br>
  <form action='/get'>
    input2: <input type='text' name='input2'>
    <input type='submit' value='Submit'>
  </form><br>
  <form action='/get'>
    input3: <input type='text' name='input3'>
    <input type='submit' value='Submit'>
  </form>

  </center>
  </body></html>)rawliteral";

const char* slider = "mySlider";

void switchLight()
{
  if(light)
  {
    brightness = 1;
    pixels.setBrightness(brightness);
    lcd.noBacklight();
    light = false;
  }
  else
  {
    brightness = 255;
    pixels.setBrightness(brightness);
    lcd.backlight();
    light = true;
  }

}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void showDayofWeek(int day)
{
  switch(random(6))
    {
      case 0:
        pixels.setPixelColor(17+day, pixels.Color((100), (random(0,100)), 0));
        j++;
        break;
      case 1:
        pixels.setPixelColor(17+day, pixels.Color((100), (random(0,100)), 0));
        j++;
        break;
      case 2:
        pixels.setPixelColor(17+day, pixels.Color((100), ((100+random(0,100))), 0));
        j++;
        break;
      case 3:
        pixels.setPixelColor(17+day, pixels.Color(0, (100), (random(0,100))));
        j++;
        break;
      case 4:
        pixels.setPixelColor(17+day, pixels.Color(0, (100), ((100+random(0,100)))));
        j++;
        break;
      case 5:
        pixels.setPixelColor(17+day, pixels.Color((random(0,100)), 0, (100)));
        j++;
        break;
      case 6:
        pixels.setPixelColor(17+day, pixels.Color((100+(random(0,100))), 0, 100));
        j++;
        break;
    }
}

void showTime(DateTime now)
{
  if(now.minute() != minute)
  {
        dec = now.hour(); 
    for(int i=4; i>=0; i--)
    {
      bin[i] = dec%2;
      dec = dec/2; 
      if(bin[i]==1)
      {
        pixels.setPixelColor(i, pixels.Color((100), ((20*i)), 0));
      }
      else
      {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
    }
    hour = now.hour();

    dec = now.minute(); 
    for(int i=5; i>=0; i--)
    {
      bin[i] = dec%2;
      dec = dec/2; 
      if(bin[i]==1)
      {
        pixels.setPixelColor(5+i, pixels.Color(0, (100), ((20*i))));
      }
      else
      {
        pixels.setPixelColor(5+i, pixels.Color(0, 0, 0));
      }
    }
    minute = now.minute();
  }

  dec = now.second(); 
  for(int i=5; i>=0; i--)
  {
    bin[i] = dec%2;
    dec = dec/2; 
    if(bin[i]==1)
    {
      pixels.setPixelColor(11+i, pixels.Color(((20*i)), 0, (100)));
    }
    else
    {
      pixels.setPixelColor(11+i, pixels.Color(0, 0, 0));
    }
  }
}

void events(DateTime now)
{
  //New Year//
  if(now.month()==1 & now.day() == 1 & now.hour() == 0)
  {
    
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  buttonUp.setDebounceTime(100);
  timeClient.begin();
  lcd.init(); 
  lcd.backlight();
  lcd.createChar(1, WifiOnChar);
  lcd.createChar(0, WifiOffChar);
  pixels.begin();


  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    Serial.flush();
    abort();
  }

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(1999, 10, 3, 20, 43, 0));
  currentTime = DateTime(F(__DATE__), F(__TIME__));

  rtc.disable32K();
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  //pinMode(ButtonUp, INPUT);
  //pinMode(ButtonDown, INPUT);

  //attachInterrupt(digitalPinToInterrupt(ButtonUp), switchLight, RISING);
  //attachInterrupt(digitalPinToInterrupt(ButtonDown), nextSetting, RISING);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(F("WiFi Failed!"));
    return;
  }
  Serial.println();
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP());
  Serial.print(F("MAC Address: "));
  Serial.println(WiFi.macAddress());

  if (MDNS.begin(dns_name)) {
    Serial.println("DNS gestartet, erreichbar unter: ");
    Serial.println("http://" + String(dns_name) + ".local/");
  }
  Serial.println("mDNS responder started");

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request)
  {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(slider)) {
      inputMessage = request->getParam(slider)->value();
      inputParam = slider;
      Serial.println(inputMessage);
      brightness = (inputMessage.toInt()*2.55);
      pixels.setBrightness(brightness);
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }

    request->send(200, "text/html", "<meta http-equiv='refresh' content='0; URL=http://" + WiFi.localIP().toString() + "'>");
  });

  server.on("/light1on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    lcd.backlight();
    request->send(204);
  });
  server.on("/light1off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    lcd.noBacklight();
    request->send(204);
  });

  server.on("/syncTime", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    epochTime = timeClient.getEpochTime();
    tm *ptm = gmtime ((time_t *)&epochTime);
    currentTime = DateTime((ptm->tm_year+1900), (ptm->tm_mon+1), (ptm->tm_mday) ,(timeClient.getHours()+1), timeClient.getMinutes(), timeClient.getSeconds());

    rtc.adjust(currentTime);
    request->send(204); 
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(newSSID)) {
      inputMessage = request->getParam(newSSID)->value();
      inputParam = newSSID;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(newPassword)) {
      inputMessage = request->getParam(newPassword)->value();
      inputParam = newPassword;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.onNotFound(notFound);
  AsyncElegantOTA.begin(&server);
  server.begin();

  
}

void loop()
{
  // put your main code here, to run repeatedly:
  buttonUp.loop(); // MUST call the loop() function first

  if(buttonUp.isPressed())
    pressedTime = millis();

  if(buttonUp.isReleased()) {
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if( pressDuration > LONG_PRESS_TIME )
      switchLight();
  }

  MDNS.update();
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    lcd.setCursor(15,1);
    lcd.write(byte(1));
  }
  else
  {
    lcd.setCursor(15,1);
    lcd.write(byte(0));
  }
  now = rtc.now();
  timeClient.update();

  if(now.dayOfTheWeek() != tag)
  {
    lcd.clear();
    pixels.clear();
    
    if(now.dayOfTheWeek()==0)       //Erster tag der Woche ändern
    {
      day = 6;
    }
    else
    {
      day = now.dayOfTheWeek()-1;
    }
    tag = now.dayOfTheWeek();

    //events(now);
  }
  if(now.minute() != minute)
  {
    showDayofWeek(day);             //Wochentag ausgeben
  }           
  showTime(now);                    //Zeit ausgeben
  pixels.show();
  //lcd.clear();


  lcd.setCursor(1, 1);              //Ausgabe an LCD
  char buf1[] = "hh:mm:ss";
  lcd.print(now.toString(buf1));

  lcd.setCursor(10, 1);              //Ausgabe an LCD
  lcd.print("[JMN]");

  lcd.setCursor(3, 0);
  char buf2[] = "DD/MM/YYYY    ";
  lcd.println(now.toString(buf2));

  //Evening Mode////////////////////////////////
  if(now.hour() == 22 & now.minute() == 0)
  {
    brightness = 0;
    pixels.setBrightness(brightness);
    lcd.noBacklight();
  }
  else if(now.hour() == 6 & now.minute() == 0)
  {
    brightness = 255;
    pixels.setBrightness(brightness);
    lcd.backlight();
  }
  ////////////////////////////////////////////

  delay(500);
}

////////////////////////////////////////////
///////////////////////////////////////////

