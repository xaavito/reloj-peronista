# 🏗️ Arquitectura MQTT - Sistema Integrado Raspberry Pi

## 📋 Resumen Ejecutivo

Esta es una propuesta de arquitectura MQTT centralizada en tu Raspberry Pi para gestionar múltiples dispositivos y servicios, incluyendo el Reloj Peronista.

---

## 🎯 Objetivos del Sistema

1. **Reloj Peronista** - Recibir notificaciones y comandos
2. **Calendarios** - Notificaciones de eventos próximos
3. **Deportes** - Notificaciones de partidos (Boca, torneos de fútbol)
4. **Estación Meteorológica** - Enviar datos de sensores en techo
5. **Extensibilidad** - Fácil agregar nuevos dispositivos/servicios

---

## 🏛️ Arquitectura General

```
┌─────────────────────────────────────────────────────────────┐
│                    RASPBERRY PI                              │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │         MQTT BROKER (Mosquitto)                    │     │
│  │         Puerto: 1883 (local) / 8883 (TLS)          │     │
│  └────────────────────────────────────────────────────┘     │
│                          ↑↓                                  │
│  ┌─────────────────┬────────────┬─────────────────────┐     │
│  │   PUBLISHER 1   │ PUBLISHER 2│   PUBLISHER 3      │     │
│  │   Calendarios   │  Deportes  │   Estación Meteo   │     │
│  │   (Script Py)   │(Script Py) │   (Script Py/MQTT) │     │
│  └─────────────────┴────────────┴─────────────────────┘     │
└─────────────────────────────────────────────────────────────┘
                          ↓ WiFi
        ┌─────────────────────────────────────────┐
        │    SUBSCRIBERS (Clientes)               │
        │                                         │
        │  ┌─────────────────┐  ┌──────────────┐ │
        │  │ Reloj Peronista │  │ Otros ESP32  │ │
        │  │   (ESP32)       │  │  /Displays   │ │
        │  └─────────────────┘  └──────────────┘ │
        └─────────────────────────────────────────┘
```

---

## 📊 Estructura de Topics (Topicos)

### Principio de Diseño
Los topics en MQTT usan estructura jerárquica con `/` como separador:
```
casa/dispositivo/tipo/accion
```

### 🏠 Topics Propuestos

#### 1. **Reloj Peronista**
```
casa/reloj-peronista/command          → Comandos al reloj
casa/reloj-peronista/notification     → Notificaciones generales
casa/reloj-peronista/calendar         → Eventos de calendario
casa/reloj-peronista/deportes         → Eventos deportivos
casa/reloj-peronista/status           → Estado del reloj (publicado por ESP32)
casa/reloj-peronista/alarm            → Control de alarma
```

#### 2. **Estación Meteorológica**
```
casa/meteo/techo/temperatura          → Temperatura del techo
casa/meteo/techo/humedad              → Humedad exterior
casa/meteo/techo/presion              → Presión atmosférica
casa/meteo/techo/viento               → Velocidad del viento
casa/meteo/techo/lluvia               → Sensor de lluvia (boolean)
casa/meteo/techo/all                  → Todos los datos en JSON
```

#### 3. **Calendarios**
```
casa/calendario/evento                → Evento próximo genérico
casa/calendario/recordatorio          → Recordatorio específico
casa/calendario/cumpleaños            → Cumpleaños del día
casa/calendario/feriado               → Feriados argentinos
```

#### 4. **Deportes**
```
casa/deportes/boca/proximo            → Próximo partido de Boca
casa/deportes/boca/en-vivo            → Partido en curso (minuto a minuto)
casa/deportes/boca/resultado          → Resultado final
casa/deportes/torneo/proximo          → Tu próximo partido en torneo
casa/deportes/torneo/recordatorio     → Recordatorio 1h antes
```

#### 5. **Sistema (Administración)**
```
casa/sistema/broadcast                → Mensaje a todos los dispositivos
casa/sistema/hora                     → Sincronización de hora
casa/sistema/status                   → Estado general del sistema
```

---

## 🔄 Flujo de Mensajes

### Ejemplo 1: Notificación de Partido de Boca

```
PUBLISHER (Script Python en RPi)
     ↓
Consulta API de fútbol cada 30min
     ↓
Detecta partido de Boca en 2 horas
     ↓
MQTT PUBLISH → casa/deportes/boca/proximo
     ↓
Payload: {"equipo": "Boca Juniors", "rival": "River Plate", 
          "hora": "20:00", "tiempo": "2h"}
     ↓
BROKER (Mosquitto en RPi) redistribuye
     ↓
SUBSCRIBER (Reloj Peronista ESP32)
     ↓
Muestra notificación en pantalla TFT
```

### Ejemplo 2: Datos de Estación Meteorológica

