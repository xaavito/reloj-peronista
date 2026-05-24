# 🖥️ Configuración de Pantalla TFT_eSPI en PlatformIO

## 📌 Método Recomendado: Usar platformio.ini

En lugar de modificar el archivo `User_Setup.h` de la librería TFT_eSPI, **podés configurar todo desde `platformio.ini`** usando `build_flags`.

### ✅ Ventajas:
- ✅ No tocar archivos de librerías
- ✅ Configuración portátil (se guarda en tu proyecto)
- ✅ Fácil de versionar con Git
- ✅ Sin conflictos al actualizar librerías

---

## 🔧 Configuración Actual (platformio.ini)

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200

; Librerías
lib_deps = 
    bodmer/TFT_eSPI@^2.5.43
    adafruit/Adafruit AHTX0@^2.0.3
    adafruit/Adafruit BMP085 Library@^1.2.2
    bblanchon/ArduinoJson@^7.0.4

; Configuración de TFT_eSPI mediante build_flags
build_flags = 
    -D USER_SETUP_LOADED=1          ; Indica que la config está cargada
    -D ILI9341_DRIVER=1             ; Driver para pantalla ILI9341
    -D TFT_WIDTH=240                ; Ancho de pantalla
    -D TFT_HEIGHT=320               ; Alto de pantalla
    -D TFT_MISO=12                  ; Pin MISO (SPI)
    -D TFT_MOSI=13                  ; Pin MOSI (SPI)
    -D TFT_SCLK=14                  ; Pin SCLK (SPI)
    -D TFT_CS=15                    ; Pin CS (Chip Select)
    -D TFT_DC=2                     ; Pin DC (Data/Command)
    -D TFT_RST=-1                   ; Pin RST (-1 = no usado)
    -D LOAD_GLCD=1                  ; Font 1 (8 pixel)
    -D LOAD_FONT2=1                 ; Font 2 (16 pixel)
    -D LOAD_FONT4=1                 ; Font 4 (26 pixel)
    -D LOAD_FONT6=1                 ; Font 6 (48 pixel)
    -D LOAD_FONT7=1                 ; Font 7 (7-segment 48 pixel)
    -D LOAD_FONT8=1                 ; Font 8 (7-segment 75 pixel)
    -D LOAD_GFXFF=1                 ; FreeFonts
    -D SPI_FREQUENCY=55000000       ; Frecuencia SPI (55MHz)
    -D SPI_READ_FREQUENCY=20000000  ; Frecuencia lectura (20MHz)
    -D USE_HSPI_PORT=1              ; Usar puerto HSPI del ESP32
```

---

## 📝 Explicación de los Parámetros

### Driver y Resolución:
```ini
-D ILI9341_DRIVER=1      # Tipo de pantalla (ILI9341)
-D TFT_WIDTH=240         # Ancho en píxeles
-D TFT_HEIGHT=320        # Alto en píxeles
```

### Pines SPI:
```ini
-D TFT_MISO=12          # Master In Slave Out
-D TFT_MOSI=13          # Master Out Slave In  
-D TFT_SCLK=14          # Serial Clock
-D TFT_CS=15            # Chip Select
-D TFT_DC=2             # Data/Command
-D TFT_RST=-1           # Reset (no conectado)
```

### Fuentes:
```ini
-D LOAD_GLCD=1          # Fuente básica 8px
-D LOAD_FONT2=1         # Fuente 16px
-D LOAD_FONT4=1         # Fuente 26px
-D LOAD_FONT6=1         # Fuente 48px
-D LOAD_FONT7=1         # 7-segment 48px (para números)
-D LOAD_FONT8=1         # 7-segment 75px (para reloj)
-D LOAD_GFXFF=1         # Fuentes Adafruit GFX
```

### Velocidad SPI:
```ini
-D SPI_FREQUENCY=55000000       # 55MHz para escritura
-D SPI_READ_FREQUENCY=20000000  # 20MHz para lectura
-D USE_HSPI_PORT=1              # Usar HSPI (no VSPI)
```

---

## 🔄 Cómo Cambiar la Configuración

### Ejemplo 1: Usar Otro Driver de Pantalla

Si tenés una **ST7789** en lugar de ILI9341:

```ini
build_flags = 
    -D USER_SETUP_LOADED=1
    -D ST7789_DRIVER=1        ; ← Cambiar driver
    -D TFT_WIDTH=240
    -D TFT_HEIGHT=240         ; ← ST7789 es 240x240
    ; ... resto de la config
```

### Ejemplo 2: Cambiar Pines

Si querés usar otros pines GPIO:

```ini
build_flags = 
    -D USER_SETUP_LOADED=1
    -D ILI9341_DRIVER=1
    -D TFT_MOSI=23            ; ← Cambiar a GPIO 23
    -D TFT_SCLK=18            ; ← Cambiar a GPIO 18
    -D TFT_CS=5               ; ← Cambiar a GPIO 5
    -D TFT_DC=16              ; ← Cambiar a GPIO 16
    ; ... resto
```

### Ejemplo 3: Reducir Fuentes (Ahorrar Memoria)

Si no necesitás todas las fuentes:

```ini
build_flags = 
    -D USER_SETUP_LOADED=1
    -D ILI9341_DRIVER=1
    ; Solo cargar las fuentes que usás:
    -D LOAD_GLCD=1            ; Fuente básica
    -D LOAD_FONT8=1           ; Solo para el reloj
    ; NO cargar FONT2, FONT4, FONT6, FONT7, GFXFF
    ; ... resto de config
```

---

## 🚀 Cómo Aplicar los Cambios

1. **Edita** el archivo `platformio.ini`
2. **Guarda** el archivo
3. **Limpia** el proyecto: 
   ```bash
   pio run --target clean
   ```
4. **Compila** nuevamente:
   ```bash
   pio run
   ```
5. **Carga** al ESP32:
   ```bash
   pio run --target upload
   ```

---

## ⚠️ Importante

### NO necesitas modificar:
- ❌ `.pio/libdeps/.../TFT_eSPI/User_Setup.h`
- ❌ Ningún archivo dentro de `.pio/`

### La configuración va SOLO en:
- ✅ `platformio.ini` (sección `build_flags`)

---

## 🔍 Drivers Disponibles

Otros drivers que podés usar (cambiar en `build_flags`):

```ini
-D ST7735_DRIVER=1          # ST7735 128x160 o 80x160
-D ST7789_DRIVER=1          # ST7789 240x240 o 240x320
-D ST7789_2_DRIVER=1        # ST7789 240x320
-D ILI9341_DRIVER=1         # ILI9341 240x320 (actual)
-D ILI9341_2_DRIVER=1       # ILI9341 240x320 v2
-D ILI9488_DRIVER=1         # ILI9488 320x480
-D HX8357D_DRIVER=1         # HX8357D 320x480
```

---

## 📚 Recursos

- [TFT_eSPI GitHub](https://github.com/Bodmer/TFT_eSPI)
- [PlatformIO Build Flags](https://docs.platformio.org/en/latest/projectconf/section_env_build.html#build-flags)
- [User_Setup Examples](https://github.com/Bodmer/TFT_eSPI/tree/master/User_Setups)

---

## ✅ Verificación

Para verificar que la configuración está correcta:

1. Compila el proyecto
2. Revisa los logs de compilación
3. Busca las líneas que confirman los defines:
   ```
   -D ILI9341_DRIVER=1
   -D TFT_CS=15
   ...
   ```
4. Si el proyecto compila sin errores, la configuración es correcta

---

**💡 Tip**: Si cambias de pantalla en el futuro, solo necesitas editar `platformio.ini` y recompilar.
