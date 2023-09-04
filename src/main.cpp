/*
   This example sketch is for use with the ESP32.
   The code below will serve a web page on the local network
   and will refresh the BPM with every heartbeat.
   On startup, the ESP32 will send it's network address
   over the serial port. Use that url in any browser
   on the same network to connect and veiw the webpage.

   Code to detect pulses from the PulseSensor,
   using an interrupt service routine.

   Here is a link to the tutorial\
   https://pulsesensor.com/pages/getting-advanced

   Copyright World Famous Electronics LLC - see LICENSE
   Contributors:
     Joel Murphy, https://pulsesensor.com
     Yury Gitman, https://pulsesensor.com
     Bradford Needham, @bneedhamia, https://bluepapertech.com

   Licensed under the MIT License, a copy of which
   should have been included with this software.

   This software is not intended for medical use.
*/

/*
   The following libraries are necessary
   for the asynchronous web server
*/
// #include "planificador.c"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <Adafruit_SH1106.h>
// #include <SPI.h>
#include <DFRobotDFPlayerMini.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

// #define PeriodOLED 150
// #define PeriodAlarm 150

// #define TICK 50

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define LOGO_WIDTH 84
#define LOGO_HEIGHT 52

const unsigned char PROGMEM logo[] = {

    // 'diseno-linea-pulso-cardiaco-ilustracion-vector-simbolo-cardiograma_692379-258', 84x52px
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0xf8, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1f, 0xe1, 0xfe, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x33, 0x06, 0x00, 0x00,
    0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x16, 0x03, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x60, 0x1c, 0x01, 0x80, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x00, 0x80,
    0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x80, 0x00, 0x50, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc0, 0x00, 0x00, 0x80, 0x00, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x80, 0x00, 0x98, 0x10, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x80, 0x60, 0x98, 0x10, 0x00,
    0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x80, 0x60, 0x88, 0x18, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00,
    0x00, 0x80, 0x61, 0x88, 0x18, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xf1, 0x08, 0x38,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x91, 0x08, 0x2c, 0x00, 0x40, 0x00, 0x00, 0x30,
    0x00, 0x00, 0x00, 0x91, 0x08, 0x24, 0x00, 0xe0, 0x00, 0x00, 0x18, 0x00, 0x03, 0xcf, 0x91, 0x0c,
    0x67, 0xff, 0xf0, 0x00, 0x00, 0x08, 0x00, 0x0f, 0xff, 0x09, 0x0c, 0x47, 0xff, 0xf0, 0x00, 0x00,
    0x0c, 0x00, 0x08, 0x00, 0x0b, 0x04, 0x40, 0x01, 0xe0, 0x00, 0x00, 0x06, 0x00, 0x18, 0x00, 0x0e,
    0x04, 0x40, 0x00, 0xe0, 0x00, 0x00, 0x03, 0x00, 0x30, 0x00, 0x0e, 0x04, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x80, 0x60, 0x00, 0x06, 0x04, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0xc0, 0x00,
    0x06, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x80, 0x00, 0x06, 0x04, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x61, 0x80, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00,
    0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};

DFRobotDFPlayerMini dfPlayer;

TaskHandle_t task1_handle = NULL;
TaskHandle_t task2_handle = NULL;

/*
tTime now;
tTime LastScheduleTime;
struct Task *tareaOLED;
struct Task *tareaAlarm;

*/

/*
   The following hardware timer setup supports ESP32
*/
hw_timer_t *sampleTimer = NULL;
portMUX_TYPE sampleTimerMux = portMUX_INITIALIZER_UNLOCKED;
// Adafruit_SH1106 display(4);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*
   Every Sketch that uses the PulseSensor Playground must
   define USE_ARDUINO_INTERRUPTS before including PulseSensorPlayground.h.
   Here, #define USE_ARDUINO_INTERRUPTS true tells the library to use
   interrupts to automatically read and process PulseSensor data.
*/
#define USE_ARDUINO_INTERRUPTS true
// #define NO_PULSE_SENSOR_SERIAL true
#include <PulseSensorPlayground.h>

/*
    We use JSON to pass data from the Arduino sketch to the Javascript
*/
JSONVar pulseData;

