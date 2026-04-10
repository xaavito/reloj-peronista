# 🔧 Solución: Pantalla Blanca TFT ILI9341

## Problema
La pantalla TFT se muestra completamente blanca después de cargar el código.

## Causas Comunes y Soluciones

### 1. ❌ Pin RESET incorrecto (MÁS COMÚN)

**Problema:** Si TFT_RST = -1 pero el pin RESET NO está conectado a 3.3V

**Solución A:** Conecta el pin RESET de la TFT a un GPIO (ej: GPIO 4)
```cpp
// En config.h, cambiar:
#define TFT_RST   4   // En lugar de -1
```

**Solución B:** Conecta físicamente el pin RESET a 3.3V con un cable

---

### 2. ❌ Pines CS o DC incorrectos

**Problema:** Los pines definidos no coinciden con las conexiones físicas

**Solución:** Verifica y ajusta en `config.h`:
```cpp
#define TFT_CS    15  // Prueba con 15, 5, o 14
#define TFT_DC    2   // Prueba con 2, 4, o 16
```

**Conexiones recomendadas:**
```
TFT → ESP32
VCC → 3.3V (o 5V según módulo)
GND → GND
CS  → GPIO 15 (configurable)
RST → GPIO 4 (o 3.3V)
DC  → GPIO 2 (configurable)
MOSI→ GPIO 23 (fijo, VSPI)
SCK → GPIO 18 (fijo, VSPI)
LED → 3.3V
MISO→ GPIO 19 (opcional)
```

---

### 3. ❌ Voltaje incorrecto

**Problema:** Algunos módulos TFT son 5V, otros 3.3V

**Solución:** 
- Verifica las especificaciones de tu módulo
- Si es 5V: conecta VCC a VIN (5V)
- Si es 3.3V: conecta VCC a 3.3V

---

### 4. ❌ Backlight no encendido

**Problema:** El pin LED no está alimentado

**Solución:**
- Conecta LED a 3.3V directamente
- O usa un resistor de 100Ω entre LED y 3.3V

---

### 5. ❌ Rotación de pantalla

**Problema:** La pantalla está en orientación incorrecta

**Solución:** En setup(), prueba diferentes rotaciones:
```cpp
tft.setRotation(0);  // Portrait (0, 2)
// o
tft.setRotation(1);  // Landscape (1, 3)
```

---

## 🧪 Código de Prueba

He creado `src/test_tft.cpp` para diagnosticar el problema.

**Para usarlo:**

1. Renombra temporalmente main.cpp:
```bash
cd src
mv main.cpp main.cpp.original
mv test_tft.cpp main.cpp
```

2. Compila y carga:
```bash
pio run -t upload
pio device monitor
```

3. Observa:
   - ¿Cambia de color la pantalla?
   - ¿Ves texto?
   - ¿Qué dice el Serial Monitor?

4. Restaura el archivo original:
```bash
mv main.cpp test_tft.cpp
mv main.cpp.original main.cpp
```

---

## ✅ Configuración Recomendada

Si nada funciona, usa esta configuración en `config.h`:

```cpp
// OPCIÓN 1: Con pin RESET en GPIO
#define TFT_CS    15
#define TFT_DC    2
#define TFT_RST   4    // ← Conecta RESET a GPIO 4

// OPCIÓN 2: Reset a 3.3V (si tienes cable extra)
#define TFT_CS    15
#define TFT_DC    2
#define TFT_RST   -1   // ← Conecta RESET físicamente a 3.3V

// OPCIÓN 3: Pines alternativos
#define TFT_CS    5
#define TFT_DC    4
#define TFT_RST   2
```

---

## 📝 Checklist de Verificación

- [ ] VCC conectado a 3.3V o 5V (según módulo)
- [ ] GND conectado
- [ ] CS conectado al GPIO definido
- [ ] DC conectado al GPIO definido
- [ ] RST conectado al GPIO o 3.3V
- [ ] MOSI a GPIO 23
- [ ] SCK a GPIO 18
- [ ] LED a 3.3V (con o sin resistor)
- [ ] Los cables están bien insertados
- [ ] El módulo TFT funciona (probado en otro proyecto)

---

## 🔍 Debug Serial

Agrega esto en setup() para ver mensajes:

```cpp
Serial.begin(115200);
delay(2000);
Serial.println("Iniciando TFT...");
SPI.begin();
Serial.println("SPI OK");
tft.begin();
Serial.println("TFT OK");
tft.fillScreen(ILI9341_RED);
Serial.println("Pantalla ROJA");
```

---

## 💡 Última Opción: Diferentes Configuraciones de Pines

Prueba estas combinaciones sistemáticamente:

**Combo 1:**
```cpp
#define TFT_CS  15
#define TFT_DC  2
#define TFT_RST 4
```

**Combo 2:**
```cpp
#define TFT_CS  5
#define TFT_DC  4
#define TFT_RST 2
```

**Combo 3:**
```cpp
#define TFT_CS  14
#define TFT_DC  27
#define TFT_RST 26
```

Recuerda: **MOSI (23) y SCK (18) son fijos para VSPI del ESP32**

---

¿Necesitas más ayuda? Dime qué ves en el Serial Monitor al correr el test.
