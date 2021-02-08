// Load libraries
#include <ESPmDNS.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <NeoPixelBus.h>

#define GRIDWIDTH 16
#define GRIDHEIGHT 16

//Defining light pannel
typedef RowMajorAlternatingLayout MyPanelLayout;
const uint8_t PanelWidth = GRIDWIDTH;
const uint8_t PanelHeight = GRIDHEIGHT;
const uint16_t PixelCount = PanelWidth * PanelHeight;
const uint8_t PixelPin = 13;


NeoTopology<MyPanelLayout> topo(PanelWidth, PanelHeight);

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

// Replace with your network credentials
const char* ssid     = "LightboardAccessPoint";
const char* password = "123456789";


// Set web server port number to 80
AsyncWebServer server(80);


// Variable to store the HTTP request
String header;


//Stored versions of the grid and colors sent from the interface.
RgbColor colorIndex[GRIDWIDTH * GRIDHEIGHT] = {};
long gridIndex[GRIDWIDTH][GRIDHEIGHT] = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}};


void processColors(String input){ //function to process incoming colors. They come in as a string, and are split every 6 characters

  Serial.println("");
  int count = 0;

  for(int i = 0; i < input.length(); i += 6){

    String hex = input.substring(i,i+6);

    Serial.println(hex);

    long number = (long) strtol( &hex[0],NULL,16);
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

    Serial.print(r);
    Serial.print(g);
    Serial.print(b);
    Serial.println("");
    colorIndex[count] = RgbColor(r,g,b);
    count++;
  };


  Serial.println("Done");

}


void processGrid(String input){ //function to process the incoming grid.

  long value;

  int countx = 0;
  int county = 0;

  for(int i = 0; i < input.length(); i += 2){

    String hexStr = input.substring(i,i+2);

    gridIndex[county][countx] = strtol(hexStr.c_str(), NULL, 16);

    if(countx == GRIDWIDTH - 1){
      county++;
      countx=0;
      } else {
      countx++;
      }

  };

  //DEBUG
  /*
  //Prints first of every row
  for(int i = 0; i < county; i++){ //prints all colors
    Serial.println(gridIndex[i][0]);
  }
    */
  Serial.println("Done");
  };



void updateLights(){

    for(int y = 0; y < PanelHeight; y++){

        for(int x = 0; x < PanelWidth; x++){

            int colorN = gridIndex[y][x];

            strip.SetPixelColor(topo.Map(x,y),colorIndex[colorN]);
            };

        }
    Serial.println("Done");
    strip.Show();
  };


void setup() { // Setup, and creating http events

  //Open Serial Port
  Serial.begin(115200);

  strip.Begin();

  if(!SPIFFS.begin(true)){Serial.println("Error starting SPIFFS");return;}; //Initialize SPIFFS, aka the chips flash file system


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);


  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);


  if(MDNS.begin("lightboard")){Serial.println("mDNS");}; //Starts mDNS, the url is lightboard.local once connected to wifi


  server.on("/updateBoard", HTTP_POST, [](AsyncWebServerRequest * request){ // update the board with incoming colors and grid

    //Serial.println("Update requested");

    String color;
    String grid;

    bool colorsFound;
    bool gridFound;

    if(request->hasArg("colors")){ // incoming colors
        color = request->arg("colors");
        colorsFound = true;
    } else {
        //Serial.println("Error: 'colors' field missing.");
        request -> send(406,"text/plain","why the heck is this the value");
        colorsFound = false;
    }


    if(request->hasArg("grid")){ // incoming grid
        grid = request->arg("grid");
        gridFound = true;
    } else {
        //Serial.println("Error: 'grid' field missing.");
        request -> send(406,"text/plain","Error: 'grid' field missing.");
        gridFound = false;
    }

    //return request
    request->send(202);

    //begin to process incoming data
    if(colorsFound && gridFound){
      processColors(color);
      processGrid(grid);
      updateLights();
    }


 });


  //HTTP events for the interface to work, including js and css files and jquery library
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request -> send(SPIFFS, "/index.html","text/html");
    });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request -> send(SPIFFS, "/script.js","text/javascript");
    });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request -> send(SPIFFS, "/style.css","text/css");
    });

  server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request -> send(SPIFFS, "/jquery.min.js","text/javascript");
    });

  server.begin();

  Serial.print("Setup Complete");
}

void loop(){};
