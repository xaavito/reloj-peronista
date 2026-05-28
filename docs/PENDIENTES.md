# 📋 Tareas Pendientes - Reloj Peronista

## 🐛 BUGS

### Alta Prioridad

- [ ] **Modo efeméride requiere múltiples presiones para salir**
  - Problema: Cuando entras al modo efeméride, hay que presionar el botón 5-6 veces para volver al modo normal
  - Comportamiento actual: Cambia entre efemérides antes de salir
  - Comportamiento esperado: Una presión = muestra efeméride, otra presión = vuelve a pantalla normal
  - Solución propuesta: Cambiar lógica a toggle simple (peronistMode ON → mostrar 1 efeméride → peronistMode OFF al presionar de nuevo)

### Baja Prioridad

- [ ] Revisar comportamiento de alarma en cambio de hora
- [ ] Verificar que la configuración persista correctamente después de varios reinicios

---

## 🔴 PRIORIDAD ALTA

- [ ] **Cambiar APIs a usuario más genérico**
  - [ ] Actualizar config.h.example con usuarios/claves genéricas
  - [ ] Documentar dónde obtener API keys de OpenWeather
  - [ ] Documentar configuración de zona horaria NTP
  - [ ] Agregar instrucciones claras en README

---

## 🎨 MEJORAS VISUALES / ESTILO

### Pantalla Principal

- [ ] **Mejorar aspecto visual de pantalla principal**
  - Objetivo: Que se vea más "fachera"
  - Ideas:
    - [ ] Agregar bordes/marcos decorativos
    - [ ] Usar gradientes o colores más atractivos
    - [ ] Reorganizar elementos para mejor balance visual
    - [ ] Iconos más grandes y vistosos
    - [ ] Efectos de sombra en texto importante
    - [ ] Panel con borde para agrupar sensores
    - [ ] Separadores visuales entre secciones
    - [ ] Ajustar tamaño de fuente de hora para que ocupe más pantalla
    - [ ] Explorar fuentes personalizadas o dibujo de dígitos custom
    - [ ] Mejorar contraste de íconos del clima

### Pantalla de Alarma

- [ ] Eliminar flickering (ver bugs)
- [ ] Mejorar mensaje motivacional (más grande, centrado)
- [ ] Agregar animación o efecto visual cuando suena

### General

- [ ] Tipografía más consistente
- [ ] Paleta de colores peronista (celeste/blanco o azul/amarillo)
- [ ] Transiciones suaves entre pantallas

---

## 💡 IDEAS / FEATURES NUEVAS

### Brillo Automático

- [ ] **Bajar brillo nocturno (11pm - 7am)**
  - Implementación:
    - Detectar rango horario 23:00 - 07:00
    - Usar `tft.setBrightness()` o similar
    - Verificar si ILI9341 soporta control de brillo por software
    - Alternativa: Controlar LED backlight por PWM (GPIO)
  - Valores propuestos:
    - Día (7am-11pm): 255 (100%)
    - Noche (11pm-7am): 50-80 (20-30%)

### Audio

- [ ] Opción para cambiar melodía de alarma
- [ ] Volumen ajustable para buzzer (PWM duty cycle)
- [ ] Considerar módulo DFPlayer Mini para audio real

### Funcionalidad

- [ ] Alarmas múltiples (lunes a viernes, fines de semana)
- [ ] Temporizador/contador regresivo
- [ ] Mostrar fase lunar
- [ ] Integración con calendario de efemérides argentinas
- [ ] Modo "No Molestar" (silenciar alarma en horarios específicos)
- [ ] Mostrar día de la semana en pantalla de fecha

### 🌟 IDEAS CREATIVAS (Sugeridas por IA)

**Temática Peronista:**
- [ ] **Frases motivacionales del General**
  - Mostrar una frase de Perón diferente cada día al despertar
  - Base de datos de ~50 frases célebres
  - Rotación aleatoria o secuencial
  
