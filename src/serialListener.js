const { SerialPort, ReadlineParser } = require("serialport");
const fetch = require("node-fetch");

const SERIAL_PORT = "/dev/ttyUSB0"; // Adjust if different
const BAUD_RATE = 115200;
const API_URL = "https://cornhacks-fall25-api.onrender.com/api/nodes/"; // middleman API endpoint

function startSerialListener() {
    const port = new SerialPort({ path: SERIAL_PORT, baudRate: BAUD_RATE });
    const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

    console.log(`[Serial] Listening on ${SERIAL_PORT}...`);

    parser.on("data", async (line) => {
        line = line.trim();
        if (!line || line === "---") return;

        console.log(`[Serial] Received: ${line}`);

        // Format is metric:value
        if (line.includes(":")) {
            const [metricType, value] = line.split(":");

            try {
                const res = await fetch(API_URL, {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({
                        metricType,
                        value: parseFloat(value),
                    }),
                });

                console.log(
                    `[API] Sent ${metricType}:${value} -> ${res.status}`
                );
            } catch (err) {
                console.error("[API Error]", err.message);
            }
        }
    });

    port.on("error", (err) => console.error("[Serial Error]", err.message));
}

module.exports = { startSerialListener };