```
PUBLISHER (Sensor en techo - ESP32/Arduino)
     ↓
Lee sensores cada 5 minutos
     ↓
MQTT PUBLISH → casa/meteo/techo/all
     ↓
Payload: {"temperatura": 25.3, "humedad": 68, 
          "presion": 1013, "viento": 12}
     ↓
SUBSCRIBERS:
  - Reloj Peronista (muestra en pantalla)
  - Home Assistant (registra histórico)
  - Script de alertas (notifica si temp > 35°C)
```

---

## 🛠️ Componentes a Implementar

### 🖥️ En Raspberry Pi

#### 1. **Broker MQTT - Mosquitto**
```bash
# Instalación
sudo apt update
sudo apt install mosquitto mosquitto-clients

# Configuración básica
sudo nano /etc/mosquitto/mosquitto.conf

# Contenido sugerido:
listener 1883
allow_anonymous false
password_file /etc/mosquitto/passwd

# Crear usuario
sudo mosquitto_passwd -c /etc/mosquitto/passwd mqtt_user

# Reiniciar
sudo systemctl restart mosquitto
```

#### 2. **Publisher de Calendarios** (Python)
```python
# ~/mqtt_publishers/calendario_publisher.py
import paho.mqtt.client as mqtt
import json
from datetime import datetime, timedelta
import time

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "mqtt_user"
MQTT_PASS = "tu_password"

def publicar_evento_calendario():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.connect(MQTT_BROKER, MQTT_PORT)
    
    # Obtener eventos de Google Calendar API o archivo local
    evento = {
        "titulo": "Reunión importante",
        "hora": "15:00",
        "tiempo_restante": "30min"
    }
    
    client.publish("casa/calendario/evento", json.dumps(evento))
    client.disconnect()

# Ejecutar cada 15 minutos con cron
if __name__ == "__main__":
    publicar_evento_calendario()
```

#### 3. **Publisher de Deportes** (Python)
```python
# ~/mqtt_publishers/deportes_publisher.py
import paho.mqtt.client as mqtt
import requests
import json
from datetime import datetime

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
API_FUTBOL = "https://api-football-v1.p.rapidapi.com/v3/fixtures"

def verificar_partidos_boca():
    client = mqtt.Client()
    client.username_pw_set("mqtt_user", "tu_password")
    client.connect(MQTT_BROKER, MQTT_PORT)
    
    # Consultar API de fútbol
    # (necesitarás una API key - hay varias gratis)
    headers = {"X-RapidAPI-Key": "TU_API_KEY"}
    params = {"team": "451", "next": "3"}  # 451 = Boca Juniors
    
    response = requests.get(API_FUTBOL, headers=headers, params=params)
    
    if response.status_code == 200:
        datos = response.json()
        proximo_partido = datos['response'][0]
        
        mensaje = {
            "rival": proximo_partido['teams']['away']['name'],
            "fecha": proximo_partido['fixture']['date'],
            "estadio": proximo_partido['fixture']['venue']['name']
        }
        
        client.publish("casa/deportes/boca/proximo", json.dumps(mensaje))
    
    client.disconnect()

if __name__ == "__main__":
    verificar_partidos_boca()
```

#### 4. **Estación Meteorológica** (ESP32/Arduino en techo)
```cpp
// Código para ESP32 en el techo con sensores
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* mqtt_server = "192.168.1.X";  // IP de tu Raspberry
const char* mqtt_user = "mqtt_user";
const char* mqtt_pass = "tu_password";

WiFiClient espClient;
PubSubClient client(espClient);

void publicar_datos_meteo() {
  float temp = leerTemperatura();
  float humedad = leerHumedad();
  float presion = leerPresion();
  
  String payload = String("{\"temperatura\":") + temp + 
                   ",\"humedad\":" + humedad + 
                   ",\"presion\":" + presion + "}";
  
  client.publish("casa/meteo/techo/all", payload.c_str());
}
```

### 📟 En ESP32 (Reloj Peronista)

#### Modificaciones Necesarias

