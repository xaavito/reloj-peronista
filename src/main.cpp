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
#include "peron_image.h"

// Inicializar display TFT con TFT_eSPI
TFT_eSPI tft = TFT_eSPI();        // Configuración viene de platformio.ini build_flags

Adafruit_AHTX0 aht;
Adafruit_BMP085 bmp;

// ========== VARIABLES GLOBALES ==========
bool peronistMode = false;  // false=modo normal, true=modo peronista (efemérides)
int currentEfemerideIndex = 0;
unsigned long lastModeButtonPress = 0;
const unsigned long MODE_BUTTON_DEBOUNCE = 300;  // 300ms debounce

int16_t marqueeX = 0;
int16_t marqueeY = 240; // Empieza abajo de la pantalla
unsigned long lastMarqueeUpdate = 0;
const unsigned long MARQUEE_SPEED = 50;

unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 432000000;  // 5 días en milisegundos (5 * 24 * 60 * 60 * 1000)

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

// Snooze
bool alarmSnoozed = false;
unsigned long snoozeStartTime = 0;
const unsigned long SNOOZE_DURATION = 480000;  // 8 minutos en milisegundos (8 * 60 * 1000)
int snoozeCount = 0;

// Botones
const int BUTTON_PIN = 27;           // Botón cambio de modo (GPIO 27 disponible - no interfiere con HSPI)
const int ALARM_BUTTON_PIN = 4;      // Botón configuración alarma

// Buzzer
const int BUZZER_PIN = 25;           // Buzzer activo 5V para alarma (GPIO 25)

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

