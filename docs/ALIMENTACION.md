# ⚡ Guía de Alimentación - Reloj Peronista

## 📊 Consumo del Proyecto

Según tu configuración actual:

| Componente | Consumo |
|------------|---------|
| ESP32 DOIT DevKit V1 | ~200mA |
| Pantalla ILI9341 240x320 | ~80-150mA |
| Sensores (AHT10 + BMP180) | ~5mA |
| Buzzer Activo 5V | ~30mA (solo cuando suena) |
| **TOTAL** | **~350-400mA** |
| **TOTAL (con alarma)** | **~380-430mA** |

**Voltaje de operación:** El ESP32 necesita **5V** en el pin VIN (que internamente regula a 3.3V)

---

## 🔌 Opciones de Alimentación

### ✅ OPCIÓN 1: Cargador USB 220V (RECOMENDADA)

**La más simple y segura:**

#### ¿Qué necesitas comprar?
- **Cargador de celular USB** (220V a 5V)
  - Mínimo: 1A (1000mA)
  - Recomendado: 2A (2000mA)
  - Precio: $500-1500 ARS

#### Ventajas:
- ✅ Muy fácil de conseguir
- ✅ Seguro (protecciones incluidas)
- ✅ No requiere modificaciones
- ✅ Puedes usar cualquier cargador de celular
- ✅ Conexión directa por el puerto USB del ESP32

#### Conexión:
```
220V AC ──► [Cargador USB] ──► Cable USB ──► ESP32
              (5V 1-2A)
```

#### Recomendaciones de compra:
- **Cargador Samsung/Xiaomi original** (5V 2A) - ~$800-1200
- **Cargador universal USB** (5V 1A) - ~$500-800
- Debe tener certificación (evitar muy baratos sin marca)

---

### ✅ OPCIÓN 2: Fuente Switching 220V a 5V

**Para instalación más permanente/integrada:**

#### ¿Qué necesitas comprar?
- **Fuente Switching 220V AC a 5V DC**
  - Potencia: 5V 1A mínimo (5W)
  - Recomendado: 5V 2A (10W)
  - Precio: $1500-3000 ARS

#### Modelos recomendados:
- **Hi-Link HLK-PM01** (5V 600mA) - Compacta
- **Mean Well IRM-03-5** (5V 600mA) - Industrial
- **Fuente genérica 5V 2A** con bornera

#### Conexión:
```
220V AC ──► [Fuente Switching] ──► VIN (ESP32)
              +5V ──────────────► VIN
              GND ──────────────► GND
```

#### Ventajas:
- ✅ Integrable en caja del proyecto
- ✅ Sin cables USB externos
- ✅ Aspecto más profesional

#### Desventajas:
- ⚠️ Requiere conocimientos de electricidad
- ⚠️ **PELIGRO**: Manipulación de 220V
- ⚠️ Necesita soldadura/conexión a bornera
- ⚠️ Requiere caja aislante adecuada

---

### ✅ OPCIÓN 3: Adaptador 220V con Jack DC

**Intermedio:**

#### ¿Qué necesitas?
1. **Adaptador 220V a 5V DC** con plug jack (como de router)
   - Salida: 5V 1-2A
   - Precio: $800-1500 ARS

2. **Conector Jack hembra** o cable para soldar

#### Conexión:
```
220V AC ──► [Adaptador] ──► Jack DC ──► VIN/GND (ESP32)
              5V 1-2A         Cable
```

#### Ventajas:
- ✅ Más compacto que USB
- ✅ Fácil de desconectar
- ✅ Seguro si usas adaptador certificado

---

## 🏆 RECOMENDACIÓN FINAL

### Para empezar (Uso en casa/escritorio):
**OPCIÓN 1: Cargador USB**
- Compra: Cualquier cargador de celular 5V 1-2A
- Precio: $500-1200
- Conecta y listo

### Para montaje permanente/decorativo:
**OPCIÓN 2: Fuente Switching integrada**
- Compra: Fuente 220V a 5V (ej: Hi-Link HLK-PM01)
- Precio: $1500-3000
- **IMPORTANTE**: Aislar bien el 220V en caja cerrada
- Requiere conocimientos de electricidad

---

## 🛒 Lista de Compras Sugerida

