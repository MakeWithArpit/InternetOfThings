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
<title>Device Setup</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=DM+Sans:wght@300;400;500;600&family=DM+Mono:wght@400;500&display=swap');

  :root {
    --bg:        #0d0f12;
    --surface:   #161a20;
    --surface2:  #1e242d;
    --border:    #2a3140;
    --accent:    #4f8ef7;
    --accent2:   #7eb8f7;
    --success:   #3ecf8e;
    --error:     #f75a5a;
    --text:      #e8eaf0;
    --muted:     #6b7585;
    --radius:    14px;
    --shadow:    0 24px 64px rgba(0,0,0,0.5);
  }

  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    font-family: 'DM Sans', sans-serif;
    background: var(--bg);
    color: var(--text);
    min-height: 100vh;
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 20px;
    background-image:
      radial-gradient(ellipse 60% 40% at 50% 0%, rgba(79,142,247,0.08) 0%, transparent 70%);
  }

  .card {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 22px;
    padding: 36px 32px 32px;
    width: 100%;
    max-width: 400px;
    box-shadow: var(--shadow);
    animation: slideUp 0.4s cubic-bezier(0.22,1,0.36,1);
  }

  @keyframes slideUp {
    from { opacity: 0; transform: translateY(24px); }
    to   { opacity: 1; transform: translateY(0); }
  }

  /* ── Header ── */
  .header { text-align: center; margin-bottom: 28px; }

  .icon-wrap {
    width: 56px; height: 56px;
    background: linear-gradient(135deg, rgba(79,142,247,0.15), rgba(79,142,247,0.05));
    border: 1px solid rgba(79,142,247,0.25);
    border-radius: 16px;
    display: flex; align-items: center; justify-content: center;
    margin: 0 auto 16px;
  }

  .icon-wrap svg { width: 26px; height: 26px; stroke: var(--accent); }

  h1 { font-size: 1.25rem; font-weight: 600; letter-spacing: -0.3px; }
  .subtitle { font-size: 0.82rem; color: var(--muted); margin-top: 5px; line-height: 1.5; }

  /* ── Divider ── */
  .divider { height: 1px; background: var(--border); margin: 20px 0; }

  /* ── Label ── */
  label {
    display: block;
    font-size: 0.75rem;
    font-weight: 500;
    color: var(--muted);
    letter-spacing: 0.6px;
    text-transform: uppercase;
    margin-bottom: 7px;
  }

  /* ── Network Selector ── */
  .field { margin-bottom: 16px; }

  .select-wrap { position: relative; }

  select {
    width: 100%;
    background: var(--surface2);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    color: var(--text);
    font-family: 'DM Sans', sans-serif;
    font-size: 0.9rem;
    padding: 12px 38px 12px 14px;
    appearance: none;
    cursor: pointer;
    transition: border-color 0.2s;
    outline: none;
  }

  select:focus { border-color: var(--accent); box-shadow: 0 0 0 3px rgba(79,142,247,0.12); }

  .select-arrow {
    position: absolute; right: 13px; top: 50%; transform: translateY(-50%);
    pointer-events: none; color: var(--muted);
  }

  /* ── Text Input ── */
  .input-wrap { position: relative; }

  input[type="text"], input[type="password"] {
    width: 100%;
    background: var(--surface2);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    color: var(--text);
    font-family: 'DM Mono', monospace;
    font-size: 0.88rem;
    padding: 12px 40px 12px 14px;
    transition: border-color 0.2s;
    outline: none;
    letter-spacing: 0.5px;
  }

  input::placeholder { color: var(--muted); font-family: 'DM Sans', sans-serif; letter-spacing: 0; }
  input:focus { border-color: var(--accent); box-shadow: 0 0 0 3px rgba(79,142,247,0.12); }

  .toggle-pass {
    position: absolute; right: 12px; top: 50%; transform: translateY(-50%);
    background: none; border: none; cursor: pointer; color: var(--muted);
    padding: 4px; display: flex; align-items: center;
    transition: color 0.2s;
  }
  .toggle-pass:hover { color: var(--text); }
  .toggle-pass svg { width: 17px; height: 17px; }

  /* ── Scan Button ── */
  .scan-btn {
    display: flex; align-items: center; gap: 6px;
    background: none;
    border: 1px solid var(--border);
    border-radius: 8px;
    color: var(--muted);
    font-family: 'DM Sans', sans-serif;
    font-size: 0.78rem;
    padding: 5px 10px;
    cursor: pointer;
    transition: all 0.2s;
    margin-top: 8px;
  }
  .scan-btn:hover { border-color: var(--accent); color: var(--accent2); }
  .scan-btn svg { width: 13px; height: 13px; }
  .scan-btn.spinning svg { animation: spin 1s linear infinite; }

  @keyframes spin { to { transform: rotate(360deg); } }

  /* ── Manual SSID Toggle ── */
  .manual-toggle {
    font-size: 0.75rem; color: var(--muted); cursor: pointer;
    text-decoration: underline; text-underline-offset: 3px;
    background: none; border: none; font-family: 'DM Sans', sans-serif;
    margin-top: 8px; display: block;
  }
  .manual-toggle:hover { color: var(--accent2); }

  .hidden { display: none !important; }

  /* ── Submit Button ── */
  .btn {
    width: 100%;
    padding: 13px;
    background: var(--accent);
    border: none;
    border-radius: var(--radius);
    color: #fff;
    font-family: 'DM Sans', sans-serif;
    font-size: 0.95rem;
    font-weight: 600;
    cursor: pointer;
    margin-top: 8px;
    transition: opacity 0.2s, transform 0.15s;
    position: relative;
    overflow: hidden;
    letter-spacing: -0.1px;
  }
  .btn:hover { opacity: 0.88; }
  .btn:active { transform: scale(0.98); }
  .btn:disabled { opacity: 0.45; cursor: not-allowed; }

  .btn .spinner {
    display: none;
    width: 18px; height: 18px;
    border: 2.5px solid rgba(255,255,255,0.3);
    border-top-color: #fff;
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
    margin: 0 auto;
  }
  .btn.loading .btn-text { display: none; }
  .btn.loading .spinner { display: block; }

  /* ── Status Banner ── */
  .status {
    display: none;
    align-items: center;
    gap: 10px;
    padding: 12px 14px;
    border-radius: 10px;
    font-size: 0.82rem;
    margin-top: 14px;
    font-weight: 500;
    line-height: 1.4;
  }
  .status.show { display: flex; }
  .status.success { background: rgba(62,207,142,0.1); border: 1px solid rgba(62,207,142,0.25); color: var(--success); }
  .status.error   { background: rgba(247,90,90,0.1);  border: 1px solid rgba(247,90,90,0.25);  color: var(--error); }
  .status.info    { background: rgba(79,142,247,0.1); border: 1px solid rgba(79,142,247,0.25); color: var(--accent2); }
  .status svg { flex-shrink: 0; width: 16px; height: 16px; }

  /* ── Signal Strength Icon in dropdown ── */
  .signal-0::before { content: "▁▂▃▄ "; color: var(--muted); font-size: 0.7em; }
  .signal-1::before { content: "▁▂▃▄ "; color: var(--error); font-size: 0.7em; }
  .signal-2::before { content: "▁▂▃▄ "; color: #f7a44f; font-size: 0.7em; }
  .signal-3::before { content: "▁▂▃▄ "; color: var(--success); font-size: 0.7em; }

  /* ── Footer ── */
  .footer {
    text-align: center;
    font-size: 0.72rem;
    color: var(--muted);
    margin-top: 22px;
    letter-spacing: 0.2px;
  }
</style>
</head>
<body>
<div class="card">

  <!-- Header -->
  <div class="header">
    <div class="icon-wrap">
      <svg viewBox="0 0 24 24" fill="none" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">
        <path d="M5 12.55a11 11 0 0 1 14.08 0"/>
        <path d="M1.42 9a16 16 0 0 1 21.16 0"/>
        <path d="M8.53 16.11a6 6 0 0 1 6.95 0"/>
        <circle cx="12" cy="20" r="1" fill="currentColor"/>
      </svg>
    </div>
    <h1>Wi-Fi Setup</h1>
    <p class="subtitle">Connect your device to your home<br>or office network</p>
  </div>

  <div class="divider"></div>

  <!-- Network Field -->
  <div class="field">
    <label>Network</label>

    <!-- Dropdown mode -->
    <div id="dropdownWrap">
      <div class="select-wrap">
        <select id="ssidSelect" onchange="onSelectChange()">
          <option value="">Scanning networks...</option>
        </select>
        <span class="select-arrow">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" width="14" height="14">
            <path d="M6 9l6 6 6-6"/>
          </svg>
        </span>
      </div>
      <button class="scan-btn" id="scanBtn" onclick="scanNetworks()">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round">
          <path d="M1 4v6h6"/><path d="M23 20v-6h-6"/>
          <path d="M20.49 9A9 9 0 0 0 5.64 5.64L1 10m22 4l-4.64 4.36A9 9 0 0 1 3.51 15"/>
        </svg>
        Refresh list
      </button>
      <button class="manual-toggle" onclick="toggleManual()">Enter network name manually →</button>
    </div>

    <!-- Manual input mode -->
    <div id="manualWrap" class="hidden">
      <input type="text" id="ssidManual" placeholder="Network name (SSID)" autocomplete="off" autocorrect="off" autocapitalize="none" spellcheck="false">
      <button class="manual-toggle" onclick="toggleManual()">← Back to network list</button>
    </div>
  </div>

  <!-- Password Field -->
  <div class="field">
    <label>Password</label>
    <div class="input-wrap">
      <input type="password" id="password" placeholder="Wi-Fi password" autocomplete="off" autocorrect="off" autocapitalize="none" spellcheck="false">
      <button class="toggle-pass" type="button" onclick="togglePass()" title="Show/hide password">
        <svg id="eyeIcon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round">
          <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/>
          <circle cx="12" cy="12" r="3"/>
        </svg>
      </button>
    </div>
  </div>

  <!-- Submit -->
  <button class="btn" id="connectBtn" onclick="connectWiFi()">
    <span class="btn-text">Connect to Network</span>
    <div class="spinner"></div>
  </button>

  <!-- Status Message -->
  <div class="status" id="statusBox">
    <svg id="statusIcon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"></svg>
    <span id="statusText"></span>
  </div>

</div>

<!-- Footer -->
<div class="footer">Hold BOOT button 3 sec to reset Wi-Fi settings</div>

<script>
  // ── State ──
  let isManual = false;
  let passVisible = false;

  // ── On Load: Scan networks ──
  window.addEventListener('load', () => { scanNetworks(); });

  // ── Toggle manual SSID input ──
  function toggleManual() {
    isManual = !isManual;
    document.getElementById('dropdownWrap').classList.toggle('hidden', isManual);
    document.getElementById('manualWrap').classList.toggle('hidden', !isManual);
    if (isManual) document.getElementById('ssidManual').focus();
  }

  // ── Toggle password visibility ──
  function togglePass() {
    passVisible = !passVisible;
    const inp = document.getElementById('password');
    inp.type = passVisible ? 'text' : 'password';
    document.getElementById('eyeIcon').innerHTML = passVisible
      ? '<path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"/><line x1="1" y1="1" x2="23" y2="23"/>'
      : '<path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/>';
  }

  // ── Dropdown change ──
  function onSelectChange() {
    const val = document.getElementById('ssidSelect').value;
    if (val === '__manual__') { toggleManual(); }
  }

  // ── Scan Wi-Fi Networks ──
  function scanNetworks() {
    const btn = document.getElementById('scanBtn');
    const sel = document.getElementById('ssidSelect');

    btn.classList.add('spinning');
    btn.disabled = true;
    sel.innerHTML = '<option value="">Scanning…</option>';

    fetch('/scan')
      .then(r => r.json())
      .then(data => {
        btn.classList.remove('spinning');
        btn.disabled = false;

        if (!data.networks || data.networks.length === 0) {
          sel.innerHTML = '<option value="">No networks found</option>';
          return;
        }

        sel.innerHTML = '<option value="">— Select a network —</option>';
        data.networks.forEach(net => {
          const opt = document.createElement('option');
          opt.value = net.ssid;
          opt.textContent = net.ssid + (net.encrypted ? ' 🔒' : '');
          sel.appendChild(opt);
        });
        // Manual entry option
        const m = document.createElement('option');
        m.value = '__manual__';
        m.textContent = '✏️  Enter manually…';
        sel.appendChild(m);
      })
      .catch(() => {
        btn.classList.remove('spinning');
        btn.disabled = false;
        sel.innerHTML = '<option value="">Scan failed. Retry?</option>';
      });
  }

  // ── Get SSID from active input ──
  function getSSID() {
    return isManual
      ? document.getElementById('ssidManual').value.trim()
      : document.getElementById('ssidSelect').value.trim();
  }

  // ── Show Status Banner ──
  function showStatus(type, iconPath, message) {
    const box  = document.getElementById('statusBox');
    const icon = document.getElementById('statusIcon');
    const text = document.getElementById('statusText');
    box.className = 'status show ' + type;
    icon.innerHTML = iconPath;
    text.textContent = message;
  }

  // ── Connect ──
  function connectWiFi() {
    const ssid = getSSID();
    const pass = document.getElementById('password').value;
    const btn  = document.getElementById('connectBtn');

    if (!ssid) {
      showStatus('error',
        '<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/>',
        'Please select or enter a network name.');
      return;
    }

    btn.classList.add('loading');
    btn.disabled = true;
    showStatus('info',
      '<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="13"/><line x1="12" y1="16" x2="12.01" y2="16"/>',
      'Saving credentials… Device will restart shortly.');

    const params = new URLSearchParams({ ssid, password: pass });

    fetch('/save', { method: 'POST', body: params, headers: { 'Content-Type': 'application/x-www-form-urlencoded' }})
      .then(r => r.json())
      .then(data => {
        btn.classList.remove('loading');
        if (data.ok) {
          showStatus('success',
            '<path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/>',
            'Saved! Device is restarting and connecting to "' + ssid + '". You can close this page.');
          btn.disabled = true;
        } else {
          showStatus('error',
            '<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/>',
            data.message || 'Something went wrong. Please try again.');
          btn.disabled = false;
        }
      })
      .catch(() => {
        btn.classList.remove('loading');
        // Device likely restarted already — treat as success
        showStatus('success',
          '<path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/><polyline points="22 4 12 14.01 9 11.01"/>',
          'Credentials sent! Device is restarting. Please wait…');
        btn.disabled = true;
      });
  }
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

  // ──────────────────────────────────────────
  //   YOUR MAIN PRODUCT CODE GOES HERE
  // ──────────────────────────────────────────
  // Wi-Fi is connected and provisioning is done.
  // Example:
  //   readSensor();
  //   publishMQTT();
  //   delay(1000);

  // Wi-Fi reconnect watchdog
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Connection lost. Reconnecting…");
    setLED(false);
    connectToSavedWiFi();
  }
}
