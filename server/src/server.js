import express from "express";
import os from "os";
import DatabaseManager from "../database/manager.js"; // Ensure the correct path and extension
const app = express();
const PORT = 3000;

const dbm = new DatabaseManager();

app.use(express.json());

app.post("/", async (req, res) => {
  try {
    const { macAddress } = req.body; // Extract macAddress from req.body

    // Now you can use macAddress for further processing
    await dbm.registrarAsistencia(macAddress);
    res.status(200).send("Asistencia registrada");
  } catch {
    res.status(500).send("error");
  }
});
app.get("/", (req, res) => {
  res.status(200).send("Servidor funcionando");
});

app.get("/cerrar-asistencia", async (req, res) => {
  try {
    await dbm.cerrarAsistenciaDelDia();
    res.status(200).send("Asistencias del dia cerradas correctamente");
  } catch {
    res.status(500).send("error");
  }
});

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
