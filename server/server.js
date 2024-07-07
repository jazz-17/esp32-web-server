import express from "express";
import { fileURLToPath } from 'url';
import { dirname } from 'path';
import os from "os";
import DatabaseManager from "./database/manager.js";
import createAsistenciaRoutes from "./routes/asistenciaRoutes.js";
import createExamenRoutes from "./routes/examenRoutes.js";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = 3000;

const dbm = new DatabaseManager();

app.use(express.json());

// Use the routes
app.get("/", (req, res) => {
  res.sendFile(__dirname + "/index.html");
});
app.use("/asistencia", createAsistenciaRoutes(dbm));
app.use("/examen", createExamenRoutes(dbm));

function getLocalIpAddress() {
  const interfaces = os.networkInterfaces();
  for (let iface in interfaces) {
    for (let alias of interfaces[iface]) {
      if (alias.family === "IPv4" && !alias.internal) {
        return alias.address;
      }
    }
  }
  return "0.0.0.0";
}

async function startServer() {
  try {
    await dbm.startDatabaseConn();
    app.listen(PORT, "0.0.0.0", () => {
      const ipAddress = getLocalIpAddress();
      console.log(`Server is running on http://${ipAddress}:${PORT}`);
    });
  } catch (error) {
    console.error("Failed to start the server:", error);
  }
}

startServer();