- [ ] **Countdown para fechas históricas**
  - "Faltan X días para el 17 de Octubre"
  - "Faltan X días para el 1° de Mayo"
  - Mostrar en pantalla principal
  - Animación especial el día del evento

- [ ] **Modo "17 de Octubre"**
  - Activación automática el 17/10
  - Animaciones especiales todo el día
  - Colores celeste y blanco alternando
  - Melodía especial (no solo para alarma)

- [ ] **Trivia Peronista**
  - Mini-juego mientras esperas que suene alarma
  - Preguntas sobre historia peronista
  - Responder con botones
  - Stats de aciertos/fallos

**Funcional/Útil:**
- [ ] **Modo "Temperatura del Mate"**
  - Mostrar si temperatura ambiente es ideal para cebar (70-80°C)
  - Ícono de mate cuando está en rango ideal
  - Alerta si hace mucho frío/calor para el mate

- [ ] **Estadísticas de uso**
  - Cuántas veces presionaste snooze este mes
  - Promedio de hora de despertar
  - Días con alarma activa vs desactivada
  - Gráficos simples en pantalla

- [ ] **Integración con feriados argentinos**
  - Detectar feriados nacionales
  - Cambiar alarma automáticamente (opcional)
  - Mostrar "¡Hoy es feriado!" en pantalla
  - API pública de feriados argentinos

- [ ] **Sincronización multi-reloj**
  - ESP-NOW para comunicar múltiples relojes
  - Snooze sincronizado (presionas en uno, se silencian todos)
  - Útil para habitaciones múltiples
  - Mesh network simple

- [ ] **Modo "Radio Peronista"**
  - Streaming de radio online (discursos, marchas)
  - Requiere módulo de audio (DFPlayer o I2S)
  - Activar con botón específico
  - Preset de URLs de radios argentinas

**Interactivo:**
- [ ] **Gesture Control**
  - Sensor de gestos (APDS-9960)
  - Wave hand = snooze
  - Swipe = cambiar pantalla
  - Touch = detener alarma

- [ ] **Modo "Despertador Progresivo"**
  - Luz que aumenta gradualmente 15min antes de alarma
  - Requiere LED RGB o NeoPixels
  - Simula amanecer
  - Volumen del buzzer también progresivo

- [ ] **Voice Feedback**
  - Módulo síntesis de voz (DFRobot o similar)
  - "Buenos días compañero, son las 7:30"
  - Anunciar temperatura, clima
  - Mensajes de Perón grabados

**Conectividad:**
- [ ] **Telegram Bot**
  - Configurar alarma desde Telegram
  - Recibir notificaciones cuando suena
  - Estadísticas por mensaje
  - Comandos: /alarm, /snooze, /stats

- [ ] **Sincronización con Google Calendar**
  - Importar eventos como alarmas
  - "Reunión en 30 minutos"
  - Requiere WiFi y API de Google

- [ ] **Dashboard Web Local**
  - Servidor web en ESP32
  - Configuración completa desde navegador
  - Gráficos de temperatura histórica
  - Responsive design
  - No requiere internet (solo WiFi local)

**Hardware Extra:**
- [ ] **E-ink Display secundario**
  - Mostrar hora permanente (bajo consumo)
  - TFT se apaga, E-ink muestra hora
  - Útil para modo nocturno

- [ ] **Sensor PIR de movimiento**
  - Detectar presencia
  - Activar pantalla automáticamente
  - Modo "smart snooze" (si sales de la cama, no snooze)

- [ ] **Botón de pánico "Perón"**
  - Presionar 3 veces = enviar mensaje de alerta
  - Útil como botón de emergencia
  - Envía ubicación/hora por Telegram/MQTT

### Sensores

- [ ] Gráficos históricos de temperatura/humedad
- [ ] Alertas de temperatura/humedad fuera de rango
- [ ] Calibración de sensores desde menú

### Red

- [ ] Portal web para configuración (WebServer)
- [ ] Actualización OTA del firmware
- [ ] MQTT para integración Home Assistant
- [ ] Notificaciones push

---

## 🔧 REFACTORING / CÓDIGO

