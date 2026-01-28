// Whack-A-Mole: Inside Out Edition
// McMaster Engineering Concrete Toboggan Team 

// some knowledgable info
// pinMode + digitalWrite control electricity, and MD_Parola controls devices.
// millis() : build-in Arduino Function which stores Number of milliseconds since the board powered on
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// wi-fi setup for website sync---------------------------------------
# include <WiFi.h> // creates esp wi-fi
#include <WebServer.h> // allow esp32 to host a website

//wifi credentials
const char* ssid = "INSLED-OUT";
const char* password = "12345678";

//webServer port 80
WebServer server(80);

String webState = "WAITING";

enum GameState {
  IDLE, // shows MECTT
  COUNTDOWN, 
  PLAYING, // shows score, level and time left with a bar
  GAME_OVER // animation??
};

enum Difficulty { MEDIUM, HARD };

struct Mole {
  uint8_t ledIndex; // 0-7 outputs of 74HC595N (QA-QH)
  int buttonPin;
  bool active;
  bool wasHit;
};

const int NUM_MOLES = 5;
const int RESET_BTN_PIN = 32;
const int DIFF_BTN_PIN  = 33;
const int BUZZER_PIN = 17;

// Winning Score ------------------------------------------------------------
const int WIN_SCORE_MED = 30;
const int WIN_SCORE_HARD = 40;

// Button Edge Tracking -------------------------------------------------------
bool lastResetBtn = HIGH;
bool lastDiffBtn = HIGH;

// Game Timing----------------------------------------------------------------------
const unsigned long GAME_DURATION = 45000; // unsigned long used because time can never be negative
const unsigned long START_INTERVAL = 1000;
const unsigned long MIN_INTERVAL = 300;
const unsigned long GAME_OVER_TIME = 10000; // 15 sec after game end the game goes back to idle state

//sends current game state
void handleState() {
  server.send(200, "text/plain", webState);
}

void handleRoot() {
  server.send(200, "text/plain", "ESP32 RUNNING");
}

int score = 0;
void handleScore() {
  server.send(200, "text/plain", String(score));
}

// Shift Register setup (SN74HC595N) with helper functions------------------------------------------------
#define SR_DATA 27 //SER
#define SR_CLOCK 26 //SRCLK
#define SR_LATCH 25 //RCLK

uint8_t srState = 0b00000000; // state variable containing 8 bits of data
                // 76543210 if Bit = 1 means ON, if 0 means OFF

void shiftRegisterWrite () { // send State from ESP32 to 74HC595
  digitalWrite(SR_LATCH, LOW); // disconnects outputs QA-QH from changes : new data is being sent but prevents it from showing yet
  shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, srState); // sends 8 bit of data: each clk pulse pushes one bit at a time as prev bit bits move 1 position 
                                                  // Uses--- SR_DATA: SER pin, SR_CLOCK: SRCLK pin---- MSBFIRST(sends bit 7 first) 
  digitalWrite(SR_LATCH, HIGH); //LEDs update all at once (no flicker)
}

void setMoleLED(uint8_t index, bool on) { // index = which LED, on/off = true/false
  if(on) {
    srState |= (1 << index); // (<< : logical left shift) (| : bitwise OR)
  }                          // (| with 1 forces HIGH), (| with 0 leaves bit unchanged)
                             // eg: (1 << 3 = 00001000)  00101001 | 00001000  = 00101001
  else {
    srState &= ~(1 << index); // (& : bitwise AND) (~ : bitwise negation(complement))
  }                           // target bit is 0, all other bits are 1 because of ~, & with 0 forces LOW, & with 1 stays unchanged
                              // eg: (1 << 3 = 00001000), ~00001000 = 11110111, 00101001 & 11110111 = 00100001
  shiftRegisterWrite();
}

void clearAllMoleLEDs() { // sets all bits to 0, all outputss go to LOW, hence all LEDs turned off
  srState = 0;
  shiftRegisterWrite();
}

// Matrix Setup ---------------------------------------------------------

# define HARDWARE_TYPE MD_MAX72XX::FC16_HW
# define MAX_DEVICES 4
# define CS_PIN 15 
# define DATA_PIN 21
# define CLK_PIN 22

