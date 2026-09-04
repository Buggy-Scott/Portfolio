/*
 * 12-Hour Format WiFi Clock with TFT Display
 * Texas Central Time (CDT/CST) with DST support
 * NTP-synced, exact 1-second timing, no drift
 */

#include <DIYables_TFT_Shield.h>
#include <TimeLib.h>
#include <WiFiS3.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h>

// ==================== CONFIGURATION ====================
const char* ssid     = "Riverbend WiFi";
const char* password = "rwifi2022";

// Texas Central Time with DST rules
TimeChangeRule cdt = {"CDT", Second, Sun, Mar, 2, -300};  // UTC-5 (DST)
TimeChangeRule cst = {"CST", First,  Sun, Nov, 2, -360};  // UTC-6 (Standard)
Timezone texasTZ(cdt, cst);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

bool timeSynced  = false;
bool isDST       = false;
// ======================================================

DIYables_TFT_ILI9486_Shield TFT_display;

// Colors
#define BLACK     DIYables_TFT::colorRGB(0,   0,   0)
#define GREEN     DIYables_TFT::colorRGB(0,   255, 0)
#define CYAN      DIYables_TFT::colorRGB(0,   255, 255)
#define MAGENTA   DIYables_TFT::colorRGB(255, 0,   255)
#define YELLOW    DIYables_TFT::colorRGB(255, 255, 0)
#define WHITE     DIYables_TFT::colorRGB(255, 255, 255)
#define RED       DIYables_TFT::colorRGB(255, 0,   0)
#define ORANGE    DIYables_TFT::colorRGB(255, 165, 0)
#define GREY      DIYables_TFT::colorRGB(128, 128, 128)
#define DARK_GRAY DIYables_TFT::colorRGB(64,  64,  64)

// ── Timing ──────────────────────────────────────────────
unsigned long lastSecondUpdate = 0;
bool          showColon        = true;
unsigned long colonTimer       = 0;
unsigned long lastNTPSync      = 0;
unsigned long lastNTPAttempt   = 0;
const unsigned long NTP_INTERVAL  = 3600000UL;
const unsigned long NTP_RETRY_MS  = 300000UL;
const int           NTP_RETRIES   = 3;

// ── Display change tracking ─────────────────────────────
int lastHour = -1, lastMin = -1, lastSec = -1;
int lastAmPm = -1, lastDay = -1, lastMon = -1, lastYr = -1;
bool lastWifiOk  = false;
bool lastNtpSync = false;
bool lastDST     = false;

// ── Layout constants ────────────────────────────────────
#define T_Y      70
#define HH_X     50
#define COL1_X   150
#define MM_X     180
#define COL2_X   280
#define SS_X     300
#define AMPM_X   400

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("\n=== 12-Hour WiFi Clock ===\n"));

  TFT_display.begin();
  TFT_display.setRotation(1);
  drawStaticBackground();

  connectToWiFi();
  delay(2000);

  timeClient.begin();
  timeClient.setUpdateInterval(60000);
  delay(500);

  if (WiFi.status() == WL_CONNECTED) {
    syncTimeFromNTP();
  } else {
    setTime(12, 0, 0, 1, 1, 2025);
    Serial.println(F("Using fallback time (no WiFi)"));
  }

  lastHour = lastMin = lastSec = lastAmPm = -1;
  lastDay  = lastMon = lastYr  = -1;

  lastSecondUpdate = millis();
  colonTimer       = millis();

  updateDisplay();
  Serial.println(F("Ready!  S=sync  H=help"));
}

// ═══════════════════════════════════════════════════════
void loop() {
  unsigned long now = millis();

  // Exact 1-second tick - reads true elapsed time, never skips
  if (now - lastSecondUpdate >= 1000) {
    lastSecondUpdate = millis();  // re-anchor timing after every sync
    updateDisplay();
  }

  // Colon blink every 750ms
  if (now - colonTimer >= 750) {
    colonTimer += 750;
    showColon = !showColon;
    drawColon();
  }

  // Periodic NTP re-sync every hour
  if (timeSynced && now - lastNTPSync >= NTP_INTERVAL) {
    if (WiFi.status() == WL_CONNECTED) syncTimeFromNTP();
    lastNTPSync = now;
  }

  // Retry NTP every 5 min if unsynced
  if (!timeSynced && WiFi.status() == WL_CONNECTED) {
    if (now - lastNTPAttempt >= NTP_RETRY_MS) {
      lastNTPAttempt = now;
      syncTimeFromNTP();
    }
  }

  if (Serial.available()) processSerialCommand();
}
// ═══════════════════════════════════════════════════════

