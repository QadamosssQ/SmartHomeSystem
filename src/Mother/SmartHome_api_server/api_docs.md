## Endpoints:

### 1. Get Light Controller
- **Endpoint:** `/api/GetLightController`
- **Method:** GET
- **Parameters:**
  - `secret_key` (string, required): Secret key for authentication.
- **Returns:**
  - Returns the current state of the light controller.

### 2. Set Light Controller
- **Endpoint:** `/api/SetLightController`
- **Method:** POST
- **Parameters:**
  - `secret_key` (string, required): Secret key for authentication.
  - `state` (string, required): Desired state for the light controller (`on`, `off`, `auto`).
- **Returns:**
  - If successful, returns 'OK'. Otherwise, returns an error message.

### 3. Get RGB Controller
- **Endpoint:** `/api/GetRGBController`
- **Method:** GET
- **Parameters:**
  - `secret_key` (string, required): Secret key for authentication.
- **Returns:**
  - Returns the current state of the RGB controller.

### 4. Set RGB Controller
- **Endpoint:** `/api/SetRGBController`
- **Method:** POST
- **Parameters:**
  - `secret_key` (string, required): Secret key for authentication.
  - `r` (integer, required): Red component of RGB (0-255).
  - `g` (integer, required): Green component of RGB (0-255).
  - `b` (integer, required): Blue component of RGB (0-255).
  - `state` (boolean, required): Desired state for the RGB controller (`True` or `False`).
- **Returns:**
  - If successful, returns 'OK'. Otherwise, returns an error message.

### 5. Get Door Lock
- **Endpoint:** `/api/GetDoorLock`
- **Method:** GET
- **Parameters:**
  - `secret_key` (string, required): Secret key for authentication.
- **Returns:**
  - Returns the current state of the door lock.

### 6. Set Door Lock
- **Endpoint:** `/api/SetDoorLock`
- **Method:** POST
- **Parameters:**
  - `secret_key` (string, required): Secret key for authentication.
  - `state` (string, required): Desired state for the door lock (`open`, `closed`, `auto`).
- **Returns:**
  - If successful, returns 'OK'. Otherwise, returns an error message.

### 7. Register User
- **Endpoint:** `/api/RegisterUser`
- **Method:** POST
- **Parameters:**
  - `login` (string, required): User login.
  - `password` (string, required): User password.
- **Returns:**
  - If successful, returns 'OK'. Otherwise, returns an error message.

### 8. Register Device
- **Endpoint:** `/api/RegisterDevice`
- **Method:** POST
- **Parameters:**
  - `login` (string, required): User login.
  - `password` (string, required): User password.
  - `device_name` (string, required): Name of the device.
  - `type` (string, required): Type of the device (must be one of: RGBController, LightController, DoorLock).
- **Returns:**
  - If successful, returns device information (name and secret key). Otherwise, returns an error message.

### 9. Show Devices Secret Keys
- **Endpoint:** `/api/ShowDevicesSecretKeys`
- **Method:** POST
- **Parameters:**
  - `login` (string, required): User login.
  - `password` (string, required): User password.
- **Returns:**
  - Returns a dictionary of device names and their corresponding secret keys.

### 10. Remove Device
- **Endpoint:** `/api/RemoveDevice`
- **Method:** POST
- **Parameters:**
  - `login` (string, required): User login.
  - `password` (string, required): User password.
  - `secret_key` (string, required): Secret key of the device to be removed.
- **Returns:**
  - If successful, returns a success message. Otherwise, returns an error message.