- [ ] Separar funciones de display en archivos separados
- [ ] Crear clase para gestión de alarma
- [ ] Optimizar uso de memoria (strings, buffers)
- [ ] Documentar funciones principales
- [ ] Agregar más comentarios al código
- [ ] Crear config.h.example actualizado
- [ ] Refactorizar código en múltiples archivos (.h/.cpp)
- [ ] Agregar modo debug con más logs
- [ ] Unit tests para funciones críticas

---

## 📱 HARDWARE

- [ ] Diseñar PCB custom
- [ ] Case impreso en 3D
- [ ] Botones físicos más ergonómicos
- [ ] Módulo RTC (DS3231) para hora precisa sin WiFi
- [ ] Batería de respaldo (CR2032)
- [ ] Sensor de luz ambiental para auto-brillo
- [ ] Diseñar carcasa 3D personalizada
- [ ] Agregar control de brillo de pantalla (PWM en backlight)
- [ ] Considerar batería/RTC para mantener hora sin WiFi

---

## 📚 DOCUMENTACIÓN

- [ ] README completo con fotos del proyecto final
- [ ] Diagrama de conexiones Fritzing
- [ ] Lista de materiales (BOM)
- [ ] Guía de instalación paso a paso
- [ ] Video demo del proyecto / Video tutorial de configuración
- [ ] Troubleshooting común / Guía de troubleshooting común
- [ ] Documentar consumo eléctrico real
- [ ] **FINAL DEL PROYECTO: Limpiar y actualizar toda la documentación**
  - [ ] Revisar todos los documentos para que reflejen los cambios finales de hardware
  - [ ] Actualizar diagramas de conexión con configuración final
  - [ ] Verificar que README, wiring y guías estén sincronizados con hardware actual
  - [ ] Eliminar documentación obsoleta o contradictoria
  - [ ] Validar que config.h.example esté actualizado

---

## ✅ COMPLETADO

- [x] Persistencia de configuración (NVS)
- [x] Marcha Peronista versión 8-bit mejorada (MP4)
- [x] Eliminación de flickeo en pantalla principal
- [x] Sistema de snooze ilimitado
- [x] Modo efemérides peronistas
- [x] Integración con OpenWeather
- [x] Sensores AHT10 y BMP180
- [x] Imagen de Perón en pantalla
- [x] Configuración de alarma por botones
- [x] **BUG FIX: Alarma que volvía a sonar después de apagarla (26/05/2026)**
- [x] **BUG FIX: Snooze verificado y funcionando correctamente (27/05/2026)**
- [x] Configuración básica del reloj con WiFi y NTP
- [x] Sistema de alarma configurable
- [x] Múltiples modos de visualización
- [x] Pronóstico del clima (OpenWeather API)
- [x] Efemérides peronistas con efecto marquee
- [x] Botones de control (modo y alarma)
- [x] Tema visual negro con texto blanco
- [x] Documentación de wiring actualizada
- [x] Corrección de GPIO del botón de modo (GPIO 27)
- [x] Cambiar todas las pantallas a fondo negro con texto blanco
- [x] Guardar configuración de alarma en EEPROM/NVS (persistente al reinicio)

---

## 📝 NOTAS

### Prioridades Inmediatas
1. ~~Arreglar bug de alarma que vuelve a sonar~~ ✅ Completado
2. Eliminar flickering en pantalla de alarma
3. Implementar brillo nocturno
4. Mejorar aspecto visual pantalla principal
5. Cambiar APIs a usuario más genérico

### Para Versión 2.0
- Portal web de configuración
- Múltiples alarmas
- DFPlayer Mini para audio real
- PCB custom

---

**Fecha de creación:** 23/05/2026  
**Última actualización:** 26/05/2026  
**Versión actual:** 1.0  
**Próxima versión:** 1.1 (Bugfixes + brillo nocturno)  
**Mantenedor:** @usuario

---

**Para agregar nuevas tareas:** Edita este archivo y marca con `[x]` las completadas.
