# 📋 Plan de Adaptación: OLED SSD1306 → TFT ILI9341

## 🎯 Objetivo
Adaptar el Reloj Peronista de pantalla OLED SSD1306 (128x32, I2C, monocromática) a pantalla TFT ILI9341 (240x320, SPI, color).

---

## 📊 Comparación de Pantallas

| Característica | OLED SSD1306 (Actual) | TFT ILI9341 (Nueva) |
|----------------|----------------------|---------------------|
| **Resolución** | 128x32 píxeles | 240x320 píxeles |
| **Protocolo** | I2C (2 pines) | SPI (5-7 pines) |
| **Color** | Monocromático (blanco) | RGB565 (16 bits, 65K colores) |
| **Tamaño** | 0.91" | 2.2" |
| **Librería** | Adafruit_SSD1306 | Adafruit_ILI9341 o TFT_eSPI |
| **Pines usados** | GPIO 21, 22 | GPIO 5, 18, 19, 23, + CS y DC |

---

## 🔧 PASO 1: Cambios en Hardware

### 1.1. Conexiones SPI necesarias para ILI9341

La pantalla ILI9341 requiere conexión SPI (más pines que I2C):

| **Pin TFT** | **Pin ESP32** | **Función** |
|-------------|---------------|-------------|
| **VCC** | 3.3V o 5V* | Alimentación |
| **GND** | GND | Tierra |
| **CS** | GPIO 5 | Chip Select |
| **RESET** | GPIO 4** o 3.3V | Reset (puede conectarse a 3.3V) |
| **DC** | GPIO 2 | Data/Command |
| **MOSI** | GPIO 23 | Data Out (SPI) |
| **SCK** | GPIO 18 | Clock (SPI) |
| **LED** | 3.3V | Backlight |
| **MISO** | GPIO 19 | Data In (SPI, opcional para SD) |

**Notas:**
- *Verifica el voltaje de tu módulo (algunos son 5V, otros 3.3V)
- **GPIO 4 actualmente usado para botón de alarma, hay que reasignarlo

### 1.2. Conflictos de Pines a Resolver

**Problema:** GPIO 4 está ocupado por el botón de alarma, pero lo necesitamos para RESET de la pantalla.

**Soluciones posibles:**

**Opción A (Recomendada):** Conectar RESET de la pantalla directo a 3.3V
- Ventaja: No ocupa GPIO
- Desventaja: No puedes resetear la pantalla por software (rara vez necesario)

**Opción B:** Reasignar botón de alarma a otro GPIO
- Cambiar botón alarma de GPIO 4 → GPIO 16 (o GPIO 17, 25, 26, 27)
- Mantener RESET en GPIO 4

### 1.3. Configuración Final de Pines Propuesta

```
ESP32 DevKit V1 - Configuración Final
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

PANTALLA TFT ILI9341 (SPI):
├─ CS    → GPIO 5
├─ RESET → 3.3V (sin GPIO, opción A)
├─ DC    → GPIO 2
├─ MOSI  → GPIO 23 (VSPI MOSI por defecto)
├─ SCK   → GPIO 18 (VSPI SCK por defecto)
├─ LED   → 3.3V
└─ MISO  → GPIO 19 (si usas SD)

SENSORES I2C (sin cambios):
├─ SCL → GPIO 22
└─ SDA → GPIO 21

BOTONES:
├─ Botón Modo   → GPIO 15 (sin cambio)
└─ Botón Alarma → GPIO 4 o GPIO 16 (según opción elegida)
```

---

## 💻 PASO 2: Cambios en Software

### 2.1. Modificar `platformio.ini`

**Cambio 1:** Actualizar librerías

```ini
lib_deps = 
    # Remover estas líneas:
    # adafruit/Adafruit SSD1306@^2.5.7
    
    # Mantener estas:
    adafruit/Adafruit GFX Library@^1.11.3
    adafruit/Adafruit BusIO@^1.14.1
    adafruit/Adafruit AHTX0@^2.0.3
    adafruit/Adafruit BMP085 Library@^1.2.2
    bblanchon/ArduinoJson@^7.0.4
    
    # Agregar nueva librería TFT:
    adafruit/Adafruit ILI9341@^1.6.0
```

**Alternativa (más optimizada):** Usar TFT_eSPI (más rápida)
```ini
lib_deps = 
    bodmer/TFT_eSPI@^2.5.34
    adafruit/Adafruit GFX Library@^1.11.3
    # ... resto igual
```

### 2.2. Modificar `src/config.h`