```cpp
// Agregar a main.cpp
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* mqtt_server = "192.168.1.X";  // IP de Raspberry
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "tu_password";

// Topics a suscribirse
const char* TOPIC_NOTIFICACIONES = "casa/reloj-peronista/notification";
const char* TOPIC_DEPORTES = "casa/reloj-peronista/deportes";
const char* TOPIC_CALENDARIO = "casa/reloj-peronista/calendar";
const char* TOPIC_METEO = "casa/meteo/techo/all";

// Callback cuando llega mensaje MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en topic: ");
  Serial.println(topic);
  
  // Convertir payload a string
  String mensaje = "";
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }
  
  Serial.println(mensaje);
  
  // Procesar según topic
  if (strcmp(topic, TOPIC_DEPORTES) == 0) {
    // Parsear JSON y mostrar notificación
    mostrarNotificacionDeporte(mensaje);
  }
  else if (strcmp(topic, TOPIC_CALENDARIO) == 0) {
    mostrarNotificacionCalendario(mensaje);
  }
  else if (strcmp(topic, TOPIC_METEO) == 0) {
    actualizarDatosMeteo(mensaje);
  }
}

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando a MQTT...");
    
    if (mqttClient.connect("RelojPeronista", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      
      // Suscribirse a topics
      mqttClient.subscribe(TOPIC_NOTIFICACIONES);
      mqttClient.subscribe(TOPIC_DEPORTES);
      mqttClient.subscribe(TOPIC_CALENDARIO);
      mqttClient.subscribe(TOPIC_METEO);
      
      // Publicar estado
      mqttClient.publish("casa/reloj-peronista/status", "online");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" reintentando en 5 seg...");
      delay(5000);
    }
  }
}

void setup() {
  // ... tu código existente ...
  
  // Configurar MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
}

void loop() {
  // ... tu código existente ...
  
  // Mantener conexión MQTT
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();
}
```

---

## 🔐 Seguridad

### Recomendaciones

1. **Autenticación**: Siempre usar usuario/contraseña
2. **TLS/SSL**: Para conexiones desde internet (puerto 8883)
3. **Firewall**: Limitar acceso solo a tu red local
4. **ACLs**: Configurar permisos por usuario/topic

```bash
# Ejemplo de ACL en /etc/mosquitto/acl
# Reloj Peronista solo puede leer ciertos topics
user reloj_peronista
topic read casa/reloj-peronista/#
topic read casa/meteo/#
topic write casa/reloj-peronista/status

# Estación meteo solo puede escribir datos
user estacion_meteo
topic write casa/meteo/techo/#

# Admin puede todo
user admin
topic #
```

---

## 📦 Formatos de Mensajes (Payloads)

### JSON Estándar

```json
// Notificación deportiva
{
  "tipo": "partido",
  "equipo": "Boca Juniors",
  "rival": "River Plate",
  "hora": "20:00",
  "fecha": "2026-06-15",
  "tiempo_restante_minutos": 120,
  "urgencia": "alta"
}

// Evento calendario
{
  "tipo": "reunion",
  "titulo": "Junta de consorcio",
  "hora": "18:30",
  "duracion_minutos": 60,
  "recordatorio_minutos": 15
}

// Datos meteorológicos
{
  "timestamp": "2026-06-02T09:30:00",
  "temperatura": 18.5,
  "humedad": 72,
  "presion": 1015,
  "viento_kmh": 15,
  "direccion_viento": "SO",
  "lluvia": false
}
```

---

## 🚀 Automatización con Cron

### En Raspberry Pi

```bash
# Editar crontab
crontab -e

# Agregar tareas programadas

# Verificar partidos de Boca cada 30 minutos
*/30 * * * * /usr/bin/python3 ~/mqtt_publishers/deportes_publisher.py

# Verificar calendario cada 15 minutos
*/15 * * * * /usr/bin/python3 ~/mqtt_publishers/calendario_publisher.py

# Publicar hora exacta cada hora (para sincronización)
0 * * * * mosquitto_pub -h localhost -u mqtt_user -P password -t casa/sistema/hora -m "$(date +%s)"

# Verificar feriados una vez al día a las 6 AM
0 6 * * * /usr/bin/python3 ~/mqtt_publishers/feriados_publisher.py
```

---

## 📈 Ventajas de esta Arquitectura

### ✅ Pros
1. **Desacoplamiento**: Publishers y subscribers no se conocen entre sí
2. **Escalabilidad**: Fácil agregar nuevos dispositivos
3. **Persistencia**: Mensajes pueden guardarse si cliente está offline (QoS)
4. **Eficiencia**: Protocolo ligero, ideal para IoT
5. **Flexibilidad**: Un mensaje puede tener múltiples consumidores
6. **Centralización**: Todo pasa por la Raspberry

### ⚠️ Contras
1. **Punto único de fallo**: Si cae Raspberry, cae todo
2. **Requiere red**: Sin WiFi no funciona
3. **Latencia**: Pequeño delay en mensajes
4. **Complejidad inicial**: Más setup que soluciones directas

---

## 🎯 Casos de Uso Específicos

### 1. Notificación de Partido de Boca
```
Script Python (RPi) → consulta API fútbol cada 30min
  ↓
Detecta partido en 2 horas
  ↓
Publica en casa/deportes/boca/proximo
  ↓
Reloj Peronista recibe mensaje
  ↓
Muestra en pantalla: "⚽ Boca vs River - 20:00hs"
  ↓
1 hora antes → publica en casa/deportes/boca/recordatorio
  ↓
Reloj emite sonido + muestra alerta
```

