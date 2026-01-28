// Firebase config
firebase.initializeApp({
  apiKey: "AIzaSyABpmiXQPgaUUBEKz4Ywt409FGGGaO8tDM",
  authDomain: "whack-a-emotion--mectt.firebaseapp.com",
  databaseURL: "https://whack-a-emotion--mectt-default-rtdb.firebaseio.com",
  projectId: "whack-a-emotion--mectt",
  storageBucket: "whack-a-emotion--mectt.appspot.com",
  messagingSenderId: "922744927952",
  appId: "1:922744927952:web:e8ba8c05e32fc3e080d0d5"
});

const tableBody = document.getElementById("table-body");
const uniFilter = document.getElementById("filter-uni");
const sortFilter = document.getElementById("filter-sort");
const todayFilter = document.getElementById("filter-today");

let allData = [];

// Read scores
firebase.database().ref("scores").on("value", snap => {
  allData = [];
  snap.forEach(child => allData.push(child.val()));
  populateUniversities();
  render();
});

function populateUniversities() {
  const unis = [...new Set(allData.map(p => p.university))];
  uniFilter.innerHTML = `<option value="ALL">All Universities</option>`;
  unis.forEach(u => {
    uniFilter.innerHTML += `<option value="${u}">${u}</option>`;
  });
}

function render() {
  tableBody.innerHTML = "";

  let filtered = [...allData];

  if (uniFilter.value !== "ALL") {
    filtered = filtered.filter(p => p.university === uniFilter.value);
  }

  if (todayFilter.checked) {
    const today = new Date().setHours(0,0,0,0);
    filtered = filtered.filter(p => p.timestamp >= today);
  }

  if (sortFilter.value === "score") {
    filtered.sort((a,b) => b.score - a.score);
  } else {
    filtered.sort((a,b) => b.timestamp - a.timestamp);
  }

  filtered.forEach((p, i) => {
    const row = document.createElement("tr");

    row.className =
      p.result === "WIN" ? "win" :
      p.result === "LOSE" ? "lose" : "close";

    row.innerHTML = `
      <td>${i + 1}</td>
      <td>${p.name}</td>
      <td>${p.university}</td>
      <td>${p.score}</td>
      <td>${p.result}</td>
    `;

    tableBody.appendChild(row);
  });
}

uniFilter.onchange = render;
sortFilter.onchange = render;
todayFilter.onchange = render;

function goHome() {
  setState(STATES.IDLE);
}
