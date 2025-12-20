/*
Button 1: LED 25 | Switch 14
Button 2: LED 26 | Switch 27
Button 3: LED 32 | Switch 12
Button 4: LED 33 | Switch 13
Button 5: LED 23 | Switch 18
Reset:    GPIO 34
*/

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
  bool active; // Is the mole currently UP???
};

// Constants
const int NUM_MOLES = 5;       // Total number of arcade buttons
const int RESET_BTN = 34;      // Reset / Start button

// timing constants
const unsigned long GAME_DURATION = 60000; // 60 seconds total game time
const unsigned long START_INTERVAL = 1200; // Initial mole speed (ms)
const unsigned long MIN_INTERVAL = 300;    // Fastest allowed speed

// Global Variables
GameState gameState = IDLE;
GameResult gameResult = NONE;

Mole moles[NUM_MOLES] = { // hardware
  {25, 14, false},
  {26, 27, false},
  {32, 12, false},
  {33, 13, false},
  {23, 18, false}
};

// Game data
int currentMole = -1; //index for active mole - (-1 = none)
int score = 0;

// Timing variables
unsigned long gameStartTime = 0;
unsigned long lastMoleTime = 0; // stores the time current mole appeareded
unsigned long moleInterval = START_INTERVAL;

// Function Declaration
void resetGame();
void spawnMole();
void clearMoles();
void updateGame();
void handleButtons();
void endGame();

// SETUP
void setup() {
  Serial.begin(115200);

  // Reset button
  pinMode(RESET_BTN, INPUT_PULLUP);  // pin mode ia ESP32 Function

  // Initialize all moles
    for (int i = 0; i < NUM_MOLES; i++) {
      pinMode(moles[i].ledPin, OUTPUT); // pin sends voltage out to turn LED ON
      pinMode(moles[i].buttonPin, INPUT_PULLUP); // pin gets input with internal pull-up resistor for stable logic when button is not pressed
      digitalWrite(moles[i].ledPin, LOW); // LEDs off initially
    }

    randomSeed(millis());

    Serial.println("System Ready. Press RESET Button to start.");
}

// MAIN LOOP

