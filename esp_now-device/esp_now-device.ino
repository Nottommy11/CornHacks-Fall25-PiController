#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 48
#define LED_COUNT 1
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Structure for receiving
typedef struct struct_message {
  float waterTemp;
  float airTemp;
  float humidity;
  float pressure;
  float tds;
  float waterLevel;
} struct_message;

struct_message incomingData;

// --- RGB flash sequence ---
void flashLEDSequence() {
  int delayMs = 70;
  pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Red
  pixels.show();
  delay(delayMs);
  pixels.setPixelColor(0, pixels.Color(0, 255, 0)); // Green
  pixels.show();
  delay(delayMs);
  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Blue
  pixels.show();
  delay(delayMs);
  pixels.clear();
  pixels.show();
}

// --- ESP-NOW receive callback (updated for ESP-IDF 5.x) ---
void onReceive(const esp_now_recv_info *info, const uint8_t *incomingDataBytes, int len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

  Serial.printf("water_temp:%.2f\n", incomingData.waterTemp);
  Serial.printf("air_temp:%.2f\n", incomingData.airTemp);
  Serial.printf("humidity:%.2f\n", incomingData.humidity);
  Serial.printf("pressure:%.2f\n", incomingData.pressure);
  Serial.printf("tds:%.2f\n", incomingData.tds);
  Serial.printf("water_level:%.2f\n", incomingData.waterLevel);
  Serial.println("---");
  Serial.println("Data relayed to Pi\n");

  flashLEDSequence();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pixels.begin();
  pixels.clear();
  pixels.show();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onReceive);  // updated signature accepted
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("ESP-NOW receiver ready!");
}

void loop() {
  // Idle heartbeat (faint red pulse)
  static unsigned long lastPulse = 0;
  static int brightness = 0;
  static int direction = 5;

  if (millis() - lastPulse > 50) {
    lastPulse = millis();
    brightness += direction;
    if (brightness >= 100 || brightness <= 0) direction = -direction;
    pixels.setPixelColor(0, pixels.Color(brightness, 0, 0));
    pixels.show();
  }
}
