// routes/asistenciaRoutes.js
import express from 'express';

const router = express.Router();

const createAsistenciaRoutes = (dbm) => {
  router.post('/ingresar', async (req, res) => {
    try {
      const { macAddress } = req.body;
      await dbm.registrarAsistencia(macAddress);
      res.status(200).send("Asistencia registrada");
    } catch (err) {
      res.status(500).send("Error al registrar asistencia");
    }
  });

  router.get('/cerrar', async (req, res) => {
    try {
      await dbm.cerrarAsistenciaDelDia();
      res.status(200).send("Asistencias del dia cerradas correctamente");
    } catch (error) {
      res.status(500).send("Error");
    }
  });

  return router;
};

export default createAsistenciaRoutes;
