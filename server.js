const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');

const app = express();
const PORT = 3000; // Aligned to match your STM32 ESP-01 network port configuration

// Middleware configuration for high-speed data packets
app.use(cors());
app.use(express.json());

// 🌟 LOCAL DATABASE CONNECTION HANDSHAKE
mongoose.connect("mongodb://127.0.0.1:27017/heart_rate_db")
  .then(() => console.log("🚀 Connected to LOCAL MongoDB Successfully!"))
  .catch(err => console.error("❌ MongoDB Connection Error:", err));

// Define data structural template layout for your documents
const pulseSchema = new mongoose.Schema({
    bpm: { type: Number, required: true },
    timestamp: { type: Date, default: Date.now }
}, { 
    versionKey: false // Clean up metadata clutter inside MongoDB Compass
});

// Mongoose turns 'Pulse' into a collection named 'pulses' automatically
const Pulse = mongoose.model('Pulse', pulseSchema);

// ⚡ STM32 INTERCEPTOR ROUTE (Listens for incoming POST from ESP-01)
app.post('/api/pulse', async (req, res) => {
    try {
        const { bpm } = req.body;
        
        if (!bpm) {
            return res.status(400).json({ success: false, message: "Missing BPM data packet" });
        }

        const newPulse = new Pulse({ bpm });
        await newPulse.save();

        console.log(`⚡ Saved 1s Pulse Data: ${bpm} BPM`);
        res.status(201).json({ success: true, message: "Heart rate entry appended safely" });
    } catch (error) {
        console.error("❌ Error processing hardware stream:", error);
        res.status(500).json({ success: false, message: "Internal Server Error" });
    }
});

// 📊 LIVE VISUALIZATION DASHBOARD ROUTE
// This generates a beautiful line chart from your MongoDB data on a 1-second auto-refresh
app.get('/dashboard', async (req, res) => {
    try {
        // Pull the last 40 entries to keep the chart clean and scrollable
        const history = await Pulse.find().sort({ timestamp: -1 }).limit(40);
        const reversedHistory = history.reverse();
        
        // Extract timestamps (formatted to time strings) and heart rates
        const timeLabels = reversedHistory.map(p => new Date(p.timestamp).toLocaleTimeString());
        const bpmData = reversedHistory.map(p => p.bpm);

        res.send(`
        <!DOCTYPE html>
        <html>
        <head>
            <title>IoT Heart Rate Dashboard</title>
            <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
            <style>
                body { font-family: 'Segoe UI', sans-serif; background: #111216; color: #ffffff; margin: 0; padding: 40px; display: flex; justify-content: center; }
                .container { width: 100%; max-width: 950px; background: #171821; padding: 30px; border-radius: 16px; box-shadow: 0 10px 30px rgba(0,0,0,0.5); border: 1px solid #232433; }
                h1 { margin: 0 0 5px 0; color: #00ff9d; font-size: 28px; text-transform: uppercase; font-weight: 700; letter-spacing: 1.5px; text-align: center;}
                .subtitle { color: #8a8d9f; text-align: center; margin-bottom: 30px; font-size: 14px; letter-spacing: 0.5px; }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>Pulse Real-Time Monitor</h1>
                <div class="subtitle">Live Streaming from STM32 • Auto-refreshing every 1s</div>
                <canvas id="liveChart" height="150"></canvas>
            </div>

            <script>
                // Triggers an automatic browser reload every 1000ms (1 second)
                setTimeout(() => { location.reload(); }, 1000);

                const ctx = document.getElementById('liveChart').getContext('2d');
                new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: ${JSON.stringify(timeLabels)},
                        datasets: [{
                            label: 'Heart Rate (BPM)',
                            data: ${JSON.stringify(bpmData)},
                            borderColor: '#00ff9d',
                            backgroundColor: 'rgba(0, 255, 157, 0.08)',
                            borderWidth: 3,
                            pointBackgroundColor: '#00ff9d',
                            pointHoverRadius: 7,
                            pointRadius: 3.5,
                            tension: 0.25
                        }]
                    },
                    options: {
                        responsive: true,
                        scales: {
                            y: { 
                                min: 40, 
                                max: 160, 
                                grid: { color: 'rgba(255, 255, 255, 0.05)' }, 
                                ticks: { color: '#8a8d9f', font: { size: 12 } } 
                            },
                            x: { 
                                grid: { color: 'rgba(255, 255, 255, 0.05)' }, 
                                ticks: { color: '#8a8d9f', font: { size: 10 } } 
                            }
                        },
                        plugins: {
                            legend: { labels: { color: '#ffffff', font: { weight: 'bold' } } }
                        }
                    }
                });
            </script>
        </body>
        </html>
        `);
    } catch (err) {
        console.error("Dashboard error:", err);
        res.status(500).send("Could not generate dashboard visualization.");
    }
});

// Start the network hosting pipeline
app.listen(PORT, '0.0.0.0', () => {
    console.log(`🚀 IoT Receiver API Listening on Port ${PORT}`);
});