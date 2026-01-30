function startGame() {
  const name = document.getElementById("playerName").value.trim();
  const uni = document.getElementById("playerUni").value.trim();
  const difficulty = document.getElementById("difficulty").value;
  const error = document.getElementById("error");

  if (!name || !uni) {
    error.innerText = "Please fill all fields";
    return;
  }

  // Save player info
  localStorage.setItem("playerName", name);
  localStorage.setItem("playerUni", uni);
  localStorage.setItem("difficulty", difficulty);

  // Reset old game data (VERY IMPORTANT)
  localStorage.removeItem("score");
  localStorage.removeItem("SAVED");

  // Move using global state machine
  setState(STATES.WAITING);
}
