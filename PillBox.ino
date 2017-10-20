#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <NeoPixelBus.h>

/////////////////////////
// Network Definitions //
/////////////////////////
const IPAddress AP_IP(192, 168, 1, 1);
const char* AP_SSID = "PillBox";
boolean SETUP_MODE;
String SSID_LIST;
DNSServer DNS_SERVER;
ESP8266WebServer WEB_SERVER(80);

/////////////////////////
// Device Definitions //
/////////////////////////
String DEVICE_TITLE = "PillBox";

const uint16_t PixelCount = 28; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor red(255, 0, 0);
RgbColor green(0, 255, 0);
RgbColor blue(0, 0, 255);
RgbColor yellow(255, 255, 0);
RgbColor black(0, 0, 0);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "9b8a99fed224476e9121ffbb1e273414";
BlynkTimer timer;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "CuriosityGym-BCK";
char pass[] = "#3Twinkle3#";

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long interval = 120000;
int previousPinValue = 0;
int pinValue;
boolean set = true;
boolean check = true;
int buzzer = 4;
boolean buzzerHigh = false;
boolean sound = false;
int i = 0;
int buzzerCount = 25;
int buzzerInterval = 500;
// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V1)
{
  pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Pin Value: ");
  Serial.println(pinValue);

  if (previousPinValue != pinValue)
  {
    set = true;
    check = true;
    buzzerInterval = 500;
    i = 0;
    buzzerHigh = true;
    sound = true;
  }
  if ((pinValue < 7) && set == true)
  {

    for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, RgbColor(black));
      if (i == pinValue) strip.SetPixelColor(pinValue, RgbColor(yellow));
      Serial.print("Pin Value: ");
      Serial.println(pinValue);
    }
    strip.Show();
    previousPinValue = pinValue;
    set = false;

  }
  if ((pinValue > 6) && (pinValue < 14) && set == true)
  {
    int led = map(pinValue, 7, 13, 13, 7);
    for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, RgbColor(black));
      if (i == led) strip.SetPixelColor(led, RgbColor(green));
      Serial.print("Pin Value: ");
      Serial.println(led);
    }
    strip.Show();
    previousPinValue = pinValue;
    set = false;
  }
  if ((pinValue > 13) && (pinValue < 21) && (set == true))
  {
    for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, RgbColor(black));
      if (i == pinValue) strip.SetPixelColor(pinValue, RgbColor(red));
      Serial.print("Pin Value: ");
      Serial.println(pinValue);
    }
    strip.Show();
    previousPinValue = pinValue;
    set = false;
  }
  if ((pinValue > 20) && (pinValue < 28) && (set == true))
  {
    int led1 = map(pinValue, 21, 27, 27, 21);
    Serial.print("led1: ");
    Serial.println(led1);
    for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, RgbColor(black));
      if (i == led1) strip.SetPixelColor(led1, RgbColor(blue));
      Serial.print("Pin Value: ");
      Serial.println(led1);
    }
    strip.Show();
    previousPinValue = pinValue;
    set = false;

  }

}

/*void setup()
  {
  // Debug console
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  WiFiManager wifiManager;
  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();
  // wifiManager.resetSettings();
  wifiManager.autoConnect("Pill_Reminder");
  Serial.println("connected...yeey :)");
  Serial.println( WiFi.SSID().c_str());
  Serial.println(WiFi.psk().c_str());

  wifiManager.setConfigPortalTimeout(180);  // after 3 minutes try to autoconnect again
  //Blynk.begin(auth, ssid, pass);
  Blynk.config(auth);
  strip.SetPixelColor(27, RgbColor(green));
  strip.Show();
  delay(1000);
  strip.SetPixelColor(27, RgbColor(black));
  strip.Show();

  }*/

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(512);
  if (loadSavedConfig()) {
    if (checkWiFiConnection()) {
      SETUP_MODE = false;
      startWebServer();
      Blynk.config(auth);
      pinMode(buzzer, OUTPUT);
      digitalWrite(buzzer, LOW);
      strip.Begin();
      strip.Show();

      strip.SetPixelColor(27, RgbColor(green));
      strip.Show();
      delay(1000);
      strip.SetPixelColor(27, RgbColor(black));
      strip.Show();
      return;
    }
  }
  SETUP_MODE = true;
  setupMode();




}


