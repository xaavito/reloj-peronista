# 🔌 Diagrama de Conexión - ESP32 y OLED SSD1306

## Conexión I2C entre ESP32 y Pantalla OLED 0.91" 128x32

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
│  3.3V ●─────────────────────────────┼──────● VCC
│                                     │
│   GND ●─────────────────────────────┼──────● GND
│                                     │
│ GPIO22●─────────────────────────────┼──────● SCL
│ (SCL) │                             │
│       │                             │
│ GPIO21●─────────────────────────────┼──────● SDA
│ (SDA) │                             │
│       │                             │
└───────┴─────────────────────────────┘
         │                             
         │                             ┌─────────────────────┐
         │                             │   OLED 0.91" 128x32 │
         │                             │     SSD1306 I2C     │
         └─────────────────────────────┤                     │
                                       │  ┌───────────────┐  │
                                       │  │               │  │
                                       │  │               │  │
                                       │  │    DISPLAY    │  │
                                       │  │               │  │
                                       │  │               │  │
                                       │  └───────────────┘  │
                                       │                     │
                                       │  VCC GND SCL SDA    │
                                       │   ●   ●   ●   ●     │
                                       └─────────────────────┘
```

## Tabla de Conexiones

### Pantalla OLED

| **Pin OLED** | **Función**     | **Pin ESP32**        | **Descripción**                    |
|--------------|-----------------|----------------------|------------------------------------|
| **VCC**      | Alimentación    | **3.3V**             | Voltaje de alimentación (3.3V)     |
| **GND**      | Tierra          | **GND**              | Tierra común                       |
| **SCL**      | Clock I2C       | **GPIO 22**          | Reloj del bus I2C                  |
| **SDA**      | Data I2C        | **GPIO 21**          | Datos del bus I2C                  |

### Botón de Modo (Opcional)

**Botón Táctil 4 Pines (6mm x 5.1mm)**

| **Botón**    | **Pin ESP32**    | **Descripción**                    |
|--------------|------------------|------------------------------------|
| Pin 1 o Pin 2 | **GPIO 15**     | Señal del botón                     |
| Pin 3 o Pin 4 | **GND**         | Tierra común                        |

**¿Cómo Funciona el Botón de 4 Pines?**

Este botón tiene 4 pines organizados en 2 pares:
```
    1 ●═══● 2
      ║ ║ ║
    3 ●═══● 4
```

- **Pines 1 y 2**: Están conectados internamente (mismo lado)
- **Pines 3 y 4**: Están conectados internamente (mismo lado)
- Al presionar: Los dos lados se conectan (1-2 se conecta con 3-4)

**Conexión Simple:**
1. Conecta **cualquier pin del lado superior** (1 o 2) a **GPIO 15**
2. Conecta **cualquier pin del lado inferior** (3 o 4) a **GND**

**Alternativas Válidas:**
- Pin 1 → GPIO 15, Pin 3 → GND ✅
- Pin 1 → GPIO 15, Pin 4 → GND ✅
- Pin 2 → GPIO 15, Pin 3 → GND ✅
- Pin 2 → GPIO 15, Pin 4 → GND ✅

**Nota**: El botón usa la resistencia pull-up interna del ESP32, no necesitas resistencia externa.

### Sensor AHT10 (Temperatura y Humedad)

| **Pin AHT10** | **Pin ESP32**    | **Descripción**                    |
|---------------|------------------|------------------------------------|
| **VCC**       | **3.3V**         | Alimentación 3.3V                   |
| **GND**       | **GND**          | Tierra común                        |
| **SCL**       | **GPIO 22**      | Reloj I2C (compartido con OLED)     |
| **SDA**       | **GPIO 21**      | Datos I2C (compartido con OLED)     |

### Sensor BMP180 (Presión y Temperatura)

| **Pin BMP180** | **Pin ESP32**   | **Descripción**                    |
|----------------|------------------|------------------------------------|
| **VCC**        | **3.3V**         | Alimentación 3.3V                   |
| **GND**        | **GND**          | Tierra común                        |
| **SCL**        | **GPIO 22**      | Reloj I2C (compartido con OLED)     |
| **SDA**        | **GPIO 21**      | Datos I2C (compartido con OLED)     |

**Nota**: Todos los dispositivos I2C (OLED, AHT10, BMP180) comparten los mismos pines SDA y SCL. Cada uno tiene una dirección I2C única.

## Vista Detallada del ESP32

```
                    ┌─────────────┐
                    │             │
                EN ●│             │● D23
                   │             │
                VP ●│             │● D22 (SCL) ◄── Conectar a SCL del OLED
                   │             │
                VN ●│             │● TXD0
                   │             │
                D34●│   ESP32     │● RXD0
                   │             │
                D35●│   DOIT      │● D21 (SDA) ◄── Conectar a SDA del OLED
                   │             │
                D32●│   DevKit    │● D19
                   │             │
                D33●│   V1        │● D18
                   │             │
                D25●│             │● D5
                   │             │
                D26●│             │● D17
                   │             │
                D27●│             │● D16
                   │             │
                D14●│             │● D4
                   │             │
                D12●│             │● D0
                   │             │
                D13●│             │● D2
                   │             │
                GND●│             │● D15 ◄── Conectar a GND del OLED
                   │             │
                VIN●│             │● 3V3  ◄── Conectar a VCC del OLED
                   │             │
                    └─────────────┘
