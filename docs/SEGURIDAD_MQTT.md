# 🔐 Seguridad MQTT - Autenticación y Autorización

## 🎯 Objetivo

Implementar autenticación robusta para tu broker MQTT sin necesidad de un sistema de registro complejo, usando credenciales autogeneradas por dispositivo.

---

## 🏗️ Arquitectura de Seguridad Propuesta

```
┌──────────────────────────────────────────────────────────────┐
│              RASPBERRY PI - MOSQUITTO BROKER                 │
│                                                               │
│  ┌────────────────────────────────────────────────────┐     │
│  │  1. Archivo de contraseñas (/etc/mosquitto/passwd)│     │
│  │     reloj_001:$6$hashed_password...                │     │
│  │     estacion_meteo:$6$hashed_password...           │     │
│  │     publisher_cal:$6$hashed_password...            │     │
│  └────────────────────────────────────────────────────┘     │
│                                                               │
│  ┌────────────────────────────────────────────────────┐     │
│  │  2. ACL (Access Control List) /etc/mosquitto/acl   │     │
│  │     user reloj_001                                 │     │
│  │       topic read casa/reloj-peronista/#           │     │
│  │       topic write casa/reloj-peronista/status     │     │
│  │                                                     │     │
│  │     user estacion_meteo                            │     │
│  │       topic write casa/meteo/techo/#              │     │
│  └────────────────────────────────────────────────────┘     │
│                                                               │
│  ┌────────────────────────────────────────────────────┐     │
│  │  3. TLS/SSL (Opcional para producción)            │     │
│  │     Certificados para encriptar comunicación       │     │
│  └────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────┘
                          ↓
        Clientes con credenciales únicas hardcodeadas
```

---

## 🔑 Método 1: Usuario/Contraseña Autogenerada (RECOMENDADO)

Este es el método más simple y efectivo. Cada dispositivo tiene un usuario único y una contraseña autogenerada.

### 1.1 Script para Generar Credenciales

Crea este script en tu Raspberry Pi:

```bash
#!/bin/bash
# ~/scripts/generar_credencial_mqtt.sh
# Script para generar credenciales únicas para dispositivos MQTT

# Verificar que se pasó un nombre de dispositivo
if [ -z "$1" ]; then
    echo "❌ Uso: $0 <nombre_dispositivo>"
    echo "Ejemplo: $0 reloj_001"
    exit 1
fi

DEVICE_NAME=$1
PASSWD_FILE="/etc/mosquitto/passwd"
CREDENTIALS_DIR="$HOME/mqtt_credentials"

# Crear directorio si no existe
mkdir -p "$CREDENTIALS_DIR"

# Generar contraseña aleatoria segura (32 caracteres)
PASSWORD=$(openssl rand -base64 32 | tr -d "=+/" | cut -c1-32)

# Agregar usuario al archivo de contraseñas de Mosquitto
echo "🔐 Generando credenciales para: $DEVICE_NAME"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Usar mosquitto_passwd para agregar/actualizar usuario
sudo mosquitto_passwd -b "$PASSWD_FILE" "$DEVICE_NAME" "$PASSWORD"

if [ $? -eq 0 ]; then
    echo "✅ Usuario creado en Mosquitto"
    
    # Guardar credenciales en archivo local para referencia
    CRED_FILE="$CREDENTIALS_DIR/${DEVICE_NAME}_credentials.txt"
    echo "# Credenciales MQTT para $DEVICE_NAME" > "$CRED_FILE"
    echo "# Generado: $(date)" >> "$CRED_FILE"
    echo "# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" >> "$CRED_FILE"
    echo "" >> "$CRED_FILE"
    echo "Usuario: $DEVICE_NAME" >> "$CRED_FILE"
    echo "Contraseña: $PASSWORD" >> "$CRED_FILE"
    echo "Broker: localhost (o IP de Raspberry)" >> "$CRED_FILE"
    echo "Puerto: 1883" >> "$CRED_FILE"
    echo "" >> "$CRED_FILE"
    echo "# Para ESP32 (Arduino/C++):" >> "$CRED_FILE"
    echo "const char* mqtt_user = \"$DEVICE_NAME\";" >> "$CRED_FILE"
    echo "const char* mqtt_password = \"$PASSWORD\";" >> "$CRED_FILE"
    echo "" >> "$CRED_FILE"
    echo "# Para Python:" >> "$CRED_FILE"
    echo "MQTT_USER = \"$DEVICE_NAME\"" >> "$CRED_FILE"
    echo "MQTT_PASS = \"$PASSWORD\"" >> "$CRED_FILE"
    
    chmod 600 "$CRED_FILE"  # Solo lectura para el propietario
    
    echo "✅ Credenciales guardadas en: $CRED_FILE"
    echo ""
    echo "📋 CREDENCIALES GENERADAS:"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "Usuario:     $DEVICE_NAME"
    echo "Contraseña:  $PASSWORD"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "⚠️  IMPORTANTE: Guarda estas credenciales de forma segura"
    echo "⚠️  Esta contraseña NO se mostrará nuevamente"
    echo ""
    echo "🔄 Recuerda reiniciar Mosquitto:"
    echo "   sudo systemctl restart mosquitto"
    
else
    echo "❌ Error al crear usuario"
    exit 1
fi
```

