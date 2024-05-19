-- Create the database
CREATE DATABASE yourdatabase;

-- Use the created database
USE yourdatabase;

-- Create the users table
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(255) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL,
    user_secret VARCHAR(32) NOT NULL UNIQUE
);

-- Create the devices table
CREATE TABLE devices (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_name VARCHAR(255) NOT NULL,
    device_type ENUM('RGBController', 'LightController', 'DoorLock') NOT NULL,
    device_secret VARCHAR(32) NOT NULL UNIQUE,
    user_id INT,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Create the LightControllerState table
CREATE TABLE LightControllerState (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id INT,
    state ENUM('on', 'off', 'auto') NOT NULL,
    FOREIGN KEY (device_id) REFERENCES devices(id)
);

-- Create the RGBControllerState table
CREATE TABLE RGBControllerState (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id INT,
    red INT CHECK (red >= 0 AND red <= 255),
    green INT CHECK (green >= 0 AND green <= 255),
    blue INT CHECK (blue >= 0 AND blue <= 255),
    state ENUM('on', 'off', 'auto') NOT NULL,
    FOREIGN KEY (device_id) REFERENCES devices(id)
);

-- Create the DoorLockState table
CREATE TABLE DoorLockState (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id INT,
    state ENUM('locked', 'unlocked') NOT NULL,
    FOREIGN KEY (device_id) REFERENCES devices(id)
);
