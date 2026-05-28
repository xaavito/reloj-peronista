# 🔍 DIAGNÓSTICO: WiFi Manager no visible

## Problemas identificados:

### 1. **Serial Monitor sin output**
- El código tiene `Serial.begin(115200)` configurado
- PlatformIO.ini tiene `monitor_speed = 115200` ✅
- **Posible causa**: El ESP32 se está reiniciando en bucle antes de imprimir

### 2. **Red WiFi no visible**
- La red debería llamarse: `Reloj_Peronista_XXXX` (XXXX = número hex del MAC)
- Sin contraseña (línea 194 del código)
- **Posible causa**: El ESP32 no está llegando a la función `connectWiFi()`

## 🔧 Soluciones a probar:

### PASO 1: Verificar Serial Monitor
1. Asegurate que el baudrate sea **115200**
2. Cierra y abre el Serial Monitor
3. Presiona el botón RESET del ESP32

### PASO 2: El problema más probable
El ESP32 puede estar fallando en la inicialización del TFT o sensores, causando un reinicio antes de llegar al WiFi Manager.

Voy a crear una versión con más debug para identificar dónde se queda.
