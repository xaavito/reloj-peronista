# 🕐 Reloj Peronista - Reloj Digital con OLED

Reloj digital estilo despertador usando ESP32 y pantalla OLED 0.91" 128x32 SSD1306 con efemérides peronistas.

## 📋 Hardware Necesario

- **ESP32 DOIT DevKit V1** (o compatible)
- **Pantalla OLED 0.91" 128x32 SSD1306 I2C** (color blanco)

## 🔌 Conexiones

Conecta la pantalla OLED al ESP32 usando los pines I2C por defecto:

| OLED Pin | ESP32 Pin |
|----------|-----------|
| VCC      | 3.3V      |
| GND      | GND       |
| SCL      | GPIO 22   |
| SDA      | GPIO 21   |

## ⚙️ Configuración

### 1. Crear Archivo de Configuración

**IMPORTANTE**: Antes de compilar, debes crear tu archivo de configuración:

```bash
# Copia el archivo ejemplo
cp src/config.h.example src/config.h
```

Este archivo **NO se subirá a GitHub** (está en .gitignore) para proteger tus credenciales.

### 2. Configurar WiFi

Abre el archivo `src/config.h` y modifica las siguientes líneas con tus credenciales:

```cpp
const char* WIFI_SSID = "TU_WIFI_SSID";          // ← Reemplaza con el nombre de tu WiFi
const char* WIFI_PASSWORD = "TU_WIFI_PASSWORD";  // ← Reemplaza con tu contraseña
```

### 3. Ajustar Zona Horaria (Opcional)

El reloj está configurado para GMT-3 (Argentina). Si necesitas cambiar la zona horaria en `src/config.h`:

```cpp
const long GMT_OFFSET_SEC = -10800;  // GMT-3 (Argentina)
```

### 4. Ajustar Intervalo de Efemérides (Opcional)

Por defecto, las efemérides se muestran cada 3 minutos (editar en `src/config.h`):

```cpp
const unsigned long EFEMERIDE_INTERVAL = 180000; // 3 minutos (en milisegundos)
```

Puedes cambiarlo a:
- 1 minuto: `60000`
- 5 minutos: `300000`
- 10 minutos: `600000`

## 🚀 Compilación y Carga

### Usando PlatformIO CLI:

```bash
# Compilar el proyecto
pio run

# Compilar y cargar al ESP32
pio run --target upload

# Ver el monitor serial
pio device monitor
```

### Usando VSCode:

1. Abre el proyecto en VSCode
2. Presiona el botón **Build** (✓) en la barra inferior
3. Presiona el botón **Upload** (→) para cargar al ESP32
4. Presiona el botón **Serial Monitor** (🔌) para ver los mensajes

## 📺 Funcionamiento

1. **Al encender**: El ESP32 se conecta al WiFi y sincroniza la hora con servidor NTP
2. **Modo Normal**: Muestra la hora en formato digital grande (HH:MM) con segundos pequeños y fecha
3. **Modo Efeméride**: Cada 3 minutos, muestra una efeméride histórica durante 3 minutos
4. **Rotación**: Las efemérides rotan automáticamente

### Formato de Pantalla

**Modo Reloj:**
```
┌────────────────────┐
│  10:45     32      │ ← Hora grande + segundos
│                    │
│  22/03/2026        │ ← Fecha
└────────────────────┘
```

**Modo Efeméride:**
```
┌────────────────────┐
│ 17 Oct 1945        │
│ Dia de la          │
│ Lealtad            │
└────────────────────┘
```

## 🔧 Personalización

### Agregar Más Efemérides

Edita el array `efemerides[]` en `src/main.cpp`:

```cpp
const char* efemerides[] = {
  "17 Oct 1945\nDia de la\nLealtad",
  "Tu nueva\nefemeride\naqui",
  // ... agrega más
};
```

**Nota**: Usa `\n` para saltos de línea. La pantalla tiene 4 líneas de texto disponibles.

### Cambiar Dirección I2C de la OLED

Si tu pantalla usa otra dirección (generalmente 0x3C o 0x3D):

```cpp
#define SCREEN_ADDRESS 0x3C  // Cambiar si es necesario
```

## 🌡️ Próximas Mejoras Planificadas

- [ ] Agregar sensor de temperatura DHT22 o BME280
- [ ] Mostrar temperatura en pantalla
- [ ] Agregar funcionalidad de alarma
- [ ] Botones para configurar alarma
- [ ] Memoria EEPROM para guardar configuración

## 📊 Recursos y Consumo

- **RAM**: ~50KB
- **Flash**: ~300KB
- **Consumo**: ~80mA (con WiFi activo)

## 🐛 Solución de Problemas

### La pantalla no enciende
- Verifica las conexiones I2C (SDA/SCL)
- Verifica que estás usando 3.3V (NO 5V)
- Prueba cambiar la dirección I2C a `0x3D`

### No se conecta al WiFi
- Verifica que el SSID y contraseña sean correctos
- Asegúrate que el WiFi sea 2.4GHz (ESP32 no soporta 5GHz)
- Revisa el monitor serial para ver mensajes de error

### La hora está incorrecta
- Verifica la configuración de zona horaria (`gmtOffset_sec`)
- Asegúrate que el ESP32 tenga conexión a internet para NTP
- La sincronización puede tardar unos segundos

## 📝 Licencia

Proyecto libre para uso personal y educativo.

---

**Desarrollado con ❤️ para mantener viva la memoria histórica**