void displayAllInfo() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(40, 100);
    tft.print("Error hora");
    return;
  }
  
  // Fondo negro
  tft.fillScreen(TFT_BLACK);
  
  // ========== HORA ARRIBA (grande) ==========
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  
  tft.setTextFont(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);  // Tamaño 3x
  tft.setCursor(20, 10);  // Lo más arriba posible
  tft.print(timeStr);
  
  // ========== FECHA (más chica) ==========
  tft.setTextFont(2);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  char dateStr[16];
  sprintf(dateStr, "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  tft.setCursor(20, 95);
  tft.print(dateStr);
  
  // ========== SENSORES (mismo tamaño que fecha) ==========
  if (sensorsAvailable) {
    tft.setTextFont(2);
    tft.setTextSize(2);
    
    // Temperatura
    tft.setCursor(10, 130);
    tft.printf("T:%.1fC", temperature);
    
    // Humedad
    tft.setCursor(10, 155);
    tft.printf("H:%.1f%%", humidity);
    
    // Presión
    tft.setCursor(10, 180);
    tft.printf("P:%.0fhPa", pressure);
  }
  
  // ========== PRONÓSTICO (3 días con íconos GRANDES) ==========
  if (weatherDataAvailable && numForecasts > 0) {
    // Mostrar hasta 3 días con íconos más grandes (sin título)
    // Bajado para no superponerse con presión
    for (int i = 0; i < numForecasts && i < 3; i++) {
      int16_t yPos = 215 + (i * 30);  // Bajado de 205 a 215, espaciado 30px
      
      // Día (más grande)
      tft.setTextSize(2);
      tft.setCursor(10, yPos);
      tft.printf("%02d", forecasts[i].dayOfMonth);
      
      // Ícono más grande del clima
      int16_t iconX = 45;
      int16_t iconY = yPos + 6;
      String weather = forecasts[i].weatherMain;
      
      if (weather == "Clear") {
        // Sol más grande
        tft.fillCircle(iconX, iconY, 5, TFT_YELLOW);
        // Rayos del sol
        for (int j = 0; j < 4; j++) {
          float angle = j * 90 * PI / 180;
          int x1 = iconX + cos(angle) * 7;
          int y1 = iconY + sin(angle) * 7;
          tft.drawPixel(x1, y1, TFT_YELLOW);
        }
      } else if (weather == "Rain" || weather == "Drizzle") {
        // Nube con lluvia más grande
        tft.fillCircle(iconX-3, iconY, 3, TFT_CYAN);
        tft.fillCircle(iconX+3, iconY, 3, TFT_CYAN);
        tft.fillRect(iconX-4, iconY, 8, 3, TFT_CYAN);
        // Gotas
        tft.drawLine(iconX-2, iconY+4, iconX-2, iconY+7, TFT_CYAN);
        tft.drawLine(iconX+2, iconY+4, iconX+2, iconY+7, TFT_CYAN);
      } else {
        // Nube más grande
        tft.fillCircle(iconX-3, iconY, 3, TFT_WHITE);
        tft.fillCircle(iconX+3, iconY, 3, TFT_WHITE);
        tft.fillRect(iconX-4, iconY, 8, 3, TFT_WHITE);
      }
      
      // Temperaturas más grandes
      tft.setTextSize(2);
      tft.setCursor(70, yPos);
      tft.printf("%.0f-%.0fC", forecasts[i].tempMin, forecasts[i].tempMax);
    }
  }
  
  // ========== FILA DE INDICADORES (Debajo de hora, antes de fecha) ==========
  int16_t indicatorY = 75;  // Bajado para no superponerse con hora
  int16_t startX = 20;      // Alineado con la hora
  
  // ÍCONO WIFI (VERDE)
  if (WiFi.status() == WL_CONNECTED) {
    int16_t wifiX = startX + 5;
    
    // Arcos WiFi en VERDE
    tft.drawCircle(wifiX, indicatorY+6, 1, TFT_GREEN);      // Base
    tft.drawCircle(wifiX, indicatorY+6, 3, TFT_GREEN);      // Nivel 1
    tft.drawCircle(wifiX, indicatorY+6, 5, TFT_GREEN);      // Nivel 2
    tft.drawCircle(wifiX, indicatorY+6, 7, TFT_GREEN);      // Nivel 3
    
    // Solo mostrar arcos superiores
    tft.fillRect(wifiX-8, indicatorY+7, 16, 8, TFT_BLACK);
  }
  
  // ÍCONO ALARMA + HORA (al lado del WiFi)
  if (alarmEnabled) {
    int16_t bellX = startX + 30;  // 25px después del WiFi
    
    // Campana amarilla
    tft.fillCircle(bellX+2, indicatorY+6, 2, TFT_YELLOW);       // Cuerpo
    tft.fillRect(bellX, indicatorY+7, 4, 2, TFT_YELLOW);        // Base
    tft.fillRect(bellX+1, indicatorY+9, 2, 1, TFT_BLACK);       // Abertura
    tft.fillCircle(bellX+2, indicatorY+4, 1, TFT_YELLOW);       // Pomo
    
    // Hora de alarma al lado
    tft.setTextFont(1);
    tft.setTextSize(2);  // Más grande para que se vea bien
    tft.setCursor(bellX + 10, indicatorY);
    tft.printf("%02d:%02d", alarmHour, alarmMinute);
    
    // ÍCONO SNOOZE si está activo
    if (alarmSnoozed) {
      tft.setTextSize(2);
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.setCursor(bellX + 58, indicatorY);
      tft.print("Z");  // Icono simple de snooze
      
      // Pequeño contador si hay múltiples snoozes
      if (snoozeCount > 1) {
        tft.setTextSize(1);
        tft.setCursor(bellX + 70, indicatorY + 8);
        tft.printf("x%d", snoozeCount);
      }
    }
  }
  
  // Volver a fuente por defecto
  tft.setTextFont(1);
  tft.setTextSize(1);
}

// Mantener displayTime para compatibilidad
void displayTime() {
  displayAllInfo();
}

void displayDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;
  
  tft.fillScreen(TFT_BLACK);
  
  // Usar FONT8 (7-segment font grande) que SÍ escala bien
  tft.setTextFont(8);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
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
  tft.fillScreen(TFT_BLACK);
  
  // ========== MOSTRAR IMAGEN DE PERÓN ==========
  // Centrar imagen (101x140 en pantalla 240x320)
  int16_t imgX = (240 - PERON_IMG_WIDTH) / 2;  // Centrado horizontal
  int16_t imgY = 20;  // Arriba
  
  // Dibujar imagen desde PROGMEM
  tft.pushImage(imgX, imgY, PERON_IMG_WIDTH, PERON_IMG_HEIGHT, peron_image);
  
  // ========== MOSTRAR EFEMÉRIDE DEBAJO ==========
  String efemText = String(efemerides[currentEfemerideIndex]);
  
  // Separar fecha y descripción
  int separatorPos = efemText.indexOf(' ', 10);
  String fecha = efemText.substring(0, 10);  // "DD/MM/AAAA"
  String descripcion = efemText.substring(11);  // Resto del texto
  
  // Posición del texto debajo de la imagen
  int16_t textY = imgY + PERON_IMG_HEIGHT + 20;
  
  // Mostrar fecha
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(40, textY);
  tft.print(fecha);
  
  // Mostrar descripción (ajustar para que quepa)
  tft.setTextSize(2);
  tft.setCursor(10, textY + 25);
  
  // Si el texto es muy largo, dividirlo en líneas
  if (descripcion.length() > 28) {
    // Primera línea
    String linea1 = descripcion.substring(0, 28);
    tft.print(linea1);
    
    // Segunda línea si hay más texto
    if (descripcion.length() > 28) {
      String linea2 = descripcion.substring(28);
      if (linea2.length() > 28) {
        linea2 = linea2.substring(0, 25) + "...";
      }
      tft.setCursor(10, textY + 45);
      tft.print(linea2);
    }
  } else {
    tft.print(descripcion);
  }
  
  // Cambiar efeméride cada 10 segundos
  static unsigned long lastEfemChange = 0;
  if (millis() - lastEfemChange >= 10000) {
    currentEfemerideIndex = random(0, numEfemerides);
    lastEfemChange = millis();
  }
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
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(30, 30);
  tft.print("TEMPERATURA");
  
  tft.setTextSize(9);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  char tempStr[10];
  sprintf(tempStr, "%.1fC", temperature);
  tft.setCursor(30, 100);
  tft.print(tempStr);
}

void displayHumidity() {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(70, 30);
  tft.print("HUMEDAD");
  
  tft.setTextSize(9);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  char humStr[10];
  sprintf(humStr, "%.1f%%", humidity);
  tft.setCursor(30, 100);
  tft.print(humStr);
}

