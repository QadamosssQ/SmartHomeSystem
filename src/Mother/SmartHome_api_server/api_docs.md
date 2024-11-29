## Smart Home System API Documentation

### Base URL
```
http://localhost:5000
```

### Authentication
- All endpoints except `/api/RegisterUser` and `/api/login` require authentication.
- Authentication is done using a user secret key passed in the request body.

### Endpoints

#### 1. Register User
- **URL:** `/api/RegisterUser`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "login": "example_user",
      "password": "example_password"
  }
  ```
- **Response:**
  ```json
  {
      "user_secret": "generated_user_secret"
  }
  ```
  
#### 2. Login
- **URL:** `/api/login`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "login": "example_user",
      "password": "example_password"
  }
  ```
- **Response:**
  ```json
  {
      "user_secret": "generated_user_secret"
  }
  ```

#### 3. Register Device
- **URL:** `/api/RegisterDevice`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "user_secret": "user_secret_key",
      "device_name": "Example Device",
      "type": "RGBController"
  }
  ```
- **Response:**
  ```json
  {
      "device_name": "Example Device",
      "device_secret": "generated_device_secret"
  }
  ```

#### 4. Get Devices
- **URL:** `/api/GetDevices`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "user_secret": "user_secret_key"
  }
  ```
- **Response:**
  ```json
  [
      {
          "name": "Example Device",
          "device_secret": "generated_device_secret",
          "type": "RGBController"
      }
  ]
  ```

#### 5. Remove Device
- **URL:** `/api/RemoveDevice`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "user_secret": "user_secret_key",
      "device_secret": "device_secret_key"
  }
  ```
- **Response:**
  ```json
  {
      "message": "Device removed"
  }
  ```

#### 6. Get Light Controller State
- **URL:** `/api/GetLightController`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "device_secret": "device_secret_key",
      "user_secret": "user_secret_key"
  }
  ```
- **Response:**
  ```json
  {
      "state": "on"
  }
  ```

#### 7. Set Light Controller State
- **URL:** `/api/SetLightController`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "device_secret": "device_secret_key",
      "user_secret": "user_secret_key",
      "state": "on" // or "off"
  }
  ```
- **Response:**
  ```json
  {
      "message": "Light controller state changed"
  }
  ```

#### 8. Get RGB Controller State
- **URL:** `/api/GetRGBController`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "device_secret": "device_secret_key",
      "user_secret": "user_secret_key"
  }
  ```
- **Response:**
  ```json
  {
      "state": "on",
      "r": 255,
      "g": 0,
      "b": 0
  }
  ```

#### 9. Set RGB Controller State
- **URL:** `/api/SetRGBController`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "device_secret": "device_secret_key",
      "user_secret": "user_secret_key",
      "r": 255,
      "g": 0,
      "b": 0,
      "state": "on" // or "off"
  }
  ```
- **Response:**
  ```json
  {
      "message": "RGB controller state changed"
  }
  ```

#### 10. Get Door Lock State
- **URL:** `/api/GetDoorLock`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "device_secret": "device_secret_key",
      "user_secret": "user_secret_key"
  }
  ```
- **Response:**
  ```json
  {
      "state": "auto"
  }
  ```

#### 11. Set Door Lock State
- **URL:** `/api/SetDoorLock`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
      "device_secret": "device_secret_key",
      "user_secret": "user_secret_key",
      "state": "auto" // ['on', 'off', 'auto']
  }
  ```
- **Response:**
  ```json
  {
      "message": "Door lock state changed"
  }
  ```

---

Feel free to let me know if you need further clarification or assistance!
