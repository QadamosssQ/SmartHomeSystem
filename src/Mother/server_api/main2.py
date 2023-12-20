from flask import Flask, request, jsonify
from flask_cors import CORS  # Import CORS from flask_cors

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

rgb_state = {'r': 0, 'g': 0, 'b': 0, 'state': False}  # Removed 'effect'
door_state = {'state': False}

@app.route('/api/GetRGBController', methods=['GET'])
def get_rgb_controller():
    return jsonify(rgb_state)

@app.route('/api/SetRGBController', methods=['POST'])  # Changed from 'PUT' to 'POST'
def set_rgb_controller():
    global rgb_state

    new_rgb_state = request.get_json()

    if 'r' in new_rgb_state and 'g' in new_rgb_state and 'b' in new_rgb_state and 'state' in new_rgb_state:  # Removed 'effect'
        if (
            0 <= new_rgb_state['r'] <= 255
            and 0 <= new_rgb_state['g'] <= 255
            and 0 <= new_rgb_state['b'] <= 255
            and new_rgb_state['state'] in [True, False]
        ):
            rgb_state = new_rgb_state
            return 'OK', 200
    return jsonify({'error': 'Invalid RGB value'}), 400

@app.route('/api/GetDoorLock', methods=['GET'])  # Changed from 'POST' to 'GET'
def get_door_lock():
    return jsonify(door_state)

@app.route('/api/SetDoorLock', methods=['POST'])  # Changed from 'PUT' to 'POST'
def set_door_lock():
    global door_state

    new_door_state = request.get_json()

    if 'state' in new_door_state:
        if new_door_state['state'] in ['on', 'off', 'auto']:
            door_state = new_door_state
            return 'OK', 200
        return jsonify({'error': 'Invalid door value'}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # Allow external connections
