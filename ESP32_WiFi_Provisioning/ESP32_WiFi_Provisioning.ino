/*
 * ============================================================
 *  ESP32 — Professional Wi-Fi Provisioning (Captive Portal)
 *  Method: AP Mode + Web Server + DNS Redirect
 *  Storage: NVS (Non-Volatile Storage)
 * ============================================================
 *
 *  LIBRARIES NEEDED (Install via Arduino Library Manager):
 *    - DNSServer   (built-in with ESP32 core)
 *    - WebServer   (built-in with ESP32 core)
 *    - Preferences (built-in with ESP32 core)
 *    - WiFi        (built-in with ESP32 core)
 *
 *  HOW IT WORKS:
 *    1. Boot → Check NVS for saved Wi-Fi credentials
 *    2. If found → Connect to saved Wi-Fi → Normal operation
 *    3. If not found → Start AP "MyDevice-Setup"
 *    4. Client connects to AP → Browser opens captive portal
 *    5. Client scans & selects Wi-Fi → Enters password → Submit
 *    6. ESP32 saves credentials → Restarts → Connects to Wi-Fi
 *    7. Hold BOOT button (GPIO0) for 3 sec → Clears credentials
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

// ── Configuration ──────────────────────────────────────────
#define AP_SSID          "MyDevice-Setup"   // AP name client will see
#define AP_PASSWORD      "12345678"         // AP password (min 8 chars, or "" for open)
#define CONNECT_TIMEOUT  15                 // Seconds to wait for Wi-Fi connect
#define RESET_PIN        0                  // GPIO0 = BOOT button on most boards
#define RESET_HOLD_MS    3000               // Hold duration to trigger reset (ms)
#define STATUS_LED       2                  // Built-in LED (GPIO2), -1 to disable
// ───────────────────────────────────────────────────────────

Preferences preferences;
WebServer    server(80);
DNSServer    dnsServer;

const byte DNS_PORT = 53;

// ════════════════════════════════════════════════════════════
//  HTML PAGE  (Lightweight — no external CDN, pure CSS + JS)
// ════════════════════════════════════════════════════════════
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
<title>Wi-Fi Setup</title>
<style>
:root{--bg:#0f0f0f;--card:#181818;--inp:#212121;--bd:#2c2c2c;--ac:#3b82f6;--tx:#f0f0f0;--mt:#666;--ok:#22c55e;--er:#ef4444;--r:10px}
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;background:var(--bg);color:var(--tx);min-height:100vh;display:flex;flex-direction:column;align-items:center;justify-content:center;padding:16px}
.card{background:var(--card);border:1px solid var(--bd);border-radius:16px;padding:30px 26px;width:100%;max-width:360px}
.logo{width:42px;height:42px;border:1px solid var(--bd);border-radius:10px;display:flex;align-items:center;justify-content:center;margin:0 auto 18px}
.logo svg{width:20px;height:20px;stroke:var(--ac)}
h1{font-size:1.1rem;font-weight:600;text-align:center;letter-spacing:-0.3px}
.sub{font-size:0.78rem;color:var(--mt);text-align:center;margin-top:4px}
hr{border:none;border-top:1px solid var(--bd);margin:18px 0}
label{display:block;font-size:0.68rem;font-weight:600;color:var(--mt);text-transform:uppercase;letter-spacing:0.8px;margin-bottom:6px}
.field{margin-bottom:13px}
.rel{position:relative}
select,input{width:100%;background:var(--inp);border:1px solid var(--bd);border-radius:var(--r);color:var(--tx);font-family:inherit;font-size:0.875rem;padding:10px 13px;outline:none;transition:border-color 0.15s;appearance:none}
select{padding-right:34px;cursor:pointer}
select:focus,input:focus{border-color:var(--ac)}
input::placeholder{color:var(--mt)}
input[type=password],input[type=text]{padding-right:38px}
.arr{position:absolute;right:11px;top:50%;transform:translateY(-50%);pointer-events:none;color:var(--mt);font-size:0.55rem}
.eye{position:absolute;right:10px;top:50%;transform:translateY(-50%);background:none;border:none;cursor:pointer;color:var(--mt);display:flex;align-items:center;transition:color 0.15s}
.eye:hover{color:var(--tx)}
.eye svg{width:15px;height:15px}
.row{margin-top:7px;display:flex;gap:7px}
.ghost{font-size:0.73rem;color:var(--mt);background:none;border:1px solid var(--bd);border-radius:7px;padding:5px 10px;cursor:pointer;font-family:inherit;transition:all 0.15s;display:flex;align-items:center;gap:5px}
.ghost:hover{border-color:var(--ac);color:var(--ac)}
.ghost svg{width:11px;height:11px}
.ghost.spin svg{animation:sp 1s linear infinite}
.lnk{font-size:0.73rem;color:var(--mt);background:none;border:none;cursor:pointer;font-family:inherit;text-decoration:underline;text-underline-offset:2px;margin-top:7px;display:block;transition:color 0.15s}
.lnk:hover{color:var(--ac)}
.btn{width:100%;padding:11px;background:var(--ac);border:none;border-radius:var(--r);color:#fff;font-family:inherit;font-size:0.88rem;font-weight:600;cursor:pointer;margin-top:5px;transition:opacity 0.15s,transform 0.1s}
.btn:hover{opacity:0.85}
.btn:active{transform:scale(0.98)}
.btn:disabled{opacity:0.4;cursor:not-allowed}
.spr{display:none;width:15px;height:15px;border:2px solid rgba(255,255,255,0.3);border-top-color:#fff;border-radius:50%;animation:sp 0.7s linear infinite;margin:0 auto}
.btn.ld .bt{display:none}
.btn.ld .spr{display:block}
.msg{display:none;align-items:flex-start;gap:8px;padding:10px 12px;border-radius:8px;font-size:0.78rem;margin-top:11px;line-height:1.4}
.msg.show{display:flex}
.msg.ok{background:rgba(34,197,94,0.08);border:1px solid rgba(34,197,94,0.18);color:var(--ok)}
.msg.er{background:rgba(239,68,68,0.08);border:1px solid rgba(239,68,68,0.18);color:var(--er)}
.msg.inf{background:rgba(59,130,246,0.08);border:1px solid rgba(59,130,246,0.18);color:var(--ac)}
.msg svg{flex-shrink:0;width:14px;height:14px;margin-top:1px}
.footer{font-size:0.68rem;color:var(--mt);text-align:center;margin-top:14px}
.hidden{display:none!important}
@keyframes sp{to{transform:rotate(360deg)}}
</style>
</head>
<body>
<div class="card">
  <div class="logo">
    <svg viewBox="0 0 24 24" fill="none" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
      <path d="M5 12.55a11 11 0 0 1 14.08 0"/>
      <path d="M1.42 9a16 16 0 0 1 21.16 0"/>
      <path d="M8.53 16.11a6 6 0 0 1 6.95 0"/>
      <circle cx="12" cy="20" r="1" fill="currentColor"/>
    </svg>
  </div>
  <h1>Wi-Fi Setup</h1>
  <p class="sub">Connect your device to a network</p>
  <hr>
  <div class="field">
    <label>Network</label>
    <div id="dw">
      <div class="rel">
        <select id="sel" onchange="onSel()">
          <option value="">Scanning...</option>
        </select>
        <span class="arr">&#9660;</span>
      </div>
      <div class="row">
        <button class="ghost" id="sb" onclick="doScan()">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round">
            <path d="M1 4v6h6"/><path d="M23 20v-6h-6"/>
            <path d="M20.49 9A9 9 0 0 0 5.64 5.64L1 10m22 4l-4.64 4.36A9 9 0 0 1 3.51 15"/>
          </svg>
          Refresh
        </button>
      </div>
      <button class="lnk" onclick="sw(true)">Enter name manually</button>
    </div>
    <div id="mw" class="hidden">
      <input type="text" id="si" placeholder="Network name (SSID)" autocomplete="off" autocorrect="off" autocapitalize="none" spellcheck="false">
      <button class="lnk" onclick="sw(false)">Back to list</button>
    </div>
  </div>
  <div class="field">
    <label>Password</label>
    <div class="rel">
      <input type="password" id="pi" placeholder="Wi-Fi password" autocomplete="off" autocorrect="off" autocapitalize="none" spellcheck="false">
      <button class="eye" type="button" onclick="toggleEye()">
        <svg id="ei" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round">
          <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/>
          <circle cx="12" cy="12" r="3"/>
        </svg>
      </button>
    </div>
  </div>
  <button class="btn" id="cb" onclick="doConnect()">
    <span class="bt">Connect</span>
    <div class="spr"></div>
  </button>
  <div class="msg" id="mb">
    <svg id="mi" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"></svg>
    <span id="mt"></span>
  </div>
</div>
<div class="footer">Hold BOOT button 3s to clear saved credentials</div>
<script>
var manual=false,showPw=false;
window.onload=doScan;
function sw(m){manual=m;document.getElementById('dw').classList.toggle('hidden',m);document.getElementById('mw').classList.toggle('hidden',!m);if(m)document.getElementById('si').focus();}
function toggleEye(){showPw=!showPw;var i=document.getElementById('pi');i.type=showPw?'text':'password';document.getElementById('ei').innerHTML=showPw?'<path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"/><line x1="1" y1="1" x2="23" y2="23"/>':'<path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/>';}
function onSel(){if(document.getElementById('sel').value==='__m__')sw(true);}
function doScan(){var b=document.getElementById('sb'),s=document.getElementById('sel');b.classList.add('spin');b.disabled=true;s.innerHTML='<option value="">Scanning...</option>';fetch('/scan').then(function(r){return r.json();}).then(function(d){b.classList.remove('spin');b.disabled=false;if(!d.networks||!d.networks.length){s.innerHTML='<option value="">No networks found</option>';return;}s.innerHTML='<option value="">Select a network</option>';d.networks.forEach(function(n){var o=document.createElement('option');o.value=n.ssid;o.textContent=n.ssid+(n.encrypted?' [Secured]':'');s.appendChild(o);});var m=document.createElement('option');m.value='__m__';m.textContent='Enter manually';s.appendChild(m);}).catch(function(){b.classList.remove('spin');b.disabled=false;s.innerHTML='<option value="">Scan failed. Retry?</option>';});}
function ssid(){return manual?document.getElementById('si').value.trim():document.getElementById('sel').value.trim();}
function showMsg(t,ic,txt){document.getElementById('mi').innerHTML=ic;document.getElementById('mt').textContent=txt;document.getElementById('mb').className='msg show '+t;}
function doConnect(){var s=ssid(),p=document.getElementById('pi').value,b=document.getElementById('cb');if(!s){showMsg('er','<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/>','Select or enter a network name.');return;}b.classList.add('ld');b.disabled=true;showMsg('inf','<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="13"/><line x1="12" y1="16" x2="12.01" y2="16"/>','Saving... Device will restart shortly.');var q=new URLSearchParams({ssid:s,password:p});fetch('/save',{method:'POST',body:q,headers:{'Content-Type':'application/x-www-form-urlencoded'}}).then(function(r){return r.json();}).then(function(d){b.classList.remove('ld');if(d.ok){showMsg('ok','<path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/>','Saved! Connecting to "'+s+'". You can close this page.');b.disabled=true;}else{showMsg('er','<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/>',d.message||'Something went wrong.');b.disabled=false;}}).catch(function(){b.classList.remove('ld');showMsg('ok','<path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/>','Credentials sent! Device is restarting.');b.disabled=true;});}
</script>
</body>
</html>
)rawliteral";

// ════════════════════════════════════════════════════════════
//  LED Helper
// ════════════════════════════════════════════════════════════
void setLED(bool on) {
#if STATUS_LED >= 0
  digitalWrite(STATUS_LED, on ? HIGH : LOW);
#endif
}

void blinkLED(int times, int ms = 150) {
#if STATUS_LED >= 0
  for (int i = 0; i < times; i++) {
    digitalWrite(STATUS_LED, HIGH); delay(ms);
    digitalWrite(STATUS_LED, LOW);  delay(ms);
  }
#endif
}

// ════════════════════════════════════════════════════════════
//  NVS — Save & Load Credentials
// ════════════════════════════════════════════════════════════
void saveCredentials(const String& ssid, const String& password) {
  preferences.begin("wifi-cfg", false);
  preferences.putString("ssid", ssid);
  preferences.putString("pass", password);
  preferences.end();
  Serial.println("[NVS] Credentials saved → SSID: " + ssid);
}

bool loadCredentials(String& ssid, String& password) {
  preferences.begin("wifi-cfg", true);
  ssid     = preferences.getString("ssid", "");
  password = preferences.getString("pass", "");
  preferences.end();
  return ssid.length() > 0;
}

void clearCredentials() {
  preferences.begin("wifi-cfg", false);
  preferences.clear();
  preferences.end();
  Serial.println("[NVS] Credentials cleared.");
}

// ════════════════════════════════════════════════════════════
//  Connect to Saved Wi-Fi
// ════════════════════════════════════════════════════════════
bool connectToSavedWiFi() {
  String ssid, pass;
  if (!loadCredentials(ssid, pass)) {
    Serial.println("[WiFi] No saved credentials found.");
    return false;
  }

  Serial.printf("[WiFi] Connecting to: %s\n", ssid.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > CONNECT_TIMEOUT * 1000UL) {
      Serial.println("\n[WiFi] Connection timed out.");
      return false;
    }
    delay(400);
    Serial.print(".");
    setLED(!digitalRead(STATUS_LED));  // Blink while connecting
  }

  setLED(true);
  Serial.printf("\n[WiFi] Connected!  IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

// ════════════════════════════════════════════════════════════
//  Web Server Handlers
// ════════════════════════════════════════════════════════════

// GET /  →  Serve the setup page
void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

// GET /scan  →  Return nearby Wi-Fi networks as JSON
void handleScan() {
  Serial.println("[Scan] Scanning networks…");
  int n = WiFi.scanNetworks(false, false, false, 300);

  String json = "{\"networks\":[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    String ssidEsc = WiFi.SSID(i);
    ssidEsc.replace("\"", "\\\"");
    json += "{\"ssid\":\"" + ssidEsc + "\",";
    json += "\"rssi\":"    + String(WiFi.RSSI(i)) + ",";
    json += "\"encrypted\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false") + "}";
  }
  json += "]}";

  Serial.printf("[Scan] Found %d networks.\n", n);
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "application/json", json);
}

// POST /save  →  Save credentials and restart
void handleSave() {
  if (!server.hasArg("ssid") || !server.hasArg("password")) {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"Missing fields.\"}");
    return;
  }

  String ssid     = server.arg("ssid");
  String password = server.arg("password");
  ssid.trim();

  if (ssid.isEmpty()) {
    server.send(400, "application/json", "{\"ok\":false,\"message\":\"SSID cannot be empty.\"}");
    return;
  }

  saveCredentials(ssid, password);
  server.send(200, "application/json", "{\"ok\":true}");
  blinkLED(3, 100);

  delay(800);
  ESP.restart();
}

// All other requests  →  Captive portal redirect
void handleNotFound() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", "Redirecting to setup page…");
}

// ════════════════════════════════════════════════════════════
//  Start Provisioning (AP + DNS + Web Server)
// ════════════════════════════════════════════════════════════
void startProvisioningMode() {
  Serial.println("[AP] Starting provisioning mode…");
  blinkLED(2, 200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  delay(200);

  IPAddress apIP = WiFi.softAPIP();
  Serial.printf("[AP] SSID: %s  |  IP: %s\n", AP_SSID, apIP.toString().c_str());

  // DNS: redirect ALL domains → device IP (Captive Portal trick)
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  // Web server routes
  server.on("/",       HTTP_GET,  handleRoot);
  server.on("/scan",   HTTP_GET,  handleScan);
  server.on("/save",   HTTP_POST, handleSave);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("[Web] Server started. Waiting for client…");
  Serial.println("──────────────────────────────────────────");
  Serial.println("  Client: Connect to Wi-Fi → " AP_SSID);
  Serial.println("  Then open browser → Setup page opens");
  Serial.println("──────────────────────────────────────────");

  setLED(true);  // LED stays ON in AP mode
}

// ════════════════════════════════════════════════════════════
//  Reset Button  (Hold BOOT/GPIO0 for 3 seconds)
// ════════════════════════════════════════════════════════════
void checkResetButton() {
  if (digitalRead(RESET_PIN) == LOW) {
    unsigned long held = millis();
    while (digitalRead(RESET_PIN) == LOW) {
      if (millis() - held > RESET_HOLD_MS) {
        Serial.println("[Reset] Button held 3s → Clearing Wi-Fi credentials…");
        blinkLED(5, 80);
        clearCredentials();
        delay(300);
        ESP.restart();
      }
      delay(50);
    }
  }
}

// ════════════════════════════════════════════════════════════
//  SETUP
// ════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n══════════════════════════════════════════");
  Serial.println("   ESP32 Wi-Fi Provisioning  |  Boot");
  Serial.println("══════════════════════════════════════════");

#if STATUS_LED >= 0
  pinMode(STATUS_LED, OUTPUT);
  setLED(false);
#endif
  pinMode(RESET_PIN, INPUT_PULLUP);

  // ── Try connecting to saved Wi-Fi ──
  if (connectToSavedWiFi()) {
    //
    // ┌─────────────────────────────────────────┐
    // │   YOUR NORMAL PRODUCT CODE STARTS HERE  │
    // │   Wi-Fi is connected, IP is available   │
    // └─────────────────────────────────────────┘
    Serial.println("[App] Device is ready. Starting main application…");
    // e.g. mqttConnect(); startSensors(); etc.

  } else {
    // No saved credentials or connection failed → provisioning
    startProvisioningMode();
  }
}

// ════════════════════════════════════════════════════════════
//  LOOP
// ════════════════════════════════════════════════════════════
void loop() {
  // Always check reset button
  checkResetButton();

  // Handle DNS + Web server only in AP/provisioning mode
  if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
    dnsServer.processNextRequest();
    server.handleClient();
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Connection lost. Reconnecting…");
    setLED(false);
    connectToSavedWiFi();
  }
}
