#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"
#include "config.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ========== VARIABLES GLOBALES ==========
unsigned long lastEfemerideTime = 0;
bool showingEfemeride = false;
int currentEfemerideIndex = 0;

// Variables para alternar entre hora y fecha
unsigned long lastDateToggleTime = 0;
bool showingDate = false;
const unsigned long DATE_SHOW_INTERVAL = 30000; // Mostrar fecha cada 30 segundos
const unsigned long DATE_DISPLAY_DURATION = 2000; // Mostrar fecha durante 2 segundos

// Variables para efecto marquesina en efemérides
int16_t marqueeX = 0;
unsigned long lastMarqueeUpdate = 0;
const unsigned long MARQUEE_SPEED = 50; // Milisegundos entre movimientos

// Variables para sincronización periódica
unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 86400000; // 24 horas en milisegundos (1 día)

// Variables para control de brillo nocturno
const int NIGHT_START_HOUR = 22;  // 22:00 (10 PM)
const int NIGHT_END_HOUR = 7;     // 07:00 (7 AM)
const uint8_t BRIGHTNESS_DAY = 255;    // Brillo máximo durante el día
const uint8_t BRIGHTNESS_NIGHT = 30;   // Brillo muy bajo durante la noche
bool isNightMode = false;
unsigned long lastBrightnessCheck = 0;
const unsigned long BRIGHTNESS_CHECK_INTERVAL = 60000; // Verificar cada minuto

// Variables para control de botón y modos
const int BUTTON_PIN = 15;  // GPIO 15 para el botón
enum DisplayMode {
  MODE_AUTO,        // Modo automático (alterna hora/fecha/efemérides)
  MODE_CLOCK_ONLY,  // Solo muestra hora
  MODE_DATE_ONLY,   // Solo muestra fecha
  MODE_EPHEMERIS_ONLY  // Solo muestra efemérides
};
DisplayMode currentMode = MODE_AUTO;
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200; // Debounce de 200ms

