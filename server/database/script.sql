-- Drop sequences if they exist
BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE alumno_seq';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -2289 THEN
            RAISE;
        END IF;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP SEQUENCE asistencia_seq';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -2289 THEN
            RAISE;
        END IF;
END;
/

-- Drop tables if they exist
BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE Asistencia';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE Alumno';
EXCEPTION
    WHEN OTHERS THEN
        IF SQLCODE != -942 THEN
            RAISE;
        END IF;
END;
/

-- Create sequence for Alumno table
CREATE SEQUENCE alumno_seq
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- Create Alumno table
CREATE TABLE Alumno (
    alumno_id INT PRIMARY KEY,
    nombre VARCHAR2(50) NOT NULL,
    apellido VARCHAR2(50) NOT NULL,
    mac_address VARCHAR2(17) UNIQUE NOT NULL
);

-- Use the sequence to populate alumno_id
CREATE OR REPLACE TRIGGER alumno_id_trigger
BEFORE INSERT ON Alumno
FOR EACH ROW
BEGIN
    SELECT alumno_seq.NEXTVAL
    INTO :new.alumno_id
    FROM dual;
END;
/

-- Create sequence for Asistencia table
CREATE SEQUENCE asistencia_seq
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- Create Asistencia table
CREATE TABLE Asistencia (
    asistencia_id INT PRIMARY KEY,
    alumno_id INT,
    fecha DATE NOT NULL,
    status VARCHAR2(10) CHECK (status IN ('Presente', 'Ausente')),
    FOREIGN KEY (alumno_id) REFERENCES Alumno(alumno_id)
);

-- Use the sequence to populate asistencia_id
CREATE OR REPLACE TRIGGER asistencia_id_trigger
BEFORE INSERT ON Asistencia
FOR EACH ROW
BEGIN
    SELECT asistencia_seq.NEXTVAL
    INTO :new.asistencia_id
    FROM dual;
END;
/

-- Insert into Alumno table
INSERT INTO Alumno (nombre, apellido, mac_address)
VALUES ('Piero (celular)', 'Doe', '24:D3:37:13:60:60');

INSERT INTO Alumno (nombre, apellido, mac_address)
VALUES ('Diego (laptop)', 'Smith', '3C:95:09:BF:2F:57');

INSERT INTO Alumno (nombre, apellido, mac_address)
VALUES ('Juan', 'Perez', '62:FF:09:BF:2F:57');

INSERT INTO Alumno (nombre, apellido, mac_address)
VALUES ('Pedro', 'A', '12:9F:DE:10:41:75');

-- Insert into Asistencia table
-- INSERT INTO Asistencia (alumno_id, fecha, status)
-- VALUES (1, TO_DATE('27-06-2024', 'DD-MM-YYYY'), 'Presente');

-- INSERT INTO Asistencia (alumno_id, fecha, status)
-- VALUES (2, TO_DATE('27-06-2024', 'DD-MM-YYYY'), 'Ausente');
