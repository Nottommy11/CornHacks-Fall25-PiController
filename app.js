const express = require("express");
const cors = require("cors");
// const dotenv = require("dotenv");
const { startSerialListener } = require("./src/serialListener.js");
dotenv.config();

const app = express();
app.use(cors());
app.use(express.json());

// Start the serial listener when the API boots
startSerialListener();

const PORT = 3000;
app.listen(PORT, () => console.log(`API running on port ${PORT}`));
