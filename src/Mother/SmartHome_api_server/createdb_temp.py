import sqlite3

conn = sqlite3.connect('db.db')
cursor = conn.cursor()

cursor.execute('''
    CREATE TABLE users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        login TEXT,
        password TEXT,
        user_secret VARCHAR(32) NOT NULL
    )
''')

cursor.execute('''
    CREATE TABLE devices (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        device_name TEXT,
        type TEXT,
        device_secret VARCHAR(32),
        ip_address VARCHAR(12),
        user_id INTEGER,
        FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE
    )
''')

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

cursor.execute('''
    CREATE TABLE DoorLockState (
        id INTEGER PRIMARY KEY,
        state VARCHAR(255),
        device_id INTEGER,
        FOREIGN KEY (device_id) REFERENCES devices (id) ON DELETE CASCADE
    )
''')

cursor.execute('''
    CREATE TABLE LightControllerState (
        id INTEGER PRIMARY KEY,
        state VARCHAR(255),
        device_id INTEGER,
        FOREIGN KEY (device_id) REFERENCES devices (id) ON DELETE CASCADE
    )
''')

conn.commit()
conn.close()
