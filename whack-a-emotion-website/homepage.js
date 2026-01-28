// Start Game button
function startGame() {
  // Move to Sign In page
  setState(STATES.SIGNIN);
}

// Rules button
function showRules() {
  setState(STATES.RULES);
}

// Leaderboard button
function showLeaderboard() {
  setState(STATES.LEADERBOARD);
}
