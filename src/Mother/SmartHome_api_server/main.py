# dodaj resztę połączeń do bd w innych funkcjach


from flask import Flask, request, jsonify, g
from flask_cors import CORS
import random
import sqlite3

app = Flask(__name__)
CORS(app)

rgb_state = {'r': 0, 'g': 0, 'b': 0, 'state': False}
door_state = {'state': 'auto'}
light_state = {'state': 'auto'}
devices = ['RGBController', 'LightController', 'DoorLock']

DATABASE = 'db.db'

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
    return db

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()


@app.route('/api/GetLightController', methods=['GET'])
def get_light_controller():
    fetch_data = request.get_json()
    if 'device_secret' in fetch_data and 'user_secret' in fetch_data:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT id FROM users WHERE user_secret = ?', (fetch_data['user_secret'],))
        user_id_tuple = cursor.fetchone()
        if user_id_tuple is not None:
            user_id = user_id_tuple[0]
            cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                           (fetch_data['device_secret'], user_id))
            device = cursor.fetchone()
            if device is not None:
                cursor.execute('SELECT * FROM LightControllerState WHERE device_id = ?', (device[0],))
                light_controller_state = cursor.fetchone()
                if light_controller_state is not None:
                    return jsonify({'state': light_controller_state[1]}), 200
                else:
                    return jsonify({'error': 'No light controller state'}), 400
            else:
                return jsonify({'error': 'Invalid device secret'}), 400
        else:
            return jsonify({'error': 'Invalid user secret'}), 400
    else:
        return jsonify({'error': 'Invalid request'}), 400


@app.route('/api/SetLightController', methods=['POST'])
def set_light_controller():
    global light_state

    fetch_data = request.get_json()

    if 'device_secret' in fetch_data:
        if check_secret_device(fetch_data['device_secret']):
            if 'state' in fetch_data:
                if fetch_data['state'] in ['on', 'off', 'auto']:
                    light_state = fetch_data
                    return 'OK', 200
                return jsonify({'error': 'Invalid light value'}), 400
        else:
            return jsonify({'error': 'Invalid secret key'}), 400
    else:
        return jsonify({'error': 'No secret key'}), 400


@app.route('/api/GetRGBController', methods=['GET'])
def get_rgb_controller():
    fetch_data = request.get_json()
    if 'device_secret' in fetch_data:
        if check_secret_device(fetch_data['device_secret']):
            return jsonify(rgb_state)
        else:
            return jsonify({'error': 'Invalid secret key'}), 400
    else:
        return jsonify({'error': 'No secret key'}), 400


@app.route('/api/SetRGBController', methods=['POST'])
def set_rgb_controller():
    global rgb_state

    fetch_data = request.get_json()
    if 'device_secret' in fetch_data:
        if check_secret_device(fetch_data['device_secret']):
            if 'r' in fetch_data and 'g' in fetch_data and 'b' in fetch_data and 'state' in fetch_data:
                if (
                        0 <= fetch_data['r'] <= 255
                        and 0 <= fetch_data['g'] <= 255
                        and 0 <= fetch_data['b'] <= 255
                        and fetch_data['state'] in [True, False]
                ):
                    rgb_state = fetch_data
                    return 'OK', 200
            return jsonify({'error': 'Invalid RGB value'}), 400
        else:
            return jsonify({'error': 'Invalid secret key'}), 400
    else:
        return jsonify({'error': 'No secret key'}), 400


@app.route('/api/GetDoorLock', methods=['GET'])
def get_door_lock():
    fetch_data = request.get_json()
    if 'device_secret' in fetch_data:
        if check_secret_device(fetch_data['device_secret']):
            return jsonify(door_state)
        else:
            return jsonify({'error': 'Invalid secret key'}), 400
    else:
        return jsonify({'error': 'No secret key'}), 400


@app.route('/api/SetDoorLock', methods=['POST'])
def set_door_lock():
    global door_state

    fetch_data = request.get_json()
    if 'device_secret' in fetch_data:
        if check_secret_device(fetch_data['device_secret']):
            if 'state' in fetch_data:
                if fetch_data['state'] in ['open', 'closed', 'auto']:
                    door_state = fetch_data
                    return 'OK', 200
                return jsonify({'error': 'Invalid door value'}), 400
        else:
            return jsonify({'error': 'Invalid secret key'}), 400
    else:
        return jsonify({'error': 'No secret key'}), 400


@app.route('/api/RegisterUser', methods=['POST'])
def register_user():
    user = request.get_json()
    if 'login' in user and 'password' in user:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT * FROM users WHERE login = ?', (user['login'],))
        if cursor.fetchone() is None:
            new_user_secret = generate_secret_key()
            cursor.execute('INSERT INTO users (login, password, user_secret) VALUES (?, ?, ?)', (user['login'], user['password'], new_user_secret))
            db.commit()
            return jsonify({'message': 'User registered', 'user_secret': new_user_secret}), 200
        return jsonify({'error': 'Username already exists'}), 400
    else:
        return jsonify({'error': 'Invalid user'}), 400