void displayPressure() {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(70, 30);
  tft.print("PRESION");
  
  tft.setTextSize(8);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
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
  
  unsigned long currentMillis = millis();
  
  // Verificar si termina snooze
  if (alarmSnoozed && (currentMillis - snoozeStartTime >= SNOOZE_DURATION)) {
    alarmSnoozed = false;
    alarmTriggered = true;
    alarmStartTime = currentMillis;
    Serial.printf("⏰ Alarma reactivada después de snooze #%d\n", snoozeCount);
  }
  
  // Activar alarma a la hora configurada
  if (timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute && 
      !alarmTriggered && !alarmSnoozed) {
    alarmTriggered = true;
    alarmStartTime = currentMillis;
    snoozeCount = 0;  // Reset contador de snooze
    Serial.println("🔔 ALARMA ACTIVADA!");
  }
  
  // Timeout de alarma (60 minutos máximo si no se apaga)
  if (alarmTriggered && (currentMillis - alarmStartTime >= 3600000)) {
    alarmTriggered = false;
    alarmSnoozed = false;
    snoozeCount = 0;
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Alarma finalizada (timeout 60min)");
  }
  
  // Reset al cambiar de hora
  if (timeinfo.tm_hour != alarmHour) {
    if (alarmTriggered || alarmSnoozed) {
      alarmTriggered = false;
      alarmSnoozed = false;
      snoozeCount = 0;
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

void displayAlarm() {
  unsigned long currentMillis = millis();
  
  // Buzzer intermitente (500ms on/off)
  if (currentMillis - lastAlarmBlink >= 500) {
    lastAlarmBlink = currentMillis;
    alarmBlinkState = !alarmBlinkState;
    digitalWrite(BUZZER_PIN, alarmBlinkState ? HIGH : LOW);
  }
  
  // ========== PANTALLA CON IMAGEN DE PERÓN ==========
  tft.fillScreen(TFT_BLACK);
  
  // Mostrar imagen de Perón centrada arriba
  int16_t imgX = (240 - PERON_IMG_WIDTH) / 2;
  int16_t imgY = 30;
  tft.pushImage(imgX, imgY, PERON_IMG_WIDTH, PERON_IMG_HEIGHT, peron_image);
  
  // Mensaje motivacional debajo
  int16_t textY = imgY + PERON_IMG_HEIGHT + 20;
  
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  // Línea 1
  tft.setCursor(10, textY);
  tft.print("El general necesita");
  
  // Línea 2
  tft.setCursor(10, textY + 20);
  tft.print("que te levantes y");
  
  // Línea 3
  tft.setCursor(10, textY + 40);
  tft.print("trabajes para sacar");
  
  // Línea 4
  tft.setCursor(10, textY + 60);
  tft.print("el pais adelante");
  
  // Mostrar contador de snooze si hay
  if (snoozeCount > 0) {
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.printf("Snooze x%d", snoozeCount);
  }
}

void displayAlarmConfig() {
  tft.fillScreen(TFT_BLACK);
  
  // Título
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 20);
  tft.print("CONFIG ALARMA");
  
  // Hora y Minuto
  tft.setTextSize(10);
  tft.setCursor(20, 80);
  
  // Hora
  if (alarmConfigField == 0) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }
  tft.printf("%02d", tempAlarmHour);
  
  // Dos puntos
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print(":");
  
  // Minuto
  if (alarmConfigField == 1) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }
  tft.printf("%02d", tempAlarmMinute);
  
  // ON/OFF
  tft.setTextSize(6);
  if (alarmConfigField == 2) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }
  tft.setCursor(80, 170);
  tft.print(tempAlarmEnabled ? " ON " : " OFF");
  
  // Instrucción
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
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
  
  // SI ALARMA SONANDO: PRESIÓN CORTA APAGA LA ALARMA
  if (alarmTriggered && buttonState == LOW && alarmButtonPressStart == 0) {
    alarmButtonPressStart = currentMillis;
  }
  
  if (alarmTriggered && buttonState == HIGH && alarmButtonPressStart > 0) {
    unsigned long pressDuration = currentMillis - alarmButtonPressStart;
    
    // Cualquier presión apaga la alarma
    if (pressDuration < 3000) {  // Hasta 3 segundos
      alarmTriggered = false;
      alarmSnoozed = false;
      snoozeCount = 0;
      digitalWrite(BUZZER_PIN, LOW);
      Serial.println("✓ Alarma APAGADA por usuario");
      
      // Mostrar confirmación
      tft.fillScreen(TFT_BLACK);
      tft.setTextSize(4);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(50, 120);
      tft.print("ALARMA");
      tft.setCursor(60, 160);
      tft.print("APAGADA");
      delay(2000);
      
      alarmButtonPressStart = 0;
      return;
    }
    
    alarmButtonPressStart = 0;
  }
  
  // MODO NORMAL: CONFIGURACIÓN DE ALARMA
  if (!alarmTriggered) {
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
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(30, 100);
    tft.print("Sin datos clima");
    return;
  }
  
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
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
  
  // Rotación vertical (portrait)
  tft.setRotation(0); // Portrait: 0=0° (vertical), 2=180° (vertical invertido)
  Serial.println("Rotacion configurada: VERTICAL");
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(40, 120);
  tft.print("RELOJ");
  tft.setCursor(20, 160);
  tft.print("PERONISTA");
  
  delay(2000);
  
  // WiFi
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(60, 150);
  tft.print("WiFi...");
  
  connectWiFi();
  
  // NTP
  if (WiFi.status() == WL_CONNECTED) {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 150);
    tft.print("Sync hora...");
    
    setupTime();
  }
  
  randomSeed(esp_random());
  
  // Botones
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ALARM_BUTTON_PIN, INPUT_PULLUP);
  
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Iniciar apagado
  Serial.println("Buzzer configurado en GPIO 25");
  
  // Sensores
  tft.fillScreen(TFT_BLACK);
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
    tft.fillScreen(TFT_BLACK);
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

