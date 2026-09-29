lgun registro?# 📅 Integración Google Calendar → MQTT → Reloj Peronista

## 🎯 Objetivo

Sincronizar eventos de tu Google Calendar y enviarlos al Reloj Peronista mediante MQTT para mostrar recordatorios.

---

## 🏗️ Arquitectura del Flujo

```
┌──────────────────────────────────────────────────────────────┐
│                     GOOGLE CALENDAR                          │
│             (tus eventos en la nube)                         │
└───────────────────────┬──────────────────────────────────────┘
                        │ API REST
                        ↓
┌──────────────────────────────────────────────────────────────┐
│                   RASPBERRY PI                                │
│                                                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  Script Python (calendario_publisher.py)            │    │
│  │  - Lee eventos próximos cada 15 minutos            │    │
│  │  - Detecta eventos en próximas 2 horas             │    │
│  │  - Publica en MQTT topic                           │    │
│  └───────────────────┬─────────────────────────────────┘    │
│                      ↓                                        │
│  ┌─────────────────────────────────────────────────────┐    │
│  │          MQTT Broker (Mosquitto)                    │    │
│  │          Topic: casa/calendario/recordatorio       │    │
│  └───────────────────┬─────────────────────────────────┘    │
└────────────────────────┬─────────────────────────────────────┘
                         │ WiFi
                         ↓
┌──────────────────────────────────────────────────────────────┐
│                   RELOJ PERONISTA (ESP32)                    │
│                                                               │
│  - Suscrito a: casa/calendario/recordatorio                 │
│  - Recibe JSON con evento                                    │
│  - Parsea y muestra en pantalla TFT                         │
│  - Opcionalmente emite alarma/buzzer                         │
└──────────────────────────────────────────────────────────────┘
```

---

## 📋 Requisitos Previos

### 1. Cuenta de Google
- Tener una cuenta de Google con Calendar activo
- Eventos creados en tu calendario

### 2. En Raspberry Pi
```bash
# Python 3 instalado
sudo apt update
sudo apt install python3 python3-pip

# Librerías necesarias
pip3 install --upgrade google-api-python-client google-auth-httplib2 google-auth-oauthlib paho-mqtt
```

### 3. Credenciales de Google Cloud
- Crear proyecto en Google Cloud Console
- Habilitar Google Calendar API
- Descargar credenciales OAuth 2.0

---

## 🔑 Paso 1: Configurar Google Calendar API

### 1.1 Crear Proyecto en Google Cloud

