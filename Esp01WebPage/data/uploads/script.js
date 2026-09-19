const lampStatus = {
    on: true,
    red: 255,
    green: 255,
    blue: 255,
    brightness: 255,
    effects: {
        breathe: false,
        pulse: false,
        rainbow: false,
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
            switch (effect) {
                case "breathe":
                    this.effects.audio = false;
                    this.effects.pulse = false;
                    this.effects.candle = false;
                    break;
                case "audio":
                    this.effects.breathe = false;
                    this.effects.pulse = false;
                    this.effects.candle = false;
                    break;
                case "pulse":
                    this.effects.breathe = false;
                    this.effects.audio = false;
                    this.effects.candle = false;
                    break;
                case "candle":
                    this.effects.breathe = false;
                    this.effects.audio = false;
                    this.effects.pulse = false;
                    break;
                case "rainbow":
                    this.effects.colorSequence = false;
                case "colorSequence":
                    this.effects.rainbow = false;
                    break;
                default:
                    break;
            }
        }
        sendDataToEsp();
    }
};

const rgbToHex = (r, g, b) =>
    "#" + [r, g, b].map(x => x.toString(16).padStart(2, '0')).join('');

function onBrightnessChange(event) {
    const value = event.target.value;
    lampStatus.setBrightness(value);
    document.getElementsByClassName("led-stripe")[0].style.opacity = value / 255;
}

function onPowerButtonClick(event) {
    const classList = event.target.classList;
    const result = classList.toggle('active');
    event.target.children[1].innerText = result ? "APAGAR" : "ENCENDER";
    document.querySelector(".led-stripe").style.visibility = result ? "visible" : "hidden";
    lampStatus.setOnState(result);
}

function onRGBButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P") {
            node.style.display = "none";
        } else {
            node.innerText = "Selecciona un color para la lampara RGB"
        }
    };
    document.getElementsByClassName("rgb-tab")[0].style.display = "flex";
}

function updateColorUI(value, updateEsp = true) {
    const colorPicker = document.getElementById("colorValue");
    colorPicker.value = value;
    document.querySelector('input[type="color"').style.boxShadow = `${value} 0px 0px 10px 0px`;
    document.getElementsByClassName("led-stripe")[0].style.backgroundImage = `repeating-linear-gradient(to right, ${value}, ${value} 15px, transparent 15px, transparent 20px)`;
    const r = parseInt(value.substring(1, 3), 16);
    const g = parseInt(value.substring(3, 5), 16);
    const b = parseInt(value.substring(5, 7), 16);
    if (updateEsp)
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

//TODO, only update ESP when input ends (user drops slider). use onchange instead of oninput?????????????? to prevent flooding esp with requests
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

function onRedInput(event) {
    const value = parseInt(event.target.value);
    const hexValue = rgbToHex(value, lampStatus.green, lampStatus.blue);
    document.querySelector("input[type='color']").value = hexValue;
    updateColorUI(hexValue, false);
}
function onGreenInput(event) {
    const value = parseInt(event.target.value);
    const hexValue = rgbToHex(lampStatus.red, value, lampStatus.blue);
    document.querySelector("input[type='color']").value = hexValue;
    updateColorUI(hexValue, false);
}

function onBlueInput(event) {
    const value = parseInt(event.target.value);
    const hexValue = rgbToHex(lampStatus.red, lampStatus.green, value);
    document.querySelector("input[type='color']").value = hexValue;
    updateColorUI(hexValue, false);
}


function onHexInputChange(event) {
    event.target.value = event.target.value.replace(/[^#a-fA-F0-9]/g, '');
    const value = event.target.value;
    if (!value.startsWith("#"))
        event.target.value = "#";
    const hexRegex = /^#[0-9A-Fa-f]{6}$/
    if (hexRegex.test(value)) {
        updateColorUI(value);
        const colorInput = document.querySelector("input[type='color']");
        colorInput.value = value;
        colorInput.dispatchEvent(new Event('change'));
    }
}

function onImgButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P") {
            node.style.display = "none";
        } else {
            node.innerText = "Sube una imagen y despues selecciona el color deseado de dicha imagen"
        }

    };
    document.getElementsByClassName("image-tab")[0].style.display = "flex";
}

function onImageDrop(event) {
    event.preventDefault();
    const data = event.dataTransfer.files;
    const fileInput = document.getElementById("imageInput");
    fileInput.files = data;
    fileInput.dispatchEvent(new Event('change'));
}

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
            document.getElementById("title").innerText = "Selecciona un color para la lampara RGB";
        });
    };
    img.src = URL.createObjectURL(event.target.files[0]);
}

function onRainbowButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    lampStatus.setEffect('rainbow', event.target.classList.toggle('active'));
    document.getElementById("colorSequence").classList.remove('active');
}

function onBreatheButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    lampStatus.setEffect('breathe', event.target.classList.toggle('active'));
    document.getElementById("audio").classList.remove('active');
    document.getElementById("pulse").classList.remove('active');
    document.getElementById("candle").classList.remove('active');
}

function onAudioButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    lampStatus.setEffect('audio', event.target.classList.toggle('active'));
    document.getElementById("breathe").classList.remove('active');
    document.getElementById("pulse").classList.remove('active');
    document.getElementById("candle").classList.remove('active');
}

function onPulseButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    lampStatus.setEffect('pulse', event.target.classList.toggle('active'));
    document.getElementById("audio").classList.remove('active');
    document.getElementById("breathe").classList.remove('active');
    document.getElementById("candle").classList.remove('active');
}

function onCandleButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    lampStatus.setEffect('candle', event.target.classList.toggle('active'));
    document.getElementById("audio").classList.remove('active');
    document.getElementById("breathe").classList.remove('active');
    document.getElementById("pulse").classList.remove('active');
}

function onColorSequenceButtonClick(event) {
    for (const node of document.getElementsByClassName('panel')[0].children) {
        if (node.tagName != "P")
            node.style.display = "none";
    };
    lampStatus.setEffect('colorSequence', event.target.classList.toggle('active'));
    document.getElementById("rainbow").classList.remove('active');
}


function sendDataToEsp() {
    fetch('/changeLampStatus', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(lampStatus)
    }).then(response => {
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        console.log("Datos enviados correctamente");
    }).catch(err => {
        console.error(err);
        //alert("Error enviando datos");
    });
}

/*
setInterval(async () => {
    const response = await fetch("/lampStatus");
    const body = await response.text();
    if (body.length > 0) {
        { on, red, green, blue, brightness, effects }=JSON.parse(body);
        alert(JSON.stringify(JSON.parse(body)));
    }
}, 1000);*/