// creates an object named matrix
MD_Parola matrix(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// initial game setup ---------------------------------------------------------------

GameState gameState = IDLE;
Difficulty difficulty = MEDIUM;

Mole moles[NUM_MOLES] = {
  {1, 16, false, false}, //QB
  {2, 23, false, false}, //QC
  {3, 19, false, false}, //QD
  {4, 13, false, false}, //QE
  {0, 18, false, false}  //QA
};

//int activeMole = -1; // -1 means no mole is on

// store time stamps instead of delays as delay freezes the game
unsigned long gameStartTime;
unsigned long lastMoleTime;
unsigned long moleInterval = START_INTERVAL;
unsigned long gameOverStartTime = 0;

// for the coundown on matrix
unsigned long countdownTimer;
int countdownValue = 3;

// for checking if player won or lost
bool didWin = false;

// set up Active Buzzer --------------------------------------------------------------
void buzzerOn() {
  digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff() {
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzerHappy() {
  buzzerOn();
  delay(100);
  buzzerOff();
  delay(60);
  buzzerOn();
  delay(120);
  buzzerOff();
}

void buzzerSad() {
  buzzerOn();
  delay(400);
  buzzerOff();
}

// Matrix Helper Function --------------------------------------------

void showText (const char* msg) { // meaning of having pointer here : msg points to a sequence of characters stored somewhere in memory, and I promise NOT to modify it
                                  // msg = address   |    msg => 'M' 'E' 'C' 'T' 'T' '/0'
                                  // *msg = first character
  matrix.displayClear(); // turns all LEDs off
  matrix.displayText(
    msg, // points to character array , passed by reference
    PA_CENTER, // centre the text
    0, // used for scrolling text - speed parameter
    0, // pause parameter for animation
    PA_PRINT, // effect in - alternatives: PA_SCROLL_LEFT, PA_SCROLL_RIGHT, PA_FADE
    PA_NO_EFFECT); // effect out 
  while (!matrix.displayAnimate()) {} // keeps calling displayAnimate() until text is fully drawn on matrix
}

void showScore() {
  char buf[10];
  sprintf(buf, "%d", score); // sprintf means: “Format text and store it in a string.”
  showText(buf);
}

void showCountdown( int n ) {
  char buf[2];
  sprintf(buf, "%d", n);
  showText(buf);
}

void showTimeBar () {
  unsigned long elapsed = millis() - gameStartTime;
  if (elapsed > GAME_DURATION) elapsed = GAME_DURATION;

  int litCols = map(GAME_DURATION - elapsed, 0, GAME_DURATION, 0, 32); // 4 modules, 8 columns each -- 4 × 8 = 32 columns

  for (int col = 0; col < 32; col++) {
    mx.setPoint(7, col, col < litCols);
  }
}

// Game Helper Functions------------------------------------------------------------

void clearMoles() {
  for(int i = 0; i < NUM_MOLES; i++) {
    moles[i].active = false;
    moles[i].wasHit = false;
  }
  clearAllMoleLEDs();
}

void spawnMole() {
    
  clearMoles();

  int count = (difficulty == HARD) ? random(2, 4) : 1;

  for(int i = 0; i < count; i++) {
    int index;

    do { // prevent same mole from lighting again
      index = random(0, NUM_MOLES);
    } while (moles[index].active);

    moles[index].active = true;
    moles[index].wasHit = false;
    setMoleLED(moles[index].ledIndex, true); // digitalWrite to set pin state
  }

  lastMoleTime = millis();
}

void evaluateMoles() {
  int hits = 0;
  int misses = 0;

  for(int i = 0; i < NUM_MOLES; i++) {
    if( moles[i].active) {
      if(moles[i].wasHit) hits++;
      else misses++;
    }
  }
  score = max(0, score - misses);
  if (hits > 0) buzzerHappy();
  else if (misses > 0) buzzerSad();
  
  showScore();
  
  moleInterval = max(MIN_INTERVAL, START_INTERVAL - score * 10); // max: prevents impossible speed, score*10: faster per point, START_INTERVAL: initial slow speed
}

void resetGame() {
  score = 0;
  moleInterval = START_INTERVAL;
  countdownValue = 3;
  countdownTimer = millis();
  clearMoles();
  gameState = COUNTDOWN;
  webState = "WAITING";
}

void endGame() {
  clearMoles();
  gameState = GAME_OVER;
  gameOverStartTime = millis();
  
  int winScore = (difficulty == MEDIUM) ? WIN_SCORE_MED : WIN_SCORE_HARD; 

  if (score >= winScore) {
    showText("WIN");
    webState = "WIN";
  }
  else if (score >= winScore - 3) {
    showText("SO CLOSE");
    webState = "CLOSE";
  }
  else {
    showText("LOST");
    webState = "LOSE";
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  // all led light up when plugges in
  srState = 0xFF;
  shiftRegisterWrite();

  pinMode(RESET_BTN_PIN, INPUT_PULLUP); //pinMode configures the pin to behave certain way (INPUT_PULLUP in this case)
  pinMode(DIFF_BTN_PIN, INPUT_PULLUP);

  pinMode(SR_DATA, OUTPUT);
  pinMode(SR_CLOCK, OUTPUT);
  pinMode(SR_LATCH, OUTPUT);

  clearAllMoleLEDs();

  for (int i = 0; i < NUM_MOLES; i++) {
  pinMode(moles[i].buttonPin, INPUT_PULLUP);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  mx.begin(); // for MD_MAX72XX
  matrix.begin(); // makes the display library ready for MD Parola
  matrix.setIntensity(5); // brightness
  matrix.displayClear();

  showText("MECTT");
  webState = "WAITING";
  randomSeed(millis());

  // Website Setup=========================================
  WiFi.softAP(ssid, password); // start esp32 as wi-fi hotspot
  Serial.println("ESP32 AP STARTED");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/state", handleState);
  server.on("/score", handleScore);
  server.begin();
}

void loop() {
    
  bool resetBtn = digitalRead(RESET_BTN_PIN);
  bool diffBtn  = digitalRead(DIFF_BTN_PIN);

  // Reset Btn
  if (lastResetBtn == HIGH && resetBtn == LOW) {
    if (gameState == IDLE || gameState == GAME_OVER) {
      resetGame();
    }
    delay(200);
  }
  lastResetBtn = resetBtn;

  // Difficulty Btn
  if ((gameState == IDLE || gameState == COUNTDOWN || gameState == GAME_OVER) && lastDiffBtn == HIGH && diffBtn == LOW) {
    difficulty = (difficulty == MEDIUM) ? HARD : MEDIUM;
    showText(difficulty == HARD ? "HARD" : "MED");
    delay(300);
  }
  lastDiffBtn = diffBtn;
  // Countdown
  if (gameState == COUNTDOWN) {
    if(millis() - countdownTimer >= 1000) { // asks if 1 second has passed since the last countdown update
      countdownTimer = millis(); // reset timer
      showCountdown(countdownValue--);

      if(countdownValue == 0) {
        showText("GO!");
        delay(300);
        matrix.displayClear();
        gameState = PLAYING;
        webState = "PLAYING";
        gameStartTime = millis();
        spawnMole();
      }
    }
  }

  if (gameState == PLAYING) {
    showTimeBar ();
    if(millis() - gameStartTime >= GAME_DURATION) {
      endGame();
      return;
    }

    if(millis() - lastMoleTime >= moleInterval) {
      evaluateMoles();
      spawnMole();
    }

   // logic to turn off moles when their button is hit 
    for(int i = 0; i < NUM_MOLES; i++) {
      if(digitalRead(moles[i].buttonPin) == LOW) {

        if(moles[i].active && !moles[i].wasHit) {
          moles[i].wasHit = true;
          setMoleLED(moles[i].ledIndex, false);

          // to update score immediately
          score++;
          buzzerHappy();
          showScore();
        }
        delay(40);
      }
    }
  }
  if(gameState == GAME_OVER) {
      if(millis() - gameOverStartTime >= GAME_OVER_TIME) {
        score = 0;
        moleInterval = START_INTERVAL;
        clearMoles();
        
        gameState = IDLE;
        showText("MECTT");
        webState = "WAITING";
      }
  } 

  // website loop ===========================================
  server.handleClient();
}