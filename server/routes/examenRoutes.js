// routes/examenRoutes.js
import express from "express";

const router = express.Router();

const createExamenRoutes = (dbm) => {
  router.get("/resultados", async (req, res) => {
    try {
      const resultados = await dbm.obtenerResultados();
      res.status(200).send(resultados);
    } catch (error) {
      res.status(500).send("Error");
    }
  });

  router.post("/guardar-respuesta", async (req, res) => {
    const { pregunta, respuesta } = req.body;
    console.log(req.body)
    try {
      if (pregunta && respuesta) {
        await dbm.guardarRespuestas(req.body);
        res.status(200).send("Respuestas guardadas correctamente");
      } else {
        res.status(400).send("Datos incompletos");
      }
    } catch (error) {
      console.log(error)
      res.status(500).send("Error");
    }
  });

  return router;
};

export default createExamenRoutes;
