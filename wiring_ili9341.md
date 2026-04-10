# 🔌 Diagrama de Conexión - ESP32 y TFT ILI9341

## Conexión SPI entre ESP32 y Pantalla TFT 2.2" 240x320

```
┌─────────────────────────────────────┐
│         ESP32 DOIT DevKit V1        │
│                                     │
│  ┌───────────────────────────────┐  │
│  │                               │  │
│  │         ┌─────────┐           │  │
│  │         │         │           │  │
│  │         │  ESP32  │           │  │
│  │         │         │           │  │
│  │         │  CHIP   │           │  │
│  │         │         │           │  │
│  │         └─────────┘           │  │
│  │                               │  │
│  └───────────────────────────────┘  │
│                                     │
│  3.3V ●─────────────────────────────┼──────● VCC/LED
│                                     │
│   GND ●─────────────────────────────┼──────● GND
│                                     │
│ GPIO23●─────────────────────────────┼──────● MOSI (SDI)
│(VSPI) │                             │
│       │                             │
│ GPIO18●─────────────────────────────┼──────● SCK (CLK)
│(VSPI) │                             │
│       │                             │
│ GPIO19●─────────────────────────────┼──────● MISO (SDO, opcional)
│(VSPI) │                             │
│       │                             │
│ GPIO 5●─────────────────────────────┼──────● CS
│       │                             │
│       │                             │
│ GPIO 2●─────────────────────────────┼──────● DC (A0)
│       │                             │
│       │                             │
│[3.3V]●┐                             │      ● RST (Reset)
│       └─────────────────────────────┼──────┘
└───────┴─────────────────────────────┘
         │                             
         │                             ┌──────────────────────┐
         │                             │  TFT 2.2" 240x320    │
         │                             │    ILI9341 SPI       │
         └─────────────────────────────┤                      │
                                       │  ┌────────────────┐  │
                                       │  │                │  │
                                       │  │                │  │
                                       │  │    DISPLAY     │  │
                                       │  │    A COLOR     │  │
                                       │  │   240 x 320    │  │
                                       │  │                │  │
                                       │  │                │  │
                                       │  └────────────────┘  │
                                       │                      │
                                       │ VCC GND CS RST DC    │
                                       │  ●   ●   ●  ●  ●     │
                                       │ MOSI CLK LED MISO    │
                                       │  ●   ●   ●   ●       │
                                       └──────────────────────┘
```

## Tabla de Conexiones

### Pantalla TFT ILI9341 (Configuración Opción A - Recomendada)

| **Pin TFT** | **Función**     | **Pin ESP32**        | **Descripción**                    |
|-------------|-----------------|----------------------|------------------------------------|
| **VCC**     | Alimentación    | **3.3V** ⚠️         | Voltaje de alimentación (verificar módulo) |
| **GND**     | Tierra          | **GND**              | Tierra común                       |
| **CS**      | Chip Select     | **GPIO 5**           | Selección de dispositivo SPI       |
| **RESET**   | Reset           | **3.3V**             | Reset conectado a 3.3V (siempre activo) |
| **DC/A0**   | Data/Command    | **GPIO 2**           | Selector datos/comandos            |
| **MOSI**    | Master Out      | **GPIO 23** (VSPI)   | Datos del ESP32 a TFT              |
| **SCK**     | Serial Clock    | **GPIO 18** (VSPI)   | Reloj SPI                          |
| **LED**     | Backlight       | **3.3V**             | Luz de fondo (siempre encendida)   |
| **MISO**    | Master In       | **GPIO 19** (VSPI)   | Solo si usas lector SD (opcional)  |

### Pantalla TFT ILI9341 (Configuración Opción B - Con Reset por software)

| **Pin TFT** | **Función**     | **Pin ESP32**        | **Descripción**                    |
|-------------|-----------------|----------------------|------------------------------------|
| **VCC**     | Alimentación    | **3.3V** ⚠️         | Voltaje de alimentación            |
| **GND**     | Tierra          | **GND**              | Tierra común                       |
| **CS**      | Chip Select     | **GPIO 5**           | Selección de dispositivo SPI       |
| **RESET**   | Reset           | **GPIO 4** ⚠️       | Reset controlado por software      |
| **DC/A0**   | Data/Command    | **GPIO 2**           | Selector datos/comandos            |
| **MOSI**    | Master Out      | **GPIO 23** (VSPI)   | Datos del ESP32 a TFT              |
| **SCK**     | Serial Clock    | **GPIO 18** (VSPI)   | Reloj SPI                          |
| **LED**     | Backlight       | **3.3V**             | Luz de fondo (siempre encendida)   |
| **MISO**    | Master In       | **GPIO 19** (VSPI)   | Solo si usas lector SD (opcional)  |

