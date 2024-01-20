let led_status = 0;
let r = 0;
let g = 0;
let b = 0;

let button = document.getElementById('set_color');
let button_onoff = document.getElementById('toggle');

function fetchAPI() {
    fetch('http://192.168.0.107:4040/api')
        .then(response => response.json())
        .then(data => {
            if (data.state == 1) {
                led_status = 1;
                button_onoff.innerText = "State: ON";
                r = parseInt(data.r);
                g = parseInt(data.g);
                b = parseInt(data.b);

                let choose_color = document.getElementById('color');
                choose_color.value = rgbToHex(r, g, b);
                console.log(rgbToHex(r, g, b));
            } else {
                led_status = "off";
                button_onoff.innerText = "State: OFF";
            }
        })
        .catch(err => {
            console.error(err);
        });

}

fetchAPI();

if (led_status == 1) {
    button_onoff.innerText = "State: ON";
} else {
    button_onoff.innerText = "State: OFF";
}

button.addEventListener('click', () => {
    let choose_color = document.getElementById('color').value;
    console.log(choose_color);

    let rgb = hexToRgb(choose_color);

    if (rgb.r == 0 && rgb.g == 0 && rgb.b == 0) {
        led_status = 0;
        button_onoff.innerText = "State: OFF";
    } else {
        led_status = 1;
        button_onoff.innerText = "State: ON";
    }

    updateAPI(rgb.r, rgb.g, rgb.b, led_status);
});

function updateAPI(r, g, b, state) {
    const data = {
        r: r,
        g: g,
        b: b,
        state: state
    };

    fetch('http://192.168.0.107:4040/api', {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
    .then(response => response.json())
    .then(result => {
        document.getElementById('response').textContent = result.message;

        refreshPage();
    })
    .catch(error => {
        console.error('Error: ', error);
        document.getElementById('response').textContent = error;
    });
}

function refreshPage() {
    setTimeout(function() {
        document.getElementById('response').textContent = '';
    }, 1500); 
  }
  

function rgbToHex(r, g, b) {
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        throw new Error('Invalid RGB values.');
    }

    var hexR = r.toString(16).padStart(2, '0');
    var hexG = g.toString(16).padStart(2, '0');
    var hexB = b.toString(16).padStart(2, '0');

    return '#' + hexR + hexG + hexB;
}

function hexToRgb(hex) {
    if (!hex) {
        throw new Error('No HEX value provided.');
    }

    var regex = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i;
    var result = regex.exec(hex);

    if (!result) {
        throw new Error('Invalid HEX value.');
    }

    return {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    };
}
