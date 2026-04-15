// ============================================
// RELOJ PERONISTA - TFT ILI9341 240x320
// CON ALARMA CONFIGURABLE
// ============================================

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h>              // Librería TFT_eSPI (reemplaza Adafruit_GFX + Adafruit_ILI9341)
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include "config.h"

// Inicializar display TFT con TFT_eSPI
TFT_eSPI tft = TFT_eSPI();        // Configuración viene de platformio.ini build_flags

Adafruit_AHTX0 aht;
Adafruit_BMP085 bmp;

// ========== VARIABLES GLOBALES ==========
unsigned long lastEfemerideTime = 0;
bool showingEfemeride = false;
int currentEfemerideIndex = 0;

unsigned long lastDateToggleTime = 0;
bool showingDate = false;

int16_t marqueeX = 0;
int16_t marqueeY = 240; // Empieza abajo de la pantalla
unsigned long lastMarqueeUpdate = 0;
const unsigned long MARQUEE_SPEED = 50;

unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 86400000;

// ========== VARIABLES ALARMA ==========
#ifdef ALARM_ENABLED
bool alarmEnabled = ALARM_ENABLED;
int alarmHour = ALARM_HOUR;
int alarmMinute = ALARM_MINUTE;
#else
bool alarmEnabled = true;
int alarmHour = 7;
int alarmMinute = 30;
#endif
bool alarmTriggered = false;
unsigned long alarmStartTime = 0;
unsigned long lastAlarmBlink = 0;
bool alarmBlinkState = false;

// Botones
const int BUTTON_PIN = 27;           // Botón cambio de modo (GPIO 27 disponible - no interfiere con HSPI)
const int ALARM_BUTTON_PIN = 4;      // Botón configuración alarma

// Configuración de alarma
bool alarmConfigMode = false;
int alarmConfigField = 0;             // 0=hora, 1=minuto, 2=on/off, 3=guardar
int tempAlarmHour = 7;
int tempAlarmMinute = 30;
bool tempAlarmEnabled = true;
unsigned long alarmButtonPressStart = 0;
bool alarmButtonLongPressDetected = false;

enum DisplayMode {
  MODE_AUTO,
  MODE_CLOCK_ONLY,
  MODE_DATE_ONLY,
  MODE_EPHEMERIS_ONLY,
  MODE_SENSORS,
  MODE_FORECAST
};
DisplayMode currentMode = MODE_AUTO;
unsigned long lastButtonPress = 0;

float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_READ_INTERVAL = 10000;
bool sensorsAvailable = false;

unsigned long lastSensorToggle = 0;
int sensorDisplayIndex = 0;  // 0=temp, 1=humidity, 2=pressure

struct DayForecast {
  float tempMin;
  float tempMax;
  String weatherMain;
  String description;
  int dayOfMonth;
};

DayForecast forecasts[3];
int numForecasts = 0;
unsigned long lastWeatherUpdate = 0;
bool weatherDataAvailable = false;

// Efemérides peronistas
const char* efemerides[] = {
"08/10/1895 Nace Juan Domingo Perón",
"07/05/1919 Nace Eva Duarte de Perón",
"04/06/1943 Golpe militar del GOU",
"27/10/1943 Perón asume Trabajo",
"02/12/1943 Secretaría de Trabajo",
"24/02/1944 Perón vicepresidente",
"17/10/1945 Día de la Lealtad",
"22/10/1945 Liberación de Perón",
"24/02/1946 Elección de Perón",
"04/06/1946 Asume Perón presidente",
"09/07/1947 Independencia Económica",
"23/09/1947 Voto femenino",
"19/06/1948 Fundación Eva Perón",
"11/03/1949 Reforma Constitucional",
"22/08/1951 Cabildo Abierto",
"31/08/1951 Renunciamiento Evita",
"26/07/1952 Fallece Eva Perón",
"16/06/1955 Bombardeo Plaza Mayo",
"16/09/1955 Derrocamiento Perón",
"17/11/1972 Regreso de Perón",
"18/11/1972 Día Militancia",
"25/05/1973 Asume Cámpora",
"12/10/1973 Asume Perón 3er mandato",
"01/07/1974 Fallece Perón",
"25/05/2003 Asume Néstor Kirchner",
"10/12/2007 Asume Cristina",
"27/10/2010 Fallece Néstor",
"10/12/2019 Asume Alberto Fernández"
};
const int numEfemerides = sizeof(efemerides) / sizeof(efemerides[0]);

