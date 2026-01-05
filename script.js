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
firebaseConfig.initializeApp(firebaseConfig);

//Reference database
const db = firebaseConfig.database();

const statusDiv = document.getElementById("status");
const scoreDiv = document.getElementById("score");

//Game States
db.ref("game/state").on("value", snapshot => {
    const state = snapshot.val();
    statusDiv.innerText = "State: " + state;
});

//Score
db.ref("game/score").on("value", snapshot => {
    scoreDiv.innerText = "Score: "+ snapshot.val();
});