#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

const int ENCODER_A = 25;
const int ENCODER_B = 26;

// Starting estimate for a common 20-detent encoder using
// all four quadrature transitions. Calibrate this for your encoder.
const float COUNTS_PER_REVOLUTION = 80.0;

const char* WIFI_NAME = "ESP32-Angle";
const char* WIFI_PASSWORD = "12345678";

WebServer server(80);

volatile int32_t encoderCount = 0;
volatile uint8_t previousAB = 0;
volatile int8_t lastDirection = 0;
volatile uint32_t lastMotionUs = 0;
portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR readEncoder() {
  uint8_t a = digitalRead(ENCODER_A);
  uint8_t b = digitalRead(ENCODER_B);
  uint8_t currentAB = (a << 1) | b;

  // Decode each valid A/B quadrature transition.
  static const int8_t transitions[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  int8_t change = transitions[(previousAB << 2) | currentAB];

  if (change != 0) {
    portENTER_CRITICAL_ISR(&encoderMux);
    encoderCount += change;
    lastDirection = (change > 0) ? 1 : -1;
    lastMotionUs = micros();
    portEXIT_CRITICAL_ISR(&encoderMux);
  }

  previousAB = currentAB;
}

void addCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Cache-Control", "no-store");
}

void handleOptions() {
  addCorsHeaders();
  server.send(204, "text/plain", "");
}

void handleData() {
  int32_t count;
  int8_t direction;
  uint32_t motionTime;

  portENTER_CRITICAL(&encoderMux);
  count = encoderCount;
  direction = lastDirection;
  motionTime = lastMotionUs;
  portEXIT_CRITICAL(&encoderMux);

  float revolutions = count / COUNTS_PER_REVOLUTION;
  float angle = fmodf(revolutions * 360.0f, 360.0f);
  if (angle < 0) angle += 360.0f;

  const char* directionText = "STATIONARY";
  if ((uint32_t)(micros() - motionTime) < 500000UL) {
    directionText = (direction > 0) ? "CLOCKWISE" : "ANTICLOCKWISE";
  }

  char json[160];
  snprintf(
    json, sizeof(json),
    "{\"angle\":%.2f,\"count\":%ld,\"revolutions\":%.4f,\"direction\":\"%s\"}",
    angle, (long)count, revolutions, directionText
  );

  addCorsHeaders();
  server.send(200, "application/json", json);
}

void handleZero() {
  portENTER_CRITICAL(&encoderMux);
  encoderCount = 0;
  lastDirection = 0;
  lastMotionUs = 0;
  portEXIT_CRITICAL(&encoderMux);

  addCorsHeaders();
  server.send(200, "text/plain", "Zero position set");
}

void handleRoot() {
  addCorsHeaders();
  server.send(200, "text/plain", "ESP32 angle API is running. Open /data to see readings.");
}

void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  previousAB = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), readEncoder, CHANGE);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_NAME, WIFI_PASSWORD);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/data", HTTP_OPTIONS, handleOptions);
  server.on("/zero", HTTP_GET, handleZero);
  server.on("/zero", HTTP_OPTIONS, handleOptions);
  server.begin();

  Serial.println();
  Serial.println("Angle API ready");
  Serial.print("Wi-Fi: ");
  Serial.println(WIFI_NAME);
  Serial.print("Password: ");
  Serial.println(WIFI_PASSWORD);
  Serial.print("ESP32 address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}