### Opción Simple (USB):
```
✅ Cargador USB 5V 2A ────────────► $800-1200
✅ Cable USB (Micro-USB o USB-C) ─► Ya tienes o $300
                              TOTAL: ~$1200
```

### Opción Integrada (Fuente interna):
```
✅ Fuente Hi-Link HLK-PM01 ───────► $2000-2500
✅ Bornera/cables ────────────────► $200-400
✅ Caja con aislación ────────────► $500-1000
✅ Fusible de seguridad ──────────► $200-300
                              TOTAL: ~$3500
```

---

## ⚠️ ADVERTENCIAS DE SEGURIDAD

### Para 220V:
- ⚡ **PELIGRO DE MUERTE** - 220V es mortal
- 🔒 Siempre trabajar con electricidad desconectada
- 📦 Aislar completamente los cables de 220V
- 🛡️ Usar caja con certificación eléctrica
- 🔌 Considerar agregar fusible de protección
- ⚠️ Si no tienes experiencia, usa OPCIÓN 1 (USB)

### Para USB/5V:
- ✅ Voltaje seguro (5V)
- ✅ No hay riesgo eléctrico significativo
- ✅ Recomendado para principiantes

---

## 🔍 ¿Necesitas comprar algo más?

### Ya tienes (según tu proyecto):
- ✅ ESP32 DOIT DevKit V1
- ✅ Pantalla ILI9341 240x320 (TFT_eSPI)
- ✅ Sensores AHT10 y BMP180
- ✅ Botones
- ✅ Cables Dupont

### Necesitas SOLO:
- 🔌 **Fuente de alimentación 220V → 5V**
  - **Opción 1**: Cargador USB ($800-1200)
  - **Opción 2**: Fuente switching ($2000-3000)

### Opcional (para montaje profesional):
- 📦 Caja/gabinete para el proyecto
- 🔌 Interruptor on/off
- 🔌 Cable de alimentación con enchufe
- 🔒 Fusible de protección (si usas fuente interna)

---

## 📐 Diagrama de Alimentación

### Opción USB (Simple):
```
        ┌──────────────┐
220V AC │  Cargador    │ 5V DC
  ║     │     USB      │  │
  ║     │  (2A)        │  │
  ╚═════►│              ├──┤ Cable USB
        └──────────────┘  │
                          │
                          ▼
                   ┌──────────┐
                   │  ESP32   │
                   │  (USB)   │
                   └──────────┘
                        │
                        ├──► Pantalla ILI9341 (240x320)
                        ├──► AHT10
                        ├──► BMP180
                        └──► Botones
```

### Opción Fuente Interna (Avanzada):
```
       ┌────────────────────────────┐
       │     CAJA DEL PROYECTO      │
       │                            │
       │  ┌──────────────┐          │
220V ══╪══► Fuente 5V    ├──┐       │
  AC   │  │  Switching   │  │       │
       │  │  (HLK-PM01)  │  │5V     │
       │  └──────────────┘  │       │
       │         │GND       │       │
       │         │          │       │
       │         │    ┌─────▼─────┐ │
       │         └────┤   ESP32   │ │
       │              │   VIN/GND │ │
       │              └───────────┘ │
       │                   │        │
       │                   ├─► Pantalla
       │                   ├─► Sensores
       │                   └─► Botones
       │                            │
       └────────────────────────────┘
        ⚠️ 220V aislado en caja cerrada
```

---

## ✅ Conclusión

**Respuesta rápida:**
- ✅ **SÍ, se puede alimentar por 220V**
- ✅ **Ya funciona por USB**
- 🛒 **Solo necesitas**: Un cargador USB 5V 2A (~$1000)
- 🔌 **Enchufalo** y listo, tendrás corriente permanente

**Mi recomendación personal:**
Empieza con un **cargador USB de celular** (Samsung/Xiaomi 5V 2A). Es:
- Seguro
- Barato
- Fácil
- Funciona perfecto para tu consumo de 350-400mA

Si después querés algo más integrado para montaje permanente, podés pasar a una fuente switching interna.

---

**¿Dónde comprar en Argentina?**
- 🛒 MercadoLibre: Buscá "cargador USB 5V 2A" o "fuente switching 5V"
- 🏪 Casas de electrónica: Electrónica Liniers, Casa Talar, etc.
- 🌐 Online: PatagoniaTec, FullH4rd, etc.