void checkModeButton() {
  unsigned long currentMillis = millis();
  
  // SI ALARMA SONANDO: BOTÓN ES SNOOZE
  if (alarmTriggered && digitalRead(BUTTON_PIN) == LOW && 
      (currentMillis - lastModeButtonPress > MODE_BUTTON_DEBOUNCE)) {
    
    lastModeButtonPress = currentMillis;
    
    // Activar snooze
    alarmSnoozed = true;
    snoozeStartTime = currentMillis;
    alarmTriggered = false;  // Temporalmente apagar alarma
    snoozeCount++;
    
    digitalWrite(BUZZER_PIN, LOW);  // Apagar buzzer
    
    Serial.printf("💤 SNOOZE #%d activado - 8 minutos\n", snoozeCount);
    
    // Mostrar mensaje de snooze
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(40, 120);
    tft.printf("SNOOZE x%d", snoozeCount);
    tft.setTextSize(2);
    tft.setCursor(50, 160);
    tft.print("8 minutos...");
    delay(2000);
    
    return;
  }
  
  // MODO NORMAL: TOGGLE MODO PERONISTA
  if (!alarmTriggered && digitalRead(BUTTON_PIN) == LOW && 
      (currentMillis - lastModeButtonPress > MODE_BUTTON_DEBOUNCE)) {
    
    lastModeButtonPress = currentMillis;
    peronistMode = !peronistMode;  // Toggle entre modos
    
    if (peronistMode) {
      // Entrando a modo peronista
      marqueeY = 320; // Reset posición vertical (empieza abajo)
      currentEfemerideIndex = random(0, numEfemerides);
      Serial.println("▶ MODO PERONISTA (Efemérides)");
    } else {
      // Volviendo a modo normal
      Serial.println("▶ MODO NORMAL (Hora y Datos)");
    }
    
    delay(200); // Debounce adicional
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Siempre chequear alarma
  checkAlarm();
  
  // IMPORTANTE: Chequear botones ANTES de mostrar pantalla
  checkModeButton();      // GPIO 27 - Snooze cuando alarma suena
  checkAlarmButton();     // GPIO 4 - Apagar alarma
  
  // Si alarma sonando, mostrar y salir (después de chequear botones)
  if (alarmTriggered) {
    displayAlarm();
    delay(50);
    return;
  }
  
  // Si en modo config, mostrar y salir
  if (alarmConfigMode) {
    displayAlarmConfig();
    delay(50);
    return;
  }
  
  // Resync hora cada 5 días
  if (currentMillis - lastTimeSync >= TIME_SYNC_INTERVAL) {
    resyncTime();
  }
  
  // Leer sensores cada 10 segundos
  if (sensorsAvailable && (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL)) {
    readSensors();
    lastSensorRead = currentMillis;
  }
  
  // Actualizar clima periódicamente
  if (currentMillis - lastWeatherUpdate >= WEATHER_UPDATE_INTERVAL) {
    fetchWeatherForecast();
  }
  
  // ========== 2 MODOS: NORMAL Y PERONISTA ==========
  
  if (peronistMode) {
    // MODO PERONISTA: Efemérides con efecto scrolling
    displayEfemeride();
    delay(20);
  } else {
    // MODO NORMAL: Pantalla principal con toda la info
    // Reducir flickeo: solo actualizar cada segundo si cambió
    static unsigned long lastFullUpdate = 0;
    if (currentMillis - lastFullUpdate >= 1000) {
      displayAllInfo();
      lastFullUpdate = currentMillis;
    }
    delay(50);  // Delay más corto para mejor respuesta de botones
  }
}