### 1.2 Hacer el Script Ejecutable

```bash
chmod +x ~/scripts/generar_credencial_mqtt.sh
```

### 1.3 Generar Credenciales para Cada Dispositivo

```bash
# Para el Reloj Peronista
~/scripts/generar_credencial_mqtt.sh reloj_peronista_001

# Para la estación meteorológica
~/scripts/generar_credencial_mqtt.sh estacion_meteo_techo

# Para el publisher de calendario
~/scripts/generar_credencial_mqtt.sh publisher_calendario

# Para el publisher de deportes
~/scripts/generar_credencial_mqtt.sh publisher_deportes

# Usuario admin (para ti)
~/scripts/generar_credencial_mqtt.sh admin
```

Salida ejemplo:
```
🔐 Generando credenciales para: reloj_peronista_001
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✅ Usuario creado en Mosquitto
✅ Credenciales guardadas en: /home/pi/mqtt_credentials/reloj_peronista_001_credentials.txt

📋 CREDENCIALES GENERADAS:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Usuario:     reloj_peronista_001
Contraseña:  Kx8nP9mQw2YvR7tJh4LdC6bN5sF3aE1Z
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

⚠️  IMPORTANTE: Guarda estas credenciales de forma segura
```

### 1.4 Reiniciar Mosquitto

```bash
sudo systemctl restart mosquitto
```

---

## 🛡️ Método 2: ACL (Access Control Lists)

Las ACLs permiten **controlar qué puede hacer cada usuario**: qué topics puede leer, escribir, etc.

### 2.1 Configurar Mosquitto para ACL

Editar `/etc/mosquitto/mosquitto.conf`:

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Agregar/modificar:
```ini
# Desactivar acceso anónimo
allow_anonymous false

# Archivo de contraseñas
password_file /etc/mosquitto/passwd

# Archivo de ACL
acl_file /etc/mosquitto/acl
```

### 2.2 Crear Archivo ACL

```bash
sudo nano /etc/mosquitto/acl
```

Contenido del archivo ACL:

```ini
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# MOSQUITTO ACCESS CONTROL LIST (ACL)
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# FORMATO:
# user <username>
#   topic [read|write|readwrite] <topic>
#
# WILDCARDS:
#   + → Un nivel (ej: casa/+/temperatura)
#   # → Múltiples niveles (ej: casa/reloj/#)

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# ADMIN (acceso completo)
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
user admin
topic readwrite #

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# RELOJ PERONISTA
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
user reloj_peronista_001
# Puede leer notificaciones dirigidas a él
topic read casa/reloj-peronista/#
# Puede leer datos meteorológicos
topic read casa/meteo/#
# Puede leer calendario
topic read casa/calendario/#
# Puede leer deportes
topic read casa/deportes/#
# Puede publicar su estado
topic write casa/reloj-peronista/status
topic write casa/reloj-peronista/heartbeat
# Puede publicar datos de sensores locales (si tiene)
topic write casa/reloj-peronista/sensores/#

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# ESTACIÓN METEOROLÓGICA
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
user estacion_meteo_techo
# Solo puede escribir datos meteorológicos
topic write casa/meteo/techo/#
# Puede publicar su estado
topic write casa/meteo/techo/status
# NO puede leer nada más (seguridad por si lo hackean)

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# PUBLISHER DE CALENDARIO
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
user publisher_calendario
# Solo puede escribir eventos de calendario
topic write casa/calendario/#

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# PUBLISHER DE DEPORTES
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
user publisher_deportes
# Solo puede escribir eventos deportivos
topic write casa/deportes/#

# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# PATRÓN PARA FUTUROS DISPOSITIVOS
# ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
# user nuevo_dispositivo_xyz
# topic read casa/nuevo_dispositivo/#
# topic write casa/nuevo_dispositivo/data
```

