#include <esp_now.h>
#include <WiFi.h>

#define PLAYER_ID 1 // Change to 2 for the second controller!

#define BUTTON_PIN 23 // Big Arcade Button
#define LED_PIN 22    // Big Button LED
#define JOY_Y_PIN 33  // Joystick Y-Axis (Vertical)
#define JOY_X_PIN 32  // Joystick X-Axis (Horizontal)

// Receiver MAC Address
uint8_t receiverAddress[] = {0xB8, 0xF8, 0x62, 0xF9, 0xE3, 0xDC};

enum CommandType
{
  CMD_NONE = 0,
  CMD_CONFIRM = 1,
  CMD_UP = 2,
  CMD_DOWN = 3,
  CMD_LEFT = 4,
  CMD_RIGHT = 5
};

typedef struct struct_message
{
  int playerId;
  CommandType command;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// TIMINGS & STATE VARIABLES
// LED Blinking
unsigned long previousLedMillis = 0;
const long ledInterval = 1000;
int ledState = LOW;

// Joystick Polling
unsigned long lastJoyTime = 0;
const int joyInterval = 100; // Joystick sends data every 100ms

// Button Interrupt
volatile bool buttonTriggered = false;
volatile unsigned long lastButtonTime = 0;
const int debounceDelay = 50; // 50ms debounce window

// ESP-NOW CALLBACK
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {}

// INTERRUPT SERVICE ROUTINE
// The IRAM_ATTR tag loads this function into RAM for the fastest possible execution
void IRAM_ATTR handleButtonInterrupt()
{
  unsigned long currentTime = millis();
  // Software debounce
  if (currentTime - lastButtonTime > debounceDelay)
  {
    buttonTriggered = true;
    lastButtonTime = currentTime;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // since button is a pullup FALLING triggers when the button is pressed (connecting it to GND).
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, FALLING);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  myData.playerId = PLAYER_ID;
  Serial.println("Sender Ready (Interrupt Driven)");
}

void loop()
{
  unsigned long currentMillis = millis();

  // 1. Blink LED
  if (currentMillis - previousLedMillis >= ledInterval)
  {
    previousLedMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  // 2. Handle Button Press (Instant Priority)
  if (buttonTriggered)
  {
    buttonTriggered = false;

    myData.command = CMD_CONFIRM;
    esp_now_send(receiverAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.println("Sent Command: CONFIRM");
  }

  // 3. Handle Joystick (Rate Limited)
  else if (currentMillis - lastJoyTime >= joyInterval)
  {
    CommandType commandToSend = CMD_NONE;

    int yVal = analogRead(JOY_Y_PIN);
    int xVal = analogRead(JOY_X_PIN);

    // Y-AXIS (Vertical)
    if (yVal < 1000)
      commandToSend = CMD_UP;
    else if (yVal > 3000)
      commandToSend = CMD_DOWN;
    // X-AXIS (Horizontal)
    else if (xVal < 1000)
      commandToSend = CMD_LEFT;
    else if (xVal > 3000)
      commandToSend = CMD_RIGHT;

    // Only send if we actually have a movement command
    if (commandToSend != CMD_NONE)
    {
      myData.command = commandToSend;
      esp_now_send(receiverAddress, (uint8_t *)&myData, sizeof(myData));
      Serial.print("Sent Command Enum: ");
      Serial.println(commandToSend);
    }

    lastJoyTime = currentMillis;
  }
}