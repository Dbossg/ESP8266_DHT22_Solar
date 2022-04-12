#include <DHT.h>
#include <DHT_U.h>
#include <ThingSpeak.h>

#include <ESP8266WiFi.h> //Librarry connecting ESP8266 to connect with Wifi

// ***********************************************Thingspeak Credentials*****************
String apiKey = "KQWTCDT7A9894IRQ"; //Write API key of your ThingSpeak channel
const char* server = "api.thingspeak.com"; // API for thingspeak
//****************************************************************************************

const char *ssid = "HH71VM_9E4F_2.4G"; // Wifi SSID of your Internet connection
const char *pass = "eF3CbDKJ"; // Password

// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 1059866;
char* readAPIKey = "0R1VN68W3QC43HWI";
char* writeAPIKey = "KQWTCDT7A9894IRQ";
const unsigned long postingInterval = 120L * 1000L;
unsigned int dataFieldOne = 1;                            // Field to write temperature data
unsigned int dataFieldTwo = 2;                       // Field to write temperature data
unsigned int df3 = 3;                       // Field to write temperature data
int SerialOn = 0;

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
const float REFERENCE_VCC = 3.3;
const float BAT_RES_VALUE_VCC = 5.1;
const float BAT_RES_VALUE_GND = 10;
int sensorValue = 0;  // value read from the pot

#define DHTPIN D2 //pin where the DHT22 is connected
DHT dht(DHTPIN, DHT22);
WiFiClient client;
void setup()
{
  if (SerialOn == 1){
    Serial.begin(9600); // Serial monitor Baudrate
  }
  delay(10);
  //******************PowerSupply to the Sensor**********************
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D1,HIGH);
  digitalWrite(D3,LOW);
  delay(1000);
  //*********************************************************
  dht.begin();
  
  if (SerialOn == 1){
    Serial.println("Trying to Connect with ");
    Serial.println(ssid);
  }
  WiFi.begin(ssid, pass); // Connecting ESP8266 with Internet enabled Wifi with above mentioned credentials
  while (WiFi.status() != WL_CONNECTED)
  {
    // If the connection was unsuccesfull, it will try again and again
    delay(500);
    if (SerialOn == 1){
      Serial.print(".");
    }
  }
  // Connection succesfull
  if (SerialOn == 1){
    Serial.println("");
    Serial.println("WiFi connected");
  }
  
  ThingSpeak.begin( client );
}

// Use this function if you want to write multiple fields simultaneously.
int write2TSData( long TSChannel, 
                  unsigned int TSField1, float field1Data, 
                  unsigned int TSField2, float field2Data, 
                  unsigned int TSField3, float field3Data){
  

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  ThingSpeak.setField( TSField3, field3Data );
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}

// Use this function if you want to write multiple fields simultaneously.
float readTSData( long TSChannel,unsigned int TSField ){
    
  float data =  ThingSpeak.readFloatField( TSChannel, TSField, readAPIKey );
  if (SerialOn == 1){
    Serial.println( " Data read from ThingSpeak: " + String( data, 9 ) );
  }
  return data;

}

// Use this function if you want to write a single field.
int writeTSData( long TSChannel, unsigned int TSField, float data ){
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, writeAPIKey ); // Write the data to the channel
  if ( writeSuccess ){
    
      if (SerialOn == 1){
          Serial.println( String(data) + " written to Thingspeak." );
      }
    }
    
    return writeSuccess;
}

void loop()
{
  float h = dht.readHumidity(); // Reading Temperature form DHT sensor
  float t = dht.readTemperature(); // Reading Humidity form DHT sensor

  // read the analog in value
  float sample1 = 0;
    // Get 100 analog read to prevent unusefully read
    for (int i = 0; i < 100; i++) {
        sample1 = sample1 + analogRead(analogInPin); //read the voltage from the divider circuit
        delay(2);
    }
    sample1 = sample1 / 100;
  float batVolt = (sample1 * REFERENCE_VCC  * (BAT_RES_VALUE_VCC + BAT_RES_VALUE_GND) / BAT_RES_VALUE_GND) / 1023;
  

  if (isnan(h) || isnan(t))
  {
    if (SerialOn == 1){
      Serial.println("Failed to read from DHT sensor!");
    }
    delay(500);
    return;
  }
  // Connecting to the Thingspeak API and Posting DATA



  write2TSData( channelID , dataFieldOne , t , dataFieldTwo , h, df3 , batVolt);      // Write the temperature in F, C, and time since starting.
  if (SerialOn == 1){
     Serial.println("Temp =  " + String( t )+ " Humidity= "+String( h )+ " Voltage= "+String( batVolt ));
      Serial.println("Waiting to initiate next data packet...");
  }
  
  // thingspeak needs minimum 15 sec delay between updates
  ESP.deepSleep(300e6);
}
