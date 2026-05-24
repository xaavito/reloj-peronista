#!/usr/bin/env python3
"""
Script para convertir imagen JPG a array C en formato RGB565
Para usar en ESP32 con TFT_eSPI
"""

from PIL import Image, ImageEnhance, ImageFilter
import sys

def rgb888_to_rgb565(r, g, b):
    """Convierte RGB888 a RGB565"""
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def convert_image_to_array(input_file, output_file, target_width=160, target_height=160):
    """Convierte imagen a array C"""
    
    print(f"Abriendo imagen: {input_file}")
    img = Image.open(input_file)
    
    # Convertir a RGB si no lo es
    if img.mode != 'RGB':
        img = img.convert('RGB')
    
    # Redimensionar manteniendo aspect ratio
    img.thumbnail((target_width, target_height), Image.Resampling.LANCZOS)
    
    # ========== MEJORAS DE IMAGEN ==========
    print("Aplicando mejoras de imagen...")
    
    # Aumentar contraste para mejorar definición
    enhancer = ImageEnhance.Contrast(img)
    img = enhancer.enhance(1.3)  # Aumentar contraste 30%
    
    # Ajustar brillo ligeramente
    enhancer = ImageEnhance.Brightness(img)
    img = enhancer.enhance(1.1)  # Aumentar brillo 10%
    
    # Aumentar saturación para colores más vivos
    enhancer = ImageEnhance.Color(img)
    img = enhancer.enhance(1.2)  # Aumentar saturación 20%
    
    # Aplicar sharpening suave para mejorar detalles
    img = img.filter(ImageFilter.SHARPEN)
    
    width, height = img.size
    print(f"Tamaño final: {width}x{height}")
    
    # Generar array C
    with open(output_file, 'w') as f:
        f.write("// Imagen de Perón convertida a RGB565\n")
        f.write(f"// Tamaño: {width}x{height}\n\n")
        f.write("#ifndef PERON_IMAGE_H\n")
        f.write("#define PERON_IMAGE_H\n\n")
        f.write("#include <Arduino.h>\n\n")
        
        f.write(f"const uint16_t PERON_IMG_WIDTH = {width};\n")
        f.write(f"const uint16_t PERON_IMG_HEIGHT = {height};\n\n")
        
        f.write("const uint16_t peron_image[] PROGMEM = {\n")
        
        pixel_count = 0
        for y in range(height):
            f.write("  ")
            for x in range(width):
                r, g, b = img.getpixel((x, y))
                rgb565 = rgb888_to_rgb565(r, g, b)
                
                f.write(f"0x{rgb565:04X}")
                
                pixel_count += 1
                if pixel_count < width * height:
                    f.write(",")
                
                if (x + 1) % 10 == 0 and x < width - 1:
                    f.write("\n  ")
            f.write("\n")
        
        f.write("};\n\n")
        f.write("#endif // PERON_IMAGE_H\n")
    
    print(f"✓ Archivo generado: {output_file}")
    print(f"✓ Total de píxeles: {pixel_count}")
    print(f"✓ Tamaño en memoria: {pixel_count * 2} bytes (~{pixel_count * 2 / 1024:.1f} KB)")

if __name__ == "__main__":
    input_file = "stl/peron.jpg"
    output_file = "include/peron_image.h"
    
    print("=" * 50)
    print("Convertidor de Imagen a RGB565 para ESP32")
    print("=" * 50)
    
    try:
        convert_image_to_array(input_file, output_file, target_width=120, target_height=140)
        print("\n✓ Conversión exitosa!")
        print(f"\nAhora incluye el archivo en tu código:")
        print(f'  #include "peron_image.h"')
    except FileNotFoundError:
        print(f"\n✗ Error: No se encontró el archivo {input_file}")
        print("Asegúrate de que la imagen existe en la carpeta correcta.")
    except Exception as e:
        print(f"\n✗ Error: {e}")