@app.route('/api/RegisterDevice', methods=['POST'])
def register_device():
    fetch_data = request.get_json()
    if 'user_secret' in fetch_data and 'device_name' in fetch_data and 'type' in fetch_data:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT id FROM users WHERE user_secret = ?', (fetch_data['user_secret'],))
        user_id_tuple = cursor.fetchone()
        if user_id_tuple is not None:
            user_id = user_id_tuple[0]
            ne_device_secret = generate_secret_key()
            if fetch_data['type'] not in devices:
                devices_str = ', '.join(devices)
                return jsonify({'error': 'Invalid device type. Available devices: ' + devices_str}), 400

            else:
                cursor.execute('INSERT INTO devices (user_id, device_name, type, device_secret) VALUES (?, ?, ?, ?)',
                               (user_id, fetch_data['device_name'], fetch_data['type'], ne_device_secret))

                switch = fetch_data['type']
                insert_group_device(cursor, switch)

                db.commit()
                return jsonify({'device_name': fetch_data['device_name'], 'device_secret': ne_device_secret}), 200
        return jsonify({'error': 'Invalid user or password'}), 400
    else:
        return jsonify({'error': 'Invalid device'}), 400


@app.route('/api/ShowDevicesSecretKeys', methods=['POST'])
def show_device_secret():
    fetch_data = request.get_json()
    if 'user_secret' in fetch_data:
        db = get_db()
        cursor = db.cursor()
        cursor.execute(
            'SELECT devices.device_secret, devices.device_name FROM devices JOIN users ON devices.user_id = users.id WHERE users.user_secret = ?',
            (fetch_data['user_secret'],))
        secret_data = cursor.fetchall()

        if secret_data:
            device_secrets = {device[1]: device[0] for device in secret_data}
            return jsonify(device_secrets), 200
        else:
            return jsonify({'error': 'No devices found for the user'}), 400
    else:
        return jsonify({'error': 'Invalid request'}), 400


@app.route('/api/RemoveDevice', methods=['POST'])
def remove_device():
    fetch_data = request.get_json()
    if 'user_secret' in fetch_data and 'device_secret' in fetch_data:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT id FROM users WHERE user_secret = ?',
                       (fetch_data['user_secret'],))
        user_id_tuple = cursor.fetchone()
        if user_id_tuple is not None:
            user_id = user_id_tuple[0]
            cursor.execute('SELECT id, type FROM devices WHERE device_secret = ? AND user_id = ?',
                           (fetch_data['device_secret'], user_id))
            device = cursor.fetchone()
            if device is not None:
                cursor.execute('DELETE FROM devices WHERE id = ?', (device[0],))
                if device[1] == 'RGBController':
                    cursor.execute('DELETE FROM RGBControllerState WHERE device_id = ?', (device[0],))
                elif device[1] == 'DoorLock':
                    cursor.execute('DELETE FROM DoorLockState WHERE device_id = ?', (device[0],))
                elif device[1] == 'LightController':
                    cursor.execute('DELETE FROM LightControllerState WHERE device_id = ?', (device[0],))
                db.commit()
                return jsonify({'message': 'Device removed'}), 200
            else:
                return jsonify({'error': 'Invalid device secret'}), 400
        else:
            return jsonify({'error': 'Invalid user secret'}), 400
    else:
        return jsonify({'error': 'Invalid request'}), 400


def generate_secret_key():
    chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890'
    secret_key = ''
    for i in range(32):
        secret_key += chars[random.randint(0, len(chars) - 1)]
    return secret_key


def check_secret_device(device_secret):
    db = get_db()
    cursor = db.cursor()
    cursor.execute('SELECT * FROM devices WHERE device_secret = ?', device_secret)
    device = cursor.fetchone()
    if device is not None:
        return True
    else:
        return False


def insert_group_device(cursor, switch):
    if switch == 'RGBController':
        cursor.execute('INSERT INTO RGBControllerState (red, green, blue, state, device_id) VALUES (?, ?, ?, ?, ?)',
                       (1, 1, 1, False, cursor.lastrowid))
    elif switch == 'DoorLock':
        cursor.execute('INSERT INTO DoorLockState (state, device_id) VALUES (?, ?)',
                       ('auto', cursor.lastrowid))
    elif switch == 'LightController':
        cursor.execute('INSERT INTO LightControllerState (state, device_id) VALUES (?, ?)',
                       ('auto', cursor.lastrowid))


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