```cpp
// ========== CONFIGURACIÓN PANTALLA TFT ILI9341 ==========
#define SCREEN_WIDTH 240    // Cambiar de 128 a 240
#define SCREEN_HEIGHT 320   // Cambiar de 32 a 320
// Eliminar: #define OLED_RESET -1
// Eliminar: #define SCREEN_ADDRESS 0x3C

// Pines SPI para ILI9341
#define TFT_CS    5
#define TFT_DC    2
// #define TFT_RST   -1  // Conectado a 3.3V

// ========== BRILLO PANTALLA ==========
#define BRIGHTNESS_DAY 255      // Ya existe
#define BRIGHTNESS_NIGHT 30     // Ya existe

// ========== CONFIGURACIÓN BOTONES (si cambias) ==========
// Si eliges opción B:
// Cambiar de GPIO 4 a GPIO 16 para botón alarma
```

### 2.3. Modificar `src/main.cpp`

**Cambios principales:**

#### A. Cambiar includes y declaraciones (líneas 1-23)

```cpp
// Remover:
// #include <Adafruit_SSD1306.h>
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Agregar:
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Inicializar TFT
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// O con RST: Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
```

#### B. Adaptar todas las funciones de dibujo

**Cambios de API necesarios:**

| OLED (Viejo) | TFT (Nuevo) |
|--------------|-------------|
| `display.clearDisplay()` | `tft.fillScreen(COLOR_FONDO)` |
| `display.display()` | `// No necesario, dibujo directo` |
| `display.setTextSize(3)` | `tft.setTextSize(5)` (ajustar proporcional) |
| `display.setTextColor(SSD1306_WHITE)` | `tft.setTextColor(ILI9341_CYAN, ILI9341_BLUE)` |
| `display.setCursor(x, y)` | `tft.setCursor(x, y)` (igual) |
| `display.print(texto)` | `tft.print(texto)` (igual) |
| `display.fillRect(...)` | `tft.fillRect(...)` (igual) |

**Colores disponibles en ILI9341:**
```cpp
ILI9341_BLACK, ILI9341_NAVY, ILI9341_DARKGREEN, ILI9341_DARKCYAN,
ILI9341_MAROON, ILI9341_PURPLE, ILI9341_OLIVE, ILI9341_LIGHTGREY,
ILI9341_DARKGREY, ILI9341_BLUE, ILI9341_GREEN, ILI9341_CYAN,
ILI9341_RED, ILI9341_MAGENTA, ILI9341_YELLOW, ILI9341_WHITE,
ILI9341_ORANGE, ILI9341_GREENYELLOW, ILI9341_PINK
```

**Colores peronistas sugeridos:**
```cpp
#define COLOR_PERONISTA_CELESTE 0x5CDF  // Celeste
#define COLOR_PERONISTA_BLANCO  ILI9341_WHITE
#define COLOR_PERONISTA_NEGRO   ILI9341_BLACK
#define COLOR_FONDO_AZUL        ILI9341_NAVY
```

#### C. Ajustar tamaños y posiciones

Con resolución 240x320 (vs 128x32), hay ~7.5x más ancho y ~10x más alto:

**Ejemplo para `displayTime()`:**

```cpp
// ANTES (128x32):
display.setTextSize(3);  // Tamaño 3
display.setCursor(8, 8); // Posición

// DESPUÉS (240x320):
tft.setTextSize(7);      // Tamaño más grande
tft.setCursor(20, 120);  // Centrado vertical aprox
```

**Fórmula aproximada:**
- Posición X: `nuevo_x = viejo_x * (240/128) ≈ viejo_x * 1.9`
- Posición Y: `nuevo_y = viejo_y * (320/32) = viejo_y * 10`
- Tamaño texto: `nuevo_size = viejo_size * 2` (o ajustar a gusto)

---

## 📝 PASO 3: Secuencia de Implementación

### Fase 1: Preparación (1 hora)
1. ☐ Desconectar todo del ESP32
2. ☐ Identificar pines de la TFT ILI9341
3. ☐ Decidir configuración de pines (Opción A o B)
4. ☐ Conectar TFT según tabla de pines
5. ☐ Reubicar botones si es necesario

### Fase 2: Configuración Básica (30 min)
6. ☐ Modificar `platformio.ini` (agregar librería ILI9341)
7. ☐ Compilar para verificar que descarga la librería
8. ☐ Modificar `config.h` (resolución y pines)
9. ☐ Crear archivo de respaldo de `main.cpp` original