### 2.3 Reiniciar Mosquitto

```bash
sudo systemctl restart mosquitto
```

### 2.4 Probar ACL

```bash
# Intentar publicar con reloj (debería funcionar en su topic)
mosquitto_pub -h localhost -u reloj_peronista_001 -P "su_password" \
  -t casa/reloj-peronista/status -m "online"

# Intentar publicar en topic no autorizado (debería fallar)
mosquitto_pub -h localhost -u reloj_peronista_001 -P "su_password" \
  -t casa/meteo/techo/temperatura -m "25"
# ❌ Error: Connection Refused: not authorised
```

---

## 🔐 Método 3: API Keys/Tokens (Avanzado)

Para mayor seguridad, puedes usar tokens que incluyan información del dispositivo.

### 3.1 Script Generador de Tokens

```python
#!/usr/bin/env python3
# ~/scripts/generar_token_mqtt.py

import secrets
import hashlib
import json
from datetime import datetime, timedelta

def generar_token_dispositivo(nombre_dispositivo, valido_dias=365):
    """
    Genera un token único para un dispositivo
    
    Formato del token: DEVICE.TIMESTAMP.HASH
    Ejemplo: reloj001.20260602.a8f3d9e2c4b1...
    """
    # Componentes del token
    timestamp = datetime.now().strftime("%Y%m%d")
    random_part = secrets.token_hex(16)  # 32 caracteres hex
    
    # Crear hash del dispositivo + timestamp + secret
    secret_key = "TU_SECRET_KEY_AQUI_CAMBIAR"  # ⚠️ CAMBIAR ESTO
    data = f"{nombre_dispositivo}:{timestamp}:{random_part}:{secret_key}"
    hash_part = hashlib.sha256(data.encode()).hexdigest()[:16]
    
    # Token final
    token = f"{nombre_dispositivo}.{timestamp}.{random_part}{hash_part}"
    
    # Fecha de expiración
    expiracion = datetime.now() + timedelta(days=valido_dias)
    
    return {
        "dispositivo": nombre_dispositivo,
        "token": token,
        "fecha_creacion": datetime.now().isoformat(),
        "fecha_expiracion": expiracion.isoformat(),
        "usuario_mqtt": nombre_dispositivo,
        "password_mqtt": token
    }

def main():
    import sys
    
    if len(sys.argv) < 2:
        print("❌ Uso: python3 generar_token_mqtt.py <nombre_dispositivo>")
        sys.exit(1)
    
    nombre = sys.argv[1]
    dias_validez = int(sys.argv[2]) if len(sys.argv) > 2 else 365
    
    print(f"\n🔐 Generando token para: {nombre}")
    print("━" * 60)
    
    token_info = generar_token_dispositivo(nombre, dias_validez)
    
    print(f"✅ Token generado")
    print(f"📅 Válido hasta: {token_info['fecha_expiracion'][:10]}")
    print("\n📋 CREDENCIALES:")
    print("━" * 60)
    print(f"Usuario: {token_info['usuario_mqtt']}")
    print(f"Token:   {token_info['token']}")
    print("━" * 60)
    
    # Guardar en archivo
    filename = f"~/mqtt_credentials/{nombre}_token.json"
    with open(filename.replace('~', os.path.expanduser('~')), 'w') as f:
        json.dump(token_info, f, indent=2)
    
    print(f"\n💾 Guardado en: {filename}")
    print("\n📝 Para usar en ESP32:")
    print(f'const char* mqtt_user = "{token_info["usuario_mqtt"]}";')
    print(f'const char* mqtt_password = "{token_info["token"]}";')
    print("")

if __name__ == "__main__":
    import os
    main()
```

### 3.2 Uso del Script

```bash
python3 ~/scripts/generar_token_mqtt.py reloj_peronista_001

# Con expiración personalizada (90 días)
python3 ~/scripts/generar_token_mqtt.py estacion_meteo 90
```

---

## 📟 Implementación en Dispositivos

### 4.1 ESP32 (Reloj Peronista)

