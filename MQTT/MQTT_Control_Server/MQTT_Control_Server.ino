#include <SPI.h>


///WeatherStage
#include "DHT.h"
#define DHTPIN 4 
#define DHTTYPE DHT11  


///MQTT Stage & Web Stage

#include <ESP8266WiFi.h>
#include <PubSubClient.h>



// ESP8266 Server
#include <ESP8266WebServer.h>

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
String webString="";

const char* ssid = "sungerbob";
const char* password = "halukbinreyhan";
const char* mqtt_server = "192.168.0.12";

// MQTT Coming Signal

char* topic = "device/control";
char* topicPublish = "device/sensor";
//char* server = "192.168.0.12"; //ip address of http://test.mosquitto.org/
//----------

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "7L7RFHV5SYCVNJNP";
const int updateThingSpeakInterval = 16 * 1000; // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// GET /update?key=[THINGSPEAK_KEY]&field1=[data 1]&field2=[data 2]...;
String GET = "GET /update?key=[ThingSpeak_(Write)7L7RFHV5SYCVNJNP]";



WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  
}

//MQTT Connect

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Homext", "I'm Connected");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//MQTT SEND & RECEIVE
char message_buff[100];
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();


    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is acive low on the ESP-01)
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }



    
  }


  //ESP8266 Web Server Stage
  ESP8266WebServer server(80);
  
void handle_root() {
  server.send(200, "text/plain", "Hello from the weather esp8266, read from /temp or /humidity");
  delay(100);
}


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
//DHT STAGE
  Serial.println("DHT Weather Coming!");
  dht.begin();
//MQTT ESP STAGE

 setup_wifi();
 
 pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 client.setServer(mqtt_server, 1883);
   client.setCallback(callback);
  
//ESP8266 Server Stage
  server.on("/", handle_root);
  server.on("/temp", [](){  // if you add this subdirectory to your webserver call, you get text below :)
  unsigned long currentMillis = millis();
   if(currentMillis - previousMillis >= interval) {
       previousMillis = currentMillis;   

  }
   float t = dht.readTemperature();
    webString="Temperature: "+ String((int)t)+" C"; // Arduino has a hard time with float to string
    server.send(200, "text/plain", webString);            // send to someones browser when asked
  });

}

void loop() {

   
  
  delay(2000);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
}
// Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
 Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

value =hic;

 server.handleClient();

  //MQTT STAGE

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
String apiKey = "7L7RFHV5SYCVNJNP";
char* serverID = "api.thingspeak.com";









  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    



//retain message
char* message = msg;
int length = strlen(message);
boolean retained = true;


    
    snprintf (msg, 75, "Isı #%ld",  value  );
    Serial.print("Publish message: ");
    Serial.println(msg);

    client.publish("Homext",(byte*)message,length,retained);

  //  client.publish("Homext", msg);

  }

 

  
}

