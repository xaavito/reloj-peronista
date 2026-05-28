# 🎨 Propuestas de Mejoras Visuales - Reloj Peronista

## 📋 Índice
1. [Diseño Actual vs. Propuesto](#diseño-actual-vs-propuesto)
2. [Paleta de Colores Peronista](#paleta-de-colores-peronista)
3. [Mejoras Específicas con Código](#mejoras-específicas-con-código)
4. [Mockups ASCII](#mockups-ascii)

---

## 🎯 Diseño Actual vs. Propuesto

### Estado Actual
- Fondo negro plano
- Texto blanco simple
- Sin separación visual entre secciones
- Hora pequeña en la esquina

### Propuesta de Mejora
- **Bordes decorativos** alrededor de secciones importantes
- **Hora grande y prominente** (ocupa más espacio)
- **Paneles con bordes** para agrupar información relacionada
- **Colores peronistas** (celeste/blanco, azul/amarillo)
- **Iconos más grandes** y separadores visuales

---

## 🎨 Paleta de Colores Peronista

```cpp
// === COLORES PERONISTAS ===
// Basados en la bandera y símbolos del peronismo

// Celeste (parte superior bandera)
#define COLOR_CELESTE      0x06BF  // Celeste claro
#define COLOR_CELESTE_OSC  0x0458  // Celeste oscuro

// Blanco/Crema
#define COLOR_BLANCO       TFT_WHITE
#define COLOR_CREMA        0xFFDE  // Crema/beige suave

// Amarillo/Dorado (sol)
#define COLOR_AMARILLO     TFT_YELLOW
#define COLOR_DORADO       0xFD20  // Dorado/naranja

// Azul (parte inferior bandera)
#define COLOR_AZUL         0x001F  // Azul oscuro
#define COLOR_AZUL_CLARO   0x051D  // Azul medio

// Acentos
#define COLOR_ROJO_PERON   0xF800  // Rojo para alarmas/alertas
#define COLOR_VERDE_OK     0x07E0  // Verde para estado OK
```

---

## 💡 Mejoras Específicas con Código

### 1. Hora Grande con Marco Decorativo

**Antes:**
```cpp
tft.setTextSize(3);
tft.setCursor(20, 10);
tft.print(timeStr);  // Hora pequeña arriba
```

**Después - Opción A: Marco Simple**
```cpp
// === HORA CON MARCO DECORATIVO ===
void drawTimeWithBorder(const char* timeStr) {
  // Marco doble para la hora
  tft.drawRoundRect(10, 10, 220, 90, 8, COLOR_CELESTE);      // Borde externo
  tft.drawRoundRect(12, 12, 216, 86, 6, COLOR_AMARILLO);     // Borde interno
  
  // Fondo del panel
  tft.fillRoundRect(14, 14, 212, 82, 6, TFT_BLACK);
  
  // Hora en grande
  tft.setTextFont(7);  // Fuente 7 = 48px altura
  tft.setTextColor(COLOR_AMARILLO, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);  // Middle Center
  tft.drawString(timeStr, 120, 55);  // Centrada
}
```

**Después - Opción B: Marco con Gradiente (simulado)**
```cpp
void drawTimeWithGradientBorder(const char* timeStr) {
  // Simular gradiente con líneas de colores variados
  for (int i = 0; i < 5; i++) {
    uint16_t color = (i % 2) ? COLOR_CELESTE : COLOR_CELESTE_OSC;
    tft.drawRoundRect(10 + i, 10 + i, 220 - i*2, 90 - i*2, 8, color);
  }
  
  // Fondo
  tft.fillRoundRect(15, 15, 210, 80, 6, TFT_BLACK);
  
  // Hora gigante
  tft.setTextFont(8);  // Fuente 8 = 75px altura (la más grande)
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(timeStr, 120, 55);
  
  // Pequeño detalle: puntos parpadeantes
  static bool colonBlink = false;
  colonBlink = !colonBlink;
  if (colonBlink) {
    tft.fillCircle(120, 40, 3, COLOR_AMARILLO);
    tft.fillCircle(120, 65, 3, COLOR_AMARILLO);
  }
}
```

---

### 2. Panel de Sensores con Borde y Título

**Antes:**
```cpp
tft.setCursor(10, 150);
tft.printf("Temp: %.1fC", temperature);
tft.setCursor(10, 170);
tft.printf("Hum: %.1f%%", humidity);
```

**Después:**
```cpp
// === PANEL DE SENSORES AGRUPADO ===
void drawSensorsPanel(float temp, float hum, float pres) {
  int panelX = 10;
  int panelY = 110;
  int panelW = 220;
  int panelH = 100;
  
  // Borde del panel (colores peronistas)
  tft.drawRoundRect(panelX, panelY, panelW, panelH, 6, COLOR_CELESTE);
  tft.drawRoundRect(panelX+1, panelY+1, panelW-2, panelH-2, 5, COLOR_CELESTE_OSC);
  
  // Título del panel
  tft.fillRect(panelX+10, panelY-8, 60, 16, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_AMARILLO, TFT_BLACK);
  tft.setCursor(panelX+15, panelY-6);
  tft.print(" SENSORES ");
  
  // Contenido del panel
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  // Temperatura con ícono
  tft.setCursor(panelX+15, panelY+20);
  tft.print("T:");
  tft.setTextColor(COLOR_AMARILLO, TFT_BLACK);
  tft.printf(" %.1fC", temp);
  
  // Humedad
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(panelX+15, panelY+45);
  tft.print("H:");
  tft.setTextColor(COLOR_CELESTE, TFT_BLACK);
  tft.printf(" %.1f%%", hum);
  
  // Presión
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(panelX+15, panelY+70);
  tft.print("P:");
  tft.setTextColor(COLOR_VERDE_OK, TFT_BLACK);
  tft.printf(" %.0fhPa", pres);
}
```

---

### 3. Indicador de Alarma Mejorado

**Antes:**
```cpp
if (alarmEnabled) {
  tft.print("⏰");
  tft.printf("%02d:%02d", alarmHour, alarmMinute);
}
```

**Después:**
```cpp
// === INDICADOR DE ALARMA CON ESTILO ===
void drawAlarmIndicator(bool enabled, int hour, int minute) {
  int x = 10;
  int y = 220;
  
  if (!enabled) {
    // Mostrar OFF en gris
    tft.setTextSize(1);
    tft.setTextColor(0x7BEF, TFT_BLACK);  // Gris
    tft.setCursor(x, y);
    tft.print("Alarma: OFF");
    return;
  }
  
  // Marco para alarma activa
  tft.drawRoundRect(x, y, 100, 35, 4, COLOR_ROJO_PERON);
  tft.fillRoundRect(x+2, y+2, 96, 31, 3, TFT_BLACK);
  
  // Ícono de campana (ASCII art simple)
  tft.setTextSize(2);
  tft.setTextColor(COLOR_AMARILLO, TFT_BLACK);
  tft.setCursor(x+8, y+8);
  tft.print("!");  // Simula campana
  
  // Hora de alarma
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(x+30, y+8);
  tft.printf("%02d:%02d", hour, minute);
  
  // Pequeño punto parpadeante para indicar "activa"
  static unsigned long lastBlink = 0;
  static bool blinkState = false;
  if (millis() - lastBlink > 500) {
    blinkState = !blinkState;
    lastBlink = millis();
  }
  if (blinkState) {
    tft.fillCircle(x+95, y+5, 3, COLOR_ROJO_PERON);
  }
}
```

---

### 4. Separadores Visuales

**Código para separar secciones:**
```cpp
// === LÍNEA SEPARADORA DECORATIVA ===
void drawSeparatorLine(int y, uint16_t color1, uint16_t color2) {
  // Línea doble con colores peronistas
  tft.drawFastHLine(20, y, 200, color1);
  tft.drawFastHLine(20, y+2, 200, color2);
  
  // Pequeños círculos en los extremos
  tft.fillCircle(20, y+1, 2, color1);
  tft.fillCircle(220, y+1, 2, color2);
}

// Uso:
drawSeparatorLine(105, COLOR_CELESTE, COLOR_AMARILLO);  // Entre hora y sensores
drawSeparatorLine(215, COLOR_AMARILLO, COLOR_CELESTE);  // Entre sensores y clima
```

---

### 5. Panel de Clima Mejorado

```cpp
// === PANEL DE CLIMA CON ÍCONO Y MARCO ===
void drawWeatherPanel(const char* desc, float temp, const char* icon) {
  int x = 10;
  int y = 260;
  
  // Marco
  tft.drawRect(x, y, 220, 50, COLOR_AZUL_CLARO);
  tft.drawRect(x+1, y+1, 218, 48, COLOR_CELESTE);
  
  // Ícono del clima (ASCII simplificado)
  tft.setTextSize(3);
  tft.setTextColor(COLOR_AMARILLO, TFT_BLACK);
  tft.setCursor(x+10, y+10);
  
  // Mapeo básico de íconos OpenWeather
  if (strstr(icon, "01")) tft.print("O");      // Sol
  else if (strstr(icon, "02")) tft.print("o");  // Parcial
  else if (strstr(icon, "03")) tft.print("=");  // Nublado
  else if (strstr(icon, "04")) tft.print("#");  // Muy nublado
  else if (strstr(icon, "09")) tft.print("'");  // Lluvia
  else if (strstr(icon, "10")) tft.print("/'"); // Lluvia+sol
  else if (strstr(icon, "11")) tft.print("!");  // Tormenta
  else if (strstr(icon, "13")) tft.print("*");  // Nieve
  else tft.print("?");
  
  // Temperatura
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(x+60, y+12);
  tft.printf("%.0fC", temp);
  
  // Descripción
  tft.setTextSize(1);
  tft.setTextColor(COLOR_CELESTE, TFT_BLACK);
  tft.setCursor(x+60, y+32);
  tft.print(desc);
}
```

---

## 📐 Mockup ASCII de Pantalla Completa

### Propuesta 1: Layout Clásico con Bordes

```
╔════════════════════════╗
║  ╔══════════════════╗  ║
║  ║   13:45          ║  ║  ← Hora GRANDE con marco doble
║  ║                  ║  ║     (celeste/amarillo)
║  ╚══════════════════╝  ║
║ ========================║  ← Separador decorativo
║  ┌─ SENSORES ────────┐ ║
║  │ T: 24.5°C        │ ║  ← Panel agrupado
║  │ H: 65.2%         │ ║     con título
║  │ P: 1013 hPa      │ ║
║  └──────────────────┘  ║
║ ========================║  ← Separador
║  ┌─ CLIMA ───────────┐ ║
║  │ O 26°C Despejado │ ║  ← Pronóstico con ícono
║  └──────────────────┘  ║
║ ========================║
║  [!] ⏰ 07:30          ║  ← Alarma con marco rojo
╚════════════════════════╝
```

### Propuesta 2: Layout Moderno con Tarjetas

```
╔════════════════════════╗
║                        ║
║      ┌──────────┐      ║
║      │  13:45   │      ║  ← Hora centrada, super grande
║      └──────────┘      ║     fuente 8 (75px)
║                        ║
║  ┌────────┐ ┌────────┐║
║  │ T 24°C │ │ H 65%  │║  ← Cards pequeñas lado a lado
║  └────────┘ └────────┘║
║  ┌────────┐ ┌────────┐║
║  │P 1013  │ │O 26°C  │║
║  └────────┘ └────────┘║
║                        ║
║  ════════════════════  ║
║                        ║
║    [!] ⏰ 07:30  [ON]  ║  ← Alarma destacada abajo
╚════════════════════════╝
```

### Propuesta 3: Layout Tipo Dashboard

```
╔════════════════════════╗
║ ┌──────────────────┐   ║
║ │   13  :  45      │   ║  ← Hora ocupa 1/3 superior
║ └──────────────────┘   ║
║ ────────────────────   ║
║ AMBIENTE          CLIMA║
║ ┌─────────┐  ┌────────┐║
║ │24.5°C   │  │ O 26°C │║  ← Split 50/50
║ │65% Hum  │  │Despej. │║
║ │1013 hPa │  │        │║
║ └─────────┘  └────────┘║
║ ────────────────────   ║
║ ⏰ 07:30      [ACTIVA] ║  ← Footer con estado
╚════════════════════════╝
```

---

## 🔧 Función Completa Integrada

Aquí está una función completa que integra todas las mejoras:

```cpp
void displayAllInfoMejorado() {
  tft.fillScreen(TFT_BLACK);
  
  // 1. HORA GRANDE CON MARCO (ocupa arriba)
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    drawTimeWithBorder(timeStr);  // Función definida arriba
  }
  
  // 2. SEPARADOR DECORATIVO
  drawSeparatorLine(105, COLOR_CELESTE, COLOR_AMARILLO);
  
  // 3. PANEL DE SENSORES
  if (sensorsAvailable) {
    drawSensorsPanel(temperature, humidity, pressure);
  }
  
  // 4. SEPARADOR
  drawSeparatorLine(215, COLOR_AMARILLO, COLOR_CELESTE);
  
  // 5. PANEL DE CLIMA
  if (weatherDataAvailable && numForecasts > 0) {
    drawWeatherPanel(forecasts[0].description, 
                     forecasts[0].temp, 
                     forecasts[0].icon);
  }
  
  // 6. INDICADOR DE ALARMA (abajo)
  drawAlarmIndicator(alarmEnabled, alarmHour, alarmMinute);
}
```

---

## 🎯 Resumen de Mejoras

| Aspecto | Antes | Después |
|---------|-------|---------|
| **Hora** | Pequeña, esquina | Grande, centrada, con marco |
| **Sensores** | Texto suelto | Panel agrupado con título |
| **Colores** | Solo blanco | Paleta peronista (celeste/amarillo) |
| **Separación** | Ninguna | Líneas decorativas entre secciones |
| **Alarma** | Simple texto | Marco rojo con indicador LED |
| **Legibilidad** | Media | Alta (mayor contraste y tamaño) |
| **Estética** | Básica | Profesional y temática |

---

## 🚀 Próximos Pasos

1. **Implementar función por función** - No hace falta cambiar todo de golpe
2. **Testear en hardware real** - Verificar que los colores se vean bien
3. **Ajustar tamaños** - La pantalla es 240x320, hay que optimizar el espacio
4. **Agregar fuentes custom** - Considerar usar fuentes TTF personalizadas
5. **Animaciones sutiles** - Transiciones suaves al cambiar de pantalla

---

**Fecha:** 27/05/2026  
**Autor:** Propuesta generada automáticamente  
**Estado:** Pendiente de implementación
