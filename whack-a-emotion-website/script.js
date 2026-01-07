//Firebase config
const firebaseConfig = {
  apiKey: "AIzaSyABpmiXQPgaUUBEKz4Ywt409FGGGaO8tDM",
  authDomain: "whack-a-emotion--mectt.firebaseapp.com",
  databaseURL: "https://whack-a-emotion--mectt-default-rtdb.firebaseio.com",
  projectId: "whack-a-emotion--mectt",
  storageBucket: "whack-a-emotion--mectt.firebasestorage.app",
  messagingSenderId: "922744927952",
  appId: "1:922744927952:web:e8ba8c05e32fc3e080d0d5",
  measurementId: "G-2D9C01YWXJ"
};

//Intialize Firebase
firebase.initializeApp(firebaseConfig);

//Reference database
const db = firebase.database();

function startGame() {
  alert("Start Game clicked");
  // Later this will open name + level screen
}

// Runs when Rules ball is clicked
function showRules() {
  alert("Rules clicked");
}

// Runs when Leaderboard ball is clicked
function showLeaderboard() {
  alert("Leaderboard clicked");
}