### Fase 3: Adaptación del Código (2-3 horas)
10. ☐ Cambiar includes en `main.cpp`
11. ☐ Cambiar inicialización de display
12. ☐ Adaptar función `setup()` (inicialización TFT)
13. ☐ Adaptar función `displayTime()` (reloj)
14. ☐ Adaptar función `displayDate()` (fecha)
15. ☐ Adaptar función `displayEfemeride()` (efemérides)
16. ☐ Adaptar funciones de sensores (temperatura, humedad, presión)
17. ☐ Adaptar función `displayAlarm()` (alarma)
18. ☐ Adaptar función `displayAlarmConfig()` (configuración)
19. ☐ Adaptar función `displayForecast()` (pronóstico)
20. ☐ Adaptar íconos del clima (escalarlos)

### Fase 4: Pruebas (1 hora)
21. ☐ Compilar y verificar que no hay errores
22. ☐ Subir código al ESP32
23. ☐ Verificar pantalla de inicio
24. ☐ Probar cada modo (AUTO, HORA, FECHA, EFEMÉRIDES, SENSORES, PRONÓSTICO)
25. ☐ Probar botones (cambio modo, configuración alarma)
26. ☐ Verificar alarma

### Fase 5: Optimización Visual (1-2 horas)
27. ☐ Ajustar tamaños de texto para mejor legibilidad
28. ☐ Ajustar posiciones para centrado óptimo
29. ☐ Implementar colores peronistas (celeste, blanco)
30. ☐ Mejorar íconos del clima (más grandes, más detallados)
31. ☐ Agregar fondo de color o imágenes (opcional)

---

## 🎨 PASO 4: Mejoras Visuales Sugeridas (Futuro)

Con la pantalla más grande y a color, podrías:

1. **Diseño más vistoso:**
   - Fondo celeste y blanco (colores peronistas)
   - Reloj digital más grande y elegante
   - Agregar símbolos o escudos peronistas

2. **Más información simultánea:**
   - Mostrar hora + fecha + temperatura al mismo tiempo
   - Panel de clima permanente en una esquina
   - Barra de estado con WiFi/sensores

3. **Animaciones:**
   - Transiciones suaves entre modos
   - Reloj analógico (opcional)
   - Gráficos de tendencia de temperatura

4. **Imágenes:**
   - Foto de Perón o Evita en pantalla de inicio
   - Escudo peronista permanente
   - Mapas de clima (si API lo permite)

---

## ⚠️ CONSIDERACIONES IMPORTANTES

### Memoria
- La ILI9341 usa más RAM que OLED (buffer más grande)
- Si tienes problemas de memoria, considera usar sprites o dibujo parcial

### Velocidad
- SPI es más rápido que I2C para transferencia de datos
- TFT_eSPI es más rápida que Adafruit_ILI9341 (considera cambiar)

### Consumo
- TFT consume más energía que OLED (especialmente el backlight)
- Considera implementar apagado automático en modo nocturno

### Orientación
- Puedes rotar la pantalla: `tft.setRotation(0)` (0, 1, 2, 3)
- 0 = portrait, 1 = landscape, etc.

---

## 📚 Recursos Útiles

### Documentación:
- [Adafruit ILI9341 Guide](https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ILI9341 Datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)

### Ejemplos de código:
- Adafruit GFX Graphics Test
- TFT_eSPI Examples (incluidos en la librería)

---

## ✅ Resumen de Cambios

| Aspecto | Cambio |
|---------|--------|
| **Hardware** | I2C (2 pines) → SPI (6-7 pines) |
| **Librería** | Adafruit_SSD1306 → Adafruit_ILI9341 |
| **Resolución** | 128x32 → 240x320 (7.5x más espacio) |
| **Colores** | Monocromático → 65K colores |
| **API Drawing** | Requiere `display()` → Dibujo directo |
| **Pines GPIO** | Libera GPIO 21, 22 (I2C) - Usa GPIO 2, 5, 18, 19, 23 (SPI) |

---

## 🚀 Próximos Pasos Recomendados

1. **Lee este documento completo**
2. **Decide qué opción de pines usar** (A o B)
3. **Crea respaldo del código actual**
4. **Empieza por Fase 1** (hardware)
5. **Avanza secuencialmente** por cada fase

---

**Tiempo estimado total:** 5-7 horas (adaptación completa)
**Dificultad:** Media (requiere ajustes en múltiples funciones)

¿Estás listo para empezar? Dime en qué paso necesitas ayuda específica.
