#ifndef PortalCustomStyles_h
#define PortalCustomStyles_h

static const char portalPage[] PROGMEM = R"rawliteral(
    document.addEventListener("DOMContentLoaded", () => {
        const style = document.createElement("style");
        style.textContent = `
        body{
            font-family: Arial, sans-serif;
            background:#181818;
            color:white;
        }

        select,
        input:not([type=submit]){
            width:100%;
            padding:10px;
            margin-top:6px;
            margin-bottom:14px;
            border-radius:8px;
            border:1px solid #444;
            background:#2b2b2b;
            color:white;
            font-size:15px;
        }

        :not([type^=s]):focus{outline:2px solid rgb(87, 174, 255)}

        input[type=submit]{
            font-size: 12px;
            background:transparent;
            color:white;
            border:1.6px solid white;
            border-radius:10px;
            padding: 10px 10px;
            cursor:pointer;
        }
        input[type="checkbox"] {
            width: initial;
        }
        a:link{color:rgb(236, 236, 236)} 

        form[action$="/restart"]{
            display:none;
        }`;

        document.head.appendChild(style);

        /*document.querySelector('input[name="WiFiSettings-secure"]')
            ?.closest("p")
            ?.remove();
    
        document.querySelector('input[name="WiFiSettings-password"]')
            ?.closest("p")
            ?.remove();
            */

        document.querySelector('input[name="password"]')
            ?.setAttribute("type", "password");

        document.querySelector('select[name="language"]')
            ?.closest("label")
            ?.remove();
        document.querySelectorAll("hr").forEach(hr => hr.remove());
    });
)rawliteral";

#endif