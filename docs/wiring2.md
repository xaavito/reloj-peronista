# 🔌 Wiring Completo - Reloj Peronista con Alarma Configurable

## 📋 Resumen de Conexiones

Este documento describe el cableado completo del Reloj Peronista incluyendo:
- ✅ Pantalla ST7789 240x240 IPS Color
- ✅ Sensores de temperatura y humedad (AHT10, BMP180)
- ✅ Botón de cambio de modo
- ✅ **Botón de configuración de alarma** ⏰

---

## 🖥️ Pantalla ST7789 240x240 (7 pines)

| **Pin Pantalla** | **Pin ESP32** | **Cable Sugerido** | **Descripción** |
|------------------|---------------|-------------------|-----------------|
| **GND**          | **GND**       | Negro             | Tierra común |
| **VCC**          | **3.3V**      | Rojo              | Alimentación 3.3V |
| **SCL**          | **GPIO 18**   | Amarillo          | Clock SPI |
| **SDA**          | **GPIO 23**   | Azul              | Data SPI |
| **RES**          | **GPIO 17**   | Naranja           | Reset |
| **DC**           | **GPIO 16**   | Morado            | Data/Command |
| **BLK**          | **3.3V**      | Blanco            | Backlight (opcional) |

**NOTA**: Módulo sin pin CS (soldado internamente)

---

## 🌡️ Sensor AHT10 (Temperatura y Humedad)

| **Pin AHT10** | **Pin ESP32** | **Cable** | **Descripción** |
|---------------|---------------|-----------|-----------------|
| **VCC**       | **3.3V**      | Rojo      | Alimentación |
| **GND**       | **GND**       | Negro     | Tierra |
| **SCL**       | **GPIO 22**   | Amarillo  | I2C Clock |
| **SDA**       | **GPIO 21**   | Verde     | I2C Data |

---

## 🌡️ Sensor BMP180 (Presión y Temperatura)

| **Pin BMP180** | **Pin ESP32** | **Cable** | **Descripción** |
|----------------|---------------|-----------|-----------------|
| **VCC**        | **3.3V**      | Rojo      | Alimentación |
| **GND**        | **GND**       | Negro     | Tierra |
| **SCL**        | **GPIO 22**   | Amarillo  | I2C Clock (compartido) |
| **SDA**        | **GPIO 21**   | Verde     | I2C Data (compartido) |

**NOTA**: AHT10 y BMP180 comparten el mismo bus I2C

---

## 🔘 Botón de Modo (Display)

| **Botón 4 Pines** | **Pin ESP32** | **Cable** | **Descripción** |
|-------------------|---------------|-----------|-----------------|
| Pin 1 o 2         | **GPIO 27**   | Cualquier | Señal del botón |
| Pin 3 o 4         | **GND**       | Negro     | Tierra |

**Funcionalidad:**
- Presión corta: Cambia entre modos de visualización
- Modos: AUTO → HORA → FECHA → EFEMÉRIDES → TEMP → HUMEDAD → CLIMA → PRONÓSTICO

---

## ⏰ Botón de Alarma

| **Botón 4 Pines** | **Pin ESP32** | **Cable** | **Descripción** |
|-------------------|---------------|-----------|-----------------|
| Pin 1 o 2         | **GPIO 4**    | Cualquier | Señal del botón |
| Pin 3 o 4         | **GND**       | Negro     | Tierra |

---

## 🔊 Buzzer Activo 5V (NUEVO)

| **Pin Buzzer** | **Pin ESP32** | **Cable** | **Descripción** |
|----------------|---------------|-----------|-----------------|
| **+** (VCC)    | **GPIO 25**   | Rojo      | Señal control |
| **-** (GND)    | **GND**       | Negro     | Tierra |

**NOTA**: Es un buzzer ACTIVO de 5V. Se controla con señal digital (HIGH/LOW).
El ESP32 da 3.3V en GPIO pero es suficiente para activar el buzzer.

### 🎮 Uso del Botón de Alarma:

**Presión LARGA (2 segundos):**
- Entra/Sale del modo de configuración de alarma

**Presión CORTA (mientras está en modo config):**
- Incrementa el valor del campo seleccionado
  - **Campo HORA**: Incrementa hora (0-23)
  - **Campo MINUTO**: Incrementa minuto (0-59)
  - **Campo ON/OFF**: Alterna alarma activada/desactivada
  - **Campo GUARDAR**: Guarda y sale

**Presión LARGA en modo config:**
- Avanza al siguiente campo

### 📺 Pantalla de Configuración:

```
┌─────────────────────────┐
│   CONFIG ALARMA         │
│                         │
│   H:[07] :[30]          │
│     ▲         ▲         │
│   Hora     Minuto       │
│                         │
│       [ON]              │
│        ▲                │
│    Activado             │
│                         │
│   [GUARDAR]             │
│       ▲                 │
│   Presión corta=SI      │
└─────────────────────────┘
```

### 🔔 Cuando Suena la Alarma:

- Pantalla parpadea en ROJO
- Texto grande "ALARMA"
- Duración: 1 minuto (configurable en config.h)
- **Para detener**: Presión corta en botón de modo (GPIO 27)

---

## 🔌 Diagrama Esquemático Completo

