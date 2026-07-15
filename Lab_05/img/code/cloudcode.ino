#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

const char* ssid = "anamika";
const char* password = "anamika123";

const char* serverUrl = "http://100.48.81.73/weather";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT22!");
    delay(10000);
    return;
  }

  Serial.printf("Temperature: %.2f °C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(serverUrl) +
                 "?temperature=" + String(temperature, 2) +
                 "&humidity=" + String(humidity, 2);

    Serial.println("Sending POST request:");
    Serial.println(url);

    http.begin(url);
    http.addHeader("accept", "application/json");

    int httpResponseCode = http.POST("");

    if (httpResponseCode > 0) {
      String response = http.getString();

      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);

      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error sending request: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected!");
  }

  Serial.println("-------------------");

  delay(10000);  // Send data every 10 seconds
}