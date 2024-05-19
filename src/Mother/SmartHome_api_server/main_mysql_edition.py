import secrets
import mysql.connector
import socket

import bcrypt
from flask import Flask, request, jsonify, g
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

devices = ['RGBController', 'LightController', 'DoorLock']

DATABASE_CONFIG = {
    'user': 'yourusername',
    'password': 'yourpassword',
    'host': 'localhost',
    'database': 'yourdatabase',
    'raise_on_warnings': True
}


def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = mysql.connector.connect(**DATABASE_CONFIG)
    return db


@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()


@app.errorhandler(404)
def not_found(error):
    return jsonify({'error': 'Not Found', 'message': 'The requested URL was not found on the server.'}), 404


@app.route('/api/GetLightControllerState', methods=['GET'])
def get_light_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:
            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = ValidateUser(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp
                cursor.execute('SELECT * FROM devices WHERE device_secret = %s AND user_id = %s',
                               (fetch_data['device_secret'], user_id))
                device = cursor.fetchone()
                if device is not None:
                    cursor.execute('SELECT * FROM LightControllerState WHERE device_id = %s', (device[0],))
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


@app.route('/api/SetLightController', methods=['POST'])
def set_light_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = ValidateUser(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

                if check_secret_device(fetch_data['device_secret']):
                    if 'state' in fetch_data:
                        if fetch_data['state'] in ['on', 'off', 'auto']:
                            cursor.execute('SELECT * FROM devices WHERE device_secret = %s AND user_id = %s',
                                           (fetch_data['device_secret'], user_id))
                            device = cursor.fetchone()
                            if device is not None:
                                cursor.execute('UPDATE LightControllerState SET state = %s WHERE device_id = %s',
                                               (fetch_data['state'], device[0]))
                                db.commit()
                                return jsonify({'message': 'Light controller state changed'}), 200
                            else:
                                return jsonify({'error': 'Invalid device secret'}), 400

                        return jsonify({'error': 'Invalid light value'}), 400
            else:
                return jsonify({'error': 'Invalid secret key'}), 400
        else:
            return jsonify({'error': 'No secret key'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/GetRGBControllerState', methods=['GET'])
def get_rgb_controller():
    return jsonify({'r': 0, 'g': 0, 'b': 0, 'state': 'off'}), 200
    # try:
    #     fetch_data = request.get_json()
    #     if 'device_secret' in fetch_data and 'user_secret' in fetch_data:
    #
    #         db = get_db()
    #         cursor = db.cursor()
    #         user_id_tuple_temp = ValidateUser(fetch_data['user_secret'])
    #         if user_id_tuple_temp is not False:
    #             user_id = user_id_tuple_temp
    #             cursor.execute('SELECT * FROM devices WHERE device_secret = %s AND user_id = %s',
    #                            (fetch_data['device_secret'], user_id))
    #             device = cursor.fetchone()
    #             if device is not None:
    #                 cursor.execute('SELECT * FROM RGBControllerState WHERE device_id = %s', (device[0],))
    #                 light_controller_state = cursor.fetchone()
    #                 if light_controller_state is not None:
    #                     return jsonify({'state': light_controller_state[1], 'r': light_controller_state[2],
    #                                     'g': light_controller_state[3], 'b': light_controller_state[4]}), 200
    #                 else:
    #                     return jsonify({'error': 'No light controller state'}), 400
    #             else:
    #                 return jsonify({'error': 'Invalid device secret'}), 400
    #         else:
    #             return jsonify({'error': 'Invalid user secret'}), 400
    #
    #     else:
    #         return jsonify({'error': 'Invalid request'}), 400
    # except Exception as e:
    #     return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/SetRGBController', methods=['POST'])
def set_rgb_controller():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = ValidateUser(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

                if check_secret_device(fetch_data['device_secret']):
                    if 'r' in fetch_data and 'g' in fetch_data and 'b' in fetch_data and 'state' in fetch_data:
                        if (
                                0 <= fetch_data['r'] <= 255
                                and 0 <= fetch_data['g'] <= 255
                                and 0 <= fetch_data['b'] <= 255
                                and fetch_data['state'] in ['on', 'off', 'auto']
                        ):
                            try:
                                cursor.execute('SELECT * FROM devices WHERE device_secret = %s AND user_id = %s',
                                               (fetch_data['device_secret'], user_id))
                                device = cursor.fetchone()
                                if device is not None:
                                    if fetch_data['r'] == 0 and fetch_data['g'] == 0 and fetch_data['b'] == 0:
                                        fetch_data['state'] = 'off'
                                    cursor.execute(
                                        'UPDATE RGBControllerState SET red = %s, green = %s, blue = %s, state = %s WHERE device_id = %s',
                                        (fetch_data['r'], fetch_data['g'], fetch_data['b'], fetch_data['state'],
                                         device[0]))
                                    db.commit()
                                    return jsonify({'message': 'RGB controller state changed'}), 200
                                else:
                                    return jsonify({'error': 'Invalid device secret'}), 400
                            except Exception as e:
                                return jsonify({'error': 'Database error: ' + str(e)}), 400
                        return jsonify({'error': 'Invalid RGB value'}), 400
                else:
                    return jsonify({'error': 'Invalid secret key'}), 400
            else:
                return jsonify({'error': 'Invalid user secret'}), 400
        else:
            return jsonify({'error': 'No secret key'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/GetDoorLockState', methods=['GET'])
def get_door_lock():
    try:
        fetch_data = request.get_json()
        if 'device_secret' in fetch_data and 'user_secret' in fetch_data:

            db = get_db()
            cursor = db.cursor()
            user_id_tuple_temp = ValidateUser(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp
                cursor.execute('SELECT * FROM devices WHERE device_secret = %s AND user_id = %s',
                               (fetch_data['device_secret'], user_id))
                device = cursor.fetchone()
                if device is not None:
                    cursor.execute('SELECT * FROM DoorLockState WHERE device_id = %s', (device[0],))
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
            user_id_tuple_temp = ValidateUser(fetch_data['user_secret'])

            if user_id_tuple_temp is not False:
                user_id = user_id_tuple_temp

                if check_secret_device(fetch_data['device_secret']):
                    if 'state' in fetch_data:
                        if fetch_data['state'] in ['locked', 'unlocked']:
                            cursor.execute('SELECT * FROM devices WHERE device_secret = %s AND user_id = %s',
                                           (fetch_data['device_secret'], user_id))
                            device = cursor.fetchone()
                            if device is not None:
                                cursor.execute('UPDATE DoorLockState SET state = %s WHERE device_id = %s',
                                               (fetch_data['state'], device[0]))
                                db.commit()
                                return jsonify({'message': 'Door lock state changed'}), 200
                            else:
                                return jsonify({'error': 'Invalid device secret'}), 400
                        return jsonify({'error': 'Invalid lock state'}), 400
            else:
                return jsonify({'error': 'Invalid secret key'}), 400
        else:
            return jsonify({'error': 'No secret key'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/RegisterDevice', methods=['POST'])
def register_device():
    try:
        fetch_data = request.get_json()
        if 'name' in fetch_data and 'type' in fetch_data and 'secret' in fetch_data:
            if fetch_data['type'] in devices:
                device_secret = secrets.token_hex(16)

                db = get_db()
                cursor = db.cursor()
                user_id_tuple_temp = ValidateUser(fetch_data['secret'])
                if user_id_tuple_temp is not False:
                    user_id = user_id_tuple_temp
                    cursor.execute('INSERT INTO devices (device_name, device_type, device_secret, user_id) VALUES (%s, %s, %s, %s)',
                                   (fetch_data['name'], fetch_data['type'], device_secret, user_id))
                    db.commit()
                    return jsonify({'message': 'Device registered', 'device_secret': device_secret}), 201
                else:
                    return jsonify({'error': 'Invalid secret key'}), 400
            else:
                return jsonify({'error': 'Invalid device type'}), 400
        else:
            return jsonify({'error': 'Missing data'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


@app.route('/api/RegisterUser', methods=['POST'])
def register_user():
    try:
        fetch_data = request.get_json()
        if 'username' in fetch_data and 'password' in fetch_data:
            username = fetch_data['username']
            password = fetch_data['password']
            password_hash = bcrypt.hashpw(password.encode('utf-8'), bcrypt.gensalt())

            db = get_db()
            cursor = db.cursor()
            cursor.execute('INSERT INTO users (username, password) VALUES (%s, %s)', (username, password_hash))
            db.commit()
            return jsonify({'message': 'User registered'}), 201
        else:
            return jsonify({'error': 'Missing data'}), 400
    except Exception as e:
        return jsonify({'error': 'An error occurred: ' + str(e)}), 500


def ValidateUser(user_secret):
    try:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT id FROM users WHERE user_secret = %s', (user_secret,))
        user_id = cursor.fetchone()
        if user_id is not None:
            return user_id[0]
        else:
            return False
    except Exception as e:
        return False


def check_secret_device(device_secret):
    try:
        db = get_db()
        cursor = db.cursor()
        cursor.execute('SELECT id FROM devices WHERE device_secret = %s', (device_secret,))
        device_id = cursor.fetchone()
        if device_id is not None:
            return True
        else:
            return False
    except Exception as e:
        return False


@app.route('/')
def index():
    return jsonify({'message': 'Hello, World!'}), 200


if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=3000)