// Lista de efemérides peronistas
const char* efemerides[] = {
"08/10/1895 Nace Juan Domingo Perón",
"07/05/1919 Nace Eva Duarte de Perón",
"04/06/1943 Golpe militar del GOU en el que emerge Perón",
"27/10/1943 Perón asume en el Departamento de Trabajo",
"02/12/1943 Creación de la Secretaría de Trabajo y Previsión",
"24/02/1944 Perón asume como vicepresidente",
"17/10/1945 Movilización obrera (Día de la Lealtad)",
"22/10/1945 Liberación de Perón tras presión popular",
"24/02/1946 Elección que lleva a Perón al poder",
"04/06/1946 Asume Perón como presidente por primera vez",
"20/12/1945 Formación del Partido Laborista",
"09/07/1947 Declaración de Independencia Económica",
"08/07/1947 Gira europea de Evita",
"23/09/1947 Sanción del voto femenino",
"03/01/1947 Creación del Consejo Económico",
"01/05/1947 Discurso clave de Evita",
"21/11/1946 Creación del IAPI",
"19/06/1948 Creación de la Fundación Eva Perón",
"11/03/1949 Reforma Constitucional",
"09/04/1949 Promulgación Constitución",
"01/05/1950 Acto del Día del Trabajador",
"22/08/1951 Cabildo Abierto del Justicialismo",
"31/08/1951 Renunciamiento de Evita",
"11/11/1951 Primera elección con voto femenino",
"01/05/1952 Último discurso de Evita",
"04/06/1952 Segundo mandato de Perón",
"26/07/1952 Fallece Eva Perón",
"26/07/1953 Primer aniversario sin Evita",
"16/06/1955 Bombardeo a Plaza de Mayo",
"16/09/1955 Derrocamiento de Perón",
"05/10/1955 Disolución del Partido Peronista",
"09/06/1956 Levantamiento peronista (Valle)",
"17/11/1972 Regreso de Perón al país",
"18/11/1972 Día de la Militancia",
"11/03/1973 Elección de Héctor Cámpora",
"25/05/1973 Asume Cámpora",
"20/06/1973 Masacre de Ezeiza",
"13/07/1973 Renuncia Cámpora",
"01/07/1973 Asume Raúl Lastiri",
"23/09/1973 Elección de Perón",
"12/10/1973 Asume Perón (tercer mandato)",
"25/09/1973 Asesinato de Rucci",
"01/05/1974 Ruptura con Montoneros",
"12/06/1974 Último discurso de Perón",
"01/07/1974 Fallece Juan Domingo Perón",
"01/07/1974 Asume Isabel Perón",
"24/03/1976 Golpe militar",
"23/01/1987 Reorganización del PJ",
"25/05/2003 Asume Néstor Kirchner",
"27/04/2003 Elección presidencial",
"27/10/2007 Elección de Cristina Fernández",
"10/12/2007 Asume Cristina Fernández",
"27/10/2010 Fallece Néstor Kirchner",
"23/10/2011 Reelección de Cristina",
"09/12/2015 Último acto de Cristina",
"10/12/2015 Fin del ciclo kirchnerista",
"27/10/2019 Elección de Alberto Fernández",
"10/12/2019 Asume Alberto Fernández",
"01/03/2020 Primer discurso presidencial",
"17/10/2020 Día de la Lealtad en pandemia",
"01/03/2021 Apertura legislativa",
"01/03/2022 Discurso presidencial",
"01/03/2023 Último discurso de gestión",
"25/05/2023 Actos políticos peronistas",
"07/05/2023 Actos por Evita",
"08/10/2023 Actos por Perón",
"17/10/2023 Movilización peronista",
"20/11/2023 Contexto electoral",
"10/12/2023 Fin del gobierno de Alberto Fernández",
"01/05/2024 Día del trabajador",
"25/05/2024 Actos peronistas",
"07/05/2024 Homenaje a Evita",
"08/10/2024 Homenaje a Perón",
"17/10/2024 Día de la Lealtad",
"17/11/2024 Día de la militancia",
"26/07/2024 Homenaje a Evita",
"27/10/2024 Recuerdo de Néstor Kirchner",
"09/07/2024 Actos independencia",
"20/06/2024 Actos políticos vinculados"
};
const int numEfemerides = sizeof(efemerides) / sizeof(efemerides[0]);

// ========== FUNCIONES ==========

void connectWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar al WiFi");
  }
}

void setupTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.println("Sincronizando hora con NTP...");
  
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 10) {
    Serial.print(".");
    delay(500);
    attempts++;
  }
  
  if (attempts < 10) {
    Serial.println("\nHora sincronizada!");
    lastTimeSync = millis(); // Guardar el momento de la sincronización
  } else {
    Serial.println("\nNo se pudo sincronizar la hora");
  }
}

void resyncTime() {
  Serial.println("Resincronizando hora con NTP...");
  
  // Verificar que seguimos conectados al WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reconectando...");
    connectWiFi();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    struct tm timeinfo;
    int attempts = 0;
    
    // Forzar resincronización configurando de nuevo
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    
    while (!getLocalTime(&timeinfo) && attempts < 10) {
      Serial.print(".");
      delay(500);
      attempts++;
    }
    
    if (attempts < 10) {
      Serial.println("\nHora resincronizada exitosamente!");
      Serial.printf("Nueva hora: %02d:%02d:%02d - %02d/%02d/%04d\n", 
                   timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                   timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
      lastTimeSync = millis();
    } else {
      Serial.println("\nError al resincronizar. Se reintentará en la próxima ventana.");
    }
  }
}

