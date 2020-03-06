/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/ryan.trisnojoyo/Documents/Particle/projects/Particle_Ext_SIM/src/Particle_Ext_SIM.ino"

#include "HttpClient.h"

/**
* Declaring the variables.
*/
void setup();
void loop();
void NormalSleep();
void sendHttpRequest();
void statusMessage();
bool initConnection();
void disconnectConnection();
#line 7 "c:/Users/ryan.trisnojoyo/Documents/Particle/projects/Particle_Ext_SIM/src/Particle_Ext_SIM.ino"
String Version = "1.0.0";

SYSTEM_MODE (MANUAL);

long int prevTime;
long int currentTime;
long int sleepTime;
int sendInterval = 600; //Send Interval in seconds
pin_t wakeUpPins[1] = {D4};
int timeout = 10000;   // length of time to wait for cellular connection (milliseconds)
String temp_payload;
bool booting = true;



unsigned int nextTime = 0;    // Next time to contact the server
HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
  { "Content-Type", "application/json" },
   { "Authorization" , "Basic yWNjZXNzMlVTMzo4Mzk0Rkdmbw==" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};

http_request_t request;
http_response_t response;

void setup() {
    Serial.begin(9600);

    initConnection();
    prevTime = Time.now(); 
    Serial.println("Starting");
}

void loop() {
  currentTime = Time.now();
  SleepResult result = System.sleepResult();
 
 /********** Send Status Message***********/
  if (currentTime - prevTime >= (sendInterval)){
    if (!Particle.connected()){
    initConnection();
    }
    statusMessage();
    Serial.println("Status Message");
    prevTime = currentTime;
  }

   ////////******* Wake Up with PIN and Send Data ********/////////////
  Serial.print("Sleep Result: ");
  Serial.println(result.reason());
 // Serial.println(result.wakeUpReason());
  Serial.println(result.wokenUpByPin());
  Serial.println(result.wokenUpByRtc());
  //Serial.println(result.wakeUpPin());
  Serial.print("Booting: ");
  Serial.println(booting);
  

  if (result.wokenUpByPin()){       // Back to sleep for remainder of normal log interval
    if (!Particle.connected()){
    initConnection();
    }
    sendHttpRequest();
    Serial.println("Send Data");
    int sleepRemainder = (sendInterval) - (Time.now() - sleepTime);
    delay(200);
    Serial.print("Sleep Again ");
    Serial.println(sendInterval);
    Serial.println(Time.now());
    Serial.println(sleepTime);
    Serial.println(sleepRemainder);
    Serial.println("Disconnecting ......");
    disconnectConnection();
    if (sleepRemainder < 0){
        Serial.println("Good Night");
        NormalSleep();
    }
    // debugMessage("Sleep from Pin Wake: " + (String)sleepRemainder);
    System.sleep(D5,RISING, sleepRemainder);
    }
     /********  Good night!  Sleep!************/
  else{
    NormalSleep();
  }
}
void NormalSleep(){
 if (!Particle.connected()){
      Serial.println("Reconnecting.........");
      initConnection();
    }
    sleepTime = Time.now();
    booting = false;
    // debugMessage("Normal sleep");
    Serial.println("Disconnecting ......");
    Serial.print("Normal Sleep ");
   
    //Serial.println(sendInterval);
    //Serial.println(sleepTime);
    disconnectConnection();
    delay(200);
    System.sleep(D5,RISING,sendInterval);
   
}
void sendHttpRequest()
{
    Serial.println();
    Serial.println("Application>\tStart of Loop.");
    // Request path and body can be set at runtime or at setup.
    request.hostname = "ws.uscubed.com";
    request.port = 80;
    request.path = "/ParticleDirect.aspx";

    // The library also supports sending a body with your request:
    temp_payload = String(Time.now());
    temp_payload += ",7,1;";

    request.body = temp_payload;

    // Get request
    http.get(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);

    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(response.body);
}

/****  Every version of code should contain a daily status message  ****/

void statusMessage()
{
    Serial.println();
    Serial.println("Application>\tStart of Loop.");
    // Request path and body can be set at runtime or at setup.
    request.hostname = "ws.uscubed.com";
    request.port = 80;
    request.path = "/ParticleDirect.aspx";

    // The library also supports sending a body with your request:
    String message =  String(Time.now()) + ",Status,";
    message += ",si:" + (String)sendInterval;
    message += ",to:" + (String)timeout + ",ver:" + Version ;

    request.body = message;

    // Get request
    http.get(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);

    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(response.body);
  
}

bool initConnection()
{
  bool retVal = false;
  Cellular.on();
  Cellular.connect();
  waitFor(Cellular.ready,timeout);
  Particle.connect();
  waitFor(Particle.connected,timeout);
  if (Cellular.ready())
    retVal = true;

  Particle.syncTime();
  return retVal;
}

void disconnectConnection()
{
  Particle.disconnect();
  waitUntil(Particle.disconnected);
  Cellular.off();
}
