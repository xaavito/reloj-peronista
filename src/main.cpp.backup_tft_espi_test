/*
 * DIAGNÓSTICO: TFT_eSPI - ILI9341
 * Prueba con la librería TFT_eSPI optimizada para ESP32
 */

#include <Arduino.h>
#include <TFT_eSPI.h>  // Librería TFT_eSPI
#include <SPI.h>

/*

#define USER_SETUP_INFO "User_Setup"

#define ILI9341_DRIVER       // Generic driver for common displays

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

#define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
#define TFT_HEIGHT 320 // ST7789 240 x 320

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#define SPI_FREQUENCY  55000000 // STM32 SPI1 only (SPI2 maximum is 27MHz)

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000

#define USE_HSPI_PORT



build_flags =
    -D USER_SETUP_LOADED=1
    -D ILI9341_DRIVER=1
    -D TFT_WIDTH=240
    -D TFT_HEIGHT=320
    -D TFT_MISO=19
    -D TFT_MOSI=23
    -D TFT_SCLK=18
    -D TFT_CS=15
    -D TFT_DC=2
    -D TFT_RST=-1
    -D LOAD_GLCD=1
    -D LOAD_FONT2=1
    -D LOAD_FONT4=1
    -D LOAD_FONT6=1
    -D LOAD_FONT7=1
    -D LOAD_FONT8=1
    -D LOAD_GFXFF=1
    -D SMOOTH_FONT=1
    -D SPI_FREQUENCY=40000000
    -D SPI_READ_FREQUENCY=20000000

*/

// Crear objeto TFT (la configuración viene de platformio.ini build_flags)
TFT_eSPI tft = TFT_eSPI();

void testTFT_eSPI(const char* configName) {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.printf("║  PROBANDO TFT_eSPI: %s\n", configName);
  Serial.println("║  Librería optimizada para ESP32       ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Inicializar TFT
  tft.init();
  tft.setRotation(1);  // Landscape (0-3 para diferentes rotaciones)
  
  Serial.println(">> ROJO...");
  tft.fillScreen(TFT_RED);
  delay(1000);
  
  Serial.println(">> VERDE...");
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  
  Serial.println(">> AZUL...");
  tft.fillScreen(TFT_BLUE);
  delay(1000);
  
  Serial.println(">> AMARILLO...");
  tft.fillScreen(TFT_YELLOW);
  delay(1000);
  
  Serial.println(">> TEXTO...");
  tft.fillScreen(TFT_BLACK);
  
  tft.setCursor(10, 50);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.println("TFT_eSPI Library");
  
  tft.setCursor(10, 80);
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(3);
  tft.println("RELOJ");
  
  tft.setCursor(10, 120);
  tft.setTextColor(TFT_YELLOW);
  tft.println("PERONISTA");
  
  // Dibujar rectángulos
  tft.drawRect(5, 5, 310, 230, TFT_WHITE);
  tft.drawRect(7, 7, 306, 226, TFT_WHITE);
  
  // Círculo azul y blanco
  tft.fillCircle(280, 200, 20, TFT_BLUE);
  tft.fillCircle(280, 200, 15, TFT_WHITE);
  
  delay(2000);
  
  // Test de fuentes grandes
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Font 4", 10, 10, 4);  // Font 4
  tft.drawString("Font 6", 10, 50, 6);  // Font 6
  tft.drawString("12:34", 10, 120, 7);  // Font 7 (7-segment)
  
  delay(2000);
  
  tft.fillScreen(TFT_WHITE);
  
  Serial.println("✓ Test completado!");
  Serial.println("Si viste los colores y texto: ¡FUNCIONA!\n");
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n");
  Serial.println("╔══════════════════════════════════════════════╗");
  Serial.println("║  TEST TFT_eSPI - ILI9341                    ║");
  Serial.println("║  Librería optimizada para ESP32             ║");
  Serial.println("╚══════════════════════════════════════════════╝");
  Serial.println();
  Serial.println("Configuración:");
  Serial.println("  - Driver: ILI9341");
  Serial.println("  - Resolución: 240x320");
  Serial.println("  - SPI: 40 MHz");
  Serial.println("  - Pines:");
  Serial.println("    * CS   = GPIO 15");
  Serial.println("    * DC   = GPIO 2");
  Serial.println("    * RST  = 3.3V (no usa GPIO)");
  Serial.println("    * MOSI = GPIO 23 (VSPI)");
  Serial.println("    * MISO = GPIO 19 (VSPI)");
  Serial.println("    * SCK  = GPIO 18 (VSPI)");
  Serial.println();
  delay(3000);
  
  // Ejecutar test
  testTFT_eSPI("ESP32 + ILI9341");
  
  // ===== FIN =====
  Serial.println("\n");
  Serial.println("╔══════════════════════════════════════════════╗");
  Serial.println("║         TEST COMPLETADO                      ║");
  Serial.println("╚══════════════════════════════════════════════╝");
  Serial.println();
  Serial.println("VENTAJAS DE TFT_eSPI:");
  Serial.println("  ✓ Mucho más rápido que Adafruit");
  Serial.println("  ✓ Optimizado para ESP32");
  Serial.println("  ✓ DMA y SPI de alta velocidad");
  Serial.println("  ✓ Sprites y double buffering");
  Serial.println("  ✓ Fuentes grandes incluidas");
  Serial.println();
  Serial.println("Si todo funciona, ya puedes usar TFT_eSPI");
  Serial.println("para tu reloj peronista con mejor rendimiento!");
  Serial.println();
}

void loop() {
  delay(5000);
  Serial.println(">>> Test completado. Presiona RESET para volver a probar. <<<");
}