**⚠️ NOTA Opción B:** Si eliges esta opción, debes reubicar el botón de alarma de GPIO 4 a GPIO 16.

### Botones

#### Con Opción A (Reset en 3.3V)
| **Botón**         | **Pin ESP32**    | **Descripción**                    |
|-------------------|------------------|------------------------------------|
| Botón Modo        | **GPIO 15**      | Cambio de modo de visualización    |
| Botón Alarma      | **GPIO 4**       | Configuración de alarma (sin cambios) |

#### Con Opción B (Reset en GPIO 4)
| **Botón**         | **Pin ESP32**    | **Descripción**                    |
|-------------------|------------------|------------------------------------|
| Botón Modo        | **GPIO 15**      | Cambio de modo de visualización    |
| Botón Alarma      | **GPIO 16** ⚠️  | Configuración de alarma (REUBICAR) |

### Sensores I2C (Sin cambios)

| **Sensor/Pin** | **Pin ESP32**    | **Descripción**                    |
|----------------|------------------|------------------------------------|
| AHT10 VCC      | **3.3V**         | Alimentación sensor temp/humedad   |
| AHT10 GND      | **GND**          | Tierra                             |
| AHT10 SCL      | **GPIO 22**      | Reloj I2C (compartido)             |
| AHT10 SDA      | **GPIO 21**      | Datos I2C (compartido)             |
| BMP180 VCC     | **3.3V**         | Alimentación sensor presión        |
| BMP180 GND     | **GND**          | Tierra                             |
| BMP180 SCL     | **GPIO 22**      | Reloj I2C (compartido)             |
| BMP180 SDA     | **GPIO 21**      | Datos I2C (compartido)             |

## Vista Detallada del ESP32 con TFT ILI9341

```
                    ┌─────────────┐
                    │             │
                EN ●│             │● D23 (MOSI) ◄── TFT MOSI
                   │             │
                VP ●│             │● D22 (SCL I2C) ◄── Sensores
                   │             │
                VN ●│             │● TXD0
                   │             │
                D34●│   ESP32     │● RXD0
                   │             │
                D35●│   DOIT      │● D21 (SDA I2C) ◄── Sensores
                   │             │
                D32●│   DevKit    │● D19 (MISO) ◄── TFT MISO
                   │             │
                D33●│   V1        │● D18 (SCK) ◄── TFT SCK
                   │             │
                D25●│             │● D5 (CS) ◄── TFT CS
                   │             │
                D26●│             │● D17
                   │             │
                D27●│             │● D16 ◄── [Botón Alarma Opc B]
                   │             │
                D14●│             │● D4 ◄── [Botón Alarma Opc A / TFT RST Opc B]
                   │             │
                D12●│             │● D0
                   │             │
                D13●│             │● D2 (DC) ◄── TFT DC
                   │             │
                GND●│             │● D15 ◄── Botón Modo
                   │             │
                VIN●│             │● 3V3 ◄── TFT VCC, LED, RST (Opc A)
                   │             │
                    └─────────────┘
```

## Vista Detallada de la Pantalla TFT ILI9341

```
┌─────────────────────────────────────┐
│                                     │
│  ╔══════════════════════════════╗   │
│  ║                              ║   │
│  ║                              ║   │
│  ║                              ║   │
│  ║      PANTALLA TFT LCD        ║   │
│  ║        A COLOR               ║   │
│  ║      240 x 320 Pixels        ║   │
│  ║         ILI9341              ║   │
│  ║                              ║   │
│  ║                              ║   │
│  ║                              ║   │
│  ║                              ║   │
│  ╚══════════════════════════════╝   │
│                                     │
│        Módulo ILI9341 SPI           │
│                                     │
│  ┌───┬───┬───┬────┬────┬────┬────┬──┬────┐
│  │VCC│GND│ CS│RESET│DC │MOSI│SCK│LED│MISO│
│  └─┬─┴─┬─┴─┬─┴──┬─┴──┬─┴──┬─┴──┬─┴─┬┴──┬─┘
│    │   │   │    │    │    │    │   │   │
│    ●   ●   ●    ●    ●    ●    ●   ●   ●
└────┼───┼───┼────┼────┼────┼────┼───┼───┼───
     1   2   3    4    5    6    7   8   9

Pin 1: VCC    - Alimentación 3.3V (o 5V según módulo)
Pin 2: GND    - Tierra
Pin 3: CS     - Chip Select
Pin 4: RESET  - Reset (conectar a 3.3V o GPIO)
Pin 5: DC/A0  - Data/Command selector
Pin 6: MOSI   - Master Out Slave In (datos)
Pin 7: SCK    - Serial Clock
Pin 8: LED    - Backlight (conectar a 3.3V)
Pin 9: MISO   - Master In Slave Out (opcional)
```