```cpp
// ========== EN config.h O HARDCODEADO ==========

// Credenciales MQTT (autogeneradas)
#define MQTT_SERVER "192.168.1.100"  // IP de tu Raspberry
#define MQTT_PORT 1883
#define MQTT_USER "reloj_peronista_001"
#define MQTT_PASSWORD "Kx8nP9mQw2YvR7tJh4LdC6bN5sF3aE1Z"

// ========== EN main.cpp ==========

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("🔌 Conectando a MQTT como: ");
    Serial.println(MQTT_USER);
    
    // Conectar con credenciales
    if (mqttClient.connect(MQTT_USER, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("✅ Autenticado correctamente");
      
      // Suscribirse a topics permitidos
      mqttClient.subscribe("casa/reloj-peronista/#");
      mqttClient.subscribe("casa/calendario/#");
      mqttClient.subscribe("casa/deportes/#");
      mqttClient.subscribe("casa/meteo/#");
      
    } else {
      Serial.print("❌ Fallo autenticación, rc=");
      Serial.println(mqttClient.state());
      // rc=-5 = no autorizado (usuario/contraseña incorrectos)
      // rc=-4 = ACL rechazó la conexión
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // ... conectar WiFi ...
  
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  conectarMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();
}
```

### 4.2 Python (Publishers en Raspberry)

```python
#!/usr/bin/env python3
# ~/mqtt_publishers/calendario_publisher.py

import paho.mqtt.client as mqtt
import json

# Credenciales autogeneradas
MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_USER = "publisher_calendario"
MQTT_PASSWORD = "vP5qW8rT3yU9iO2pA7sD4fG1hJ6kL0zX"

def conectar_mqtt():
    """Conecta al broker MQTT con autenticación"""
    client = mqtt.Client(client_id=f"{MQTT_USER}_pid{os.getpid()}")
    
    # Configurar credenciales
    client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    
    # Callbacks
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        return client
    except Exception as e:
        print(f"❌ Error conectando: {e}")
        return None

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"✅ Conectado como: {MQTT_USER}")
    elif rc == 5:
        print("❌ Autenticación fallida: usuario/contraseña incorrectos")
    else:
        print(f"❌ Conexión fallida, código: {rc}")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print(f"⚠️  Desconexión inesperada: {rc}")

# Usar en el código
if __name__ == "__main__":
    client = conectar_mqtt()
    if client:
        # Publicar eventos
        client.publish("casa/calendario/recordatorio", 
                      json.dumps({"evento": "Reunión"}))
        client.disconnect()
```

---

## 🔄 Sistema de Rotación de Contraseñas (Opcional)

Para mayor seguridad, puedes rotar contraseñas periódicamente.

### 5.1 Script de Rotación

```bash
#!/bin/bash
# ~/scripts/rotar_password_mqtt.sh

DEVICE=$1
OLD_PASS=$2

if [ -z "$DEVICE" ] || [ -z "$OLD_PASS" ]; then
    echo "❌ Uso: $0 <dispositivo> <password_actual>"
    exit 1
fi

# Generar nueva contraseña
NEW_PASS=$(openssl rand -base64 32 | tr -d "=+/" | cut -c1-32)

echo "🔄 Rotando contraseña para: $DEVICE"

# Actualizar en Mosquitto
sudo mosquitto_passwd -b /etc/mosquitto/passwd "$DEVICE" "$NEW_PASS"

if [ $? -eq 0 ]; then
    echo "✅ Contraseña actualizada en broker"
    echo ""
    echo "📋 NUEVA CONTRASEÑA:"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "Usuario:  $DEVICE"
    echo "Password: $NEW_PASS"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "⚠️  Actualiza el dispositivo con la nueva contraseña"
    echo "⚠️  La contraseña anterior dejará de funcionar tras reiniciar Mosquitto"
    echo ""
    echo "Para aplicar cambios: sudo systemctl restart mosquitto"
fi
```

---

## 📊 Monitoreo de Seguridad

### 6.1 Ver Intentos de Conexión

```bash
# Ver log de Mosquitto en tiempo real
sudo tail -f /var/log/mosquitto/mosquitto.log

# Filtrar solo errores de autenticación
sudo grep "authentication failed" /var/log/mosquitto/mosquitto.log
```

### 6.2 Script de Auditoría

```bash
#!/bin/bash
# ~/scripts/auditoria_mqtt.sh

echo "🔍 AUDITORÍA MQTT - $(date)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

echo ""
echo "👥 USUARIOS REGISTRADOS:"
sudo cat /etc/mosquitto/passwd | cut -d':' -f1 | nl

echo ""
echo "🔐 ÚLTIMOS INTENTOS FALLIDOS (últimas 24h):"
sudo journalctl -u mosquitto --since "24 hours ago" | \
  grep -i "authentication failed" | tail -10

echo ""
echo "✅ CONEXIONES EXITOSAS (últimas 24h):"
sudo journalctl -u mosquitto --since "24 hours ago" | \
  grep -i "client .* connected" | wc -l

echo ""
echo "📊 ESTADO DEL BROKER:"
sudo systemctl status mosquitto | grep "Active:"
```