/*
   Declare an instance of PulseSensor to access
   all the PulseSensor Playground functions.
*/
PulseSensorPlayground pulseSensor;

/*
   Pinout:
     PULSE_INPUT = Analog Input. Connected to the pulse sensor
      purple (signal) wire.
     PULSE_BLINK = digital Output. Connected to an LED (and 1K series resistor)
      that will flash on each detected pulse.
     PULSE_FADE = digital Output. PWM pin onnected to an LED (and 1K series resistor)
      that will smoothly fade with each pulse.
      NOTE: PULSE_FADE must be a pin that supports PWM. Do not use
      pin 9 or 10, because those pins' PWM interferes with the sample timer.
     THRESHOLD should be set higher than the PulseSensor signal idles
      at when there is nothing touching it. The expected idle value
      should be 512, which is 1/2 of the ADC range. To check the idle value
      open a serial monitor and make note of the PulseSensor signal values
      with nothing touching the sensor. THRESHOLD should be a value higher
      than the range of idle noise by 25 to 50 or so. When the library
      is finding heartbeats, the value is adjusted based on the pulse signal
      waveform. THRESHOLD sets the default when there is no pulse present.
      Adjust as neccesary.
*/
const int PULSE_INPUT = 32;
// const int PULSE_BLINK = 2;
// const int PULSE_FADE = 5;
const int THRESHOLD = 500;
int redPin = 19;
int greenPin = 18;
int bluePin = 5;

/*  Replace with your network credentials  */
const char *ssid = "NETLIFE-HIDALGO";
const char *password = "06081970";
/*const char *ssid = "Microcontroladores";
const char *password = "raspy123";*/

/*
    Create AsyncWebServer object on port 80
    Create an Event Source on /events
*/
AsyncWebServer server(80);
AsyncEventSource events("/events");

/*
    The following code between the two "rawliteral" tags
    will be stored as text. It contains the html,
    css, and javascript that will be used to build
    the asynchronous server.
*/
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
      <style>
      html {
        font-family: Arial; 
        display: inline-block; 
        margin: 0px auto;
        text-align: center;
      }
      h2 { font-size: 3.0rem; }
      p { font-size: 3.0rem; }
      .reading { 
        font-size: 2.0rem;
        color:black;
      }
      .dataType {
        font-size: 1.8rem;
      }
    </style>
  </head>
  <body>
      <h2>PulseSensor Server Juan-Milena</h2>
      <p 
        <span class="reading"> Heart Rate</span>
        <span id="bpm"></span>
        <span class="dataType">bpm</span>
      </p> 
  </body>
<script>
window.addEventListener('load', getData);

function getData(){
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var Jobj = JSON.parse(this.responseText);
      console.log(Jobj);
      document.getElementById("bpm").innerHTML = Jobj.heartrate;
    }
  }; 
  xhr.open("GET", "/data", true);
  xhr.send();
}

if (!!window.EventSource) {
  var source = new EventSource('/events');

  source.addEventListener('open', function(e) {
    console.log("Events Connection");
  }, false);

  source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnection");
    }
  }, false);

  source.addEventListener('new_data', function(e) {
    console.log("new_data", e.data);
    var Jobj = JSON.parse(e.data);
    document.getElementById("bpm").innerHTML = Jobj.heartrate;
  }, false);
}
</script>
</html>)rawliteral";

/*  Package the BPM in a JSON object  */
boolean sendPulseSignal = false;

String updatePulseDataJson()
{
  pulseData["heartrate"] = String(pulseSensor.getBeatsPerMinute());
  String jsonString = JSON.stringify(pulseData);
  return jsonString;
}

