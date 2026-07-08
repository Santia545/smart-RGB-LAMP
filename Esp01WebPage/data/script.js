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

function onColorChange(event) {
    const value = event.target.value;
    document.getElementById("colorValue").value = value;
}