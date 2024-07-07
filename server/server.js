import express from "express";
import os from "os";
import DatabaseManager from "./database/manager.js"; // Ensure the correct path and extension
const app = express();
const PORT = 3000;

const dbm = new DatabaseManager();

app.use(express.json());

app.post("/asistencia/ingresar", async (req, res) => {
  try {
    const { macAddress } = req.body;
    await dbm.registrarAsistencia(macAddress);
    res.status(200).send("Asistencia registrada");
  } catch (err) {
    res.status(500).send("Error al registrar asistencia");
  }
});
app.get("/", (req, res) => {
  res.status(200).send("Servidor funcionando");
});

app.get("/asistencia/cerrar", async (req, res) => {
  try {
    await dbm.cerrarAsistenciaDelDia();
    res.status(200).send("Asistencias del dia cerradas correctamente");
  } catch {
    res.status(500).send("error");
  }
});

// Handle POST request
app.post('/submit', (req, res) => {
  const { pregunta, respuesta } = req.body;

  if (pregunta && respuesta) {
      const query = 'INSERT INTO respuestas (pregunta, respuesta) VALUES (?, ?)';
      db.query(query, [pregunta, respuesta], (error, results) => {
          if (error) {
              res.status(500).send('Error: ' + error.message);
          } else {
              res.send('Nueva respuesta guardada correctamente');
          }
      });
  } else {
      res.status(400).send('Datos incompletos');
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