void drawStaticBackground() {
  TFT_display.fillScreen(BLACK);

  // Outer border
  TFT_display.drawRect(5,  5,  470, 310, YELLOW);
  TFT_display.drawRect(6,  6,  468, 308, YELLOW);

  // Header bar
  TFT_display.fillRect(0, 0, 480, 45, BLACK);
  TFT_display.setCursor(120, 12);
  TFT_display.setTextColor(YELLOW);
  TFT_display.setTextSize(3);
  TFT_display.print(F("  Digital Clock"));

  // Time area box
  TFT_display.fillRect(20,  55, 440, 100, BLACK);
  TFT_display.drawRect(20,  55, 440, 100, YELLOW);

  // Date area box
  TFT_display.fillRect(20, 165, 440,  75, BLACK);
  TFT_display.drawRect(20, 165, 440,  75, YELLOW);

  // Status bar
  TFT_display.fillRect(0, 290, 480, 30, BLACK);
}

inline void clearAt(int x, int y, int w, int h) {
  TFT_display.fillRect(x, y, w, h, BLACK);
}

void updateDisplay() {
  int rawH = hour(),  minV = minute(),  secV = second();
  int dayV = day(),   monV = month(),   yrV  = year();

  bool isPM = (rawH >= 12);
  int  dH   = rawH % 12;
  if (dH == 0) dH = 12;

  // Hour
  if (dH != lastHour) {
    lastHour = dH;
    clearAt(HH_X, T_Y, 95, 70);
    TFT_display.setCursor(HH_X, T_Y);
    TFT_display.setTextColor(GREEN);
    TFT_display.setTextSize(6);
    if (dH < 10) TFT_display.print('0');
    TFT_display.print(dH);
  }

  // Minute
  if (minV != lastMin) {
    lastMin = minV;
    clearAt(MM_X, T_Y, 95, 70);
    TFT_display.setCursor(MM_X, T_Y);
    TFT_display.setTextColor(GREEN);
    TFT_display.setTextSize(6);
    if (minV < 10) TFT_display.print('0');
    TFT_display.print(minV);
  }

  // Static MM:SS colon
  if (secV != lastSec || lastSec == -1) {
    TFT_display.fillRect(COL2_X, T_Y, 20, 70, BLACK);
    TFT_display.setCursor(COL2_X, T_Y);
    TFT_display.setTextColor(GREEN);
    TFT_display.setTextSize(6);
    TFT_display.print(':');
  }

  // Second
  if (secV != lastSec) {
    lastSec = secV;
    clearAt(SS_X, T_Y + 10, 70, 55);
    TFT_display.setCursor(SS_X, T_Y + 10);
    TFT_display.setTextColor(CYAN);
    TFT_display.setTextSize(4);
    if (secV < 10) TFT_display.print('0');
    TFT_display.print(secV);
  }

  // AM/PM
  int ampmV = isPM ? 1 : 0;
  if (ampmV != lastAmPm) {
    lastAmPm = ampmV;
    clearAt(AMPM_X, T_Y + 10, 60, 55);
    TFT_display.setCursor(AMPM_X, T_Y + 10);
    TFT_display.setTextColor(isPM ? ORANGE : CYAN);
    TFT_display.setTextSize(3);
    TFT_display.print(isPM ? F("PM") : F("AM"));
  }

  drawColon();

  // Date
  if (dayV != lastDay || monV != lastMon || yrV != lastYr) {
    lastDay = dayV;  lastMon = monV;  lastYr = yrV;

    clearAt(25, 170, 430, 40);
    TFT_display.setCursor(30, 172);
    TFT_display.setTextColor(YELLOW);
    TFT_display.setTextSize(3);

    if (dayV < 10) TFT_display.print('0');
    TFT_display.print(dayV);
    TFT_display.print(' ');

    const char* mons[] = {"JAN","FEB","MAR","APR","MAY","JUN",
                          "JUL","AUG","SEP","OCT","NOV","DEC"};
    TFT_display.print(mons[monV - 1]);
    TFT_display.print(' ');
    TFT_display.print(yrV);

    clearAt(25, 212, 430, 20);
    TFT_display.setCursor(30, 214);
    TFT_display.setTextColor(CYAN);
    TFT_display.setTextSize(2);
    TFT_display.print(dayStr(weekday()));
  }

  // Status bar (only redraws when something changed)
  bool wifiOk = (WiFi.status() == WL_CONNECTED);
  if (wifiOk != lastWifiOk || timeSynced != lastNtpSync || isDST != lastDST) {
    lastWifiOk  = wifiOk;
    lastNtpSync = timeSynced;
    lastDST     = isDST;
    drawStatusBar(wifiOk);
  }
}

