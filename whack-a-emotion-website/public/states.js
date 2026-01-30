//States for the game
const STATES = {
  IDLE: "IDLE",// Homepage
  SIGNIN: "SIGNIN",// Player info
  WAITING: "WAITING",// Waiting for ESP start
  PLAYING: "PLAYING",// 45s GIF movie
  WIN: "WIN",  LOSE: "LOSE",  SO_CLOSE: "SO_CLOSE", // results page
  LEADERBOARD: "LEADERBOARD",
  RULES: "RULES"
};

// store states in local storage to be able to survive pageloads
function setState(newState) {
  localStorage.setItem("GAME_STATE", newState);
  console.log("STATE →", newState); // debugging message
  navigateByState(newState);
}

function getState() {
  return localStorage.getItem("GAME_STATE") || STATES.IDLE;
}

//decides which page opens 
function navigateByState(state) {
  switch (state) {

    case STATES.IDLE:
      window.location.href = "homepage.html";
      break;

    case STATES.SIGNIN:
      window.location.href = "signin.html";
      break;

    case STATES.WAITING:
      window.location.href = "waiting.html";
      break;

    case STATES.PLAYING:
      window.location.href = "play.html";
      break;

    case STATES.WIN:
    case STATES.LOSE:
    case STATES.SO_CLOSE:
      window.location.href = "result.html";
      break;

    case STATES.LEADERBOARD:
      window.location.href = "leaderboard.html";
      break;

    case STATES.RULES:
      window.location.href = "rules.html";
      break;

    default:
      console.warn("Unknown state:", state);
      setState(STATES.IDLE);
  }
}

// on page load check state and redirect as needed
document.addEventListener("DOMContentLoaded", () => {
  const state = getState();
  console.log("Page loaded, current state:", state);
});

// temporary esp stimulation - callef in waiting state 
function simulateESP() {
  console.warn("⚠ ESP SIMULATION MODE");

  setTimeout(() => setState(STATES.PLAYING), 3000);      // after 3s
  setTimeout(() => setState(STATES.SO_CLOSE), 48000);    // after 48s
}

//real esp connection
function pollESP() {
  setInterval(async () => {
    try {
      const res = await fetch("http://192.168.4.1/state");
      const espState = await res.text();

      if (espState !== getState()) {
        setState(espState);
      }

    } catch (e) {
      console.log("ESP not reachable");
    }
  }, 1000);
}

db.ref("scores").push({
  name: localStorage.getItem("playerName"),
  university: localStorage.getItem("playerUni"),
  difficulty: localStorage.getItem("difficulty"),
  result: getState(),
  timestamp: Date.now()
});