// ========== FUNCIONES ==========
void drawPeronSilhouette(); // Prototipo de función

void connectWiFi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi OK!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAIL");
  }
}

void setupTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.println("Sync NTP...");
  
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10) {
    Serial.print(".");
    delay(500);
    attempts++;
  }
  
  if (attempts < 10) {
    Serial.println("\nHora OK!");
    lastTimeSync = millis();
  } else {
    Serial.println("\nHora FAIL");
  }
}

void resyncTime() {
  Serial.println("Resync NTP...");
  
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    struct tm timeinfo;
    int attempts = 0;
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    while (!getLocalTime(&timeinfo) && attempts < 10) {
      Serial.print(".");
      delay(500);
      attempts++;
    }
    
    if (attempts < 10) {
      Serial.println("\nResync OK!");
      lastTimeSync = millis();
    }
  }
}

void displayTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.fillScreen(COLOR_FONDO_AZUL);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(40, 150);
    tft.print("Error hora");
    return;
  }
  
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  // Usar FONT8 (7-segment font grande) que SÍ escala bien
  tft.setTextFont(8);
  tft.setTextSize(1); // Reducido 80% desde size 3
  tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  
  // FONT8 con size 1 es el tamaño base
  int16_t x = 60;
  int16_t y = 100;
  
  tft.setCursor(x, y);
  tft.print(timeStr);
  
  // Volver a fuente por defecto
  tft.setTextFont(1);
  tft.setTextSize(1);
}

void displayDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  // Usar FONT8 (7-segment font grande) que SÍ escala bien
  tft.setTextFont(8);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  
  char dateStr[16]; // Buffer más grande para evitar overflow
  sprintf(dateStr, "%02d %02d", timeinfo.tm_mday, timeinfo.tm_mon + 1);
  
  int16_t x = 80;
  int16_t y = 100;
  
  tft.setCursor(x, y);
  tft.print(dateStr);
  
  // Volver a fuente por defecto
  tft.setTextFont(1);
  tft.setTextSize(1);
}

void displayEfemeride() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastMarqueeUpdate >= MARQUEE_SPEED) {
    lastMarqueeUpdate = currentMillis;
    marqueeY -= 2; // Subir texto (efecto Star Wars)
  }
  
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  // Dibujar imagen de Perón de fondo (silueta simple)
  drawPeronSilhouette();
  
  String efemText = String(efemerides[currentEfemerideIndex]);
  
  // Dividir en líneas para efecto vertical
  int lineHeight = 30;
  int yPos = marqueeY;
  
  // Resetear cuando el texto sale por arriba
  if (marqueeY < -100) {
    marqueeY = 240;
    currentEfemerideIndex = random(0, numEfemerides);
  }
  
  // Dibujar texto con efecto perspectiva (más grande abajo, más chico arriba)
  tft.setTextSize(4);
  tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  
  // Centrar texto
  int16_t x = 10;
  tft.setCursor(x, yPos);
  tft.print(efemText);
  
  // Línea adicional con año (si existe en el texto)
  tft.setTextSize(3);
  tft.setCursor(x + 20, yPos + 35);
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
}

// Dibujar silueta simple de Perón
void drawPeronSilhouette() {
  // Silueta estilizada de cabeza/busto en el centro
  int16_t centerX = 160;
  int16_t centerY = 120;
  
  // Dibuja una silueta muy tenue para no interferir con el texto
  uint16_t silhouetteColor = tft.color565(30, 50, 80); // Azul muy oscuro
  
  // Cabeza (círculo)
  tft.fillCircle(centerX, centerY - 40, 25, silhouetteColor);
  
  // Hombros/pecho (trapecio simplificado)
  tft.fillTriangle(centerX - 35, centerY, centerX + 35, centerY, centerX, centerY - 15, silhouetteColor);
  tft.fillRect(centerX - 35, centerY, 70, 40, silhouetteColor);
  
  // Detalles de gorra militar
  tft.fillRect(centerX - 28, centerY - 65, 56, 8, silhouetteColor);
}

void readSensors() {
  if (!sensorsAvailable) return;
  
  sensors_event_t humid, temp;
  aht.getEvent(&humid, &temp);
  temperature = temp.temperature;
  humidity = humid.relative_humidity;
  pressure = bmp.readPressure() / 100.0;
  
  Serial.printf("T:%.1f H:%.1f P:%.1f\n", temperature, humidity, pressure);
}