void adjustBrightness() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return; // Si no hay hora, no ajustar
  }
  
  int currentHour = timeinfo.tm_hour;
  bool shouldBeNight = false;
  
  // Verificar si estamos en horario nocturno (22:00 - 07:00)
  if (currentHour >= NIGHT_START_HOUR || currentHour < NIGHT_END_HOUR) {
    shouldBeNight = true;
  }
  
  // Solo cambiar si el modo cambió
  if (shouldBeNight != isNightMode) {
    isNightMode = shouldBeNight;
    
    if (isNightMode) {
      // Activar modo nocturno (brillo bajo)
      display.ssd1306_command(0x81); // Comando de contraste
      display.ssd1306_command(BRIGHTNESS_NIGHT);
      Serial.println("Modo nocturno activado - Brillo bajo");
    } else {
      // Activar modo día (brillo alto)
      display.ssd1306_command(0x81); // Comando de contraste
      display.ssd1306_command(BRIGHTNESS_DAY);
      Serial.println("Modo día activado - Brillo alto");
    }
  }
}

void displayTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 12);
    display.print("Error de hora");
    display.display();
    return;
  }
  
  display.clearDisplay();
  
  // Mostrar hora en formato GRANDE ocupando toda la altura (HH:MM)
  display.setTextSize(4);  // Tamaño 4 = 32px alto (máximo para pantalla de 32px)
  display.setTextColor(SSD1306_WHITE);
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  
  // Centrar horizontalmente, posicionar desde Y=0 para ocupar toda la altura
  // Tamaño 4: cada caracter = 24px ancho, 5 caracteres = 120px
  int16_t x = (SCREEN_WIDTH - 120) / 2;  // Centrado horizontal
  int16_t y = 0;   // Desde arriba para ocupar toda la altura (32px)
  
  display.setCursor(x, y);
  display.print(timeStr);
  
  display.display();
}

void displayDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  
  display.clearDisplay();
  
  // Mostrar fecha grande ocupando máxima altura
  display.setTextSize(4);  // Tamaño 4 para ocupar toda la altura disponible
  display.setTextColor(SSD1306_WHITE);
  
  char dateStr[12];
  sprintf(dateStr, "%02d/%02d", 
          timeinfo.tm_mday, 
          timeinfo.tm_mon + 1);  // Solo día y mes
  
  // Centrar la fecha horizontalmente, desde arriba verticalmente
  // Tamaño 4: cada caracter = 24px ancho, 5 caracteres (DD/MM) = 120px
  int16_t x = (SCREEN_WIDTH - 120) / 2;
  int16_t y = 0;  // Desde arriba para ocupar toda la altura
  
  display.setCursor(x, y);
  display.print(dateStr);
  
  display.display();
}

void displayEfemeride() {
  unsigned long currentMillis = millis();
  
  // Actualizar posición de marquesina cada MARQUEE_SPEED ms
  if (currentMillis - lastMarqueeUpdate >= MARQUEE_SPEED) {
    lastMarqueeUpdate = currentMillis;
    marqueeX -= 2; // Mover 2 píxeles a la izquierda
  }
  
  display.clearDisplay();
  display.setTextSize(3);  // Texto GRANDE para efemérides - ocupa casi todo el alto
  display.setTextColor(SSD1306_WHITE);
  
  // Obtener el texto de la efeméride actual (sin saltos de línea)
  String efemText = String(efemerides[currentEfemerideIndex]);
  efemText.replace("\n", " - ");  // Reemplazar saltos por guiones
  
  // Calcular ancho del texto
  int16_t textWidth = efemText.length() * 18;  // 18px por caracter en tamaño 3
  
  // Si el texto llegó al final, reiniciar desde la derecha
  if (marqueeX < -textWidth) {
    marqueeX = SCREEN_WIDTH;
  }
  
  // Centrar verticalmente (tamaño 3 = 24px de alto)
  int16_t y = (SCREEN_HEIGHT - 24) / 2;
  
  display.setCursor(marqueeX, y);
  display.print(efemText);
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== RELOJ PERONISTA ===");
  
  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Error: No se encontró la pantalla SSD1306");
    for (;;); // No continuar si no hay pantalla
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Iniciando...");
  display.display();
  delay(1000);
  
  // Conectar WiFi
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Conectando WiFi");
  display.display();
  connectWiFi();
  
  // Configurar hora
  if (WiFi.status() == WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Sincronizando\nhora...");
    display.display();
    setupTime();
  }
  
  delay(1000);
  
  // Inicializar generador de números aleatorios
  randomSeed(analogRead(0));
  
  // Configurar botón con resistencia pull-up interna
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Sistema iniciado!");
  Serial.println("Presiona el boton para cambiar modos:");
  Serial.println("  AUTO -> SOLO HORA -> SOLO FECHA -> SOLO EFEMERIDES");
}

