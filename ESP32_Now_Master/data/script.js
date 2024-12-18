// Get current sensor readings when the page loads
window.addEventListener('load', getReadings);

// Fetch sensor data from the ESP32 server for real-time readings
/*function getReadings() {
  fetch('/readings')
    .then(response => response.json())
    .then(data => {
      updateGauges(data);
    })
    .catch(error => {
      console.error('Error fetching readings:', error);
    });
}*/

function getReadings(month) {
  const url = month === 'november' ? '/readings1' : '/readings2';
  fetch(url)
    .then(response => response.json())
    .then(data => {
      updateGauges(data);
    })
    .catch(error => {
      console.error('Error fetching readings:', error);
    });
}

// Update the gauges with real-time data from the ESP32
function updateGauges(data) {
  // Assuming the data contains temperature, humidity, moisture, and lux
  gaugeTemp.value = data.temperature;
  gaugeHum.value = data.humidity;
  gaugeMoisture.value = data.moisture;
  gaugeLight.value = data.lux;

  // Dynamically update the gauges
  gaugeTemp.update();
  gaugeHum.update();
  gaugeMoisture.update();
  gaugeLight.update();
}

let currentFile = null; // Variable to store the current file being viewed

// Handle Show Graphs Button Click for November
document.getElementById('novemberBtn').addEventListener('click', function() {
  currentFile = 'november'; // Set the current file to November data
  const graphsSection = document.getElementById('graphs-section');
  const gaugeSection = document.getElementById('gauge-section');

  // Show the graphs section
  graphsSection.style.display = 'block';
  gaugeSection.style.display = 'block';

  // Fetch and display graphs for November
  updateGraphs(currentFile);
  updateGauges(currentFile);
});

// Handle Show Graphs Button Click for December
document.getElementById('decemberBtn').addEventListener('click', function() {
  currentFile = 'december'; // Set the current file to December data
  const graphsSection = document.getElementById('graphs-section');

  // Show the graphs section
  graphsSection.style.display = 'block';

  // Fetch and display graphs for December
  updateGraphs(currentFile);
});

// Fetch historical data from the SD card for a specific month (November or December)
function fetchSDCardData(month) {
  const url = month === 'november' ? '/november-data' : '/december-data';
  return fetch(url)
    .then(response => {
      if (!response.ok) {
        throw new Error('Failed to fetch data from the server');
      }
      return response.json();
    })
    .catch(error => {
      console.error(`Error fetching ${month} data:`, error);
    });
}

// Fetch and plot the data from the selected month (November or December)
function updateGraphs(month) {
  fetchSDCardData(month).then(data => {
    if (data && data.length > 0) {
      // Plot the graphs with the data for the selected month
      updateGraph(temperatureGraph, data.map(d => ({ timestamp: d.timestamp, value: d.temperature })), 'Temperature (°C)', 'rgba(255, 99, 132, 1)');
      updateGraph(humidityGraph, data.map(d => ({ timestamp: d.timestamp, value: d.humidity })), 'Humidity (%)', 'rgba(54, 162, 235, 1)');
      updateGraph(moistureGraph, data.map(d => ({ timestamp: d.timestamp, value: d.soil_moisture })), 'Soil Moisture (%)', 'rgba(75, 192, 192, 1)');
      updateGraph(lightGraph, data.map(d => ({ timestamp: d.timestamp, value: d.light_intensity })), 'Light Intensity (lux)', 'rgba(255, 206, 86, 1)');
    } else {
      console.warn(`No data available for ${month}.`);
    }
  });
}

// Filter graphs based on the selected date range
function filterGraphsByDate(startDate, endDate) {
  fetchSDCardData().then(data => {
    const filteredData = data.filter(d => {
      const date = new Date(d.timestamp);
      return date >= new Date(startDate) && date <= new Date(endDate);
    });

    updateGraph(temperatureGraph, filteredData.map(d => ({ timestamp: d.timestamp, value: d.temperature })), 'Temperature (°C)', 'rgba(255, 99, 132, 1)');
    updateGraph(humidityGraph, filteredData.map(d => ({ timestamp: d.timestamp, value: d.humidity })), 'Humidity (%)', 'rgba(54, 162, 235, 1)');
    updateGraph(moistureGraph, filteredData.map(d => ({ timestamp: d.timestamp, value: d.soil_moisture })), 'Soil Moisture (%)', 'rgba(75, 192, 192, 1)');
    updateGraph(lightGraph, filteredData.map(d => ({ timestamp: d.timestamp, value: d.light_intensity })), 'Light Intensity (lux)', 'rgba(255, 206, 86, 1)');
  });
}