void displayTemperature() {
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  tft.setTextSize(4);
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  tft.setCursor(30, 30);
  tft.print("TEMPERATURA");
  
  tft.setTextSize(9);
  tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  char tempStr[10];
  sprintf(tempStr, "%.1fC", temperature);
  tft.setCursor(30, 100);
  tft.print(tempStr);
}

void displayHumidity() {
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  tft.setTextSize(4);
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  tft.setCursor(70, 30);
  tft.print("HUMEDAD");
  
  tft.setTextSize(9);
  tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  char humStr[10];
  sprintf(humStr, "%.1f%%", humidity);
  tft.setCursor(30, 100);
  tft.print(humStr);
}

void displayPressure() {
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  tft.setTextSize(4);
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  tft.setCursor(70, 30);
  tft.print("PRESION");
  
  tft.setTextSize(8);
  tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  char pressStr[15];
  sprintf(pressStr, "%.0fhPa", pressure);
  tft.setCursor(10, 100);
  tft.print(pressStr);
}

String urlEncode(String str) {
  String encoded = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (c == ' ') encoded += '+';
    else if (isalnum(c)) encoded += c;
    else {
      encoded += '%';
      if (c < 16) encoded += '0';
      encoded += String(c, HEX);
    }
  }
  return encoded;
}

void fetchWeatherForecast() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String cityEncoded = urlEncode(String(OPENWEATHER_CITY));
  String url = "http://api.openweathermap.org/data/2.5/forecast?q=";
  url += cityEncoded + "," + OPENWEATHER_COUNTRY;
  url += "&appid=" + String(OPENWEATHER_API_KEY);
  url += "&units=metric&lang=es&cnt=24";
  
  Serial.println("Obteniendo clima...");
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      numForecasts = 0;
      int lastDay = -1;
      JsonArray list = doc["list"].as<JsonArray>();
      
      for (JsonObject item : list) {
        long dt = item["dt"];
        struct tm* timeinfo = localtime((time_t*)&dt);
        int day = timeinfo->tm_mday;
        
        if (day != lastDay && timeinfo->tm_hour >= 12 && timeinfo->tm_hour <= 15) {
          if (numForecasts < 3) {
            forecasts[numForecasts].dayOfMonth = day;
            forecasts[numForecasts].tempMin = item["main"]["temp_min"];
            forecasts[numForecasts].tempMax = item["main"]["temp_max"];
            forecasts[numForecasts].weatherMain = item["weather"][0]["main"].as<String>();
            numForecasts++;
            lastDay = day;
          }
        }
      }
      
      weatherDataAvailable = (numForecasts > 0);
      lastWeatherUpdate = millis();
      Serial.printf("Clima: %d días\n", numForecasts);
    }
  }
  http.end();
}

// ========== FUNCIONES DE ALARMA ==========

void checkAlarm() {
  if (!alarmEnabled) return;
  
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  
  if (timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute && !alarmTriggered) {
    alarmTriggered = true;
    alarmStartTime = millis();
    Serial.println("🔔 ALARMA ACTIVADA!");
  }
  
  if (alarmTriggered) {
    #ifdef ALARM_DURATION
    unsigned long duration = ALARM_DURATION;
    #else
    unsigned long duration = 60000;
    #endif
    
    if (millis() - alarmStartTime >= duration) {
      alarmTriggered = false;
      Serial.println("Alarma finalizada (timeout)");
    }
    
    // Detener con botón de modo
    if (digitalRead(BUTTON_PIN) == LOW) {
      alarmTriggered = false;
      Serial.println("Alarma detenida por usuario");
      delay(500);
    }
  }
  
  // Reset si cambia la hora
  if (timeinfo.tm_hour != alarmHour) {
    alarmTriggered = false;
  }
}

void displayAlarm() {
  unsigned long currentMillis = millis();
  
  #ifdef ALARM_BLINK_INTERVAL
  unsigned long blinkInterval = ALARM_BLINK_INTERVAL;
  #else
  unsigned long blinkInterval = 500;
  #endif
  
  if (currentMillis - lastAlarmBlink >= blinkInterval) {
    lastAlarmBlink = currentMillis;
    alarmBlinkState = !alarmBlinkState;
  }
  
  if (alarmBlinkState) {
    // Pantalla llena invertida
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(7);
    tft.setCursor(10, 80);
    tft.print("ALARMA!");
  } else {
    tft.fillScreen(COLOR_FONDO_AZUL);
  }
}