void checkButton() {
  // Leer botón (LOW = presionado porque usa pull-up)
  if (digitalRead(BUTTON_PIN) == LOW && (millis() - lastButtonPress > DEBOUNCE_DELAY)) {
    lastButtonPress = millis();
    
    // Cambiar al siguiente modo
    currentMode = (DisplayMode)((currentMode + 1) % 4);
    
    // Mostrar en serial qué modo se activó
    Serial.print("Modo cambiado a: ");
    switch(currentMode) {
      case MODE_AUTO:
        Serial.println("AUTOMATICO (hora/fecha/efemerides)");
        break;
      case MODE_CLOCK_ONLY:
        Serial.println("SOLO HORA");
        break;
      case MODE_DATE_ONLY:
        Serial.println("SOLO FECHA");
        break;
      case MODE_EPHEMERIS_ONLY:
        Serial.println("SOLO EFEMERIDES");
        marqueeX = SCREEN_WIDTH;  // Reiniciar marquesina
        currentEfemerideIndex = random(0, numEfemerides);
        break;
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Verificar botón (siempre primero)
  checkButton();
  
  // Verificar si es momento de resincronizar la hora (cada 24 horas)
  if (currentMillis - lastTimeSync >= TIME_SYNC_INTERVAL) {
    resyncTime();
  }
  
  // Verificar y ajustar brillo cada minuto
  if (currentMillis - lastBrightnessCheck >= BRIGHTNESS_CHECK_INTERVAL) {
    adjustBrightness();
    lastBrightnessCheck = currentMillis;
  }
  
  // Gestión de modos
  switch(currentMode) {
    case MODE_AUTO:
      // Modo automático: alterna hora/fecha/efemérides
      if (currentMillis - lastEfemerideTime >= EFEMERIDE_INTERVAL) {
        if (!showingEfemeride) {
          showingEfemeride = true;
          marqueeX = SCREEN_WIDTH;
          currentEfemerideIndex = random(0, numEfemerides);
          lastEfemerideTime = currentMillis;
        } else {
          showingEfemeride = false;
          lastEfemerideTime = currentMillis;
        }
      }
      
      if (showingEfemeride) {
        displayEfemeride();
        delay(20);
      } else {
        if (!showingDate && (currentMillis - lastDateToggleTime >= DATE_SHOW_INTERVAL)) {
          showingDate = true;
          lastDateToggleTime = currentMillis;
          displayDate();
        } else if (showingDate && (currentMillis - lastDateToggleTime >= DATE_DISPLAY_DURATION)) {
          showingDate = false;
          lastDateToggleTime = currentMillis;
          displayTime();
        } else {
          if (showingDate) {
            displayDate();
          } else {
            displayTime();
          }
        }
        delay(1000);
      }
      break;
      
    case MODE_CLOCK_ONLY:
      // Solo muestra hora
      displayTime();
      delay(1000);
      break;
      
    case MODE_DATE_ONLY:
      // Solo muestra fecha
      displayDate();
      delay(1000);
      break;
      
    case MODE_EPHEMERIS_ONLY:
      // Solo muestra efemérides en marquesina
      displayEfemeride();
      delay(20);
      break;
  }
}