## Diagrama Esquemático Completo

### Opción A (Recomendada - Reset a 3.3V)

```
ESP32                              TFT ILI9341
┌──────────┐                      ┌──────────┐
│          │                      │          │
│   3.3V   ├─────────────────────►│   VCC    │  (Rojo)
│          │          ┌──────────►│   LED    │  
│          │          │           │          │
│   GND    ├──────────┼──────────►│   GND    │  (Negro)
│          │          │           │          │
│  GPIO 5  ├──────────┼──────────►│   CS     │  (Naranja)
│          │          │           │          │
│  GPIO 2  ├──────────┼──────────►│   DC     │  (Verde)
│          │          │           │          │
│  GPIO23  ├──────────┼──────────►│   MOSI   │  (Azul)
│  (VSPI)  │          │           │          │
│          │          │           │          │
│  GPIO18  ├──────────┼──────────►│   SCK    │  (Amarillo)
│  (VSPI)  │          │           │          │
│          │          │           │          │
│  GPIO19  ├──────────┼──────────►│   MISO   │  (Púrpura, opcional)
│  (VSPI)  │          │           │          │
│          │          │           │          │
│   3.3V   ├──────────┘           │   RESET  │  (Blanco)
│          │                      │          │
└──────────┘                      └──────────┘

SENSORES I2C
┌──────────┐                      ┌──────────┐
│  ESP32   │                      │AHT10/BMP │
│          │                      │          │
│  GPIO22  ├─────────────────────►│   SCL    │
│  (SCL)   │                      │          │
│  GPIO21  ├─────────────────────►│   SDA    │
│  (SDA)   │                      │          │
└──────────┘                      └──────────┘

BOTONES
┌──────────┐
│  ESP32   │
│          │
│  GPIO15  ├───[Botón Modo]───┐
│          │                  GND
│  GPIO 4  ├───[Botón Alarma]─┐
│          │                  GND
└──────────┘
```

## ⚠️ Notas Importantes

### Voltaje de Alimentación
- **CRÍTICO**: Algunos módulos ILI9341 son **5V**, otros son **3.3V**
- **Verifica el voltaje** de tu módulo antes de conectar
- Módulos con regulador de voltaje incorporado → Usar **5V**
- Módulos sin regulador → Usar **3.3V**
- ⚡ **Conectar 5V a un módulo de 3.3V lo destruirá**

### Pines SPI del ESP32
- El ESP32 tiene dos buses SPI: **VSPI** y **HSPI**
- Esta configuración usa **VSPI** (por defecto):
  - MOSI = GPIO 23
  - MISO = GPIO 19
  - SCK = GPIO 18
  - CS = Definible (usamos GPIO 5)

### Pin MISO (GPIO 19)
- Solo necesario si tu módulo tiene **lector de tarjeta SD**
- Si no usas SD, puedes dejar MISO sin conectar
- GPIO 19 quedaría libre para otros usos

### Backlight (LED)
- Conectando LED a 3.3V → Pantalla siempre encendida
- Para control de brillo variable → Conectar LED a un pin PWM (GPIO 25, 26, 27)
- Resistencia de 100-220Ω en serie recomendada (algunos módulos ya la incluyen)

### Pin DC (Data/Command)
- Algunos módulos lo llaman **A0** o **RS**
- Es el mismo pin, solo diferente nomenclatura

### Pin RESET
- **Opción A**: Conectar a 3.3V (pantalla siempre activa, más simple)
- **Opción B**: Conectar a GPIO 4 (permite reset por software)
- Si conectas a GPIO, usa resistencia pull-up de 10kΩ a 3.3V

### Velocidad SPI
- La ILI9341 soporta hasta **40 MHz**
- El ESP32 puede configurarse para alta velocidad
- Mayor velocidad = actualizaciones más rápidas
- Código: `SPISettings(40000000, MSBFIRST, SPI_MODE0)`

