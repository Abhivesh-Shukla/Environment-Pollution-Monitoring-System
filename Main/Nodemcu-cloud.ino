#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

char ssid[] = "Xav!er";            
char pass[] = "12345678";           

unsigned long Channel_ID = 1347787;     
const int Field_1 = 1;                 
const int Field_2 = 2;                 
const char * myWriteAPIKey = "8UxxxxxxxxxxxxxOX"; 

int x;
int y;
WiFiClient  client;

void setup()
{
  Serial.begin(115200);

  while (!Serial) {
    ; 
  }
  
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  internet();
}

void loop()
{
  internet();
  get_value();
  upload();
}


void internet()
{
  if (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      while (WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, pass);
        Serial.print(".");
        delay(5000);
      }
      Serial.println("\nWIFI CONNECTED.");
    }
}


const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        

float value1 = 0.0;
float value2 = 0.0;

boolean newData = false;



void get_value() {
  if (Serial.available() > 0)
  {
    recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
         
        parseData();
        showParsedData();
        newData = false;
    }
  }
  else
  {
    get_value();
  }
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; 
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }
        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseData() {     

    char * strtokIndx; 

    strtokIndx = strtok(tempChars, ","); 
    value1 = atof(strtokIndx);     

    strtokIndx = strtok(NULL, ",");    
    value2 = atof(strtokIndx);     

}



void showParsedData() {
    Serial.print("value1: ");
    Serial.println(value1);
    Serial.print("value2: ");
    Serial.println(value2);
}


void upload()
{ 
  ThingSpeak.setField(Field_1, value1);
  ThingSpeak.setField(Field_2, value2);
  if (Serial.available() > 0)
  {  
     x = ThingSpeak.writeFields(Channel_ID,myWriteAPIKey);
     
     if (x == 200){
       Serial.print("Data Updated.");
      }
     else{
       Serial.print("Data upload failed, retrying....");
       upload();
      }
  }
  else{
    Serial.print("Serial Pin Connection Error!!, retrying....");
  }
     delay(15000);
}
