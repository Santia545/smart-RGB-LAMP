const lampStatus = {
    on: true,
    red: 255,
    green: 255,
    blue: 255,
    brightness: 255,
    effects: {
        breathe: true,
        pulse: false,
        rainbow: true,
        audio: true,
    },
    setOnState: function (on) {
        this.on = on;
        sendDataToEsp();
    },
    setRed: function (r) {
        this.red = r;
        sendDataToEsp();
    },
    setGreen: function (g) {
        this.green = g;
        sendDataToEsp();
    },
    setBlue: function (b) {
        this.blue = b;
        sendDataToEsp();
    },
    setColor: function (r, g, b) {
        this.red = r;
        this.green = g;
        this.blue = b;
        sendDataToEsp();
    },
    setBrightness: function (brightness) {
        this.brightness = brightness;
        sendDataToEsp();
    },
    setEffect: function (effect, value) {
        if (this.effects.hasOwnProperty(effect)) {
            this.effects[effect] = value;
        }
        sendDataToEsp();
    }
};

const rgbToHex = (r, g, b) =>
    "#" + [r, g, b].map(x => x.toString(16).padStart(2, '0')).join('');

function loadImage(event) {
    var canvas = document.getElementById("imageCanvas");
    var ctx = canvas.getContext("2d");
    canvas.style.display = "block";
    var img = new Image();
    img.onload = function () {
        canvas.width = img.width;
        canvas.height = img.height;
        ctx.drawImage(img, 0, 0);
        canvas.addEventListener("click", function (e) {
            var rect = canvas.getBoundingClientRect();
            const scaleX = canvas.width / rect.width;
            const scaleY = canvas.height / rect.height;
            const x = Math.floor((e.clientX - rect.left) * scaleX);
            const y = Math.floor((e.clientY - rect.top) * scaleY);
            const pixelData = ctx.getImageData(x, y, 1, 1).data;
            var rgbColor = `rgb(${pixelData[0]}, ${pixelData[1]}, ${pixelData[2]})`;
            const colorInput = document.querySelector("input[type='color']");
            colorInput.value = rgbToHex(pixelData[0], pixelData[1], pixelData[2]);
            colorInput.dispatchEvent(new Event('change'));
            document.getElementsByClassName("image-tab")[0].style.display = "none";
            document.getElementsByClassName("rgb-tab")[0].style.display = "flex";
        });
    };
    img.src = URL.createObjectURL(event.target.files[0]);
}

function onBrightnessChange(event) {
    const value = event.target.value;
    lampStatus.setBrightness(value);
    document.getElementsByClassName("led-stripe")[0].style.opacity = value / 255;
}

function onRGBButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    document.getElementsByClassName("rgb-tab")[0].style.display = "flex";
}
function onImgButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    document.getElementsByClassName("image-tab")[0].style.display = "flex";
}
function updateColorUI(value) {
    const colorPicker = document.getElementById("colorValue");
    colorPicker.value = value;
    document.getElementsByClassName("led-stripe")[0].style.backgroundImage = `repeating-linear-gradient(to right, ${value}, ${value} 15px, transparent 15px, transparent 20px)`;
    const r = parseInt(value.substring(1, 3), 16);
    const g = parseInt(value.substring(3, 5), 16);
    const b = parseInt(value.substring(5, 7), 16);
    lampStatus.setColor(r, g, b);
}

function onColorChange(event) {
    const value = event.target.value;
    const r = parseInt(value.substring(1, 3), 16);
    const g = parseInt(value.substring(3, 5), 16);
    const b = parseInt(value.substring(5, 7), 16);
    document.getElementById('redSlider').value = r;
    document.getElementById('greenSlider').value = g;
    document.getElementById('blueSlider').value = b;
    updateColorUI(value);
}

//TODO, only update ESP when input ends (user drops slider). use onchange instead of oninput??????????????
function onRedChange(event) {
    const value = parseInt(event.target.value);
    const hexValue = rgbToHex(value, lampStatus.green, lampStatus.blue);
    document.querySelector("input[type='color']").value = hexValue;
    updateColorUI(hexValue);
}
function onGreenChange(event) {
    const value = parseInt(event.target.value);
    const hexValue = rgbToHex(lampStatus.red, value, lampStatus.blue);
    document.querySelector("input[type='color']").value = hexValue;
    updateColorUI(hexValue);
}

function onBlueChange(event) {
    const value = parseInt(event.target.value);
    const hexValue = rgbToHex(lampStatus.red, lampStatus.green, value);
    document.querySelector("input[type='color']").value = hexValue;
    updateColorUI(hexValue);
}
function onHexInputChange(event) {
    const value = event.target.value;
    const hexRegex = /^#[0-9A-Fa-f]{6}$/
    if (hexRegex.test(value)) {
        updateColorUI(value);
        const colorInput = document.querySelector("input[type='color']");
        colorInput.value = value;
        colorInput.dispatchEvent(new Event('change'));
    }
}

function sendDataToEsp() {
    fetch('/changeLampStatus', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(lampStatus)
    }).then(() => console.log("Datos enviados correctamente")).catch(() => alert("Error enviando datos"));
}

/*
setInterval(async () => {
    const response = await fetch("/lampStatus");
    const body = await response.text();
    if (body.length > 0) {
        { red, green, blue, brightness, effects }=JSON.parse(body);
        alert(JSON.stringify(JSON.parse(body)));
    }
}, 1000);*/