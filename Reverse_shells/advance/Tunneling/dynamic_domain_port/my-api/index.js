const express = require("express");
const fs = require("fs");
const path = require("path");
const app = express();

// PORT for Vercel deployment
const port = process.env.PORT || 3000;

// Route to provide ngrok info
app.get("/tunnel", (req, res) => {
  const jsonPath = path.join(__dirname, "tunnel.json");

  fs.readFile(jsonPath, "utf8", (err, data) => {
    if (err) {
      console.error("Error reading JSON file:", err);
      return res.status(500).json({ error: "Could not read tunnel info" });
    }
    try {
      const jsonData = JSON.parse(data);
      res.json(jsonData); // exactly like before
    } catch (parseErr) {
      console.error("Error parsing JSON:", parseErr);
      res.status(500).json({ error: "Invalid JSON format" });
    }
  });
});

// Start server
app.listen(port, () => {
  console.log(`API running on port ${port}`);
});
