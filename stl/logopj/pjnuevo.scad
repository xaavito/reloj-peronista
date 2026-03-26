$fn = 100;

// ===== CONFIG =====
escala = 0.1;
altura_base = 2;

// alturas progresivas (de menor a mayor)
h1 = 0.4;
h2 = 0.6;
h3 = 0.8;
h4 = 1.0;
h5 = 1.3;
h6 = 1.6;
h7 = 2.0;

// ===== BASE =====
cylinder(d=45, h=altura_base);

// ===== CAPAS =====

// 1 - fondo general
translate([0,0,altura_base])
linear_extrude(height=h1)
scale([escala,escala,1])
import("recuadro.svg");

// 2 - marco
translate([0,0,altura_base + h1])
linear_extrude(height=h2)
scale([escala,escala,1])
import("marco.svg");

// 3 - borde
translate([0,0,altura_base + h1 + h2])
linear_extrude(height=h3)
scale([escala,escala,1])
import("borde.svg");

// 4 - manos (parte importante)
translate([0,0,altura_base + h1 + h2 + h3])
linear_extrude(height=h4)
scale([escala,escala,1])
import("manos.svg");

// 5 - verdes (detalle medio)
translate([0,0,altura_base + h1 + h2 + h3 + h4])
linear_extrude(height=h5)
scale([escala,escala,1])
import("verdes.svg");

// 6 - rojo (antorcha/fuego)
translate([0,0,altura_base + h1 + h2 + h3 + h4 + h5])
linear_extrude(height=h6)
scale([escala,escala,1])
import("rojo.svg");

// 7 - azul (detalle superior o fondo final)
translate([0,0,altura_base + h1 + h2 + h3 + h4 + h5 + h6])
linear_extrude(height=h7)
scale([escala,escala,1])
import("azul.svg"); 