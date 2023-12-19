from flask import Flask, request, jsonify

app = Flask(__name__)

rgb_state = {'r': 0, 'g': 0, 'b': 0, 'effect': 0, 'state': False}
door_state = {'state': False}

@app.route('/api/GetRGBController', methods=['POST'])
def get_rgb_controller():
    return jsonify(rgb_state)

@app.route('/api/SetRGBController', methods=['PUT'])
def set_rgb_controller():
    global rgb_state

    new_rgb_state = request.get_json()

    if 'r' in new_rgb_state and 'g' in new_rgb_state and 'b' in new_rgb_state and 'effect' in new_rgb_state and 'state' in new_rgb_state:
        if (
            0 <= new_rgb_state['r'] <= 255
            and 0 <= new_rgb_state['g'] <= 255
            and 0 <= new_rgb_state['b'] <= 255
            and 0 <= new_rgb_state['effect'] <= 255
            and new_rgb_state['state'] in [True, False]
        ):
            rgb_state = new_rgb_state
            return 'OK', 200  # Return an "OK" message with HTTP status code 200

    return jsonify({'error': 'Invalid RGB value'}), 400  # Return an error message with HTTP status code 400 for bad request

@app.route('/api/GetDoorLock', methods=['POST'])
def get_door_lock():
    return jsonify(door_state)

@app.route('/api/SetDoorLock', methods=['PUT'])
def set_door_lock():
    global door_state

    new_door_state = request.get_json()

    if 'state' in new_door_state:
        if new_door_state['state'] in ['on', 'off', 'auto']:
            door_state = new_door_state
            return 'OK', 200  # Return an "OK" message with HTTP status code 200
        return jsonify({'error': 'Invalid door value'}), 400  # Return an error message with HTTP status code 400 for bad request

if __name__ == '__main__':
    app.debug = True
    app.run()