### Consumo de Energía
- TFT consume **más** que OLED (especialmente backlight)
- Backlight típico: 20-100 mA
- Considera fuente de alimentación adecuada
- Para batería, implementa apagado de backlight

## 🔧 Verificación de Conexiones

Antes de cargar el código, verifica:

**Pantalla TFT:**
- [ ] VCC del TFT conectado a voltaje correcto (3.3V o 5V según módulo)
- [ ] GND del TFT conectado a GND del ESP32
- [ ] CS conectado a GPIO 5
- [ ] RESET conectado a 3.3V (Opción A) o GPIO 4 (Opción B)
- [ ] DC conectado a GPIO 2
- [ ] MOSI conectado a GPIO 23
- [ ] SCK conectado a GPIO 18
- [ ] LED conectado a 3.3V
- [ ] MISO conectado a GPIO 19 (si usas SD)

**Sensores I2C (sin cambios):**
- [ ] SCL de sensores → GPIO 22
- [ ] SDA de sensores → GPIO 21

**Botones:**
- [ ] Botón modo → GPIO 15
- [ ] Botón alarma → GPIO 4 (Opción A) o GPIO 16 (Opción B)

## 🧪 Prueba de Conexión

Para verificar que la pantalla funciona:

1. Conecta solo la pantalla (sin sensores primero)
2. Carga un sketch de prueba (ver ejemplos Adafruit)
3. La pantalla debe mostrar gráficos de prueba
4. Verifica colores: rojo, verde, azul
5. Si funciona, conecta los demás componentes

### Código de Prueba Básico

```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS   5
#define TFT_DC   2
// #define TFT_RST  -1  // Conectado a 3.3V

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  Serial.begin(115200);
  Serial.println("Test TFT ILI9341");
  
  tft.begin();
  tft.setRotation(0);  // 0-3 para rotar
  
  // Fondo negro
  tft.fillScreen(ILI9341_BLACK);
  
  // Texto blanco
  tft.setCursor(50, 100);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.println("RELOJ");
  
  tft.setCursor(20, 140);
  tft.println("PERONISTA");
  
  Serial.println("OK!");
}

void loop() {
  // Nada
}
```

## 📊 Comparación: I2C (OLED) vs SPI (TFT)

| Característica | I2C (OLED anterior) | SPI (TFT nueva) |
|----------------|---------------------|-----------------|
| **Pines datos** | 2 (SDA, SCL) | 3 (MOSI, MISO, SCK) |
| **Pines control** | 0 | 2 (CS, DC) |
| **Pines totales** | 4 (VCC, GND, SDA, SCL) | 9 (VCC, GND, CS, RST, DC, MOSI, SCK, LED, MISO) |
| **Velocidad** | Hasta 400 kHz (I2C Fast) | Hasta 40 MHz (SPI) |
| **Complejidad** | Simple | Media |
| **Rendimiento** | Más lento | Mucho más rápido |

## 🛠️ Solución de Problemas

### Pantalla en blanco
1. Verifica voltaje VCC (3.3V vs 5V)
2. Verifica todas las conexiones
3. Prueba conectar RESET a GPIO en vez de 3.3V
4. Verifica que CS y DC están en pines correctos

### Pantalla con colores incorrectos
1. Verifica conexión MOSI
2. Verifica conexión SCK
3. Revisa código de colores RGB565

### Pantalla con "nieve" o ruido
1. Cables demasiado largos (acortar)
2. Interferencia (separar de cables de potencia)
3. Velocidad SPI demasiado alta (reducir MHz)

### No compila el código
1. Instala librería Adafruit_ILI9341
2. Instala Adafruit_GFX
3. Verifica platformio.ini

## 📸 Sugerencias de Cableado

**Recomendado:**
- Usar **cables Dupont** de buena calidad
- Mantener cables **lo más cortos posible** (< 15 cm ideal)
- Usar **protoboard** para pruebas iniciales
- Para instalación final, considerar **soldadura directa**
- Agrupar cables por función (alimentación, datos, control)

**Códigos de color sugeridos:**
- Rojo: VCC (3.3V o 5V)
- Negro: GND
- Amarillo: SCK (Clock)
- Azul: MOSI (Data)
- Verde: DC
- Naranja: CS
- Blanco: RESET

---

**✅ Con estas conexiones tu Reloj Peronista con TFT ILI9341 debería funcionar perfectamente!**

**Siguiente paso:** Ver `PLAN_ADAPTACION_ILI9341.md` para los cambios de software.