void drawColon() {
  clearAt(COL1_X, T_Y, 26, 70);
  if (showColon) {
    TFT_display.setCursor(COL1_X, T_Y);
    TFT_display.setTextColor(GREEN);
    TFT_display.setTextSize(6);
    TFT_display.print(':');
  }
}

void drawStatusBar(bool wifiOk) {
  TFT_display.fillRect(0, 290, 480, 30, BLACK);

  TFT_display.setCursor(10, 297);
  TFT_display.setTextSize(2);
  if (wifiOk) {
    TFT_display.setTextColor(GREEN);
    TFT_display.print(F("WiFi OK "));
    TFT_display.setTextSize(1);
    TFT_display.print(WiFi.RSSI());
    TFT_display.print(F("dBm"));
  } else {
    TFT_display.setTextColor(RED);
    TFT_display.print(F("No WiFi"));
  }

  TFT_display.setCursor(200, 297);
  TFT_display.setTextSize(2);
  TFT_display.setTextColor(timeSynced ? YELLOW : ORANGE);
  TFT_display.print(timeSynced ? F("NTP OK") : F("NTP --"));

  TFT_display.setCursor(390, 297);
  TFT_display.setTextSize(2);
  TFT_display.setTextColor(isDST ? YELLOW : CYAN);
  TFT_display.print(isDST ? F("CDT") : F("CST"));
}

void connectToWiFi() {
  Serial.print(F("Connecting to WiFi"));
  WiFi.begin(ssid, password);
  for (int i = 0; i < 30 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print('.');
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("\nConnected!"));
    Serial.print(F("  IP: "));    Serial.println(WiFi.localIP());
    Serial.print(F("  RSSI: ")); Serial.print(WiFi.RSSI()); Serial.println(F(" dBm"));
  } else {
    Serial.println(F("\nFailed to connect."));
  }
}

void syncTimeFromNTP() {
  if (WiFi.status() != WL_CONNECTED) { timeSynced = false; return; }

  Serial.print(F("NTP sync"));
  bool ok = false;
  for (int i = 0; i < NTP_RETRIES && !ok; i++) {
    if (i) delay(1500);
    ok = timeClient.update();
    Serial.print('.');
  }

  if (ok) {
    unsigned long utc = timeClient.getEpochTime();
    int testYr = year((time_t)utc);
    if (testYr < 2024 || testYr > 2035) {
      Serial.println(F(" Bad time!"));
      timeSynced = false;
      return;
    }

    TimeChangeRule *tcr;
    time_t local = texasTZ.toLocal(utc, &tcr);
    setTime(local);

    isDST      = (tcr == &cdt);
    timeSynced = true;
    lastNTPSync      = millis();
    lastSecondUpdate = millis();  // re-anchor timing after every sync

    Serial.print(F(" OK -> "));
    Serial.print(hour()); Serial.print(':');
    if (minute() < 10) Serial.print('0');
    Serial.print(minute()); Serial.print(':');
    if (second() < 10) Serial.print('0');
    Serial.println(second());

    lastHour = lastMin = lastSec = lastAmPm = -1;
    lastDay  = lastMon = lastYr  = -1;
    updateDisplay();

  } else {
    timeSynced = false;
    Serial.println(F(" FAILED"));
  }
}

void processSerialCommand() {
  char c = Serial.read();
  switch (toupper(c)) {

    case 'S':
      Serial.println(F("Manual NTP sync..."));
      syncTimeFromNTP();
      break;

    case 'H':
      Serial.println(F("\n── Commands ──"));
      Serial.println(F("S  Sync NTP now"));
      Serial.println(F("W  WiFi status"));
      Serial.println(F("T  Print time"));
      Serial.println(F("H  This help"));
      break;

    case 'W':
      Serial.print(F("WiFi: "));
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print(F("Connected  IP="));
        Serial.print(WiFi.localIP());
        Serial.print(F("  RSSI="));
        Serial.print(WiFi.RSSI());
        Serial.println(F(" dBm"));
      } else {
        Serial.println(F("Disconnected"));
      }
      break;

    case 'T':
      Serial.print(F("Time: "));
      if (hour() < 10) Serial.print('0');
      Serial.print(hour()); Serial.print(':');
      if (minute() < 10) Serial.print('0');
      Serial.print(minute()); Serial.print(':');
      if (second() < 10) Serial.print('0');
      Serial.print(second());
      Serial.print(F("  Date: "));
      if (month() < 10) Serial.print('0');
      Serial.print(month()); Serial.print('/');
      if (day() < 10) Serial.print('0');
      Serial.print(day()); Serial.print('/');
      Serial.print(year());
      Serial.print(F("  "));
      Serial.println(isDST ? F("CDT") : F("CST"));
      break;
  }
}
