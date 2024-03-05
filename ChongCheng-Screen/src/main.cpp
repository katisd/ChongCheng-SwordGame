#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_now.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long previousMillis = 0;          // Variable to store the last time the function was called
const unsigned long timeoutInterval = 500; // Timeout interval in milliseconds

struct struct_game {
  uint8_t gameStage;
  uint8_t playerNumber;
  uint8_t action;
  int player1health;
  int player2health;
} myData;

volatile bool newDataAvailable = false;

void displayWaiting() {
  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 10);
  display1.println("Waiting...");
  display1.display();

  display2.clearDisplay();
  display2.setTextSize(2);
  display2.setTextColor(WHITE);
  display2.setCursor(0, 10);
  display2.println("Waiting...");
  display2.display();
}

void displayPlaying(int playerNum) {
  Adafruit_SSD1306 *display;
  if (playerNum == 1) {
    display = &display1;
  } else {
    display = &display2;
  }

  display->clearDisplay();
  display->setTextSize(2);
  display->setTextColor(WHITE);
  display->setCursor(0, 10);
  display->println("Player: " + String(playerNum));
  display->println("HP: " + String(playerNum == 1 ? myData.player1health : myData.player2health));
  display->display();
}

void displayGotHit(int playerNum) {
  Adafruit_SSD1306 *display;
  if (playerNum == 1) {
    display = &display1;
  } else {
    display = &display2;
  }

  previousMillis = millis();
  display->clearDisplay();
  display->setTextSize(2);
  display->setTextColor(WHITE);
  display->setCursor(0, 10);
  display->println();
  display->println("Got Hit!");
  display->display();
}

void displayBlock(int playerNum) {
  Adafruit_SSD1306 *display;
  if (playerNum == 1) {
    display = &display1;
  } else {
    display = &display2;
  }

  previousMillis = millis();
  display->clearDisplay();
  display->setTextSize(2);
  display->setTextColor(WHITE);
  display->setCursor(0, 10);
  display->println();
  display->println("Block!");
  display->display();
}

void displayWin(int playerNum) {
  sleep(1);
  Adafruit_SSD1306 *displayWin;
  Adafruit_SSD1306 *displayLose;
  if (playerNum == 1) {
    displayWin = &display1;
    displayLose = &display2;
  } else {
    displayWin = &display2;
    displayLose = &display1;
  }

  displayWin->clearDisplay();
  displayWin->setTextSize(2);
  displayWin->setTextColor(WHITE);
  displayWin->setCursor(0, 10);
  displayWin->println("Player " + String(playerNum));
  displayWin->println("You Win!");
  displayWin->println("Yay!");
  displayWin->display();

  displayLose->clearDisplay();
  displayLose->setTextSize(2);
  displayLose->setTextColor(WHITE);
  displayLose->setCursor(0, 10);
  displayLose->println("Player " + String(playerNum == 1 ? 2 : 1));
  displayLose->println("You Lose!");
  displayLose->println("Boo!");
  displayLose->display();
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Game Stage: ");
  Serial.println(myData.gameStage);
  Serial.print("Player Number: ");
  Serial.println(myData.playerNumber);
  Serial.print("Action: ");
  Serial.println(myData.action);

  if (myData.gameStage == 0) {
    displayWaiting();

  } else if (myData.gameStage == 1) {
    if (myData.action == 1) {
      displayBlock(myData.playerNumber);
      displayPlaying(myData.playerNumber == 1 ? 2 : 1);
    } else if (myData.action == 2) {
      displayGotHit(myData.playerNumber);
      displayPlaying(myData.playerNumber == 1 ? 2 : 1);
    } else if (myData.action == 0) {
      displayPlaying(1);
      displayPlaying(2);
    }

  } else if (myData.gameStage == 2) {
    displayWin(myData.playerNumber);
  }
  // newDataAvailable = true;
}

void setup() {
  Serial.begin(9600);
  Serial.println("ESP-NOW Receiver");
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (!display1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed display1"));
    for (;;)
      ;
  }

  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed display2"));
    for (;;)
      ;
  }

  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 10);
  display1.println("Player 1");
  display1.println("connecting...");
  display1.display();

  display2.clearDisplay();
  display2.setTextSize(2);
  display2.setTextColor(WHITE);
  display2.setCursor(0, 10);
  display2.println("Player 2");
  display2.println("connecting...");
  display2.display();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    display1.clearDisplay();
    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.setCursor(0, 10);
    display1.println("Connection");
    display1.println("failed");
    display1.display();

    display2.clearDisplay();
    display2.setTextSize(2);
    display2.setTextColor(WHITE);
    display2.setCursor(0, 10);
    display2.println("Connection");
    display2.println("failed");
    display2.display();
    return;
  }

  sleep(1);

  display1.clearDisplay();
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 10);
  display1.println("Connected!");
  display1.display();

  display2.clearDisplay();
  display2.setTextSize(2);
  display2.setTextColor(WHITE);
  display2.setCursor(0, 10);
  display2.println("Connected!");
  display2.display();

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  // reset the display after showing the hit/block message
  if ((millis() - previousMillis) > timeoutInterval && myData.gameStage == 1) {
    displayPlaying(1);
    displayPlaying(2);
  }
}
