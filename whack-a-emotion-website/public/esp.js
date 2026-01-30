const ESP_IP = "http://192.168.4.1";

setInterval(async () => {
  try {
    const res = await fetch(`${ESP_IP}/state`);
    const state = await res.text();

    if (state === "PLAYING") {
      window.location.href = "play.html";
    }

    if (state === "WIN" || state === "LOSE" || state === "CLOSE") {
      window.location.href = `result.html?state=${state.toLowerCase()}`;
    }
  } catch (e) {
    console.log("ESP not reachable");
  }
}, 1000);
