// Whack-A-Mole Inside Out Edition: McMaster Engineering Concrete Toboggan Team 

// some knowledgable info
// pinMode + digitalWrite control electricity, LEDC controls waveforms, and MD_Parola controls devices.
// millis() : build-in Arduino Function which stores Number of milliseconds since the board powered on
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

enum GameState {
  IDLE, // shows MECTT
  COUNTDOWN, 
  PLAYING, // shows score, level and time left with a bar
  GAME_OVER // animation??
};

enum Difficulty { MEDIUM, HARD };

struct Mole {
  int ledPin;
  int buttonPin;
  bool active;
};

const int NUM_MOLES = 5;
const int RESET_BTN_PIN = 19;
const int DIFF_BTN_PIN  = 16;
const int BUZZER_PIN = 4;

// Game Timing
const unsigned long GAME_DURATION = 60000; // unsigned long used because time can never be negative
const unsigned long START_INTERVAL = 1000;
const unsigned long MIN_INTERVAL = 200;

// Matrix Setup ---------------------------------------------------------

# define HARDWARE_TYPE MD_MAX72XX::FC16_HW
# define MAX_DEVICES 4
# define CS_PIN 5 
# define DATA_PIN 21
# define CLK_PIN 22

// creates an object named matrix of type MD_Parola
MD_Parola matrix(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// initial game setup ---------------------------------------------------------------

GameState gameState = IDLE;
Difficulty difficulty = MEDIUM;

Mole moles[NUM_MOLES] = {
  {25, 14, false},
  {26, 27, false},
  {32, 12, false},
  {33, 13, false},
  {23, 18, false}
};

int score = 0;
int activeMole = -1; // -1 means no mole is on

// store time stamps instead of delays as delay freezes the game
unsigned long gameStartTime;
unsigned long lastMoleTime;
unsigned long moleInterval = START_INTERVAL;

// for the coundown on matrix
unsigned long countdownTimer;
int countdownValue = 3;

// set up Passive Buzzer --------------------------------------------------------------

const int BUZZER_CH = 0;

void buzzerHappy() {
  ledcWriteTone(BUZZER_CH, 1200); // Happy sound with 1200 Hz square wave
  delay(100); // tone keeps playing for the delay time 
  ledcWriteTone(BUZZER_CH, 1600); 
  delay(100);
  ledcWriteTone(BUZZER_CH,0);
}

void buzzerSad() {
  ledcWriteTone(BUZZER_CH, 400);
  delay(300);
  ledcWriteTone(BUZZER_CH, 0);
}

// Matrix Helper Function --------------------------------------------

void showText (const char* msg) { // meaning of having pointer here : msg points to a sequence of characters stored somewhere in memory, and I promise NOT to modify it
                                  // msg = address   |    msg => 'M' 'E' 'C' 'T' 'T' '/0'
                                  // *msg = first character
  matrix.displayClear(); // turns all LEDs off
  matrix.displayText(
    msg, // points to character array , passed by reference
    PA_CENTER, // centre the text
    80, // used for scrolling text - speed parameter
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


// Game Helper Functions------------------------------------------------------------

void clearMoles() {
  for(int i = 0; i < NUM_MOLES; i++) {
    moles[i].active = false;
    digitalWrite(moles[i].ledPin, LOW);
  }
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
    digitalWrite(moles[index].ledPin, HIGH); // digitalWrite to set pin state
  }

  lastMoleTime = millis();
}

void resetGame() {
  score = 0;
  moleInterval = START_INTERVAL;
  gameState = COUNTDOWN;
  countdownValue = 3;
  countdownTimer = millis();
  clearMoles();
}

void endGame() {
  gameState = GAME_OVER;
  clearMoles();

  if (score >= 50) {
    showText("WIN");
    buzzerHappy();;
  }
  else {
    showText("LOST");
    buzzerSad();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RESET_BTN_PIN, INPUT_PULLUP);
  pinMode(DIFF_BTN_PIN, INPUT_PULLUP);

  for(int i = 0; i < NUM_MOLES; i++) {
    pinMode(moles[i].ledPin, OUTPUT); //pinMode configures the pin to behave certain way (OUTPUT in this case)
    pinMode(moles[i].buttonPin, INPUT_PULLUP); 
  }

  ledcAttach(BUZZER_PIN, 2000, 8); // 2000 - frequency(Hz), 8 - Resolution(bits),  we use ledc for buzzer as it takes square wave form

  matrix.begin(); // makes the display library ready
  matrix.setIntensity(5); // brightness
  matrix.displayClear();

  showText("MECTT");
  randomSeed(millis());
}

void loop() {
  
  // toggle difficulty
  if(digitalRead(DIFF_BTN_PIN) == LOW) {
    difficulty = (difficulty == MEDIUM) ? HARD : MEDIUM;
    showText(difficulty == HARD ? "HARD" : "MED");
    delay(500);
  }

  // Start or Reset
  if (digitalRead(RESET_BTN_PIN) == LOW && (gameState == IDLE || gameState == GAME_OVER)) {
    resetGame();
    delay(300);
  }

  if (gameState == COUNTDOWN) {
    if(millis() - countdownTimer >= 1000) { // asks if 1 second has passed since the last countdown update
      countdownTimer = millis(); // reset timer
      showCountdown(countdownValue--);

      if(countdownValue == 0) {
        showText("GO!");
        delay(300);
        gameState = PLAYING;
        gameStartTime = millis();
        spawnMole();
      }
    }
  }

  if (gameState == PLAYING) {
    if(millis() - gameStartTime >= GAME_DURATION) {
      endGame();
      return;
    }

    if(millis() - lastMoleTime >= moleInterval) {
      spawnMole();
    }

    showScore();

    for(int i = 0; i < NUM_MOLES; i++) {
      if(digitalRead(moles[i].buttonPin) == LOW) {

        if(moles[i].active) {
          score++;
          buzzerHappy();
          moleInterval = max(MIN_INTERVAL, START_INTERVAL - score * 10); // max: prevents impossible speed, score*10: faster per point, START_INTERVAL: initial slow speed
        }
        else{
          score--;
          buzzerSad();
        }

        clearMoles();
        delay(150);
      }
    }
  }
  if (gameState == IDLE) {
    showText("MECTT");
  }
}
