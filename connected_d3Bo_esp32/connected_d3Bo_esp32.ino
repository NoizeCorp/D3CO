/*
  ESP32 MQTT sample

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP32 board/library.

  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include "DHTesp.h"


#define LED_PIN     14

// Information about the LED strip itself
#define NUM_LEDS    48
#define LED_TYPE    WS2811
#define CHIPSET     WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  255
#define BUILTIN_LED 2
// Update these with values suitable for your network.

const char* ssid     = "Noize-Node";
const char* password = "lusuro-anagogicos%?-gaudioso-castritiis";

const char* mqtt_server = "192.168.1.186";

IPAddress local_IP(192, 168, 1, 115);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

DHTesp dht;

int pir_sensor = 5;
int gas_sensor = 33;
int lux_sensor = 32;
int touch_sensor = 4;
int dht_sensor = 27;

float gas_value;
float lux_value;
float lux_threshold = 300;

uint8_t touch_threshold = 10;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

/** Comfort profile */
ComfortState cf;
/** Pin number for DHT11 data pin */
float dew_point = 0.0;
float temp = 0.0;
String status_temp = "status";
float humidity = 0.0;

bool initTemp() {
  byte resultValue = 0;
  // Initialize temperature sensor
  dht.setup(dht_sensor, DHTesp::DHT22);
  Serial.println("DHT initiated");
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Check topic vf
  if (strncmp (topic,"office/vf",10) == 0 || strncmp (topic,"office/d3Bo/vf",15) == 0 ) {
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // Handle Vf
    } else if ((char) payload[0] == 'p') {
      Serial.println("Pinky mood");
      snprintf (msg, 40, "Pinky mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::DeepPink);
    } else if ((char) payload[0] == 'b') {
      Serial.println("Blue mood");
      snprintf (msg, 40, "Blue mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::Blue);
    } else if ((char) payload[0] == 'c') {
      Serial.println("Coral mood");
      snprintf (msg, 40, "Coral mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::Coral);
    } else if ((char) payload[0] == 'l') {
      Serial.println("Lime mood");
      snprintf (msg, 40, "Lime mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::Lime);
    } else if ((char) payload[0] == 's') {
      Serial.println("LightSeaGreen mood");
      snprintf (msg, 40, "LightSeaGreen mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::LightSeaGreen);
    } else if ((char) payload[0] == 'a') {
      Serial.println("Aqua mood");
      snprintf (msg, 40, "Aqua mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::Aqua);
    } else if ((char) payload[0] == 't') {
      Serial.println("SteelBlue mood");
      snprintf (msg, 40, "SteelBlue mood\n");
      client.publish("office/vf", msg);
      transition(CRGB::SteelBlue);
    } else if ((char) payload[0] == 'x') {
      Serial.println("Off");
      turn_off();
    } 

  }

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      // Turn the visual debug LED
      digitalWrite(BUILTIN_LED, HIGH);
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("office/d3Bo/sensors", "Ca va la forme!  i'm d3Bo_office");
      client.publish("office/values", "La forme! I'm d3Bo_office");
      // ... and resubscribe
      client.subscribe("office/d3Bo/vf");
      client.subscribe("office/vf");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_pir() {
  pinMode(pir_sensor, INPUT);
}

void init_gas_sensor() {
  pinMode(gas_sensor, INPUT);
}

void init_lux_sensor() {
  pinMode(lux_sensor, INPUT);
}

void init_touch_sensor(){
   pinMode(touch_sensor, OUTPUT);
}

void init_visual_feedback(){
  
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    
    snprintf (msg, 40, "d3Bo_office online Service Available\n");
    client.publish("office/vf", msg);
    transition(CRGB::Coral);
    delay(2000);
    turn_off();
    
}

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  init_gas_sensor();
  init_lux_sensor();
  setup_wifi();
  setup_pir();
  initTemp();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  init_visual_feedback();
}

boolean handle_touch_sensor(){
  boolean state = false;
  if (touchRead(touch_sensor)<= touch_threshold){
    state = true;  
    Serial.println("touched");
    snprintf (msg, 40, "touched");
    client.publish("office/values", msg);
    
  }
  return state;
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg >= 2000) {
    
    getTemperature(temp, humidity, dew_point, status_temp);

    lux_value = analogRead(lux_sensor);
    gas_value = analogRead(gas_sensor);
    Serial.println(gas_value);
    Serial.println(lux_value);

    // PIR
    int is_detected = digitalRead(pir_sensor);
    Serial.println(is_detected);

    snprintf (msg, 180, "g,%.2f,t,%.2f,h,%.2f,l,%.2f,o,%d,t,%d\n", gas_value, temp, humidity, lux_value, (int)is_detected, (int) handle_touch_sensor());
    client.publish("office/values", msg);
    lastMsg = now;

    check_presence_and_light(lux_value,is_detected);
  }
}

void check_presence_and_light(float lux, int pir){
  if (lux <= lux_threshold && pir == 1){
    // turn on light and handle timer with animation back to previous
  }
}
