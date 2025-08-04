#include <WiFiNINA.h>       // For connecting Arduino to WiFi
#include <DHT.h>            // For reading data from DHT temperature sensor

#define DHTPIN 2            
#define DHTTYPE DHT22    
DHT dht(DHTPIN, DHTTYPE);   

// WiFi name and password
char ssid[] = "Galaxy A15 5G 543F";
char pass[] = "muskanwifi";

// ThingSpeak server and  API key
const char server[] = "api.thingspeak.com";
String apiKey = "JBXVMUE3QIPQFHED";
WiFiClient client;

// Array to store last 5 temperature readings
float temps[5];
int i = 0;  // Index to keep track of current position in array

void setup() {
  Serial.begin(9600);    
  dht.begin();          

}

void loop() {
  float t = dht.readTemperature();  // Read temperature from sensor

  if (isnan(t)) {                   // Check if reading failed
    Serial.println("invalid reading ");
    return;                       
  }

  Serial.print("Reading: ");        
  Serial.println(t);

  temps[i] = t;                     // Store temperature in array
  i = (i + 1) % 5;                  // Move to next index, cycle back to 0 after 5 readings

  if (i == 0) {                     // Once 5 readings are taken
    float sum = 0;
    for (int j = 0; j < 5; j++) sum += temps[j];  // Add all readings
    float avg = sum / 5.0;                         // Calculate average

    Serial.print("Average: ");
    Serial.println(avg);

   
    if (client.connect(server, 80)) {
      //  URL to send average temperature to ThingSpeak
      String url = "/update?api_key=" + apiKey + "&field1=" + String(avg);
      
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Connection: close\r\n\r\n");

      delay(10);  // Small pause before reading the response

      // Read and print server response (optional)
      while (client.available()) {
        Serial.write(client.read());
      }

      client.stop();  // Close connection
    } else {
      Serial.println("Failed to connect to server.");
    }

    delay(30000);  // Wait 30 seconds before next upload (to reduce data load)
  } else {
    delay(1000);   // Wait 1 second before next reading
  }
}