```

## Vista Detallada de la Pantalla OLED

```
┌────────────────────────────────┐
│                                │
│  ╔═══════════════════════════╗ │
│  ║                           ║ │
│  ║                           ║ │
│  ║      PANTALLA OLED        ║ │
│  ║      128 x 32 Pixels      ║ │
│  ║                           ║ │
│  ║                           ║ │
│  ╚═══════════════════════════╝ │
│                                │
│     Módulo SSD1306 I2C         │
│                                │
│      ┌───┬───┬───┬───┐         │
│      │VCC│GND│SCL│SDA│         │
│      └─┬─┴─┬─┴─┬─┴─┬─┘         │
│        │   │   │   │           │
│        ●   ●   ●   ●           │
└────────┼───┼───┼───┼───────────┘
         │   │   │   │
         1   2   3   4

Pin 1: VCC - Alimentación 3.3V
Pin 2: GND - Tierra
Pin 3: SCL - Clock I2C
Pin 4: SDA - Data I2C
```

## Diagrama Esquemático

```
ESP32                          OLED SSD1306
┌──────────┐                  ┌──────────┐
│          │                  │          │
│   3.3V   ├─────────────────►│   VCC    │  (Rojo)
│          │                  │          │
│   GND    ├─────────────────►│   GND    │  (Negro)
│          │                  │          │
│  GPIO22  ├─────────────────►│   SCL    │  (Amarillo/Verde)
│  (SCL)   │                  │          │
│          │                  │          │
│  GPIO21  ├─────────────────►│   SDA    │  (Azul/Blanco)
│  (SDA)   │                  │          │
│          │                  │          │
└──────────┘                  └──────────┘
```

## ⚠️ Notas Importantes

### Voltaje
- ⚡ **IMPORTANTE**: La pantalla OLED debe conectarse a **3.3V**, NO a 5V
- Conectar a 5V puede dañar permanentemente la pantalla
- El ESP32 opera a 3.3V de forma nativa

### Dirección I2C
- La dirección I2C por defecto suele ser `0x3C`
- Algunas pantallas pueden usar `0x3D`
- Si la pantalla no funciona, intenta cambiar en el código:
  ```cpp
  #define SCREEN_ADDRESS 0x3D  // Cambiar de 0x3C a 0x3D
  ```

### Resistencias Pull-Up
- Los pines GPIO21 (SDA) y GPIO22 (SCL) del ESP32 tienen resistencias pull-up internas
- NO es necesario agregar resistencias pull-up externas
- El módulo OLED también suele incluir resistencias pull-up de 4.7kΩ

### Longitud de Cables
- Mantén los cables lo más cortos posible (idealmente < 20cm)
- Cables muy largos pueden causar problemas de comunicación I2C
- Si necesitas cables más largos, considera usar cables blindados

### Identificación de Pines
- Algunos módulos OLED tienen el orden de pines diferente
- **Verifica siempre** las etiquetas en tu módulo antes de conectar
- Los órdenes más comunes son:
  - `VCC - GND - SCL - SDA` (más común)
  - `GND - VCC - SCL - SDA`
  - `GND - VCC - SDA - SCL`

## 🔧 Verificación de Conexiones

Antes de cargar el código, verifica:

- [ ] VCC del OLED conectado a 3.3V del ESP32
- [ ] GND del OLED conectado a GND del ESP32
- [ ] SCL del OLED conectado a GPIO22 del ESP32
- [ ] SDA del OLED conectado a GPIO21 del ESP32
- [ ] Ningún cable suelto o con mal contacto
- [ ] Polaridad correcta (3.3V y no 5V)

## 🧪 Prueba de Conexión

Una vez conectado, el ESP32 debe:
1. Detectar la pantalla al iniciar
2. Mostrar "Iniciando..." en la pantalla
3. Conectarse al WiFi
4. Sincronizar la hora
5. Mostrar el reloj funcionando

Si hay problemas, revisa el monitor serial (115200 baud) para mensajes de error.

## 📸 Sugerencia

Es recomendable usar:
- **Cables Dupont** hembra-hembra si tu OLED tiene pines macho
- **Protoboard** para hacer una conexión más estable antes del montaje final
- **Soldadura** para una instalación permanente (opcional)

---

**✅ Con estas conexiones tu Reloj Peronista debería funcionar perfectamente!**
