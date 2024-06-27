# Documentación de la API

## URL Base
- URL Base: `https://localhost:3000`

## Endpoints

### `POST /write-sheet`

#### Descripción
Este endpoint se utiliza para escribir los datos recolectados en el formulario de Rapimoney hacia Google Spreadsheet.

#### Solicitud
- **Método:** POST
- **Encabezados:**
  - `Content-Type`: application/json
- **Cuerpo:**
  - `nombres`
  - `dni`
  - `celular`
  - `tarjeta`
  - `monto`
  - `region`
  - `occupation`

#### Respuesta
- **Respuesta Exitosa:**
  - **Código de Estado:** 200 OK
  - **Cuerpo:**
    ```json
    {
      "success": "Data written successfully",
      "info": "<details about the writing data>"
    }
    ```
- **Respuesta de Error:**
  - **Código de Estado:** 400 Bad Request o 500 Internal Server Error
  - **Cuerpo:**
    ```json
    {
      "error": "Internal Server Error",
      "details": "<error message>"
    }
    ```

## Variables de Entorno

Crea un archivo `.env` en la raíz de tu proyecto y define las siguientes variables:

- `API_PORT`:
- `SPREADSHEET_ID`:
- `KEY_FILE_PATH`:
- `SCOPES`:
- `SHEET_NAME`:
- `VALUE_INPUT_OPTION`:
- `URL_ORIGIN`:

# Ejecutando el Servidor
Para iniciar el servidor, ejecuta el siguiente comando en tu terminal:

```bash
node server.js
```

# Llamando a la API
```bash
curl -X POST -H "Content-Type: application/json" -d '{
  "nombres": "John Doe",
  "dni": "123456789",
  "celular": "123", 
  "tarjeta": "1234 5678 9012 3456",
  "monto": 100.00,
  "region": "Some Region",
  "occupation": "Some occupation"
}' http://localhost:3326/write-sheet


```


