const statusElement = document.getElementById("status");
const toggleButton = document.getElementById("toggle-alerts");

async function fetchSensorData() {
  const response = await fetch("/api/sensors");
  const data = await response.json();
  statusElement.innerHTML = `
    Water Level 1: ${data.waterLevel1.toFixed(2)} m<br>
    Water Level 2: ${data.waterLevel2.toFixed(2)} m<br>
    Water Level 3: ${data.waterLevel3.toFixed(2)} m<br>
    Flow Rate: ${data.flowRate.toFixed(2)} L/min
  `;
}

toggleButton.addEventListener("click", async () => {
  const response = await fetch("/api/toggle-notifications", { method: "POST" });
  const data = await response.json();
  alert(`Notifications are now ${data.notifications ? "enabled" : "disabled"}`);
});

setInterval(fetchSensorData, 1000);
