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
const db = firebase.database();

//hides all screens
function hideAllScreens() {
  document.querySelectorAll('.screen') //document: webpage, querySelectorAll : find all elements that match the class selector in brackets
    .forEach(screen=> {
      screen.classList.remove('active'); // remove active class from each list
    });
}

function goHome(){
  hideAllScreens();
  document.getElementById('home-screen') // getElementById finds one element by ID
    .classList.add('active');
}

function goToSetup(){
  alert("Player Setup");
}

function showRules(){
  hideAllScreens();
  document.getElementById('rules-screen')
    .classList.add('active');
}

function showLeaderboard() {
  hideAllScreens();
  document.getElementById('leaderboard-screen')
    .classList.add('active');
}