### 2. Estación Meteorológica Integrada
```
Sensor en techo (ESP32) lee temp/hum/presión cada 5min
  ↓
Publica en casa/meteo/techo/all
  ↓
Reloj Peronista se suscribe y recibe datos
  ↓
Muestra datos locales del techo en vez de OpenWeather
  ↓
Ventaja: Datos más precisos y sin depender de API externa
```

### 3. Calendario Familiar
```
Script Python sincroniza con Google Calendar
  ↓
Detecta evento en 30 minutos
  ↓
Publica en casa/calendario/recordatorio
  ↓
Reloj muestra: "📅 Reunión en 30 min"
  ↓
Puede emitir alarma personalizada
```

---

## 🔧 Herramientas de Debug

### Comandos Útiles

```bash
# Suscribirse a todos los mensajes (útil para debug)
mosquitto_sub -h localhost -u mqtt_user -P password -t '#' -v

# Publicar mensaje de prueba
mosquitto_pub -h localhost -u mqtt_user -P password -t casa/reloj-peronista/notification -m '{"texto":"Prueba"}'

# Ver logs del broker
sudo tail -f /var/log/mosquitto/mosquitto.log

# Ver clientes conectados (requiere plugin)
mosquitto_sub -h localhost -u mqtt_user -P password -t '$SYS/broker/clients/connected'
```

### Apps Móviles para Testing
- **MQTT Dash** (Android) - Dashboard personalizable
- **MyMQTT** (iOS) - Cliente simple
- **MQTT Explorer** (Desktop) - Visualización de topics

---

## 📚 Recursos Adicionales

### APIs Útiles
- **Fútbol**: API-Football, Football-Data.org
- **Calendario**: Google Calendar API, Caldav
- **Feriados Argentina**: https://nolaborables.com.ar/api/
- **Clima**: OpenWeather (ya lo tenés), Weather Underground

### Librerías Python
```bash
pip3 install paho-mqtt requests python-dateutil google-api-python-client
```

### Librerías Arduino/ESP32
- PubSubClient (MQTT)
- ArduinoJson (parsing mensajes)

---

## 🎨 Integración con Home Assistant (Opcional)

Si querés llevar esto al siguiente nivel:

```yaml
# configuration.yaml en Home Assistant
mqtt:
  broker: 192.168.1.X
  username: mqtt_user
  password: tu_password

sensor:
  - platform: mqtt
    name: "Temperatura Techo"
    state_topic: "casa/meteo/techo/temperatura"
    unit_of_measurement: "°C"
  
  - platform: mqtt
    name: "Estado Reloj Peronista"
    state_topic: "casa/reloj-peronista/status"

automation:
  - alias: "Notificar cuando llueve"
    trigger:
      platform: mqtt
      topic: casa/meteo/techo/lluvia
      payload: 'true'
    action:
      service: mqtt.publish
      data:
        topic: casa/reloj-peronista/notification
        payload: '{"texto":"¡Está lloviendo!","urgencia":"alta"}'
```

---

## 🏁 Próximos Pasos

### Fase 1 - Setup Básico (1-2 días)
1. Instalar Mosquitto en Raspberry Pi
2. Configurar usuario/contraseña
3. Crear script de prueba Python
4. Modificar código del Reloj Peronista para conectar a MQTT

### Fase 2 - Publishers (1 semana)
1. Script de deportes con API de fútbol
2. Script de calendario (Google Calendar o similar)
3. Script de feriados argentinos

### Fase 3 - Estación Meteorológica (depende de hardware)
1. Ensamblar sensores en techo
2. Programar ESP32 publisher
3. Integrar con reloj

### Fase 4 - Refinamiento
1. Agregar más notificaciones
2. Dashboard web de administración
3. Integración con Home Assistant

---

## 💡 Conclusión

**SÍ, tu comprensión es correcta**: 

> "Alguien debería informar al MQTT con algún tipo de tópico y por cada tópico los clientes consumen o no"

Exactamente. El flujo es:

1. **Publishers** (informantes) → Publican mensajes en TOPICS específicos
2. **Broker MQTT** (Raspberry) → Recibe y redistribuye mensajes
3. **Subscribers** (clientes) → Se suscriben a TOPICS de interés y reciben mensajes

Es como un sistema de radio: publishers son las estaciones, broker es el aire, y subscribers son las radios que sintonizan las estaciones que les interesan.

La belleza de MQTT es que los publishers no necesitan saber quiénes son los subscribers, y viceversa. Todos solo hablan con el broker central.

---

**¿Preguntas? ¿Necesitás que desarrolle alguna parte en específico?**