```
                     ESP32 DOIT DevKit V1
                    ┌─────────────────┐
                    │                 │
         3.3V ◄─────┤ 3.3V            │
          GND ◄─────┤ GND        GPIO23├────► SDA (Pantalla)
                    │            GPIO18├────► SCL (Pantalla)
                    │            GPIO17├────► RES (Pantalla)
                    │            GPIO16├────► DC (Pantalla)
                    │            GPIO22├────► SCL (I2C Sensores)
                     │            GPIO21├────► SDA (I2C Sensores)
                     │            GPIO25├────► Buzzer (+)
                     │            GPIO27├────► Botón Modo
  Botón Alarma ◄─────┤ GPIO 4          │
                    │                 │
                    └─────────────────┘

Pantalla ILI9341:      Sensores I2C:        Botones:
GND ← GND              AHT10 + BMP180        ┌───┐
VCC ← 3.3V             VCC ← 3.3V            │ ⚬ │← GPIO 27 (Modo)
SCL ← GPIO 18          GND ← GND             │   │
SDA ← GPIO 23          SCL ← GPIO 22         │ ⚬ │← GND
RES ← GPIO 17          SDA ← GPIO 21         └───┘
DC  ← GPIO 16
BLK ← 3.3V                                   ┌───┐
                                             │ ⚬ │← GPIO 4 (Alarma)
                      Buzzer Activo:         │   │
                      + ← GPIO 25            │ ⚬ │← GND
                      - ← GND                └───┘
```

---

## ⚙️ Configuración en config.h

```cpp
// ========== CONFIGURACIÓN ALARMA ==========
#define ALARM_ENABLED true          // Activar/desactivar alarma
#define ALARM_HOUR 7                // Hora inicial (0-23)
#define ALARM_MINUTE 30             // Minuto inicial (0-59)
#define ALARM_DURATION 60000        // Duración en ms (1 minuto)
#define ALARM_BLINK_INTERVAL 500    // Velocidad parpadeo (ms)
```

---

## 🎯 Funcionalidades de la Alarma

### ✅ Configuración:
- ✅ Hora configurable (0-23)
- ✅ Minuto configurable (0-59)
- ✅ ON/OFF por alarma
- ✅ Guardado permanente (mientras esté encendido)
- ✅ Interfaz visual clara

### ✅ Activación:
- ✅ Se activa automáticamente a la hora configurada
- ✅ Pantalla parpadea en rojo
- ✅ Duración configurable
- ✅ Se puede detener con botón de modo

### ✅ Indicador:
- ✅ Se muestra en monitor serial si está activa
- ✅ Formato: `Alarma: 07:30`

---

## 🔧 Lista de Verificación Completa

### Pantalla ST7789:
- [ ] VCC → 3.3V (NO 5V)
- [ ] GND → GND
- [ ] SCL → GPIO 18
- [ ] SDA → GPIO 23
- [ ] RES → GPIO 17
- [ ] DC → GPIO 16
- [ ] BLK → 3.3V (opcional)

### Sensores I2C:
- [ ] AHT10 VCC → 3.3V
- [ ] AHT10 GND → GND
- [ ] AHT10 SCL → GPIO 22
- [ ] AHT10 SDA → GPIO 21
- [ ] BMP180 VCC → 3.3V
- [ ] BMP180 GND → GND
- [ ] BMP180 SCL → GPIO 22 (compartido)
- [ ] BMP180 SDA → GPIO 21 (compartido)

### Botones:
- [ ] Botón Modo: 1 pin → GPIO 27, otro → GND
- [ ] Botón Alarma: 1 pin → GPIO 4, otro → GND

### Buzzer:
- [ ] Buzzer + → GPIO 25
- [ ] Buzzer - → GND

### Software:
- [ ] config.h editado con WiFi y API key
- [ ] Código compilado sin errores
- [ ] ESP32 conectado a USB

---

## 📝 Notas Importantes

### ⚠️ Voltaje:
- **SIEMPRE usar 3.3V** para pantalla y sensores
- **NUNCA usar 5V** en pantalla ST7789

### 🔌 Buses:
- **SPI (Pantalla)**: GPIO 18, 23, 17, 16
- **I2C (Sensores)**: GPIO 22, 21
- **GPIO Botones**: GPIO 27 (Modo), GPIO 4 (Alarma)
- **GPIO Buzzer**: GPIO 25

### 🎛️ Resistencias Pull-up:
- **NO necesarias** en botones (ESP32 tiene internas)
- **NO necesarias** en I2C (sensores las tienen)

### 🔋 Alimentación:
- ESP32 consume ~200mA
- Pantalla consume ~50-100mA
- Sensores consumen ~5mA
- **Total**: ~300mA (puerto USB suficiente)

---

## 🚀 Primeros Pasos

1. **Conecta todo** según las tablas
2. **Verifica** voltajes con multímetro
3. **Edita** config.h con WiFi y API
4. **Compila y carga** con PlatformIO
5. **Prueba** botón de modo
6. **Configura** alarma con botón GPIO 4

---

## 🎨 Ejemplo de Uso - Configurar Alarma para las 6:30 AM

1. **Presiona LARGO** botón alarma (GPIO 4) → Entra modo config
2. Pantalla muestra: `H:[07] :[30]` (valores actuales)
3. **Presiona CORTO** repetidas veces → Hora cambia 07→08→...→06
4. **Presiona LARGO** → Avanza a minutos (minuto se marca)
5. **Presiona CORTO** repetidas veces → 30→31→...→29→30 (OK)
6. **Presiona LARGO** → Avanza a ON/OFF
7. Verifica que diga **ON** (si no, presiona corto)
8. **Presiona LARGO** → Avanza a GUARDAR
9. **Presiona CORTO** → ¡Guardado!
10. Pantalla vuelve al modo normal

---

**✅ Con este wiring tu Reloj Peronista tendrá alarma configurable!** ⏰🇦🇷

