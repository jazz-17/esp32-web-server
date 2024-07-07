import oracle from "oracledb";

const dbConfig = {
  user: "esp32",
  password: "123",
  connectString: "localhost/xepdb1",
};
// curl -X POST   -H "Content-Type: application/json"   -d '{"macAddress": "24:D3:37:13:60:60"}'   http://localhost:3000/
class DatabaseManager {
  async startDatabaseConn() {
    try {
      await oracle.createPool(dbConfig);
      console.log("Connection pool started");
      process
        .once("SIGTERM", () => this.closeDatabaseConn())
        .once("SIGINT", () => this.closeDatabaseConn());
    } catch (error) {
      console.error("Failed to start the database connection:", error);
      throw error;
    }
  }
  async closeDatabaseConn() {
    try {
      await oracle.getPool().close(10);
      console.log("Connection pool closed");
      process.exit(0);
    } catch (err) {
      console.error("Error closing the connection pool:", err);
      process.exit(1);
    }
  }
  async registrarAsistencia(macAddress) {
    let connection;
    try {
      connection = await oracle.getConnection(dbConfig);

      // Query to fetch alumno_id based on mac_address
      const query = `
        SELECT alumno_id
        FROM Alumno
        WHERE mac_address = :mac_address`;

      const result = await connection.execute(query, [macAddress]);

      if (result.rows.length === 0) {
        throw new Error(
          `No se encontraron alumnos con ese MAC address: ${macAddress}`
        );
      }

      const alumnoId = result.rows[0][0]; // Assuming alumno_id is in the first column

      // Check if attendance already exists for this alumno_id
      const checkAttendanceQuery = `
        SELECT COUNT(*)
        FROM Asistencia
        WHERE alumno_id = :alumno_id
        AND fecha = TRUNC(SYSDATE)`; // Check for today's date

      const checkResult = await connection.execute(checkAttendanceQuery, [
        alumnoId,
      ]);
      const attendanceCount = checkResult.rows[0][0];

      if (attendanceCount > 0) {
        console.log(
          `Ya existe un registro de asistencia para el alumno con código ${alumnoId} hoy.`
        );
        return;
      }

      // Insert into Asistencia table
      const insertQuery = `
        INSERT INTO Asistencia (alumno_id, fecha, status)
        VALUES (:alumno_id, TRUNC(SYSDATE), 'Presente')`; // SYSDATE for current date

      await connection.execute(insertQuery, [alumnoId]);

      console.log("Registro de asistencia creado exitosamente.");

      // Commit the transaction
      await connection.commit();
    } catch (err) {
      console.log("Error al registrar asistencia:", err);
      throw err; // Rethrow the error to handle it in the caller function
    } finally {
      if (connection) {
        try {
          await connection.close();
          console.log("Connection closed.");
        } catch (err) {
          console.log("Error closing connection:", err);
        }
      }
    }
  }

  async cerrarAsistenciaDelDia() {
    let connection;
    try {
      connection = await oracle.getConnection(dbConfig);

      // Query to fetch all students
      const query = `
        SELECT alumno_id
        FROM Alumno`;

      const result = await connection.execute(query);

      for (let i = 0; i < result.rows.length; i++) {
        const alumnoId = result.rows[i][0];

        // Check if there is already an attendance record for today
        const checkAttendanceQuery = `
          SELECT COUNT(*) AS count
          FROM Asistencia
          WHERE alumno_id = :alumno_id
            AND fecha = TRUNC(SYSDATE)`;

        const checkResult = await connection.execute(checkAttendanceQuery, [
          alumnoId,
        ]);

        const attendanceExists = checkResult.rows[0][0] > 0;

        if (!attendanceExists) {
          // Insert absence record for today
          const insertQuery = `
            INSERT INTO Asistencia (alumno_id, fecha, status)
            VALUES (:alumno_id, TRUNC(SYSDATE), 'Ausente')`;

          await connection.execute(insertQuery, [alumnoId]);

          console.log(
            `Alumno con código ${alumnoId} marcado como Ausente por el día de hoy`
          );
        }
      }

      // Commit the transaction
      await connection.commit();
      console.log("Asistencias de hoy cerradas.");
    } catch (err) {
      console.error("Error al cerrar asistencias:", err);
      throw err; // Rethrow the error to handle it in the caller function
    } finally {
      if (connection) {
        try {
          await connection.close();
          console.log("Connection closed.");
        } catch (err) {
          console.error("Error closing connection:", err);
        }
      }
    }
  }

  async guardarRespuestas(data) {
    const { pregunta, respuesta } = data;
    let connection;
    try {
      connection = await oracle.getConnection(dbConfig);

      const insertQuery = `
        INSERT INTO respuestas (pregunta, respuesta)
        VALUES (:pregunta, :respuesta)`;

      await connection.execute(insertQuery, [pregunta, respuesta]);

      console.log("Respuestas guardadas correctamente.");

      // Commit the transaction
      await connection.commit();
    } catch (err) {
      console.error("Error al guardar respuestas:", err);
      throw err; // Rethrow the error to handle it in the caller function
    } finally {
      if (connection) {
        try {
          await connection.close();
          console.log("Connection closed.");
        } catch (err) {
          console.error("Error closing connection:", err);
        }
      }
    }
  }

  async obtenerResultados() {
    let connection;
    try {
      connection = await oracle.getConnection(dbConfig);

      const query = "SELECT id, pregunta, respuesta FROM respuestas";

      const result = await connection.execute(query);

      return result.rows;
    } catch (err) {
      console.error("Error al obtener resultados:", err);
      throw err; // Rethrow the error to handle it in the caller function
    } finally {
      if (connection) {
        try {
          await connection.close();
          console.log("Connection closed.");
        } catch (err) {
          console.error("Error closing connection:", err);
        }
      }
    }
  }
}
export default DatabaseManager;
