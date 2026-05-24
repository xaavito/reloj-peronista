# Integración OpenWeather API - Reloj Peronista

## Descripción

Se agregó integración con OpenWeather API para mostrar pronóstico del tiempo en el Reloj Peronista. Además de la temperatura del sensor local, ahora puedes ver el pronóstico meteorológico de tu ciudad.

## Características Agregadas

### Nuevo Modo: MODE_FORECAST
- **Temperatura actual**: Muestra la temperatura del pronóstico de tu ciudad
- **Min/Max**: Temperaturas mínima y máxima del día
- **Sensación térmica**: Temperatura percibida
- **Descripción**: Condiciones climáticas en español

### Actualización Automática
- El pronóstico se actualiza cada 10 minutos automáticamente
- Primera actualización al iniciar el dispositivo

## Configuración

### 1. Obtener API Key de OpenWeather

1. Ve a [OpenWeather](https://openweathermap.org/api)
2. Crea una cuenta gratuita
3. Genera tu API key
4. Copia tu API key

### 2. Configurar el Proyecto

1. Copia `src/config.h.example` a `src/config.h`:
   ```bash
   cp src/config.h.example src/config.h
   ```

2. Edita `src/config.h` y agrega tus credenciales:
   ```cpp
   // WiFi
   const char* WIFI_SSID = "TU_WIFI";
   const char* WIFI_PASSWORD = "TU_PASSWORD";
   
   // OpenWeather API
   const char* OPENWEATHER_API_KEY = "tu_api_key_aqui";
   const char* OPENWEATHER_CITY = "Buenos Aires";  // Tu ciudad
   const char* OPENWEATHER_COUNTRY = "AR";  // Código de país
   ```

## Uso

### Modos de Visualización

Presiona el botón para ciclar entre los modos:

1. **AUTO**: Alterna automáticamente hora/fecha/efemérides
2. **HORA**: Solo muestra la hora
3. **FECHA**: Solo muestra la fecha
4. **EFEMÉRIDES**: Solo muestra efemérides peronistas
5. **TEMPERATURA**: Temperatura del sensor local
6. **HUMEDAD**: Humedad del sensor local
7. **CLIMA**: Alterna entre temperatura y humedad del sensor
8. **FORECAST**: **NUEVO!** Pronóstico de OpenWeather (alterna entre temp actual, min/max, y sensación térmica)

### Visualización del Pronóstico

En modo FORECAST, la pantalla muestra **3 días** en columnas:

```
┌──────────┬──────────┬──────────┐
│   Día 1  │   Día 2  │   Día 3  │
│    15    │    16    │    17    │
│          │          │          │
│    ☀️    │    ☁️    │    🌧️    │
│          │          │          │
│  25° 18° │  22° 16° │  19° 14° │
└──────────┴──────────┴──────────┘
```

**Cada columna muestra:**
- **Arriba**: Día del mes
- **Centro**: Ícono del clima (sol, nube, lluvia, nieve, tormenta)
- **Abajo**: Temperatura máxima (izq) y mínima (der)

## Dependencias Agregadas

- **ArduinoJson** v7.0.4: Para parsear respuestas JSON de la API
- **HTTPClient**: Cliente HTTP para ESP32 (incluido en el framework)

## API Endpoint Utilizado

```
http://api.openweathermap.org/data/2.5/weather
```

Parámetros:
- `q`: Ciudad y país
- `appid`: Tu API key
- `units=metric`: Temperaturas en Celsius
- `lang=es`: Descripciones en español

## Datos Mostrados

El pronóstico incluye:
- Temperatura actual (°C)
- Sensación térmica (°C)
- Temperatura mínima (°C)
- Temperatura máxima (°C)
- Humedad (%)
- Descripción del clima

## Troubleshooting

### "Sin datos clima"
- Verifica que tu API key sea válida
- Confirma que estás conectado a WiFi
- Revisa que el nombre de ciudad y código de país sean correctos
- Chequea el monitor serial para ver mensajes de error

### Error HTTP
- API key inválida o expirada
- Límite de llamadas excedido (plan gratuito: 60 llamadas/minuto)
- Problemas de conectividad

### Problemas de compilación
- Asegúrate de tener instalada la librería ArduinoJson v7.0.4
- Verifica que `config.h` existe (no solo `config.h.example`)

## Notas

- La versión gratuita de OpenWeather permite 1,000 llamadas/día
- Con actualizaciones cada 10 minutos: ~144 llamadas/día
- Los datos se muestran en español gracias al parámetro `lang=es`
- El sistema maneja automáticamente reconexiones WiFi si se pierde la conexión

## Monitor Serial

Para ver los mensajes de debug, conecta el monitor serial a 115200 baud. Verás:
- Conexión WiFi
- Obtención de pronóstico
- Datos del clima recibidos
- Cambios de modo

¡Disfrutá del pronóstico en tu Reloj Peronista!
