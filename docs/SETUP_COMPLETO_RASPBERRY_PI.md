# 🚀 Setup Completo MQTT + Google Calendar - Raspberry Pi

Este es un prompt copiable y pegable para configurar tu Raspberry Pi con:
- ✅ Broker MQTT (Mosquitto) con autenticación
- ✅ Sistema de credenciales autogeneradas
- ✅ ACLs (control de permisos)
- ✅ Publisher de Google Calendar
- ✅ Publishers de ejemplo (deportes, etc.)

---

## 📋 ÍNDICE DE BLOQUES COPIABLES

1. [Instalación Mosquitto](#1-instalación-mosquitto)
2. [Scripts de Seguridad](#2-scripts-de-seguridad)
3. [Configuración ACL](#3-configuración-acl)
4. [Publisher Google Calendar](#4-publisher-google-calendar)
5. [Publishers Ejemplo](#5-publishers-ejemplo)
6. [Configuración Cron](#6-configuración-cron)
7. [Testing y Verificación](#7-testing-y-verificación)

---

## 1. INSTALACIÓN MOSQUITTO

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 1: INSTALAR MOSQUITTO
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Actualizar sistema
sudo apt update && sudo apt upgrade -y

# Instalar Mosquitto broker y cliente
sudo apt install -y mosquitto mosquitto-clients

# Habilitar inicio automático
sudo systemctl enable mosquitto

# Crear directorios necesarios
mkdir -p ~/mqtt_credentials
mkdir -p ~/mqtt_publishers
mkdir -p ~/scripts
mkdir -p ~/logs

# Verificar instalación
mosquitto -h
```

---

## 2. SCRIPTS DE SEGURIDAD

### Script 1: Generar Credenciales

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 2A: CREAR SCRIPT GENERADOR DE CREDENCIALES
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat > ~/scripts/generar_credencial_mqtt.sh << 'EOFSCRIPT'
#!/bin/bash
# Script para generar credenciales únicas para dispositivos MQTT

if [ -z "$1" ]; then
    echo "❌ Uso: $0 <nombre_dispositivo>"
    echo "Ejemplo: $0 reloj_001"
    exit 1
fi

DEVICE_NAME=$1
PASSWD_FILE="/etc/mosquitto/passwd"
CREDENTIALS_DIR="$HOME/mqtt_credentials"

mkdir -p "$CREDENTIALS_DIR"

# Generar contraseña aleatoria segura (32 caracteres)
PASSWORD=$(openssl rand -base64 32 | tr -d "=+/" | cut -c1-32)

echo "🔐 Generando credenciales para: $DEVICE_NAME"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Usar mosquitto_passwd para agregar/actualizar usuario
sudo mosquitto_passwd -b "$PASSWD_FILE" "$DEVICE_NAME" "$PASSWORD"

if [ $? -eq 0 ]; then
    echo "✅ Usuario creado en Mosquitto"
    
    # Guardar credenciales
    CRED_FILE="$CREDENTIALS_DIR/${DEVICE_NAME}_credentials.txt"
    cat > "$CRED_FILE" << EOF
# Credenciales MQTT para $DEVICE_NAME
# Generado: $(date)
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Usuario: $DEVICE_NAME
Contraseña: $PASSWORD
Broker: localhost (o IP de Raspberry)
Puerto: 1883

# Para ESP32 (Arduino/C++):
const char* mqtt_user = "$DEVICE_NAME";
const char* mqtt_password = "$PASSWORD";

# Para Python:
MQTT_USER = "$DEVICE_NAME"
MQTT_PASS = "$PASSWORD"
EOF
    
    chmod 600 "$CRED_FILE"
    
    echo "✅ Credenciales guardadas en: $CRED_FILE"
    echo ""
    echo "📋 CREDENCIALES GENERADAS:"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "Usuario:     $DEVICE_NAME"
    echo "Contraseña:  $PASSWORD"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "⚠️  IMPORTANTE: Guarda estas credenciales de forma segura"
    echo ""
    echo "🔄 Recuerda reiniciar Mosquitto:"
    echo "   sudo systemctl restart mosquitto"
else
    echo "❌ Error al crear usuario"
    exit 1
fi
EOFSCRIPT

# Hacer ejecutable
chmod +x ~/scripts/generar_credencial_mqtt.sh

echo "✅ Script generar_credencial_mqtt.sh creado"
```

### Script 2: Auditoría

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 2B: CREAR SCRIPT DE AUDITORÍA
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat > ~/scripts/auditoria_mqtt.sh << 'EOFSCRIPT'
#!/bin/bash

echo "🔍 AUDITORÍA MQTT - $(date)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo ""
echo "👥 USUARIOS REGISTRADOS:"
sudo cat /etc/mosquitto/passwd | cut -d':' -f1 | nl

echo ""
echo "🔐 ÚLTIMOS INTENTOS FALLIDOS (últimas 24h):"
sudo journalctl -u mosquitto --since "24 hours ago" | \
  grep -i "authentication failed" | tail -10 || echo "  (ninguno)"

echo ""
echo "✅ CONEXIONES EXITOSAS (últimas 24h):"
sudo journalctl -u mosquitto --since "24 hours ago" | \
  grep -i "client .* connected" | wc -l

echo ""
echo "📊 ESTADO DEL BROKER:"
sudo systemctl status mosquitto | grep "Active:"

echo ""
echo "🌐 CLIENTES CONECTADOS AHORA:"
sudo netstat -tnp 2>/dev/null | grep :1883 | wc -l
EOFSCRIPT

chmod +x ~/scripts/auditoria_mqtt.sh

echo "✅ Script auditoria_mqtt.sh creado"
```

---

## 3. CONFIGURACIÓN ACL

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 3A: CONFIGURAR MOSQUITTO.CONF
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Backup del archivo original
sudo cp /etc/mosquitto/mosquitto.conf /etc/mosquitto/mosquitto.conf.backup

# Crear configuración
sudo tee /etc/mosquitto/mosquitto.conf > /dev/null << 'EOF'
# Mosquitto Configuration for IoT Home System
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Listener
listener 1883
protocol mqtt

# Seguridad
allow_anonymous false
password_file /etc/mosquitto/passwd
acl_file /etc/mosquitto/acl

# Persistencia
persistence true
persistence_location /var/lib/mosquitto/

# Logging
log_dest file /var/log/mosquitto/mosquitto.log
log_type all
log_timestamp true

# Conexiones
max_connections -1
EOF

echo "✅ mosquitto.conf configurado"

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 3B: CREAR ARCHIVO ACL
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

sudo tee /etc/mosquitto/acl > /dev/null << 'EOF'
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# MOSQUITTO ACCESS CONTROL LIST (ACL)
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# ADMIN (acceso completo)
user admin
topic readwrite #

# RELOJ PERONISTA
user reloj_peronista_001
topic read casa/reloj-peronista/#
topic read casa/meteo/#
topic read casa/calendario/#
topic read casa/deportes/#
topic write casa/reloj-peronista/status
topic write casa/reloj-peronista/heartbeat
topic write casa/reloj-peronista/sensores/#

# ESTACIÓN METEOROLÓGICA
user estacion_meteo_techo
topic write casa/meteo/techo/#
topic write casa/meteo/techo/status

# PUBLISHER DE CALENDARIO
user publisher_calendario
topic write casa/calendario/#

# PUBLISHER DE DEPORTES
user publisher_deportes
topic write casa/deportes/#

# PUBLISHER DE FERIADOS
user publisher_feriados
topic write casa/calendario/feriado/#
EOF

echo "✅ ACL configurado"

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 3C: GENERAR CREDENCIALES INICIALES
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Generar credenciales para cada usuario
~/scripts/generar_credencial_mqtt.sh admin
~/scripts/generar_credencial_mqtt.sh reloj_peronista_001
~/scripts/generar_credencial_mqtt.sh estacion_meteo_techo
~/scripts/generar_credencial_mqtt.sh publisher_calendario
~/scripts/generar_credencial_mqtt.sh publisher_deportes
~/scripts/generar_credencial_mqtt.sh publisher_feriados

# Reiniciar Mosquitto
sudo systemctl restart mosquitto

echo ""
echo "✅ Mosquitto reiniciado con nueva configuración"
echo ""
echo "📂 Credenciales guardadas en: ~/mqtt_credentials/"
echo ""
```

---

## 4. PUBLISHER GOOGLE CALENDAR

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 4A: INSTALAR DEPENDENCIAS PYTHON
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Instalar pip si no está
sudo apt install -y python3-pip

# Instalar librerías necesarias
pip3 install --upgrade google-api-python-client google-auth-httplib2 google-auth-oauthlib paho-mqtt

echo "✅ Dependencias Python instaladas"

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 4B: CREAR PUBLISHER DE GOOGLE CALENDAR
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat > ~/mqtt_publishers/calendario_publisher.py << 'EOFPYTHON'
#!/usr/bin/env python3
"""
Publisher de Google Calendar a MQTT
Lee eventos y publica notificaciones
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

# MQTT (CAMBIAR CON TUS CREDENCIALES)
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "publisher_calendario"  # Obtener de ~/mqtt_credentials/
MQTT_PASS = "CAMBIAR_POR_PASSWORD_GENERADA"
MQTT_TOPIC_BASE = "casa/calendario"

# Google Calendar API
SCOPES = ['https://www.googleapis.com/auth/calendar.readonly']
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CREDENTIALS_FILE = os.path.join(SCRIPT_DIR, 'credentials.json')
TOKEN_FILE = os.path.join(SCRIPT_DIR, 'token.json')

# Configuración de notificaciones
MINUTOS_ANTICIPACION = [120, 60, 30, 15, 5]

# ========== FUNCIONES ==========

def obtener_credenciales():
    """Obtiene o refresca las credenciales de Google Calendar API"""
    creds = None
    
    if os.path.exists(TOKEN_FILE):
        creds = Credentials.from_authorized_user_file(TOKEN_FILE, SCOPES)
    
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(CREDENTIALS_FILE, SCOPES)
            creds = flow.run_local_server(port=0)
        
        with open(TOKEN_FILE, 'w') as token:
            token.write(creds.to_json())
    
    return creds

def obtener_eventos_proximos(service, minutos=180):
    """Obtiene eventos que ocurrirán en los próximos X minutos"""
    now = datetime.datetime.utcnow()
    time_min = now.isoformat() + 'Z'
    time_max = (now + datetime.timedelta(minutes=minutos)).isoformat() + 'Z'
    
    print(f"🔍 Buscando eventos entre {now.strftime('%H:%M')} y {(now + datetime.timedelta(minutes=minutos)).strftime('%H:%M')}")
    
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
        
        if 'T' in start:
            start_dt = datetime.datetime.fromisoformat(start.replace('Z', '+00:00'))
        else:
            start_dt = datetime.datetime.fromisoformat(start + 'T00:00:00')
        
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
    """Publica un evento en MQTT"""
    if evento['minutos_restantes'] <= 5:
        topic = f"{MQTT_TOPIC_BASE}/urgente"
        urgencia = "alta"
    elif evento['minutos_restantes'] <= 30:
        topic = f"{MQTT_TOPIC_BASE}/recordatorio"
        urgencia = "media"
    else:
        topic = f"{MQTT_TOPIC_BASE}/proximo"
        urgencia = "baja"
    
    payload = {
        'titulo': evento['titulo'],
        'hora': evento['hora'],
        'fecha': evento['fecha'],
        'minutos_restantes': evento['minutos_restantes'],
        'urgencia': urgencia,
        'ubicacion': evento['ubicacion']
    }
    
    result = client.publish(topic, json.dumps(payload), qos=1, retain=False)
    
    if result.rc == mqtt.MQTT_ERR_SUCCESS:
        print(f"✅ Publicado en {topic}: {evento['titulo']}")
    else:
        print(f"❌ Error al publicar: {result.rc}")

def cargar_eventos_notificados():
    """Carga el registro de eventos ya notificados"""
    try:
        with open(os.path.join(SCRIPT_DIR, 'eventos_notificados.json'), 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        return {}

def guardar_evento_notificado(evento_id, minutos):
    """Guarda que ya se notificó un evento"""
    notificados = cargar_eventos_notificados()
    
    if evento_id not in notificados:
        notificados[evento_id] = []
    
    notificados[evento_id].append(minutos)
    
    # Limpiar eventos antiguos
    for eid in list(notificados.keys()):
        if len(notificados[eid]) > 10:
            del notificados[eid]
    
    with open(os.path.join(SCRIPT_DIR, 'eventos_notificados.json'), 'w') as f:
        json.dump(notificados, f)

def main():
    """Función principal"""
    print("\n" + "="*60)
    print("📅 GOOGLE CALENDAR → MQTT PUBLISHER")
    print("="*60)
    
    # 1. Autenticar con Google
    print("\n🔐 Autenticando con Google Calendar...")
    creds = obtener_credenciales()
    service = build('calendar', 'v3', credentials=creds)
    print("✅ Autenticación exitosa")
    
    # 2. Obtener eventos próximos
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
        minutos = evento['minutos_restantes']
        evento_id = evento['id']
        
        umbral_notificacion = None
        for umbral in MINUTOS_ANTICIPACION:
            if minutos <= umbral:
                umbral_notificacion = umbral
        
        if umbral_notificacion is None:
            continue
        
        if evento_id in notificados and umbral_notificacion in notificados[evento_id]:
            print(f"⏭️  Ya notificado: {evento['titulo']} ({minutos} min)")
            continue
        
        publicar_evento_mqtt(client, evento)
        guardar_evento_notificado(evento_id, umbral_notificacion)
    
    # 5. Desconectar
    client.disconnect()
    print("\n✅ Proceso completado")
    print("="*60 + "\n")

if __name__ == '__main__':
    main()
EOFPYTHON

chmod +x ~/mqtt_publishers/calendario_publisher.py

echo "✅ Publisher de Google Calendar creado"
echo ""
echo "⚠️  PASOS SIGUIENTES:"
echo "1. Conseguir credentials.json de Google Cloud Console"
echo "2. Copiarlo a ~/mqtt_publishers/credentials.json"
echo "3. Editar calendario_publisher.py y cambiar MQTT_PASS"
echo "4. Ejecutar manualmente para autorizar: python3 ~/mqtt_publishers/calendario_publisher.py"
echo ""
```

---

## 5. PUBLISHERS EJEMPLO

### Publisher de Deportes

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 5A: PUBLISHER DE DEPORTES (EJEMPLO)
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat > ~/mqtt_publishers/deportes_publisher.py << 'EOFPYTHON'
#!/usr/bin/env python3
"""
Publisher de eventos deportivos a MQTT
Ejemplo con datos mock - reemplazar con API real
"""

import paho.mqtt.client as mqtt
import json
from datetime import datetime, timedelta

# Configuración MQTT (CAMBIAR CON TUS CREDENCIALES)
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "publisher_deportes"
MQTT_PASS = "CAMBIAR_POR_PASSWORD_GENERADA"

def publicar_partido_boca():
    """Ejemplo: publicar partido de Boca"""
    
    # En producción, obtener datos de API real
    # API-Football, Football-Data.org, etc.
    
    # Datos de ejemplo
    proximo_partido = {
        "equipo": "Boca Juniors",
        "rival": "River Plate",
        "hora": "20:00",
        "fecha": "15/06/2026",
        "estadio": "La Bombonera",
        "tiempo_restante_horas": 4
    }
    
    print("🏆 Publicando partido de Boca...")
    
    client = mqtt.Client(client_id="deportes_publisher")
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        
        # Publicar en topic de deportes
        topic = "casa/deportes/boca/proximo"
        payload = json.dumps(proximo_partido)
        
        result = client.publish(topic, payload, qos=1)
        
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"✅ Partido publicado: {proximo_partido['equipo']} vs {proximo_partido['rival']}")
        else:
            print(f"❌ Error al publicar: {result.rc}")
        
        client.disconnect()
        
    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == '__main__':
    publicar_partido_boca()
EOFPYTHON

chmod +x ~/mqtt_publishers/deportes_publisher.py

echo "✅ Publisher de deportes creado"
```

### Publisher de Feriados

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 5B: PUBLISHER DE FERIADOS ARGENTINOS
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat > ~/mqtt_publishers/feriados_publisher.py << 'EOFPYTHON'
#!/usr/bin/env python3
"""
Publisher de feriados argentinos a MQTT
Usa API pública: nolaborables.com.ar
"""

import paho.mqtt.client as mqtt
import requests
import json
from datetime import datetime

# Configuración MQTT (CAMBIAR CON TUS CREDENCIALES)
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "publisher_feriados"
MQTT_PASS = "CAMBIAR_POR_PASSWORD_GENERADA"

def obtener_feriados():
    """Obtiene feriados argentinos de API pública"""
    year = datetime.now().year
    url = f"https://nolaborables.com.ar/api/v2/feriados/{year}"
    
    try:
        response = requests.get(url, timeout=10)
        if response.status_code == 200:
            return response.json()
        else:
            print(f"❌ Error API: {response.status_code}")
            return []
    except Exception as e:
        print(f"❌ Error: {e}")
        return []

def publicar_feriado_hoy():
    """Publica si hoy es feriado"""
    print("🗓️  Verificando feriados...")
    
    feriados = obtener_feriados()
    hoy = datetime.now().strftime("%Y-%m-%d")
    
    feriado_hoy = None
    for feriado in feriados:
        if feriado.get('fecha') == hoy:
            feriado_hoy = feriado
            break
    
    if not feriado_hoy:
        print("ℹ️  Hoy no es feriado")
        return
    
    # Conectar a MQTT
    client = mqtt.Client(client_id="feriados_publisher")
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        
        payload = {
            "fecha": feriado_hoy['fecha'],
            "motivo": feriado_hoy['motivo'],
            "tipo": feriado_hoy['tipo']
        }
        
        topic = "casa/calendario/feriado"
        result = client.publish(topic, json.dumps(payload), qos=1)
        
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"✅ Feriado publicado: {feriado_hoy['motivo']}")
        
        client.disconnect()
        
    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == '__main__':
    publicar_feriado_hoy()
EOFPYTHON

chmod +x ~/mqtt_publishers/feriados_publisher.py

echo "✅ Publisher de feriados creado"
```

---

## 6. CONFIGURACIÓN CRON

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 6: CONFIGURAR TAREAS AUTOMÁTICAS (CRON)
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Agregar tareas a crontab
(crontab -l 2>/dev/null; cat << 'EOFCRON'

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# MQTT PUBLISHERS - Reloj Peronista
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Verificar calendario cada 15 minutos
*/15 * * * * cd ~/mqtt_publishers && /usr/bin/python3 calendario_publisher.py >> ~/logs/calendario.log 2>&1

# Verificar deportes cada 30 minutos
*/30 * * * * cd ~/mqtt_publishers && /usr/bin/python3 deportes_publisher.py >> ~/logs/deportes.log 2>&1

# Verificar feriados una vez al día (6 AM)
0 6 * * * cd ~/mqtt_publishers && /usr/bin/python3 feriados_publisher.py >> ~/logs/feriados.log 2>&1

# Auditoría semanal (lunes 9 AM)
0 9 * * 1 ~/scripts/auditoria_mqtt.sh >> ~/logs/auditoria.log 2>&1

EOFCRON
) | crontab -

echo "✅ Tareas cron configuradas"
echo ""
echo "Para ver las tareas: crontab -l"
echo "Para editar: crontab -e"
echo ""
```

---

## 7. TESTING Y VERIFICACIÓN

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 7A: COMANDOS DE TESTING
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

echo "🧪 TESTING MQTT"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Test 1: Verificar que Mosquitto está corriendo
echo ""
echo "1️⃣  Estado de Mosquitto:"
sudo systemctl status mosquitto | grep "Active:"

# Test 2: Verificar usuarios creados
echo ""
echo "2️⃣  Usuarios MQTT registrados:"
sudo cat /etc/mosquitto/passwd | cut -d':' -f1

# Test 3: Obtener credenciales del admin
echo ""
echo "3️⃣  Credenciales de admin:"
cat ~/mqtt_credentials/admin_credentials.txt | grep -E "Usuario:|Contraseña:"

# Test 4: Publicar mensaje de prueba
echo ""
echo "4️⃣  Test de publicación (en otra terminal ejecuta el subscriber):"
echo "   Terminal 1 (subscriber):"
echo "   mosquitto_sub -h localhost -u admin -P <password_admin> -t 'casa/#' -v"
echo ""
echo "   Terminal 2 (publisher):"
echo "   mosquitto_pub -h localhost -u admin -P <password_admin> -t casa/test -m 'Hola desde Mosquitto'"
echo ""

# Test 5: Listar archivos de credenciales
echo "5️⃣  Archivos de credenciales generados:"
ls -lh ~/mqtt_credentials/

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ Setup completo"
echo ""
```

### Script de Test Completo

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 7B: SCRIPT DE TEST COMPLETO
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat > ~/scripts/test_mqtt.sh << 'EOFSCRIPT'
#!/bin/bash

echo "🧪 TEST COMPLETO MQTT"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Obtener password de admin
ADMIN_PASS=$(grep "Contraseña:" ~/mqtt_credentials/admin_credentials.txt | cut -d' ' -f2)

if [ -z "$ADMIN_PASS" ]; then
    echo "❌ No se pudo obtener password de admin"
    exit 1
fi

# Test 1: Publicar mensaje
echo ""
echo "1️⃣  Publicando mensaje de prueba..."
mosquitto_pub -h localhost -u admin -P "$ADMIN_PASS" \
  -t casa/test/mensaje -m "Test desde script"

if [ $? -eq 0 ]; then
    echo "   ✅ Publicación exitosa"
else
    echo "   ❌ Error en publicación"
fi

# Test 2: Suscribirse y esperar mensaje
echo ""
echo "2️⃣  Suscribiéndose a topic de prueba (5 segundos)..."
timeout 5s mosquitto_sub -h localhost -u admin -P "$ADMIN_PASS" \
  -t casa/test/# -v &

sleep 1

# Publicar mientras está suscrito
mosquitto_pub -h localhost -u admin -P "$ADMIN_PASS" \
  -t casa/test/recepcion -m "Mensaje recibido correctamente"

wait

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ Tests completados"
EOFSCRIPT

chmod +x ~/scripts/test_mqtt.sh

echo "✅ Script de test creado: ~/scripts/test_mqtt.sh"
echo ""
echo "Ejecutar con: ~/scripts/test_mqtt.sh"
```

---

## 8. RESUMEN DE ARCHIVOS GENERADOS

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# BLOQUE 8: VERIFICAR TODO LO CREADO
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

echo ""
echo "📂 RESUMEN DE ARCHIVOS GENERADOS"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "Scripts de gestión:"
echo "  ~/scripts/generar_credencial_mqtt.sh"
echo "  ~/scripts/auditoria_mqtt.sh"
echo "  ~/scripts/test_mqtt.sh"
echo ""
echo "Publishers MQTT:"
echo "  ~/mqtt_publishers/calendario_publisher.py"
echo "  ~/mqtt_publishers/deportes_publisher.py"
echo "  ~/mqtt_publishers/feriados_publisher.py"
echo ""
echo "Credenciales (en ~/mqtt_credentials/):"
ls ~/mqtt_credentials/*.txt 2>/dev/null || echo "  (vacío - generar con los scripts)"
echo ""
echo "Configuración Mosquitto:"
echo "  /etc/mosquitto/mosquitto.conf"
echo "  /etc/mosquitto/acl"
echo "  /etc/mosquitto/passwd"
echo ""
echo "Logs:"
echo "  ~/logs/calendario.log"
echo "  ~/logs/deportes.log"
echo "  ~/logs/feriados.log"
echo "  ~/logs/auditoria.log"
echo "  /var/log/mosquitto/mosquitto.log"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
```

---

## 🎯 PASOS SIGUIENTES

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# PASOS FINALES PARA COMPLETAR EL SETUP
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

cat << 'EOFSTEPS'

📝 PASOS SIGUIENTES PARA COMPLETAR EL SISTEMA:

1️⃣  GOOGLE CALENDAR API:
   - Ir a https://console.cloud.google.com/
   - Crear proyecto "RelojPeronista"
   - Habilitar "Google Calendar API"
   - Crear credenciales OAuth 2.0 (Desktop App)
   - Descargar credentials.json
   - Copiar a ~/mqtt_publishers/credentials.json
   - Ejecutar: python3 ~/mqtt_publishers/calendario_publisher.py
   - Autorizar en navegador (se genera token.json)

2️⃣  ACTUALIZAR PASSWORDS EN PUBLISHERS:
   - Editar ~/mqtt_publishers/calendario_publisher.py
   - Cambiar MQTT_PASS con el valor de ~/mqtt_credentials/publisher_calendario_credentials.txt
   - Repetir para deportes_publisher.py y feriados_publisher.py

3️⃣  COPIAR CREDENCIALES AL ESP32 (Reloj Peronista):
   - Abrir ~/mqtt_credentials/reloj_peronista_001_credentials.txt
   - Copiar usuario y password al código del ESP32
   - Ver archivo "docs/INTEGRACION_GOOGLE_CALENDAR.md" sección 4.2

4️⃣  PROBAR EL SISTEMA:
   - Ejecutar: ~/scripts/test_mqtt.sh
   - En otra terminal: mosquitto_sub -h localhost -u admin -P <pass> -t '#' -v
   - Publicar manualmente: mosquitto_pub -h localhost -u admin -P <pass> -t casa/test -m "hola"
   - Verificar logs: tail -f ~/logs/*.log

5️⃣  VERIFICAR CRON:
   - crontab -l
   - Esperar 15 minutos para ver si ejecuta calendario_publisher
   - Ver logs: tail -f ~/logs/calendario.log

6️⃣  MONITOREO:
   - Estado: sudo systemctl status mosquitto
   - Logs: sudo tail -f /var/log/mosquitto/mosquitto.log
   - Auditoría: ~/scripts/auditoria_mqtt.sh

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✅ COMANDOS ÚTILES:

# Ver todos los mensajes MQTT en tiempo real
mosquitto_sub -h localhost -u admin -P <password_admin> -t '#' -v

# Publicar mensaje de prueba
mosquitto_pub -h localhost -u admin -P <password_admin> -t casa/test -m "prueba"

# Ver logs en tiempo real
tail -f ~/logs/calendario.log

# Reiniciar Mosquitto
sudo systemctl restart mosquitto

# Ver estado
sudo systemctl status mosquitto

# Ver credenciales generadas
cat ~/mqtt_credentials/admin_credentials.txt

# Ejecutar publisher manualmente
cd ~/mqtt_publishers && python3 calendario_publisher.py

# Ejecutar test completo
~/scripts/test_mqtt.sh

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔗 DOCUMENTACIÓN COMPLETA EN:
   - docs/ARQUITECTURA_MQTT.md
   - docs/INTEGRACION_GOOGLE_CALENDAR.md
   - docs/SEGURIDAD_MQTT.md

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

EOFSTEPS

echo ""
echo "✅ Setup completo! 🎉"
echo ""
```

---

## 🚀 EJECUCIÓN RÁPIDA

Para ejecutar todo el setup de una vez, copia y pega este comando completo:

```bash
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# SETUP COMPLETO EN UN SOLO COMANDO
# ⚠️  REVISAR Y ENTENDER ANTES DE EJECUTAR
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# (Copiar todos los bloques anteriores en orden, o ejecutarlos uno por uno)
```

---

## 📚 REFERENCIA RÁPIDA

### Estructura de Directorios
```
~/
├── scripts/
│   ├── generar_credencial_mqtt.sh
│   ├── auditoria_mqtt.sh
│   └── test_mqtt.sh
├── mqtt_publishers/
│   ├── calendario_publisher.py
│   ├── deportes_publisher.py
│   ├── feriados_publisher.py
│   ├── credentials.json (Google)
│   └── token.json (autogenerado)
├── mqtt_credentials/
│   ├── admin_credentials.txt
│   ├── reloj_peronista_001_credentials.txt
│   ├── publisher_calendario_credentials.txt
│   └── ...
└── logs/
    ├── calendario.log
    ├── deportes.log
    └── feriados.log
```

### Topics MQTT
```
casa/
├── reloj-peronista/
│   ├── notification
│   ├── calendar
│   ├── deportes
│   └── status
├── calendario/
│   ├── urgente
│   ├── recordatorio
│   ├── proximo
│   └── feriado
├── deportes/
│   └── boca/
│       ├── proximo
│       └── resultado
└── meteo/
    └── techo/
        ├── temperatura
        ├── humedad
        └── all
```

---

**✅ FIN DEL SETUP - Todo listo para copiar y pegar! 🚀**
