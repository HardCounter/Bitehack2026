#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

void showGameOver();
void resetRhythmGame();
void resetF1Game();

#include "Games/Audio.h"
#include "Games/RC.h"
#include "Games/Simon.h"
#include "Games/Memory.h"
#include "Games/Rhythm.h"
#include "Games/F1.h"
#include "Games/Menu.h"

// HARDWARE
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define NEOPIXEL_PIN D0
#define NUM_LEDS 7

// AUDIO CONFIG
#define SPEAKER_PIN D1
#define SOUND_CHANNEL 0
#define NOTE_C3 131
#define NOTE_E3 165
#define NOTE_G3 196
#define NOTE_A3 220

// Motor Pins
#define AIN1_PIN D3
#define AIN2_PIN D2
#define PWMA_PIN D6
#define BIN1_PIN D8
#define BIN2_PIN D9
#define PWMB_PIN D7

#define MOTOR_SPEED 200
#define TURN_SPEED 180

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_NeoPixel ring(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

enum CommandType
{
  CMD_NONE = 0,
  CMD_CONFIRM = 1,
  CMD_UP = 2,
  CMD_DOWN = 3,
  CMD_RIGHT = 4,
  CMD_LEFT = 5
};
typedef struct struct_message
{
  int playerId;
  CommandType command;
} struct_message;
struct_message incomingData;

enum Gamestate
{
  MAIN_MENU,
  PLAYING_RHYTHM,
  PLAYING_F1,
  PLAYING_CAR,
  PLAYING_COOP,
  PLAYING_SIMON,
  PLAYING_MEMORY,
  GAME_OVER
};
Gamestate currentState = MAIN_MENU;

int menuSelection = 0;
int winnerId = 0;
volatile int wirelessMenuMove = 0;
volatile bool wirelessConfirm = false;
volatile bool p1GameClick = false;
volatile bool p2GameClick = false;
volatile unsigned long ignoreInputUntil = 0;

// ESP-NOW CALLBACK
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataPtr, int len)
{
  memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));
  unsigned long now = millis();

  if (incomingData.playerId == 1)
    p1LastRecv = now;
  if (incomingData.playerId == 2)
    p2LastRecv = now;
  lastRecvTime = now;

  if (now < ignoreInputUntil)
    return;

  if (currentState == MAIN_MENU || currentState == GAME_OVER)
  {
    if (incomingData.command == CMD_UP)
      wirelessMenuMove = -1;
    if (incomingData.command == CMD_DOWN)
      wirelessMenuMove = 1;
    if (incomingData.command == CMD_CONFIRM)
      wirelessConfirm = true;
  }
  else if (currentState == PLAYING_MEMORY)
  {
    if (incomingData.command >= CMD_UP && incomingData.command <= CMD_LEFT)
    {
      if (incomingData.playerId == 1)
        p1MemInput = incomingData.command;
      if (incomingData.playerId == 2)
        p2MemInput = incomingData.command;
    }
  }
  else if (currentState == PLAYING_CAR)
  {
    if (incomingData.command == CMD_CONFIRM)
    {
      currentState = MAIN_MENU;
      stopCar();
      ignoreInputUntil = now + 1000;
    }
    else
    {
      currentCarCommand = incomingData.command;
    }
  }
  else if (currentState == PLAYING_COOP)
  {
    if (incomingData.command == CMD_CONFIRM)
    {
      currentState = MAIN_MENU;
      stopCar();
      ignoreInputUntil = now + 1000;
    }
    else
    {
      if (incomingData.playerId == 1)
        p1CoopCmd = incomingData.command;
      if (incomingData.playerId == 2)
        p2CoopCmd = incomingData.command;
    }
  }
  else
  {
    if (incomingData.command == CMD_CONFIRM)
    {
      if (incomingData.playerId == 1)
        p1GameClick = true;
      if (incomingData.playerId == 2)
        p2GameClick = true;
    }
  }
}

void setup()
{
  Serial.begin(115200);

  ledcSetup(SOUND_CHANNEL, 2000, 8);
  ledcAttachPin(SPEAKER_PIN, SOUND_CHANNEL);

  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);
  stopCar();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("OLED failed"));
    for (;;)
      ;
  }
  Wire.setClock(400000);
  ring.begin();
  ring.setBrightness(50);
  ring.show();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
    return;
  esp_now_register_recv_cb(OnDataRecv);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("RC SYSTEM");
  display.display();
  delay(1000);
}

void loop()
{
  display.clearDisplay();
  updateAudio();

  switch (currentState)
  {
  case MAIN_MENU:
    handleMenu();
    break;
  case PLAYING_RHYTHM:
    runRhythmGame();
    break;
  case PLAYING_F1:
    runF1Game();
    break;
  case PLAYING_CAR:
    runCarGame();
    break;
  case PLAYING_COOP:
    runCoopGame();
    break;
  case PLAYING_SIMON:
    runSimonGame();
    break;
  case PLAYING_MEMORY:
    runMemoryGame();
    break;
  case GAME_OVER:
    showGameOver();
    break;
  }
  display.display();
}

void showGameOver()
{
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  if (winnerId == 0)
  {
    display.setCursor(10, 20);
    display.print("GAME OVER");
  }
  else
  {
    display.setCursor(20, 10);
    display.print("WINNER!");
    display.setTextSize(3);
    display.setCursor(45, 35);
    display.print("P");
    display.print(winnerId);
  }
  display.setTextSize(1);
  display.setCursor(10, 55);
  display.print("Press BIG BUTTON");
  if (wirelessConfirm)
  {
    wirelessConfirm = false;
    currentState = MAIN_MENU;
    delay(300);
  }
}