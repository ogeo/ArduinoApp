/*
  oGeo
  
 We have created this sketch during the Codemotion 2012 Hackaton,
 organized by WhyMCA in Roncade(TV),Italy.
 (http://www.whymca.org/wiki/codehack-2012)
 
 The aim was to create something that could manage an emergency in 
 case of catastrophical events.
  
 This sketch sends some values (gathered by the sensors)
 to a webserver (sample-env-kmuwwhu5qf.elasticbeanstalk.com)
 using an Arduino Wiznet Ethernet shield. 
 
 We used an Arduino Mega 2560
 
 Sensors:
 -Triple Axis Accelerometer - MMA7361
 -DHT 11 Humidity & Temperature Sensor
 -Gas sensor(MQ5)
 -Current sensor ACS711
 
 assembled by 19 Nov 2012
 by Andrea Vial and Giulio Pilotto
 
 We give thanks to the creator of DnsWebClient sketch,
 AcceleroMMA7361 and dht11 libraries.
 
 */
#include <SPI.h>
#include <Ethernet.h>
#include <AcceleroMMA7361.h>
#include <dht11.h>

#define DHT11PIN A1
#define LED1 24
#define LED2 26
#define SLEEPPIN 8
#define SELFTESTPIN 7
#define ZEROGPIN 6
#define GSELECTPIN 5
#define XPIN A2
#define YPIN A3
#define ZPIN A4
#define REFV 5
#define GASPIN A5
#define CURRENTPIN A0

AcceleroMMA7361 accelero;
dht11 DHT11;
EthernetClient client;

int x;
int y;
int z;
int gas;
int current;
int moisture;
int temp;

const int deltaX = 337;
const int deltaY = 337;
const int deltaZ = 504;
const int sense = 7;

byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xE3 };
char serverName[] = "sample-env-kmuwwhu5qf.elasticbeanstalk.com";

void setup() {

  Serial.begin(9600);
  pinMode(GASPIN, INPUT);
  pinMode(CURRENTPIN, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(DHT11PIN, INPUT);
  
  accelero.begin(SLEEPPIN, SELFTESTPIN, ZEROGPIN, GSELECTPIN, XPIN, YPIN, ZPIN);
  accelero.setARefVoltage(REFV);                
  accelero.setSensitivity(HIGH);                 
  accelero.calibrate();

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  delay(1000);
  Serial.println("connecting...");
  
  //in the first time we send the default values
  if (client.connect(serverName, 80)) {  
    digitalWrite(LED1, HIGH);
    client.println("GET /rest/units/0?accelerometrox=337&accelerometroy=337&accelerometroz=504&umidita=5&temperatura=12&gas=1&corrente=13 HTTP/1.0");
    client.println();
  } 
  else {
    Serial.println("connection failed");
  }
  
   //write the response of the server
   while (client.available()) {
     char c = client.read();
     Serial.print(c);
  }
}

void loop()
{
  int chk = DHT11.read(DHT11PIN);
  
  x = accelero.getXRaw();
  y = accelero.getYRaw();
  z = accelero.getZRaw();
  gas=analogRead(GASPIN);
  current=analogRead(CURRENTPIN);
  moisture= (int)DHT11.humidity;
  temp = (int)DHT11.temperature;
  
  String data="";
  data+="&accelerometrox=" + (String)x;
  data+="&accelerometroy=" +(String)y;
  data+="&accelerometroz="+(String)z;
  data+="&umidita="+(String)temp;
  data+="&temperatura="+ (String)moisture;
  data+="&gas="+(String)gas;
  data+="&corrente="+ (String)current;
  
  if (client.connect(serverName, 80)) {
    
    digitalWrite(LED2, HIGH);
    
    client.println("GET /rest/units/0?"+data+" HTTP/1.0");
    client.println();
    
    Serial.println("connected");
    Serial.print("Read sensors: ");
    Serial.print("x: " +x);
    Serial.print("__y: " +y);
    Serial.print("__z: " +z);
    Serial.print("--Gas: " + gas);
    Serial.print("--Current: " + current);
    Serial.print("--Humidity (%): ");
    Serial.print((float)DHT11.humidity, 2);
    Serial.print("--Temperature (oC): ");
    Serial.print((float)DHT11.temperature, 2);
    
    delay(500);
    digitalWrite(LED2, LOW);
    
    //write in the serial the response of the server
    while (client.available()) {
       char c = client.read();
       Serial.print(c);
    }

  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    
    delay(5000);
  }
}
