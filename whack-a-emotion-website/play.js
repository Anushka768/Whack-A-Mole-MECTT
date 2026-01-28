// to fix mobile landscape view
function setViewportHeight() {
  const vh = window.innerHeight * 1;
  document.documentElement.style.setProperty("--vh", `${vh}px`);
}

setViewportHeight();
window.addEventListener("resize", setViewportHeight);// Grab all scenes
const scenes = document.querySelectorAll(".scene");

// Which scene is currently showing
let currentScene = 0;

// TOTAL duration = 45 sec
// 13 scenes → 3.45 sec each
const SCENE_DURATION = 4500;

/* Show next scene */
function nextScene() {
  scenes[currentScene].classList.remove("active");

  currentScene++;

  if (currentScene < scenes.length) {
    scenes[currentScene].classList.add("active");
  } else {
    // Notify system that play finished
    setState(STATES.RESULT);
  }
}

setInterval(nextScene, SCENE_DURATION);
