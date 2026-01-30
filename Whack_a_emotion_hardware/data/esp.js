// esp.js
const ESP_IP = "http://192.168.4.1";

// Poll ESP32 for current game state
function pollESPState() {
  setInterval(async () => {
    try {
      const res = await fetch(`${ESP_IP}/state`);
      const espState = (await res.text()).trim();

      if (espState && espState !== getState()) {
        console.log("ESP STATE →", espState);
        setState(espState);
      }
    } catch (err) {
      console.log("ESP not reachable");
    }
  }, 1000);
}

// Fetch final score from ESP32 (used on result page)
async function fetchFinalScore() {
  try {
    const res = await fetch(`${ESP_IP}/score`);
    return (await res.text()).trim();
  } catch (err) {
    console.warn("Failed to fetch score from ESP");
    return null;
  }
}