void displayAlarmConfig() {
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  // Título
  tft.setTextSize(4);
  tft.setTextColor(COLOR_PERONISTA_BLANCO);
  tft.setCursor(10, 20);
  tft.print("CONFIG ALARMA");
  
  // Hora y Minuto
  tft.setTextSize(10);
  tft.setCursor(20, 80);
  
  // Hora
  if (alarmConfigField == 0) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  }
  tft.printf("%02d", tempAlarmHour);
  
  // Dos puntos
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  tft.print(":");
  
  // Minuto
  if (alarmConfigField == 1) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.setTextColor(COLOR_PERONISTA_CELESTE, COLOR_FONDO_AZUL);
  }
  tft.printf("%02d", tempAlarmMinute);
  
  // ON/OFF
  tft.setTextSize(6);
  if (alarmConfigField == 2) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  }
  tft.setCursor(80, 170);
  tft.print(tempAlarmEnabled ? " ON " : " OFF");
  
  // Instrucción
  tft.setTextSize(2);
  tft.setTextColor(COLOR_PERONISTA_BLANCO, COLOR_FONDO_AZUL);
  tft.setCursor(10, 215);
  if (alarmConfigField == 3) {
    tft.print("SAVE? Corto=SI");
  } else {
    tft.print("C=+1 L=Siguiente");
  }
}

void checkAlarmButton() {
  int buttonState = digitalRead(ALARM_BUTTON_PIN);
  unsigned long currentMillis = millis();
  
  // Detectar inicio de presión
  if (buttonState == LOW && alarmButtonPressStart == 0) {
    alarmButtonPressStart = currentMillis;
    alarmButtonLongPressDetected = false;
  }
  
  // Detectar presión larga
  if (buttonState == LOW && !alarmButtonLongPressDetected) {
    if (currentMillis - alarmButtonPressStart >= BUTTON_LONG_PRESS_DURATION) {
      alarmButtonLongPressDetected = true;
      
      if (alarmConfigMode) {
        // En modo config: avanzar campo
        alarmConfigField = (alarmConfigField + 1) % 4;
        Serial.printf("Campo: %d\n", alarmConfigField);
      } else {
        // Entrar a modo config
        alarmConfigMode = true;
        alarmConfigField = 0;
        tempAlarmHour = alarmHour;
        tempAlarmMinute = alarmMinute;
        tempAlarmEnabled = alarmEnabled;
        Serial.println("▶ Modo config alarma");
      }
      delay(300);
    }
  }
  
  // Detectar liberación del botón
  if (buttonState == HIGH && alarmButtonPressStart > 0) {
    unsigned long pressDuration = currentMillis - alarmButtonPressStart;
    
    // Presión corta
    if (pressDuration < BUTTON_LONG_PRESS_DURATION && !alarmButtonLongPressDetected) {
      if (alarmConfigMode) {
        if (alarmConfigField == 0) {
          // Incrementar hora
          tempAlarmHour = (tempAlarmHour + 1) % 24;
          Serial.printf("Hora: %02d\n", tempAlarmHour);
        } else if (alarmConfigField == 1) {
          // Incrementar minuto
          tempAlarmMinute = (tempAlarmMinute + 1) % 60;
          Serial.printf("Minuto: %02d\n", tempAlarmMinute);
        } else if (alarmConfigField == 2) {
          // Toggle ON/OFF
          tempAlarmEnabled = !tempAlarmEnabled;
          Serial.println(tempAlarmEnabled ? "ON" : "OFF");
        } else if (alarmConfigField == 3) {
          // Guardar
          alarmHour = tempAlarmHour;
          alarmMinute = tempAlarmMinute;
          alarmEnabled = tempAlarmEnabled;
          alarmConfigMode = false;
          Serial.printf("✓ Guardado: %02d:%02d %s\n", 
            alarmHour, alarmMinute, alarmEnabled ? "ON" : "OFF");
        }
        delay(200);
      }
    }
    
    alarmButtonPressStart = 0;
  }
}

// Íconos del clima más grandes
void drawSunIcon(int16_t x, int16_t y) {
  // Sol más grande
  tft.fillCircle(x, y, 8, TFT_YELLOW);
  // Rayos
  for (int i = 0; i < 8; i++) {
    float angle = i * 45 * PI / 180;
    int x1 = x + cos(angle) * 12;
    int y1 = y + sin(angle) * 12;
    int x2 = x + cos(angle) * 16;
    int y2 = y + sin(angle) * 16;
    tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
  }
}

