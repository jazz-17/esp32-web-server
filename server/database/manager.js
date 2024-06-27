import oracle from "oracledb";

const dbConfig = {
  user: "esp32",
  password: "123",
  connectString: "localhost/xepdb1",
};
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

      // Insert into Asistencia table
      const insertQuery = `
        INSERT INTO Asistencia (alumno_id, fecha, status)
        VALUES (:alumno_id, SYSDATE, 'Presente')`; // SYSDATE for current date

      await connection.execute(insertQuery, [alumnoId]);

      console.log("Attendance record inserted successfully.");

      // Commit the transaction
      await connection.commit();
      console.log("Changes committed successfully.");
    } catch (err) {
      console.error("Error registering attendance:", err);
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

  async cerrarAsistenciaDelDia() {
    let connection;
    try {
      connection = await oracle.getConnection(dbConfig);

      // Get current date
      const currentDate = new Date().toISOString().slice(0, 10);

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
            AND fecha = TO_DATE(:current_date, 'YYYY-MM-DD')`;

        const checkResult = await connection.execute(checkAttendanceQuery, [
          alumnoId,
          currentDate,
        ]);

        const attendanceExists = checkResult.rows[0][0] > 0;

        if (!attendanceExists) {
          // Insert absence record for today
          const insertQuery = `
            INSERT INTO Asistencia (alumno_id, fecha, status)
            VALUES (:alumno_id, TO_DATE(:current_date, 'YYYY-MM-DD'), 'Ausente')`;

          await connection.execute(insertQuery, [alumnoId, currentDate]);

          console.log(
            `Marked student with alumno_id ${alumnoId} as Ausente for ${currentDate}`
          );
        }
      }

      // Commit the transaction
      await connection.commit();
      console.log("Attendance closed successfully for today.");
    } catch (err) {
      console.error("Error closing attendance:", err);
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
