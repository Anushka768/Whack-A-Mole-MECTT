const GAME_STATE = localStorage.getItem("GAME_STATE"); // WIN / LOSE / CLOSE
const ESP_IP = "http://192.168.4.1";

//fetch score
fetch(`${ESP_IP}/score`)
  .then(res => res.text())
  .then(score => {
    saveToFirebase(parseInt(score));
  });

// save fetched score to firebase
function saveToFirebase(score) {
  const name = localStorage.getItem("playerName");
  const uni = localStorage.getItem("playerUni");
  const difficulty = localStorage.getItem("difficulty");

  firebase.database().ref("leaderboard").push({
    name,
    university: uni,
    difficulty,
    score,
    time: Date.now()
  });
}

/* Each state has EXACTLY 5 GIFs */
const gifSets = {
  win: [
    { src: "joy1.gif", duration: 1000 },
    { src: "joy2.gif", duration: 1000 },
    { src: "joy3.gif", duration: 2500 },
    { src: "joy4.gif", duration: 4800 },
    { src: "joy5 together.gif", duration: 1000 }
  ],

  lose: [
    { src: "lost1.gif", duration: 2000 },
    { src: "lost2.gif", duration: 4400 },
    { src: "lost3.gif", duration: 2500 }, 
    { src: "lost4.gif", duration: 2000 }
  ],
  close: [
    { src: "lost1.gif", duration: 2000 },
    { src: "lost2.gif", duration: 4400 },
    { src: "lost3.gif", duration: 2500 }, 
    { src: "lost4.gif", duration: 2000 }
  ]
};

const container = document.getElementById("result-container");
const sequence = gifSets[GAME_STATE];

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
    // Save score ONCE
    setState(STATES.LEADERBOARD);
  }
}

playNext();