void drawCloudIcon(int16_t x, int16_t y) {
  // Nube más grande
  tft.fillCircle(x-6, y, 5, TFT_WHITE);
  tft.fillCircle(x, y-3, 6, TFT_WHITE);
  tft.fillCircle(x+6, y, 5, TFT_WHITE);
  tft.fillRect(x-8, y, 16, 6, TFT_WHITE);
}

void drawRainIcon(int16_t x, int16_t y) {
  // Nube con lluvia más grande
  drawCloudIcon(x, y-5);
  tft.drawLine(x-6, y+5, x-6, y+10, TFT_CYAN);
  tft.drawLine(x, y+6, x, y+11, TFT_CYAN);
  tft.drawLine(x+6, y+5, x+6, y+10, TFT_CYAN);
}

void displayForecast() {
  if (!weatherDataAvailable || numForecasts == 0) {
    tft.fillScreen(COLOR_FONDO_AZUL);
    tft.setTextSize(3);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(30, 100);
    tft.print("Sin datos clima");
    return;
  }
  
  tft.fillScreen(COLOR_FONDO_AZUL);
  
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(50, 10);
  tft.print("PRONOSTICO");
  
  // Mostrar 3 días con íconos más grandes
  for (int i = 0; i < numForecasts && i < 3; i++) {
    int16_t yPos = 60 + (i * 50);
    
    // Día
    tft.setTextSize(4);
    tft.setCursor(10, yPos);
    tft.printf("%02d", forecasts[i].dayOfMonth);
    
    // Ícono del clima
    int16_t iconX = 80;
    int16_t iconY = yPos + 15;
    String weather = forecasts[i].weatherMain;
    if (weather == "Clear") {
      drawSunIcon(iconX, iconY);
    } else if (weather == "Rain" || weather == "Drizzle") {
      drawRainIcon(iconX, iconY);
    } else if (weather == "Clouds") {
      drawCloudIcon(iconX, iconY);
    } else {
      drawCloudIcon(iconX, iconY);
    }
    
    // Temperaturas
    tft.setTextSize(3);
    tft.setCursor(120, yPos);
    tft.printf("%.0f", forecasts[i].tempMin);
    tft.drawCircle(165, yPos+5, 2, TFT_WHITE); // símbolo grados
    
    tft.setCursor(180, yPos);
    tft.printf("%.0f", forecasts[i].tempMax);
    tft.drawCircle(225, yPos+5, 2, TFT_WHITE); // símbolo grados
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);  // Más tiempo para abrir Serial Monitor
  
  Serial.println("\n\n=== RELOJ PERONISTA TFT ILI9341 ===");
  Serial.println("Inicializando SPI...");
  
  // Inicializar SPI explícitamente
  SPI.begin();
  Serial.println("SPI OK");
  
  // Inicializar TFT
  Serial.println("Inicializando TFT...");
  tft.init();
  Serial.println("TFT inicializado");
  
  // Rotación horizontal (landscape)
  tft.setRotation(1); // Landscape: 1=90° (horizontal), 3=270° (horizontal invertido)
  Serial.println("Rotacion configurada: HORIZONTAL");
  
  tft.fillScreen(COLOR_FONDO_AZUL);
  tft.setTextSize(4);
  tft.setTextColor(COLOR_PERONISTA_CELESTE);
  tft.setCursor(40, 120);
  tft.print("RELOJ");
  tft.setCursor(20, 160);
  tft.print("PERONISTA");
  
  delay(2000);
  
  // WiFi
  tft.fillScreen(COLOR_FONDO_AZUL);
  tft.setTextSize(3);
  tft.setTextColor(COLOR_PERONISTA_BLANCO);
  tft.setCursor(60, 150);
  tft.print("WiFi...");
  
  connectWiFi();
  
  // NTP
  if (WiFi.status() == WL_CONNECTED) {
    tft.fillScreen(COLOR_FONDO_AZUL);
    tft.setCursor(20, 150);
    tft.print("Sync hora...");
    
    setupTime();
  }
  
  randomSeed(esp_random());
  
  // Botones
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ALARM_BUTTON_PIN, INPUT_PULLUP);
  
  // Sensores
  tft.fillScreen(COLOR_FONDO_AZUL);
  tft.setCursor(30, 150);
  tft.print("Sensores...");
  
  
  if (aht.begin()) {
    Serial.println("AHT10 OK");
    sensorsAvailable = true;
  }
  
  if (bmp.begin()) {
    Serial.println("BMP180 OK");
    sensorsAvailable = true;
  }
  
  if (sensorsAvailable) {
    readSensors();
  }
  
  // Clima
  if (WiFi.status() == WL_CONNECTED) {
    tft.fillScreen(COLOR_FONDO_AZUL);
    tft.setCursor(60, 150);
    tft.print("Clima...");
    
    fetchWeatherForecast();
  }
  
  delay(1000);
  
  Serial.println("Sistema OK!");
  Serial.println("Modos: AUTO->HORA->FECHA->EFEM->SENSORES->PRONOSTICO");
  
  if (alarmEnabled) {
    Serial.printf("⏰ Alarma: %02d:%02d ON\n", alarmHour, alarmMinute);
  } else {
    Serial.println("⏰ Alarma: OFF");
  }
}