void loop()
{

  if (SETUP_MODE) {
    DNS_SERVER.processNextRequest();
    
  }
  else
  {
    Blynk.run();
    if (previousPinValue == pinValue)
    {
      currentMillis = millis();
      if (check == true)
      {
        previousMillis = currentMillis;
        check = false;
      }

      if (sound == true)
      {
        if ((currentMillis - previousMillis1 < buzzerInterval) && (i < buzzerCount) && (buzzerHigh == true))
        {
          digitalWrite(buzzer, HIGH);
        }
        if ((currentMillis - previousMillis1 > buzzerInterval) && (buzzerHigh == true))
        {
          i++;
          buzzerHigh = false;
          previousMillis1 = currentMillis;
        }

        if ((currentMillis - previousMillis1 <  buzzerInterval) && (i < buzzerCount) && (buzzerHigh == false))
        {
          digitalWrite(buzzer, LOW);
        }
        if ((currentMillis - previousMillis1 > buzzerInterval) && (buzzerHigh == false))
        {
          i++;
          buzzerHigh = true;
          previousMillis1 = currentMillis;
        }
        if (i == 20) sound = false;
      }
      if (currentMillis - previousMillis > interval)
      {
        for (int i = 0; i < PixelCount; i++)
        {
          strip.SetPixelColor(i, RgbColor(black));
        }
        strip.Show();
        digitalWrite(buzzer, LOW);
      }
    }
  }


WEB_SERVER.handleClient();

}

void initHardware()
{
  // Serial and EEPROM
  Serial.begin(115200);

  delay(10);

}

/////////////////////////////
// AP Setup Mode Functions //
/////////////////////////////

// Load Saved Configuration from EEPROM
boolean loadSavedConfig() {
  Serial.println("Reading Saved Config....");
  String ssid = "";
  String password = "";
  if (EEPROM.read(0) != 0) {
    for (int i = 0; i < 32; ++i) {
      ssid += char(EEPROM.read(i));
    }
    Serial.print("SSID: ");
    Serial.println(ssid);
    for (int i = 32; i < 96; ++i) {
      password += char(EEPROM.read(i));
    }
    Serial.print("Password: ");
    Serial.println(password);
    WiFi.begin(ssid.c_str(), password.c_str());
    return true;
  }
  else {
    Serial.println("Saved Configuration not found.");
    return false;
  }
}

// Boolean function to check for a WiFi Connection
boolean checkWiFiConnection() {
  int count = 0;
  Serial.print("Waiting to connect to the specified WiFi network");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      return (true);
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;
}

