// result.js
const GAME_STATE = (localStorage.getItem("GAME_STATE") || "").toLowerCase();

const container = document.getElementById("result-container");

// score panel elements (must exist in result.html)
const scorePanel = document.getElementById("score-panel");

const scoreEl = document.getElementById("player-score");

/* GIF SEQUENCES */
const gifSets = {
  win: [
    { src: "joy1.jpg", duration: 1000 },
    { src: "joy2.jpg", duration: 1000 },
    { src: "joy4.jpg", duration: 1000 }
  ],
  lose: [
    { src: "lost1.jpg", duration: 1000 },
    { src: "lost2.jpg", duration: 1000 },
    { src: "lost3.jpg", duration: 1000 },
    { src: "lost4.jpg", duration: 1000 }
  ],
  close: [
    { src: "lost1.jpg", duration: 1000 },
    { src: "lost2.jpg", duration: 1000 },
    { src: "lost3.jpg", duration: 1000 },
    { src: "lost4.jpg", duration: 1000 }
  ]
};

let sequenceKey;
if (GAME_STATE == "win") sequenceKey = "win";
else if (GAME_STATE == "lose") sequenceKey = "lose";
else if (GAME_STATE == "close") sequenceKey = "close";
const sequence = gifSets[sequenceKey];

if (!sequence) {
  console.error("Invalid GAME_STATE:", GAME_STATE);
}

// Play GIFs one by one
let index = 0;

function playNext() {
  container.innerHTML = "";

  const img = document.createElement("img");
  img.src = sequence[index].src;
  img.className = "scene active";
  container.appendChild(img);

  const duration = sequence[index].duration;
  index++;

  if (index < sequence.length) {
    setTimeout(playNext, duration);
  } else {
    showFinalScore();
  }
}

// Show final score screen
async function showFinalScore() {
  const score = await fetchFinalScore();

  scoreEl.innerText = score ? `Final Score: ${score}` : "Score unavailable";

  scorePanel.classList.remove("hidden");
  
}

// Start playback
if (sequence) playNext();

setTimeout(() => {
  setState(STATE.IDLE);
}, 5000
); 
