import secrets
import socket
import sqlite3

import bcrypt
from flask import Flask, request, jsonify, g
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

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


@app.errorhandler(404)
def not_found(error):
    return jsonify({'error': 'Not Found', 'message': 'The requested URL was not found on the server.'}), 404


@app.route('/api/GetLightController', methods=['POST'])
def get_light_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:
            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp
                cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                               (fetch_data['device_secret'], user_id))
                device = cursor.fetchone()
                if device is not None:
                    cursor.execute('SELECT * FROM LightControllerState WHERE device_id = ?', (device[0],))
                    light_controller_state = cursor.fetchone()
                    if light_controller_state is not None:
                        return jsonify({'state': light_controller_state[1]}), 200
                    else:
                        return jsonify({'error': 'Lack of data or invalid device type'}), 400
                else:
                    return jsonify({'error': 'Invalid device secret'}), 400
            else:
                return jsonify({'error': 'Invalid user secret'}), 400
        else:
            return jsonify({'error': 'Invalid request'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/SetLightController', methods=['POST'])
def set_light_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data and 'state' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

                if check_device_secret(fetch_data['device_secret']):
                    if 'state' in fetch_data:
                        if fetch_data['state'] == "on" or fetch_data['state'] == "off":
                            cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                                           (fetch_data['device_secret'], user_id))
                            device = cursor.fetchone()
                            if device is not None:
                                cursor.execute('UPDATE LightControllerState SET state = ? WHERE device_id = ?',
                                               (fetch_data['state'], device[0]))
                                db.commit()
                                return jsonify({'message': 'Light controller state changed'}), 200
                            else:
                                return jsonify({'error': 'Invalid device secret'}), 400
                        else:
                            return jsonify({'error': 'Invalid state value'}), 400
                    else:
                        return jsonify({'error': 'state value is missing'}), 400
                else:
                    return jsonify({'error': 'Invalid device_secret value'}), 400
            else:
                return jsonify({'error': 'Invalid secret key'}), 400
        else:
            return jsonify({'error': 'secret_key value is missing'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/GetRGBController', methods=['POST'])
def get_rgb_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])
            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp
                cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                               (fetch_data['device_secret'], user_id))
                device = cursor.fetchone()
                if device is not None:
                    cursor.execute('SELECT * FROM RGBControllerState WHERE device_id = ?', (device[0],))
                    light_controller_state = cursor.fetchone()
                    if light_controller_state is not None:
                        return jsonify({'state': light_controller_state[4], 'r': light_controller_state[1],
                                        'g': light_controller_state[2], 'b': light_controller_state[3]}), 200
                    else:
                        return jsonify({'error': 'Lack of data or invalid device type'}), 400
                else:
                    return jsonify({'error': 'Invalid device secret'}), 400
            else:
                return jsonify({'error': 'Invalid user secret'}), 400

        else:
            return jsonify({'error': 'Invalid request'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/SetRGBController', methods=['POST'])
def set_rgb_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

                if check_device_secret(fetch_data['device_secret']):
                    if 'r' in fetch_data and 'g' in fetch_data and 'b' in fetch_data and 'state' in fetch_data:
                        if (
                                (0 > fetch_data['r'] or fetch_data['r'] > 255) or
                                (0 > fetch_data['g'] or fetch_data['g'] > 255) or
                                (0 > fetch_data['b'] or fetch_data['b'] > 255) or
                                fetch_data['state'] != 0) or fetch_data['state'] != 1:
                            return jsonify({'error': 'Invalid RGB or state value'}), 400
                        else:
                            try:
                                cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                                               (fetch_data['device_secret'], user_id))
                                device = cursor.fetchone()
                                if device is not None:
                                    if fetch_data['r'] == 0 and fetch_data['g'] == 0 and fetch_data['b'] == 0:
                                        fetch_data['state'] = 'off'
                                    cursor.execute(
                                        'UPDATE RGBControllerState SET red = ?, green = ?, blue = ?, state = ? WHERE device_id = ?',
                                        (fetch_data['r'], fetch_data['g'], fetch_data['b'], fetch_data['state'],
                                         device[0]))
                                    db.commit()
                                    return jsonify({'message': 'RGB controller state changed'}), 200
                                else:
                                    return jsonify({'error': 'Invalid device secret'}), 400
                            except Exception as e:
                                return jsonify({'error': 'Database error: ' + str(e)}), 400
                        return jsonify({'error': 'Invalid RGB or state value'}), 400
                else:
                    return jsonify({'error': 'Invalid secret key'}), 400
            else:
                return jsonify({'error': 'Invalid user secret'}), 400
        else:
            return jsonify({'error': 'No secret key'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/GetDoorLock', methods=['POST'])
def get_door_lock():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp
                cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                               (fetch_data['device_secret'], user_id))
                device = cursor.fetchone()
                if device is not None:
                    cursor.execute('SELECT * FROM DoorLockState WHERE device_id = ?', (device[0],))
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
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/SetDoorLock', methods=['POST'])
def set_door_lock():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

                if check_device_secret(fetch_data['device_secret']):
                    if 'state' in fetch_data:
                        if fetch_data['state'] in ['on', 'off', 'auto']:

                            cursor.execute('SELECT * FROM devices WHERE device_secret = ? AND user_id = ?',
                                           (fetch_data['device_secret'], user_id))
                            device = cursor.fetchone()
                            if device is not None:
                                cursor.execute('UPDATE DoorLockState SET state = ? WHERE device_id = ?',
                                               (fetch_data['state'], device[0]))
                                db.commit()
                                return jsonify({'message': 'Door lock state changed'}), 200
                            else:
                                return jsonify({'error': 'Invalid device secret'}), 400

                        return jsonify({'error': 'Invalid door lock value'}), 400
                else:
                    return jsonify({'error': 'Invalid secret key'}), 400
            else:
                return jsonify({'error': 'Invalid user secret'}), 400

        else:
            jsonify({'error': 'Invalid request'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/RegisterUser', methods=['POST'])
def register_user():
    try:
        user = request.get_json()
        if 'login' in user and 'password' in user:
            user['password'] = hash_password(user['password'])
            db = get_db()
            cursor = db.cursor()
            cursor.execute('SELECT * FROM users WHERE login = ?', (user['login'],))
            if cursor.fetchone() is None:
                new_user_secret = secrets.token_urlsafe(32)
                cursor.execute('INSERT INTO users (login, password, user_secret) VALUES (?, ?, ?)',
                               (user['login'], user['password'], new_user_secret))
                db.commit()
                return jsonify({'user_secret': new_user_secret}), 200
            return jsonify({'error': 'Username already exists'}), 400
        else:
            return jsonify({'error': 'Invalid user'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/RegisterDevice', methods=['POST'])
def register_device():
    try:
        fetch_data = request.get_json()
        if 'user_secret' in fetch_data and 'device_name' in fetch_data and 'type' in fetch_data:
            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp
                new_device_secret = secrets.token_urlsafe(32)

                if fetch_data['type'] not in devices:
                    devices_str = ', '.join(devices)
                    return jsonify({'error': 'Invalid device type. Available devices: ' + devices_str}), 400

                else:
                    cursor.execute(
                        'INSERT INTO devices (user_id, device_name, type, device_secret) VALUES (?, ?, ?, ?)',
                        (user_id, fetch_data['device_name'], fetch_data['type'], new_device_secret))

                    switch = fetch_data['type']
                    insert_group_device(cursor, switch)

                    db.commit()
                    return jsonify({'device_name': fetch_data['device_name'], 'device_secret': new_device_secret}), 200
            return jsonify({'error': 'Invalid user_secret'}), 400
        else:
            return jsonify({'error': 'Invalid device'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/login', methods=['POST'])
def get_user_secret():
    try:
        user = request.get_json()
        if 'login' in user and 'password' in user:
            db = get_db()
            cursor = db.cursor()
            cursor.execute('SELECT * FROM users WHERE login = ? LIMIT 1', (user['login'],))
            user_data = cursor.fetchone()
            if user_data is not None:
                if verify_password(user['password'], user_data[2]):
                    return jsonify({'user_secret': user_data[3]}), 200
                else:
                    return jsonify({'error': 'Invalid password'}), 400
            else:
                return jsonify({'error': 'Invalid login'}), 400
        else:
            return jsonify({'error': 'Invalid user'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/GetDevices', methods=['POST'])
def show_device_secret():
    try:
        fetch_data = request.get_json()
        if 'user_secret' in fetch_data:
            db = get_db()
            cursor = db.cursor()
            cursor.execute(
                'SELECT devices.device_secret, devices.device_name, devices.type FROM devices JOIN users ON devices.user_id = users.id WHERE users.user_secret = ?',
                (fetch_data['user_secret'],))
            secret_data = cursor.fetchall()

            if secret_data:
                device_secrets = [{'name': device[1], 'secret_key': device[0], 'type': device[2]} for device in
                                  secret_data]
                return jsonify(device_secrets), 200
            else:
                return jsonify({'error': 'No devices found for the user'}), 400
        else:
            return jsonify({'error': 'Invalid request'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/RemoveDevice', methods=['POST'])
def remove_device():
    try:
        fetch_data = request.get_json()
        if 'user_secret' in fetch_data and 'device_secret' in fetch_data:
            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = validate_user(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

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
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


def check_device_secret(device_secret):
    try:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT * FROM devices WHERE device_secret = ?', (device_secret,))
        device = cursor.fetchone()
        if device is not None:
            return True
        else:
            return False
    except Exception as e:
        print(f"Error in check_secret_device: {e}")
        return False


def insert_group_device(cursor, switch):
    try:
        if switch == 'RGBController':
            cursor.execute('INSERT INTO RGBControllerState (red, green, blue, state, device_id) VALUES (?, ?, ?, ?, ?)',
                           (1, 1, 1, False, cursor.lastrowid))
        elif switch == 'DoorLock':
            cursor.execute('INSERT INTO DoorLockState (state, device_id) VALUES (?, ?)',
                           ('auto', cursor.lastrowid))
        elif switch == 'LightController':
            cursor.execute('INSERT INTO LightControllerState (state, device_id) VALUES (?, ?)',
                           ('auto', cursor.lastrowid))
    except Exception as e:
        print(f"Error in insert_group_device: {e}")
        return False


def validate_user(user_secret):
    try:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT id FROM users WHERE user_secret = ?', (user_secret,))
        user_id = cursor.fetchone()
        if user_id is not None:
            return user_id[0]
        else:
            return False
    except Exception as e:
        print(f"Error in ValidateUser: {e}")
        return False


def hash_password(password):
    salt = bcrypt.gensalt()
    hashed_password = bcrypt.hashpw(password.encode('utf-8'), salt)
    return hashed_password


def verify_password(input_password, stored_hashed_password):
    return bcrypt.checkpw(input_password.encode('utf-8'), stored_hashed_password)


def is_valid_ip(ip_address):
    try:
        socket.inet_pton(socket.AF_INET, ip_address)
        return True
    except socket.error:
        return False


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)



# Example of how to use headers in api !!!! NEVER PUT "_" as a header name
# @app.route('/api/GetLightController', methods=['GET'])
# def get_light_controller():
#     try:
#
#         if 'device-secret' in request.headers and 'user-secret' in request.headers:
#             db = get_db()
#             cursor = db.cursor()
#             user_id_tuple_temp = valid