1. Ve a [Google Cloud Console](https://console.cloud.google.com/)
2. Crea un nuevo proyecto: "RelojPeronista"
3. Selecciona el proyecto

### 1.2 Habilitar Calendar API

1. En el menú → "APIs & Services" → "Library"
2. Busca "Google Calendar API"
3. Click en "Enable"

### 1.3 Crear Credenciales OAuth

1. En "APIs & Services" → "Credentials"
2. Click "+ CREATE CREDENTIALS" → "OAuth client ID"
3. Si es la primera vez, configurar "OAuth consent screen":
   - User Type: **External** (si no tienes Google Workspace)
   - App name: "Reloj Peronista"
   - User support email: tu email
   - Developer contact: tu email
   - Scopes: No agregar ninguno aún
   - Test users: **Agregar tu email** (importante!)
   - Guardar y continuar
4. Volver a "Credentials" → "Create OAuth client ID"
   - Application type: **Desktop app**
   - Name: "RelojPeronistaDesktop"
5. **DESCARGAR** el archivo JSON (credentials.json)
6. Guardar en Raspberry Pi: `~/mqtt_publishers/credentials.json`

### 1.4 Primer Autorización (solo una vez)

La primera vez que corras el script, se abrirá un navegador para autorizar:
```bash
# Esto generará un token.json que se usará en adelante
python3 calendario_publisher.py
```

**Importante**: Si tu Raspberry no tiene interfaz gráfica (headless), puedes:
1. Ejecutar el script en tu PC primero
2. Copiar el `token.json` generado a la Raspberry
3. O usar el modo "remote authorization"

---

## 📝 Paso 2: Script Python para Raspberry Pi

### 2.1 Script Completo: `calendario_publisher.py`

```python
#!/usr/bin/env python3
"""
Script para leer eventos de Google Calendar y publicarlos en MQTT
Se ejecuta periódicamente (cada 15 minutos con cron)
"""

import os
import json
import datetime
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build
import paho.mqtt.client as mqtt

# ========== CONFIGURACIÓN ==========

# MQTT
MQTT_BROKER = "localhost"  # o IP de tu Raspberry si corres esto desde otro lugar
MQTT_PORT = 1883
MQTT_USER = "mqtt_user"
MQTT_PASS = "tu_password"
MQTT_TOPIC_BASE = "casa/calendario"

# Google Calendar API
SCOPES = ['https://www.googleapis.com/auth/calendar.readonly']
CREDENTIALS_FILE = 'credentials.json'
TOKEN_FILE = 'token.json'

# Configuración de notificaciones
MINUTOS_ANTICIPACION = [120, 60, 30, 15, 5]  # Notificar en estos momentos antes del evento

# ========== FUNCIONES ==========

def obtener_credenciales():
    """
    Obtiene o refresca las credenciales de Google Calendar API
    """
    creds = None
    
    # El archivo token.json almacena los tokens de acceso/refresco del usuario
    if os.path.exists(TOKEN_FILE):
        creds = Credentials.from_authorized_user_file(TOKEN_FILE, SCOPES)
    
    # Si no hay credenciales válidas, autenticar
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(CREDENTIALS_FILE, SCOPES)
            creds = flow.run_local_server(port=0)
        
        # Guardar credenciales para la próxima ejecución
        with open(TOKEN_FILE, 'w') as token:
            token.write(creds.to_json())
    
    return creds

def obtener_eventos_proximos(service, minutos=180):
    """
    Obtiene eventos que ocurrirán en los próximos X minutos
    
    Args:
        service: Objeto del servicio de Google Calendar
        minutos: Ventana de tiempo a consultar (default: 3 horas)
    
    Returns:
        Lista de eventos con formato simplificado
    """
    now = datetime.datetime.utcnow()
    time_min = now.isoformat() + 'Z'  # 'Z' indica UTC
    time_max = (now + datetime.timedelta(minutes=minutos)).isoformat() + 'Z'
    
    print(f"🔍 Buscando eventos entre {now.strftime('%H:%M')} y {(now + datetime.timedelta(minutes=minutos)).strftime('%H:%M')}")
    
    # Llamar a la API
    events_result = service.events().list(
        calendarId='primary',
        timeMin=time_min,
        timeMax=time_max,
        singleEvents=True,
        orderBy='startTime',
        maxResults=10
    ).execute()
    
    events = events_result.get('items', [])
    
    eventos_formateados = []
    
    for event in events:
        start = event['start'].get('dateTime', event['start'].get('date'))
        
        # Parsear fecha
        if 'T' in start:  # Evento con hora
            start_dt = datetime.datetime.fromisoformat(start.replace('Z', '+00:00'))
        else:  # Evento de día completo
            start_dt = datetime.datetime.fromisoformat(start + 'T00:00:00')
        
        # Calcular minutos restantes
        ahora = datetime.datetime.now(start_dt.tzinfo)
        minutos_restantes = int((start_dt - ahora).total_seconds() / 60)
        
        evento_formateado = {
            'titulo': event.get('summary', 'Sin título'),
            'descripcion': event.get('description', ''),
            'inicio': start_dt.strftime('%Y-%m-%d %H:%M'),
            'hora': start_dt.strftime('%H:%M'),
            'fecha': start_dt.strftime('%d/%m'),
            'minutos_restantes': minutos_restantes,
            'ubicacion': event.get('location', ''),
            'id': event['id']
        }
        
        eventos_formateados.append(evento_formateado)
        print(f"  📅 {evento_formateado['titulo']} - {evento_formateado['hora']} (en {minutos_restantes} min)")
    
    return eventos_formateados

def publicar_evento_mqtt(client, evento):
    """
    Publica un evento en MQTT
    """
    # Topic según tipo de notificación
    if evento['minutos_restantes'] <= 5:
        topic = f"{MQTT_TOPIC_BASE}/urgente"
        urgencia = "alta"
    elif evento['minutos_restantes'] <= 30:
        topic = f"{MQTT_TOPIC_BASE}/recordatorio"
        urgencia = "media"
    else:
        topic = f"{MQTT_TOPIC_BASE}/proximo"
        urgencia = "baja"
    
    # Payload JSON
    payload = {
        'titulo': evento['titulo'],
        'hora': evento['hora'],
        'fecha': evento['fecha'],
        'minutos_restantes': evento['minutos_restantes'],
        'urgencia': urgencia,
        'ubicacion': evento['ubicacion']
    }
    
    # Publicar
    result = client.publish(topic, json.dumps(payload), qos=1, retain=False)
    
    if result.rc == mqtt.MQTT_ERR_SUCCESS:
        print(f"✅ Publicado en {topic}: {evento['titulo']}")
    else:
        print(f"❌ Error al publicar: {result.rc}")

def cargar_eventos_notificados():
    """
    Carga el registro de eventos ya notificados para evitar duplicados
    """
    try:
        with open('eventos_notificados.json', 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        return {}

def guardar_evento_notificado(evento_id, minutos):
    """
    Guarda que ya se notificó un evento en determinado momento
    """
    notificados = cargar_eventos_notificados()
    
    if evento_id not in notificados:
        notificados[evento_id] = []
    
    notificados[evento_id].append(minutos)
    
    # Limpiar eventos antiguos (más de 24 horas)
    ahora = datetime.datetime.now()
    for eid in list(notificados.keys()):
        # Si tiene más de 100 entradas, es viejo
        if len(notificados[eid]) > 10:
            del notificados[eid]
    
    with open('eventos_notificados.json', 'w') as f:
        json.dump(notificados, f)

def main():
    """
    Función principal
    """
    print("\n" + "="*60)
    print("📅 GOOGLE CALENDAR → MQTT PUBLISHER")
    print("="*60)
    
    # 1. Autenticar con Google
    print("\n🔐 Autenticando con Google Calendar...")
    creds = obtener_credenciales()
    service = build('calendar', 'v3', credentials=creds)
    print("✅ Autenticación exitosa")
    
    # 2. Obtener eventos próximos (próximas 3 horas)
    print("\n📅 Obteniendo eventos próximos...")
    eventos = obtener_eventos_proximos(service, minutos=180)
    
    if not eventos:
        print("ℹ️  No hay eventos próximos")
        return
    
    print(f"✅ Encontrados {len(eventos)} eventos")
    
    # 3. Conectar a MQTT
    print("\n🔌 Conectando a MQTT broker...")
    client = mqtt.Client(client_id="google_calendar_publisher")
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        print("✅ Conectado a MQTT")
    except Exception as e:
        print(f"❌ Error conectando a MQTT: {e}")
        return
    
    # 4. Procesar eventos y publicar
    print("\n📤 Publicando eventos...")
    notificados = cargar_eventos_notificados()
    
    for evento in eventos:
        # Determinar si hay que notificar según minutos restantes
        minutos = evento['minutos_restantes']
        evento_id = evento['id']
        
        # Buscar el umbral de notificación más cercano
        umbral_notificacion = None
        for umbral in MINUTOS_ANTICIPACION:
            if minutos <= umbral:
                umbral_notificacion = umbral
        
        if umbral_notificacion is None:
            continue  # Evento muy lejano
        
        # Verificar si ya se notificó en este umbral
        if evento_id in notificados and umbral_notificacion in notificados[evento_id]:
            print(f"⏭️  Ya notificado: {evento['titulo']} ({minutos} min)")
            continue
        
        # Publicar evento
        publicar_evento_mqtt(client, evento)
        
        # Registrar notificación
        guardar_evento_notificado(evento_id, umbral_notificacion)
    
    # 5. Desconectar
    client.disconnect()
    print("\n✅ Proceso completado")
    print("="*60 + "\n")

if __name__ == '__main__':
    main()
```

### 2.2 Archivo de Eventos Notificados

El script crea automáticamente `eventos_notificados.json` para llevar registro y evitar notificar el mismo evento múltiples veces.

---

## ⚙️ Paso 3: Configurar Ejecución Automática (Cron)

### 3.1 Hacer el script ejecutable
```bash
chmod +x ~/mqtt_publishers/calendario_publisher.py
```

### 3.2 Agregar a crontab
```bash
crontab -e
```

Agregar esta línea:
```bash
# Ejecutar cada 15 minutos
*/15 * * * * cd ~/mqtt_publishers && /usr/bin/python3 calendario_publisher.py >> ~/logs/calendario.log 2>&1
```

### 3.3 Crear directorio de logs
```bash
mkdir -p ~/logs
```

### 3.4 Ver logs en tiempo real
```bash
tail -f ~/logs/calendario.log
```

---

## 📟 Paso 4: Modificar ESP32 (Reloj Peronista)

### 4.1 Agregar a `platformio.ini`
```ini
lib_deps =
    ; ... tus librerías existentes ...
    knolleary/PubSubClient@^2.8
```

### 4.2 Agregar a `main.cpp`

```cpp
// ========== AGREGAR AL INICIO ==========
#include <PubSubClient.h>

// Cliente MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Configuración MQTT
const char* mqtt_server = "192.168.1.100";  // IP de tu Raspberry
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "tu_password";

// Topics
const char* TOPIC_CALENDARIO_URGENTE = "casa/calendario/urgente";
const char* TOPIC_CALENDARIO_RECORDATORIO = "casa/calendario/recordatorio";
const char* TOPIC_CALENDARIO_PROXIMO = "casa/calendario/proximo";

// Cola de notificaciones
struct Notificacion {
  String titulo;
  String hora;
  String urgencia;
  int minutos_restantes;
  unsigned long timestamp;
};

#define MAX_NOTIFICACIONES 5
Notificacion cola_notificaciones[MAX_NOTIFICACIONES];
int num_notificaciones = 0;

// ========== CALLBACK MQTT ==========

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("\n📬 Mensaje MQTT recibido en: %s\n", topic);
  
  // Convertir payload a String
  String mensaje = "";
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }
  
  Serial.println("Payload: " + mensaje);
  
  // Parsear JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, mensaje);
  
  if (error) {
    Serial.printf("❌ Error parsing JSON: %s\n", error.c_str());
    return;
  }
  
  // Extraer datos
  String titulo = doc["titulo"].as<String>();
  String hora = doc["hora"].as<String>();
  String urgencia = doc["urgencia"].as<String>();
  int minutos = doc["minutos_restantes"];
  
  Serial.printf("  📅 Evento: %s\n", titulo.c_str());
  Serial.printf("  🕐 Hora: %s\n", hora.c_str());
  Serial.printf("  ⏰ En %d minutos\n", minutos);
  Serial.printf("  🚨 Urgencia: %s\n", urgencia.c_str());
  
  // Agregar a cola de notificaciones
  if (num_notificaciones < MAX_NOTIFICACIONES) {
    cola_notificaciones[num_notificaciones].titulo = titulo;
    cola_notificaciones[num_notificaciones].hora = hora;
    cola_notificaciones[num_notificaciones].urgencia = urgencia;
    cola_notificaciones[num_notificaciones].minutos_restantes = minutos;
    cola_notificaciones[num_notificaciones].timestamp = millis();
    num_notificaciones++;
  }
  
  // Si es urgente (5 min o menos), emitir sonido
  if (urgencia == "alta" && minutos <= 5) {
    // Emitir tono de alarma corto
    tone(BUZZER_PIN, 2000, 200);
    delay(300);
    tone(BUZZER_PIN, 2500, 200);
  }
  
  // Forzar actualización de pantalla
  displayAllInfo();
}

// ========== CONECTAR MQTT ==========

void conectarMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ No hay WiFi, no se puede conectar MQTT");
    return;
  }
  
  while (!mqttClient.connected()) {
    Serial.print("🔌 Conectando a MQTT broker...");
    
    // Generar client ID único
    String clientId = "RelojPeronista-";
    clientId += String(ESP.getEfuseMac() & 0xFFFF, HEX);
    
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println(" ✅ Conectado!");
      
      // Suscribirse a topics de calendario
      mqttClient.subscribe(TOPIC_CALENDARIO_URGENTE);
      mqttClient.subscribe(TOPIC_CALENDARIO_RECORDATORIO);
      mqttClient.subscribe(TOPIC_CALENDARIO_PROXIMO);
      
      Serial.println("✅ Suscrito a topics de calendario");
      
      // Publicar estado online
      mqttClient.publish("casa/reloj-peronista/status", "online");
      
    } else {
      Serial.printf(" ❌ Fallo, rc=%d\n", mqttClient.state());
      Serial.println("Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

// ========== MOSTRAR NOTIFICACIONES EN PANTALLA ==========

void mostrarNotificacionesCalendario() {
  if (num_notificaciones == 0) return;
  
  // Mostrar solo la notificación más reciente/urgente
  Notificacion &notif = cola_notificaciones[num_notificaciones - 1];
  
  // Calcular posición (abajo en la pantalla)
  int16_t y_notif = 280;
  
  // Fondo según urgencia
  uint16_t bgColor = TFT_BLUE;
  if (notif.urgencia == "alta") bgColor = TFT_RED;
  else if (notif.urgencia == "media") bgColor = TFT_ORANGE;
  
  // Dibujar barra de notificación
  tft.fillRect(0, y_notif, 240, 40, bgColor);
  
  // Texto
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, bgColor);
  tft.setCursor(5, y_notif + 5);
  tft.printf("📅 %s", notif.titulo.c_str());
  
  tft.setCursor(5, y_notif + 20);
  tft.printf("En %d min - %s", notif.minutos_restantes, notif.hora.c_str());
  
  // Limpiar notificaciones antiguas (más de 10 minutos)
  unsigned long ahora = millis();
  for (int i = 0; i < num_notificaciones; i++) {
    if (ahora - cola_notificaciones[i].timestamp > 600000) {  // 10 min
      // Mover todas las siguientes
      for (int j = i; j < num_notificaciones - 1; j++) {
        cola_notificaciones[j] = cola_notificaciones[j + 1];
      }
      num_notificaciones--;
      i--;
    }
  }
}

// ========== MODIFICAR setup() ==========

void setup() {
  // ... tu código existente ...
  
  // Configurar MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setBufferSize(512);  // Aumentar buffer para JSON más grandes
  
  // Conectar a MQTT
  if (WiFi.status() == WL_CONNECTED) {
    conectarMQTT();
  }
}

// ========== MODIFICAR loop() ==========

void loop() {
  // ... tu código existente ANTES de displayAllInfo() ...
  
  // Mantener conexión MQTT
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected()) {
      conectarMQTT();
    }
    mqttClient.loop();
  }
  
  // ... resto de tu código ...
  
  // Al final, después de displayAllInfo(), mostrar notificaciones
  mostrarNotificacionesCalendario();
}
```

---

## 🧪 Paso 5: Probar el Sistema

### 5.1 Test Manual del Script Python

```bash
cd ~/mqtt_publishers
python3 calendario_publisher.py
```

Deberías ver:
```
============================================================
📅 GOOGLE CALENDAR → MQTT PUBLISHER
============================================================

🔐 Autenticando con Google Calendar...
✅ Autenticación exitosa

📅 Obteniendo eventos próximos...
  📅 Reunión de trabajo - 15:30 (en 45 min)
  📅 Cumpleaños Juan - 19:00 (en 165 min)
✅ Encontrados 2 eventos

🔌 Conectando a MQTT broker...
✅ Conectado a MQTT

📤 Publicando eventos...
✅ Publicado en casa/calendario/recordatorio: Reunión de trabajo

✅ Proceso completado
============================================================
```

### 5.2 Monitorear MQTT en Tiempo Real

En otra terminal de la Raspberry:
```bash
mosquitto_sub -h localhost -u mqtt_user -P tu_password -t 'casa/calendario/#' -v
```

### 5.3 Crear Evento de Prueba en Google Calendar

1. Abre Google Calendar en tu navegador
2. Crea un evento para dentro de 10 minutos
3. Espera a que el script se ejecute (o ejecútalo manualmente)
4. Observa el monitor MQTT y el serial del ESP32

### 5.4 Verificar en el Reloj

- La notificación debería aparecer en la parte inferior de la pantalla
- Si faltan 5 minutos o menos, debería emitir un tono

---

## 🎨 Personalización

### Cambiar Umbrales de Notificación

En `calendario_publisher.py`:
```python
MINUTOS_ANTICIPACION = [120, 60, 30, 15, 5]  # Modifica según necesites
```

### Filtrar por Calendario Específico

Si tienes múltiples calendarios:
```python
# En obtener_eventos_proximos(), cambiar:
events_result = service.events().list(
    calendarId='email@gmail.com',  # O ID específico del calendario
    # ...
)
```

### Palabras Clave para Filtrar

Mostrar solo eventos con ciertas palabras:
```python
# En main(), después de obtener eventos:
eventos_filtrados = [e for e in eventos if 'trabajo' in e['titulo'].lower()]
```

---

## 🐛 Troubleshooting

### Error: "credentials.json not found"
```bash
# Verificar que el archivo existe
ls -la ~/mqtt_publishers/credentials.json
```

### Error: "User not in test users"
- Ve a Google Cloud Console
- OAuth consent screen → Test users
- Agrega tu email

### Error: "token expired"
```bash
# Borrar token y reautenticar
rm ~/mqtt_publishers/token.json
python3 calendario_publisher.py
```

### ESP32 no recibe mensajes
```bash
# Verificar conexión MQTT desde PC
mosquitto_sub -h IP_RASPBERRY -u mqtt_user -P password -t '#'

# Verificar que ESP32 está conectado
mosquitto_pub -h localhost -u mqtt_user -P password -t casa/reloj-peronista/test -m "hola"
```

---

## 📊 Formato de Mensajes MQTT

### Topic: `casa/calendario/recordatorio`
```json
{
  "titulo": "Reunión con cliente",
  "hora": "15:30",
  "fecha": "02/06",
  "minutos_restantes": 30,
  "urgencia": "media",
  "ubicacion": "Oficina Central"
}
```

### Topic: `casa/calendario/urgente`
```json
{
  "titulo": "Llamada importante",
  "hora": "14:05",
  "fecha": "02/06",
  "minutos_restantes": 3,
  "urgencia": "alta",
  "ubicacion": ""
}
```

---

## 🚀 Mejoras Futuras

1. **Múltiples calendarios**: Leer de varios calendarios (trabajo, personal, etc.)
2. **Categorización**: Colores diferentes según tipo de evento
3. **Respuesta de voz**: Integrar con TTS para anunciar eventos
4. **Integración con alarma**: Usar evento de calendario como alarma
5. **Dashboard web**: Ver próximos eventos en interfaz web
6. **Sincronización bidireccional**: Crear eventos desde el reloj

---

## ✅ Resumen

Este sistema te permite:
- ✅ Leer eventos de Google Calendar automáticamente
- ✅ Enviarlos al Reloj Peronista vía MQTT
- ✅ Mostrar recordatorios en pantalla
- ✅ Emitir alertas sonoras para eventos urgentes
- ✅ Todo sin intervención manual

Una vez configurado, funciona automáticamente 24/7. Solo necesitas crear eventos en tu Google Calendar como siempre y el reloj te avisará.
