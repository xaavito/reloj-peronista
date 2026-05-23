# 📋 Tareas Pendientes - Reloj Peronista

## 🔴 Prioridad Alta

- [ ] **Cambiar APIs a usuario más genérico**
  - [ ] Actualizar config.h.example con usuarios/claves genéricas
  - [ ] Documentar dónde obtener API keys de OpenWeather
  - [ ] Documentar configuración de zona horaria NTP
  - [ ] Agregar instrucciones claras en README

## 🟡 Prioridad Media

- [ ] **Mejoras Visuales**
  - [x] Cambiar todas las pantallas a fondo negro con texto blanco
  - [ ] Ajustar tamaño de fuente de hora para que ocupe más pantalla
  - [ ] Explorar fuentes personalizadas o dibujo de dígitos custom
  - [ ] Mejorar contraste de íconos del clima

- [ ] **Funcionalidad**
  - [ ] Guardar configuración de alarma en EEPROM/NVS (persistente al reinicio)
  - [ ] Agregar más tonos/melodías para alarma
  - [ ] Modo noche (brillo reducido automático)
  - [ ] Mostrar día de la semana en pantalla de fecha

## 🟢 Prioridad Baja

- [ ] **Documentación**
  - [ ] Actualizar README con fotos del proyecto final
  - [ ] Video tutorial de configuración
  - [ ] Guía de troubleshooting común
  - [ ] Documentar consumo eléctrico real

- [ ] **Hardware**
  - [ ] Diseñar carcasa 3D personalizada
  - [ ] Agregar control de brillo de pantalla (PWM en backlight)
  - [ ] Considerar batería/RTC para mantener hora sin WiFi

- [ ] **Código**
  - [ ] Refactorizar código en múltiples archivos (.h/.cpp)
  - [ ] Optimizar uso de memoria
  - [ ] Agregar modo debug con más logs
  - [ ] Unit tests para funciones críticas

## ✅ Completado

- [x] Configuración básica del reloj con WiFi y NTP
- [x] Integración de sensores (AHT10, BMP180)
- [x] Sistema de alarma configurable
- [x] Múltiples modos de visualización
- [x] Pronóstico del clima (OpenWeather API)
- [x] Efemérides peronistas con efecto marquee
- [x] Botones de control (modo y alarma)
- [x] Tema visual negro con texto blanco
- [x] Documentación de wiring actualizada
- [x] Corrección de GPIO del botón de modo (GPIO 27)

## 📝 Notas

- Fecha de creación: 23/05/2026
- Última actualización: 23/05/2026
- Mantenedor: @usuario

---

**Para agregar nuevas tareas:** Edita este archivo y marca con `[x]` las completadas.
