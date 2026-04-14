# 📌 Conexión de Pines - TFT ILI9341

## ✅ Configuración Actual (Funcionando)

### Conexión Física ESP32 ↔ TFT ILI9341

```
┌─────────────────┐         ┌─────────────────┐
│   TFT ILI9341   │         │   ESP32 DevKit  │
├─────────────────┤         ├─────────────────┤
│                 │         │                 │
│ VCC     ────────┼─────────┼──── 3.3V        │
│ GND     ────────┼─────────┼──── GND         │
│ CS      ────────┼─────────┼──── GPIO 15     │
│ RESET   ────────┼─────────┼──── 3.3V        │
│ DC/RS   ────────┼─────────┼──── GPIO 2      │
│ MOSI    ────────┼─────────┼──── GPIO 23     │
│ SCK     ────────┼─────────┼──── GPIO 18     │
│ LED     ────────┼─────────┼──── 3.3V        │
│ MISO    ────────┼─────────┼──── GPIO 19     │
│                 │         │   (opcional)    │
└─────────────────┘         └─────────────────┘
```

### Tabla de Conexiones

| **Pin TFT** | **→** | **Pin ESP32** | **Función** | **Notas** |
|-------------|-------|---------------|-------------|-----------|
| **VCC** | → | **3.3V** | Alimentación | Algunos módulos aceptan 5V, verifica el tuyo |
| **GND** | → | **GND** | Tierra | Común |
| **CS** | → | **GPIO 15** | Chip Select | Configurable |
| **RESET** | → | **3.3V** | Reset | Conectado directo a 3.3V (no usa GPIO) |
| **DC** | → | **GPIO 2** | Data/Command | Configurable |
| **MOSI** | → | **GPIO 23** | Master Out Slave In | **FIJO** (VSPI) |
| **SCK** | → | **GPIO 18** | Serial Clock | **FIJO** (VSPI) |
| **LED** | → | **3.3V** | Backlight | Siempre encendido |
| **MISO** | → | **GPIO 19** | Master In Slave Out | Opcional (para SD) |

---

## 🔧 Configuración en Software (config.h)

```cpp
// Pines configurables
#define TFT_CS    15   // Chip Select
#define TFT_DC    2    // Data/Command
#define TFT_RST   -1   // Reset (conectado a 3.3V físicamente)

// Pines fijos del ESP32 (VSPI)
// MOSI → GPIO 23 (no se define, automático)
// SCK  → GPIO 18 (no se define, automático)
// MISO → GPIO 19 (no se define, automático)
```

---

## ⚠️ Notas Importantes

### 1. **Pin RESET**
- En la configuración actual: `TFT_RST = -1`
- Significa: el pin RESET de la pantalla está conectado **físicamente a 3.3V**
- **NO** está conectado a ningún GPIO del ESP32
- Esto libera el GPIO 4 para el botón de alarma

### 2. **Pines SPI Fijos**
Los siguientes pines **NO se pueden cambiar** (están definidos por hardware del ESP32 para VSPI):
- MOSI = GPIO 23
- SCK = GPIO 18
- MISO = GPIO 19

### 3. **Pines Configurables**
Estos sí se pueden cambiar si es necesario:
- CS (actualmente GPIO 15)
- DC (actualmente GPIO 2)
- RST (actualmente conectado a 3.3V, pero podría usar un GPIO si se necesita)

### 4. **Backlight (LED)**
- Conectado directo a 3.3V (siempre encendido)
- Si quieres control por software, conecta a un GPIO con PWM

---

## 🔌 Otros Componentes Conectados

### Sensores I2C (sin cambios)
| **Componente** | **Pin** | **ESP32** |
|----------------|---------|-----------|
| AHT20/BMP180 | SDA | GPIO 21 |
| AHT20/BMP180 | SCL | GPIO 22 |

### Botones
| **Botón** | **ESP32** |
|-----------|-----------|
| Botón Modo | GPIO 15 |
| Botón Alarma | GPIO 4 |

---

## 📋 Checklist de Verificación

Antes de conectar, verifica:

- [ ] VCC de la TFT conectado a 3.3V (o 5V según tu módulo)
- [ ] GND conectado
- [ ] CS a GPIO 15
- [ ] RESET a 3.3V (con cable)
- [ ] DC a GPIO 2
- [ ] MOSI a GPIO 23
- [ ] SCK a GPIO 18
- [ ] LED a 3.3V
- [ ] Todos los cables bien insertados
- [ ] No hay cortos circuitos
- [ ] El ESP32 no está conectado al USB mientras conectas

---

## 🎨 Esquema Visual Simplificado

```
ESP32 DEVKIT V1
┌──────────────────────────────┐
│                              │
│  3.3V ───┬─────────── VCC    │──┐
│          │                   │  │
│          └─────────── RESET  │  │  TFT
│  GND  ──────────────── GND   │  │  ILI9341
│  15   ──────────────── CS    │  │
│  2    ──────────────── DC    │  │
│  23   ──────────────── MOSI  │  │
│  18   ──────────────── SCK   │──┘
│  19   ──────────────── MISO (opcional)
│                              │
│  21   ──────────────── SDA (sensores I2C)
│  22   ──────────────── SCL (sensores I2C)
│                              │
│  4    ──────────────── Botón Alarma
│  15   ──────────────── Botón Modo
│                              │
└──────────────────────────────┘
```

---

## ✅ Estado: Configuración Probada y Funcionando

Esta configuración ha sido probada con el código de prueba y **funciona correctamente**.

Fecha de última verificación: 10/04/2026
