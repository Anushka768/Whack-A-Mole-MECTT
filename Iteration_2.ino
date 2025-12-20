/*
Key design ideas:
 - enum → game states (clean logic, no magic booleans)
 - struct → group LED + button per mole
 - millis() → non-blocking timing - to let program run by returning immediately 
 - INPUT_PULLUP → stable button readings
 
 Button 1: LED 25 | Switch 14
Button 2: LED 26 | Switch 27
Button 3: LED 32 | Switch 12
Button 4: LED 33 | Switch 13
Button 5: LED 23 | Switch 18
Reset:    GPIO 19
*/


#include <Arduino.h>

// ENUMS
enum GameState { // to define all valid states
  IDLE,
  PLAYING,
  GAME_OVER
};

enum GameResult { // for final outcome
  NONE,
  WIN,
  LOSE
};

// Structs - for hardware
struct Mole { 
  int ledPin;
  int buttonPin;
  bool active;      // true if mole is currently up
};

// Constants
const int NUM_MOLES = 5;       // Total number of arcade buttons
const int RESET_BTN = 19;      // Reset / Start button

// timing constants
const unsigned long GAME_DURATION = 60000; // 60 seconds total game time
const unsigned long START_INTERVAL = 1200; // Initial mole speed (ms)
const unsigned long MIN_INTERVAL = 300;    // Fastest allowed speed

// Global Variables
GameState gameState = IDLE;
GameResult gameResult = NONE;

Mole moles[NUM_MOLES] = { // hardware
  {25, 14},
  {26, 27},
  {32, 12},
  {33, 13},
  {23, 18}
};

// Game data
int score = 0;
int activeMole = -1;             // Which mole is currently ON

// Timing variables
unsigned long gameStartTime = 0;
unsigned long lastMoleTime = 0; // stores the time current mole appeareded
unsigned long moleInterval = START_INTERVAL;

// Function Declaration
void resetGame();
void spawnMole();
void clearMoles();
void updateGame();
void handleHit();
void endGame();


void setup() {
  Serial.begin(115200);

  // Reset button
  pinMode(RESET_BTN, INPUT_PULLUP);  // pin mode ia ESP32 Function

  // Initialize all moles
    for (int i = 0; i < NUM_MOLES; i++) {
      pinMode(moles[i].ledPin, OUTPUT); // pin sends voltage out to turn LED ON
      pinMode(moles[i].buttonPin, INPUT_PULLUP); // pin gets input with internal pull-up resistor for stable logic when button is not pressed
      digitalWrite(moles[i].ledPin, LOW); // LEDs off initially
      moles[i].active = false;                   // All moles inactive initially
  }

    randomSeed(millis());

    Serial.println("System Ready. Press RESET Button to start.");
}

void loop() {

  unsigned long now = millis();
  /*
    RESET BUTTON CHECK
    Active-LOW logic:
    - Not pressed → HIGH
    - Pressed → LOW
  */
  // RESET / START BUTTON - always checked
  if (digitalRead(RESET_BTN) == LOW) { // when pressed digitalread = LOW , when not pressed dogotalread = HIGH
    if (gameState == IDLE || gameState == GAME_OVER) {
    resetGame();
    delay(300); // debounce
  }
}

  // Only run game logic when playing
  if (gameState == PLAYING) {
    updateGame();

    // Check mole buttons
    for (int i = 0; i < NUM_MOLES; i++) {
      if (digitalRead(moles[i].buttonPin) == LOW) {
        handleHit(i);
        delay(150); // debounce
      }
    }
  }
}

void resetGame() {
  score = 0;
  moleInterval = START_INTERVAL;
  activeMole = -1;

  gameState = PLAYING;
  gameResult = NONE;

  // Capture the start time of the game
  gameStartTime = millis();
  lastMoleTime = millis();

  clearMoles();
  spawnMole();

  Serial.println("=== NEW GAME STARTED ===");
}

void updateGame() {

  // End game after 60 seconds
  if (millis() - gameStartTime >= GAME_DURATION) {
    endGame();
    return;
  }

    /*
    Non-blocking timing:
    Instead of delay(), elapsed time is checked.
    This keeps the system responsive to buttons and future modules.
  */
  if (millis() - lastMoleTime >= moleInterval) {
    spawnMole();
    lastMoleTime = millis();
  }
}

void spawnMole() {

  // Turn off all LEDs first
  clearMoles();

  int newMole;

  // Prevent same mole from appearing twice consecutively
  do {
    newMole = random(0, NUM_MOLES);
  } while (newMole == activeMole);

  activeMole = newMole;
  moles[activeMole].active = true;

  // Turn ON the selected mole LED
  digitalWrite(moles[activeMole].ledPin, HIGH);
}

void handleHit(int moleIndex) {

  // Correct mole hit
  if (moles[moleIndex].active) {
    score++;
    Serial.print("HIT! Score: ");
    Serial.println(score);

    // Speed up game gradually
    moleInterval = max(MIN_INTERVAL, START_INTERVAL - score * 10);
  } 
  // Wrong button pressed
  else {
    score--;
    Serial.print("MISS! Score: ");
    Serial.println(score);
  }

  // Turn off all moles after hit/miss
  clearMoles();
  activeMole = -1;
}

void clearMoles() {

  // Turn off every LED and clear active flags
  for (int i = 0; i < NUM_MOLES; i++) {
    moles[i].active = false;
    digitalWrite(moles[i].ledPin, LOW);
  }
}

void endGame() {

  gameState = GAME_OVER;
  clearMoles();

  // win/lose condition
  if (score >= 50) {
    gameResult = WIN;
    Serial.println("=== YOU WIN ===");
  } else {
    gameResult = LOSE;
    Serial.println("=== YOU LOSE ===");
  }

  Serial.print("FINAL SCORE: ");
  Serial.println(score);

}