---

## 🚨 Mejores Prácticas de Seguridad

### 1. **Nunca usar contraseñas simples**
```bash
# ❌ MAL
mosquitto_passwd -b /etc/mosquitto/passwd reloj 12345

# ✅ BIEN
~/scripts/generar_credencial_mqtt.sh reloj
```

### 2. **Usar ACLs siempre**
- Limita el daño si un dispositivo es comprometido
- Principio de "menor privilegio"

### 3. **No hardcodear credenciales admin en dispositivos**
```cpp
// ❌ NUNCA HAGAS ESTO
#define MQTT_USER "admin"
#define MQTT_PASSWORD "admin123"

// ✅ USA CREDENCIALES ESPECÍFICAS
#define MQTT_USER "reloj_peronista_001"
#define MQTT_PASSWORD "Kx8nP9mQw2YvR7tJh4LdC6bN5sF3aE1Z"
```

### 4. **Rotar contraseñas periódicamente**
```bash
# Cada 6 meses o 1 año
~/scripts/rotar_password_mqtt.sh reloj_peronista_001 "password_actual"
```

### 5. **Monitorear intentos fallidos**
```bash
# Configurar alerta si hay muchos intentos fallidos
# (posible ataque de fuerza bruta)
```

### 6. **TLS/SSL para conexiones remotas** (Ver siguiente sección)

---

## 🔒 Bonus: TLS/SSL (Opcional pero recomendado)

Si necesitas acceder al broker desde internet o quieres encriptar todo.

### 7.1 Generar Certificados

```bash
# Instalar herramientas
sudo apt install openssl

# Generar certificado autofirmado
cd /etc/mosquitto/certs
sudo openssl req -new -x509 -days 3650 -extensions v3_ca \
  -keyout ca.key -out ca.crt

sudo openssl genrsa -out server.key 2048
sudo openssl req -out server.csr -key server.key -new
sudo openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key \
  -CAcreateserial -out server.crt -days 3650
```

### 7.2 Configurar Mosquitto

En `/etc/mosquitto/mosquitto.conf`:
```ini
# Puerto seguro (TLS)
listener 8883
certfile /etc/mosquitto/certs/server.crt
keyfile /etc/mosquitto/certs/server.key
cafile /etc/mosquitto/certs/ca.crt
```

### 7.3 Conectar desde ESP32 con TLS

```cpp
#include <WiFiClientSecure.h>

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

void setup() {
  // Certificado CA (copiar contenido de ca.crt)
  const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDXTCCAkWgAwIBAgIJAL...
-----END CERTIFICATE-----
)EOF";
  
  secureClient.setCACert(ca_cert);
  mqttClient.setServer(MQTT_SERVER, 8883);  // Puerto 8883
}
```

---

## ✅ Resumen Final

### ✨ Sistema de Seguridad Completo:

1. **Autenticación**: Cada dispositivo tiene usuario/password único autogenerado
2. **Autorización**: ACLs limitan qué puede hacer cada dispositivo
3. **Auditoría**: Logs para detectar intentos sospechosos
4. **Encriptación** (opcional): TLS/SSL para proteger comunicaciones

### 🎯 Flujo de Onboarding de Nuevo Dispositivo:

```bash
# 1. Generar credenciales
~/scripts/generar_credencial_mqtt.sh nuevo_dispositivo

# 2. Agregar ACL
sudo nano /etc/mosquitto/acl
# (agregar permisos para el nuevo dispositivo)

# 3. Reiniciar broker
sudo systemctl restart mosquitto

# 4. Copiar credenciales al dispositivo
# 5. ¡Listo!
```

### 🔐 Nivel de Seguridad Logrado:

- ✅ Sin acceso anónimo
- ✅ Contraseñas fuertes autogeneradas (32 chars random)
- ✅ ACLs para limitar acceso por topic
- ✅ Credenciales únicas por dispositivo
- ✅ Auditoría de accesos
- ✅ (Opcional) Encriptación TLS/SSL

Este sistema es **simple de administrar** pero **muy seguro** para uso doméstico/IoT.
