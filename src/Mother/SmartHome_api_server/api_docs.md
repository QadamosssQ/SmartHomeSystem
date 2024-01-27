# API Documentation

## 1. Get Light Controller State

- **Endpoint:** `/api/GetLightControllerState`
- **Method:** `GET`
- **Parameters:**
  - `device_secret` (string): Device secret key.
  - `user_secret` (string): User secret key.

- **Response:**
  - `200 OK`: Returns the light controller state if successful.
    ```json
    {"state": "on"}
    ```
  - `400 Bad Request`: Invalid request or device secret.
    ```json
    {"error": "Invalid request"}
    ```
  - `500 Internal Server Error`: An error occurred on the server.
    ```json
    {"error": "An error occurred: ..."}


## 2. Set Light Controller State

- **Endpoint:** `/api/SetLightController`
- **Method:** `POST`
- **Parameters:**
  - `device_secret` (string): Device secret key.
  - `user_secret` (string): User secret key.
  - `state` (string): New state ("on", "off", "auto").

- **Response:**
  - `200 OK`: Light controller state changed successfully.
    ```json
    {"message": "Light controller state changed"}
    ```
  - `400 Bad Request`: Invalid request, device secret, or light value.
    ```json
    {"error": "Invalid request"}
    ```
  - `500 Internal Server Error`: An error occurred on the server.
    ```json
    {"error": "An error occurred: ..."}


## 3. Get RGB Controller State

- **Endpoint:** `/api/GetRGBControllerState`
- **Method:** `GET`
- **Parameters:**
  - `device_secret` (string): Device secret key.
  - `user_secret` (string): User secret key.

- **Response:**
  - `200 OK`: Returns the RGB controller state if successful.
    ```json
    {"state": "on", "r": 255, "g": 0, "b": 0}
    ```
  - `400 Bad Request`: Invalid request or device secret.
    ```json
    {"error": "Invalid request"}
    ```
  - `500 Internal Server Error`: An error occurred on the server.
    ```json
    {"error": "An error occurred: ..."}


## 4. Set RGB Controller State

- **Endpoint:** `/api/SetRGBController`
- **Method:** `POST`
- **Parameters:**
  - `device_secret` (string): Device secret key.
  - `user_secret` (string): User secret key.
  - `r` (int): Red component (0-255).
  - `g` (int): Green component (0-255).
  - `b` (int): Blue component (0-255).
  - `state` (string): New state ("on", "off", "auto").

- **Response:**
  - `200 OK`: RGB controller state changed successfully.
    ```json
    {"message": "RGB controller state changed"}
    ```
  - `400 Bad Request`: Invalid request, device secret, or RGB value.
    ```json
    {"error": "Invalid request"}
    ```
  - `500 Internal Server Error`: An error occurred on the server.
    ```json
    {"error": "An error occurred: ..."}