// Update the chart with new data
function updateGraph(graph, data, label, color) {
  graph.data.labels = data.map(d => d.timestamp); // Use the timestamp as the x-axis labels
  graph.data.datasets[0].data = data.map(d => d.value); // The data for the graph
  graph.data.datasets[0].borderColor = color; // Set the line color
  graph.update(); // Redraw the graph with the updated data
}

// Set the graph and gauge refresh interval to 10 seconds
setInterval(function() {
  // Update gauges from the ESP32 sensor data
  getReadings();

  // Update the graphs with the latest data from the SD card
  updateGraphs();
}, 10000); // Update every 10 seconds

// Initialize Graphs (Temperature, Humidity, Soil Moisture, Light Intensity)
var temperatureGraph = setupGraph('temperature-graph', 'Temperature (°C)');
var humidityGraph = setupGraph('humidity-graph', 'Humidity (%)');
var moistureGraph = setupGraph('moisture-graph', 'Soil Moisture (%)');
var lightGraph = setupGraph('light-graph', 'Light Intensity (lux)');

// Initialize Graphs with Chart.js and different colors
function setupGraph(canvasId, label) {
  return new Chart(document.getElementById(canvasId).getContext('2d'), {
    type: 'line',
    data: {
      labels: [],  // Initially empty, will be filled with timestamps
      datasets: [{
        label: label,
        data: [], // Initially empty, will be filled with corresponding values
        borderColor: '', // Default color for the line
        borderWidth: 2,
        fill: false // No fill under the line
      }]
    },
    options: {
      responsive: true,
      plugins: {
        legend: {
          display: true,
          labels: {
            boxWidth: 15,
            color: '#000'
          }
        }
      },
      scales: {
        x: { title: { display: true, text: 'Time' } },
        y: { title: { display: true, text: label } }
      }
    }
  });
}

// Initialize gauges
var gaugeTemp = new RadialGauge({
  renderTo: 'gauge-temperature',
  width: 300,
  height: 300,
  units: "Temperature (°C)",
  minValue: 0,
  maxValue: 50,
  colorPlate: "#fff",
  animationDuration: 1500,
  highlights: [
    { from: 0, to: 10, color: "#00FFFF" },
    { from: 10, to: 30, color: "#006400" },
    { from: 30, to: 40, color: "#DC143C" }
  ],
  majorTicks : [0,10,20,30,40,50]
}).draw();

var gaugeHum = new RadialGauge({
  renderTo: 'gauge-humidity',
  width: 300,
  height: 300,
  units: "Humidity (%)",
  minValue: 0,
  maxValue: 100,
  colorPlate: "#fff",
  animationDuration: 1500,
  highlights: [
    { from: 0, to: 30, color: "#00FFFF" },
    { from: 30, to: 60, color: "#006400" },
    { from: 60, to: 100, color: "#DC143C" }
  ]
}).draw();

var gaugeMoisture = new RadialGauge({
  renderTo: 'gauge-moisture',
  width: 300,
  height: 300,
  units: "Soil Moisture (%)",
  minValue: 0,
  maxValue: 100,
  colorPlate: "#fff",
  animationDuration: 1500,
  highlights: [
    { from: 0, to: 30, color: "#DC143C" },
    { from: 30, to: 70, color: "#006400" },
    { from: 70, to: 100, color: "#00FFFF" }
  ]
}).draw();

var gaugeLight = new RadialGauge({
  renderTo: 'gauge-light',
  width: 300,
  height: 300,
  units: "Light (lux)",
  minValue: 0,
  maxValue: 35000,
  colorPlate: "#fff",
  animationDuration: 1500,
  highlights: [
    { from: 0, to: 10760, color: "#00FFFF" },
    { from: 10760, to: 26900, color: "#006400" },
    { from: 26900, to: 35000, color: "#DC143C" }
  ],
  majorTicks : [0, 10760, 26900, 3500]
}).draw();

// Initial call to update the graphs and gauges when the page loads
window.addEventListener('load', function() {
  getReadings();  // Get the latest readings from the ESP32
  updateGraphs();  // Get the latest data from the SD card
});

// Add event listener for the filter button
document.getElementById('filter-button').addEventListener('click', function () {
  const startDate = document.getElementById('start-date').value;
  const endDate = document.getElementById('end-date').value;

  if (startDate && endDate) {
    filterGraphsByDate(startDate, endDate);
  } else {
    alert("Please select both start and end dates.");
  }
});