void display_val1(int bpm)
{

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  display.print("BPM:");
  display.display();
  display.println(bpm);
  display.display();

  display.drawLine(0, 10, 128, 10, WHITE);
  display.display();

  display.drawBitmap((display.width() - LOGO_WIDTH) / 2, ((display.height() - LOGO_HEIGHT) / 2) + 7, logo, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.display();
}

void tdisplay_val(void *args)
{

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(1500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 0);
  display.print("BPM:");
  display.display();
  display.println(pulseSensor.getBeatsPerMinute());
  display.display();

  display.drawLine(0, 10, 128, 10, WHITE);
  display.display();

  display.drawBitmap((display.width() - LOGO_WIDTH) / 2, ((display.height() - LOGO_HEIGHT) / 2) + 7, logo, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.display();
}

/*
void display_val(int bpm)
{

  if (!display.begin(SH1106_SWITCHCAPVCC, 0x3D))
  { // Address 0x3D for 128x64
    Serial.println(F("SH1106 allocation failed"));
    for (;;)
      ;
  }
  //display.begin(SH1106_SWITCHCAPVCC, 0x3c);
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("BPM:");
  display.display();

  display.drawLine(0, 10, 128, 10, WHITE);
  display.display();

  display.drawBitmap((display.width() - LOGO_WIDTH) / 2, ((display.height() - LOGO_HEIGHT) / 2) + 7, logo, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
  display.display();
}
*/

/*
    Begin the WiFi and print the server url
    to the serial port on connection
*/

void printControlInfo()
{
  Serial.println("PulseSensor ESP32 Example");
  Serial.print("\nPulseSensor Server url: ");
  Serial.println(WiFi.localIP());
  Serial.println("Send 'b' to begin sending PulseSensor signal data");
  Serial.println("Send 'x' to stop sendin PulseSensor signal data");
  Serial.println("Send '?' to print this message");
}

void serialCheck()
{
  if (Serial.available() > 0)
  {
    char inChar = Serial.read();
    switch (inChar)
    {
    case 'b':
      sendPulseSignal = true;
      break;
    case 'x':
      sendPulseSignal = false;
      break;
    case '?':
      if (!printControlInfo)
      {
        printControlInfo();
      }
      break;
    default:
      break;
    }
  }
}

/*
    This function prints the control information to the serial monitor
*/

void beginWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Attempting to connect to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(" ~");
    delay(1000);
  }
  Serial.println("\nConnected");
}

/*
    This is the interrupt service routine.
    We need to declare it after the PulseSensor Playground
    library is compiled, so that the onSampleTime
    function is known.
*/
void IRAM_ATTR onSampleTime()
{
  portENTER_CRITICAL_ISR(&sampleTimerMux);
  PulseSensorPlayground::OurThis->onSampleTime();
  portEXIT_CRITICAL_ISR(&sampleTimerMux);
}

/*
   When sendPulseSignal is true, PulseSensor Signal data
   is sent to the serial port for user monitoring.
   Modified by keys received on the Serial port.
   Use the Serial Plotter to view the PulseSensor Signal wave.
*/

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void alarmas(void *args)
{

  while (pulseSensor.getBeatsPerMinute() > 110)
  {
    dfPlayer.loop(5);
    setColor(255, 0, 0); // rojo
    delay(500);
    setColor(0, 0, 0); // rojo
    delay(500);
  }

  setColor(0, 255, 0); // verde 2
  dfPlayer.play(1);
}

void taskLED(void *parameters)
{

  for (;;)
  {
    while (pulseSensor.getBeatsPerMinute() > 110)
    {
      dfPlayer.play(5);
      
      setColor(255, 0, 0); // rojo
      delay(500);
      setColor(0, 0, 0); // rojo
      delay(500);
    }
    dfPlayer.play(1);
    setColor(0, 255, 0); // verde 2
    vTaskSuspend(task1_handle);
    
  
    
  }
}

void taskOLED(void *parameters)
{

  for (;;)
  {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for (;;)
        ;
    }
    delay(500);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 0);
    display.print("BPM:");
    display.display();
    display.println(pulseSensor.getBeatsPerMinute());
    display.display();

    display.drawLine(0, 10, 128, 10, WHITE);
    display.display();

    display.drawBitmap((display.width() - LOGO_WIDTH) / 2, ((display.height() - LOGO_HEIGHT) / 2) + 7, logo, LOGO_WIDTH, LOGO_HEIGHT, WHITE);
    display.display();
  }
}