void checkButton() {
  if (digitalRead(BUTTON_PIN) == LOW && (millis() - lastButtonPress > BUTTON_DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    currentMode = (DisplayMode)((currentMode + 1) % 6);
    
    Serial.print("Modo: ");
    switch(currentMode) {
      case MODE_AUTO: Serial.println("AUTO"); break;
      case MODE_CLOCK_ONLY: Serial.println("HORA"); break;
      case MODE_DATE_ONLY: Serial.println("FECHA"); break;
      case MODE_EPHEMERIS_ONLY:
        Serial.println("EFEMERIDES");
        marqueeY = 240; // Reset posición vertical
        currentEfemerideIndex = random(0, numEfemerides);
        break;
      case MODE_SENSORS: 
        Serial.println("SENSORES"); 
        lastSensorToggle = millis();
        sensorDisplayIndex = 0;
        break;
      case MODE_FORECAST: Serial.println("PRONOSTICO"); break;
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Siempre chequear alarma
  checkAlarm();
  
  // Si alarma sonando, mostrar y salir
  if (alarmTriggered) {
    displayAlarm();
    delay(50);
    return;
  }
  
  // Chequear botón de alarma
  checkAlarmButton();
  
  // Si en modo config, mostrar y salir
  if (alarmConfigMode) {
    displayAlarmConfig();
    delay(50);
    return;
  }
  
  // Botón de modo normal
  checkButton();
  
  if (currentMillis - lastTimeSync >= TIME_SYNC_INTERVAL) {
    resyncTime();
  }
  
  if (sensorsAvailable && (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL)) {
    readSensors();
    lastSensorRead = currentMillis;
  }
  
  if (currentMillis - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL) {
    fetchWeatherForecast();
  }
  
  switch(currentMode) {
    case MODE_AUTO:
      if (currentMillis - lastEfemerideTime >= EFEMERIDE_INTERVAL) {
        showingEfemeride = !showingEfemeride;
        if (showingEfemeride) {
          marqueeY = 240; // Reset posición vertical
          currentEfemerideIndex = random(0, numEfemerides);
        }
        lastEfemerideTime = currentMillis;
      }
      
      if (showingEfemeride) {
        displayEfemeride();
        delay(20);
      } else {
        if (!showingDate && (currentMillis - lastDateToggleTime >= DATE_SHOW_INTERVAL)) {
          showingDate = true;
          lastDateToggleTime = currentMillis;
        } else if (showingDate && (currentMillis - lastDateToggleTime >= DATE_DISPLAY_DURATION)) {
          showingDate = false;
          lastDateToggleTime = currentMillis;
        }
        
        if (showingDate) {
          displayDate();
        } else {
          displayTime();
        }
        delay(1000);
      }
      break;
      
    case MODE_CLOCK_ONLY:
      displayTime();
      delay(1000);
      break;
      
    case MODE_DATE_ONLY:
      displayDate();
      delay(1000);
      break;
      
    case MODE_EPHEMERIS_ONLY:
      displayEfemeride();
      delay(20);
      break;
      
    case MODE_SENSORS:
      if (sensorsAvailable) {
        // Alternar entre temperatura, humedad y presión cada 3 segundos
        if (currentMillis - lastSensorToggle >= SENSOR_TOGGLE_INTERVAL) {
          sensorDisplayIndex = (sensorDisplayIndex + 1) % 3;
          lastSensorToggle = currentMillis;
        }
        
        switch(sensorDisplayIndex) {
          case 0: displayTemperature(); break;
          case 1: displayHumidity(); break;
          case 2: displayPressure(); break;
        }
      }
      delay(1000);
      break;
      
    case MODE_FORECAST:
      displayForecast();
      delay(1000);
      break;
  }
}
