import sqlite3

# Connect to the database
conn = sqlite3.connect('db.db')
cursor = conn.cursor()

# Create 'users' table
cursor.execute('''
    CREATE TABLE users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        login TEXT,
        password TEXT,
        user_secret VARCHAR(32) NOT NULL
    )
''')

# Create 'devices' table
cursor.execute('''
    CREATE TABLE devices (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        device_name TEXT,
        type TEXT,
        device_secret VARCHAR(32),
        user_id INTEGER,
        FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE
    )
''')

# Create 'RGBControllerState' table
cursor.execute('''
    CREATE TABLE RGBControllerState (
        id INTEGER PRIMARY KEY,
        red INTEGER CHECK (red >= 0 AND red <= 255),
        green INTEGER CHECK (green >= 0 AND green <= 255),
        blue INTEGER CHECK (blue >= 0 AND blue <= 255),
        state BOOLEAN,
        device_id INTEGER,
        FOREIGN KEY (device_id) REFERENCES devices (id) ON DELETE CASCADE
    )
''')

# Create 'DoorLockState' table
cursor.execute('''
    CREATE TABLE DoorLockState (
        id INTEGER PRIMARY KEY,
        state VARCHAR(255),
        device_id INTEGER,
        FOREIGN KEY (device_id) REFERENCES devices (id) ON DELETE CASCADE
    )
''')

# Create 'LightControllerState' table
cursor.execute('''
    CREATE TABLE LightControllerState (
        id INTEGER PRIMARY KEY,
        state VARCHAR(255),
        device_id INTEGER,
        FOREIGN KEY (device_id) REFERENCES devices (id) ON DELETE CASCADE
    )
''')

# Commit changes and close connection
conn.commit()
conn.close()