// Start the web server and build out pages
void startWebServer() {
  if (SETUP_MODE) {
    Serial.print("Starting Web Server at IP address: ");
    Serial.println(WiFi.softAPIP());
    // Settings Page
    WEB_SERVER.on("/settings", []() {
      String s = "<h2>Wi-Fi Settings</h2>";
      s += "<p>Please select the SSID of the network you wish to connect to and then enter the password and submit.</p>";
      s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
      s += SSID_LIST;
      s += "</select>";
      s += "<br><br>Password: <input name=\"pass\" length=64 type=\"password\"><br><br>";
      s += "<input type=\"submit\"></form>";
      WEB_SERVER.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    // setap Form Post
    WEB_SERVER.on("/setap", []() {
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      String ssid = urlDecode(WEB_SERVER.arg("ssid"));
      Serial.print("SSID: ");
      Serial.println(ssid);
      String pass = urlDecode(WEB_SERVER.arg("pass"));
      Serial.print("Password: ");
      Serial.println(pass);
      Serial.println("Writing SSID to EEPROM...");
      for (int i = 0; i < ssid.length(); ++i) {
        EEPROM.write(i, ssid[i]);
      }
      Serial.println("Writing Password to EEPROM...");
      for (int i = 0; i < pass.length(); ++i) {
        EEPROM.write(32 + i, pass[i]);
      }
      EEPROM.commit();
      Serial.println("Write EEPROM done!");
      String s = "<h1>WiFi Setup complete.</h1>";
      s += "<p>The button will be connected automatically to \"";
      s += ssid;
      s += "\" after the restart.";
      WEB_SERVER.send(200, "text/html", makePage("Wi-Fi Settings", s));
      ESP.restart();
    });
    // Show the configuration page if no path is specified
    WEB_SERVER.onNotFound([]() {
      String s = "<h1>WiFi Configuration Mode</h1>";
      s += "<p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      WEB_SERVER.send(200, "text/html", makePage("Access Point mode", s));
    });
  }
  else {
    Serial.print("Starting Web Server at ");
    Serial.println(WiFi.localIP());
    WEB_SERVER.on("/", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      String s = "<h1>PillBox Status</h1>";
      s += "<h3>Network Details</h3>";
      s += "<p>Connected to: " + String(WiFi.SSID()) + "</p>";
      s += "<p>IP Address: " + ipStr + "</p>";

      s += "<h3>Options</h3>";
      s += "<p><a href=\"/reset\">Clear Saved Wi-Fi Settings</a></p>";
      WEB_SERVER.send(200, "text/html", makePage("Station mode", s));
    });
    WEB_SERVER.on("/reset", []() {
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      String s = "<h1>Wi-Fi settings was reset.</h1><p>Please reset device.</p>";
      WEB_SERVER.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
    });
  }
  WEB_SERVER.begin();
  //triggerButtonEvent(IFTTT_NOTIFICATION_EVENT);
}

// Build the SSID list and setup a software access point for setup mode
void setupMode() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  for (int i = 0; i < n; ++i) {
    SSID_LIST += "<option value=\"";
    SSID_LIST += WiFi.SSID(i);
    SSID_LIST += "\">";
    SSID_LIST += WiFi.SSID(i);
    SSID_LIST += "</option>";
  }
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID);
  DNS_SERVER.start(53, "*", AP_IP);
  startWebServer();
  Serial.print("Starting Access Point at \"");
  Serial.print(AP_SSID);
  Serial.println("\"");
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<style>";
  // Simple Reset CSS
  s += "*,*:before,*:after{-webkit-box-sizing:border-box;-moz-box-sizing:border-box;box-sizing:border-box}";
  s += "html{font-size:100%;-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}html,button,input";
  s += ",select,textarea{font-family:sans-serif}article,aside,details,figcaption,figure,footer,header,";
  s += "hgroup,main,nav,section,summary{display:block}body,form,fieldset,legend,input,select,textarea,";
  s += "button{margin:0}audio,canvas,progress,video{display:inline-block;vertical-align:baseline}";
  s += "audio:not([controls]){display:none;height:0}[hidden],template{display:none}img{border:0}";
  s += "svg:not(:root){overflow:hidden}body{font-family:sans-serif;font-size:16px;font-size:1rem;";
  s += "line-height:22px;line-height:1.375rem;color:#585858;font-weight:400;background:#fff}";
  s += "p{margin:0 0 1em 0}a{color:#cd5c5c;background:transparent;text-decoration:underline}";
  s += "a:active,a:hover{outline:0;text-decoration:none}strong{font-weight:700}em{font-style:italic}";
  // Basic CSS Styles
  s += "body{font-family:sans-serif;font-size:16px;font-size:1rem;line-height:22px;line-height:1.375rem;";
  s += "color:#585858;font-weight:400;background:#fff}p{margin:0 0 1em 0}";
  s += "a{color:#cd5c5c;background:transparent;text-decoration:underline}";
  s += "a:active,a:hover{outline:0;text-decoration:none}strong{font-weight:700}";
  s += "em{font-style:italic}h1{font-size:32px;font-size:2rem;line-height:38px;line-height:2.375rem;";
  s += "margin-top:0.7em;margin-bottom:0.5em;color:#343434;font-weight:400}fieldset,";
  s += "legend{border:0;margin:0;padding:0}legend{font-size:18px;font-size:1.125rem;line-height:24px;line-height:1.5rem;font-weight:700}";
  s += "label,button,input,optgroup,select,textarea{color:inherit;font:inherit;margin:0}input{line-height:normal}";
  s += ".input{width:100%}input[type='text'],input[type='email'],input[type='tel'],input[type='date']";
  s += "{height:36px;padding:0 0.4em}input[type='checkbox'],input[type='radio']{box-sizing:border-box;padding:0}";
  // Custom CSS
  s += "header{width:100%;background-color: #2c3e50;top: 0;min-height:60px;margin-bottom:21px;font-size:15px;color: #fff}.content-body{padding:0 1em 0 1em}header p{font-size: 1.25rem;float: left;position: relative;z-index: 1000;line-height: normal; margin: 15px 0 0 10px}";
  s += "</style>";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += "<header><p>" + DEVICE_TITLE + "</p></header>";
  s += "<div class=\"content-body\">";
  s += contents;
  s += "</div>";
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

/////////////////////////
// Debugging Functions //
/////////////////////////

void wipeEEPROM()
{
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);

  EEPROM.end();
}