void setup()
{

  /*
     115200 baud provides about 11 bytes per millisecond.
     The delay allows the port to settle so that
     we don't miss out on info about the server url
     in the Serial Monitor so we can connect a browser.
  */
  Serial.begin(115200);

  delay(1000);
   

  beginWiFi();
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  

  /*
     ESP32 analogRead defaults to 13 bit resolution
     PulseSensor Playground library works with 10 bit
  */
  analogReadResolution(10);

  /*  Configure the PulseSensor manager  */
  pulseSensor.analogInput(PULSE_INPUT);
  // pulseSensor.blinkOnPulse(PULSE_BLINK);
  // pulseSensor.fadeOnPulse(PULSE_FADE);
  pulseSensor.setSerial(Serial);
  pulseSensor.setThreshold(THRESHOLD);

  /*  Now that everything is ready, start reading the PulseSensor signal. */

  /*
      When the server gets a request for the root url
      serve the html
  */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html); });

  /*  Request for the latest PulseSensor data  */
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String json = updatePulseDataJson();
    request->send(200, "application/json", json);
    json = String(); });

  /*
      Handler for when a client connects to the server
      Only send serial feedback when NOT sending PulseSensor Signal data
      Send event with short message and set reconnect timer to 2 seconds
  */
  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(!sendPulseSignal){
      if(client->lastId()){
        Serial.println("Client Reconnected");
      } else {
        Serial.println("New Client Connected");
      }
    }
    client->send("hello", NULL, millis(), 20000); });

  // verde

  /*  Create a handler for events  */
  server.addHandler(&events);

  /*  Start the server  */
  server.begin();

  /*  Print the control information to the serial monitor  */
  printControlInfo();

  /*
      This will set up and start the timer interrupt on ESP32.
      The interrupt will occur every 2000uS or 500Hz.

  */
  sampleTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(sampleTimer, &onSampleTime, true);
  timerAlarmWrite(sampleTimer, 2000, true);
  timerAlarmEnable(sampleTimer);

  Serial2.begin(9600);
  dfPlayer.begin(Serial2); // Use softwareSerial to communicate with mp3

  dfPlayer.setTimeOut(1000); // Set serial communication time out 500ms
  dfPlayer.volume(30);       // Set volume value (0~30).
  dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
  dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  dfPlayer.play(1);

  //setColor(0, 255, 0);

  xTaskCreate(
      taskLED,
      "taskled",
      2000,
      NULL,
      1,
      &task1_handle);

  xTaskCreate(
      taskOLED,
      "task0led",
      3000,
      NULL,
      1,
      NULL);

 

 
  /*
    tTime now = TimeNow() + 1;

    tareaOLED = SchedulerAddTask(now, 0, PeriodOLED, tdisplay_val);
    TaskEnable(tareaOLED);

    tareaAlarm = SchedulerAddTask(now, 0, PeriodAlarm, alarmas);
    TaskEnable(tareaAlarm);*/

  // alarmas();
}

void loop()
{

  //display_val1(pulseSensor.getBeatsPerMinute());

  if(pulseSensor.getBeatsPerMinute() > 110 && task1_handle!=NULL)
    {
     vTaskResume(task1_handle);
    }
  /*if (TimePassed(LastScheduleTime) > TICK) {
    //Ejecuta el planificador
    SchedulerRun();
    LastScheduleTime = TimeNow();
  }*/

 

  /*
       Option to send the PulseSensor Signal data
       to serial port for verification
  */
  if (sendPulseSignal)
  {
    delay(20);
    Serial.println(pulseSensor.getLatestSample());
  }

  /*
       If a beat has happened since we last checked,
       update the json data file to the server.
       Also, send the new BPM value to the serial port
       if we are not monitoring the pulse signal.
  */
  if (pulseSensor.sawStartOfBeat())
  {
    events.send(updatePulseDataJson().c_str(), "new_data", millis());
    if (!sendPulseSignal)
    {
      Serial.print(pulseSensor.getBeatsPerMinute());
      Serial.println(" bpm");
    }
  }
  /*  Check to see if there are any commands sent to us  */

  serialCheck();
}

/*
    This function checks to see if there are any commands available
    on the Serial port. When you send keyboard characters 'b' or 'x'
    you can turn on and off the signal data stream.
*/
