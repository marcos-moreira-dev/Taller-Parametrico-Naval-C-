#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para arreglar acentos en el código fuente
Uso: python fix_accents.py
"""

import os
import re

# Directorio del proyecto
PROJECT_DIR = "src"

# Reemplazos comunes
REPLACEMENTS = {
    # Teoria / Teoría
    'Teoria': 'Teoría',
    'teoria': 'teoría',
    
    # Simulacion / Simulación  
    'Simulacion': 'Simulación',
    'simulacion': 'simulación',
    
    # Navegacion / Navegación
    'Navegacion': 'Navegación',
    'navegacion': 'navegación',
    
    # Configuracion / Configuración
    'Configuracion': 'Configuración',
    'configuracion': 'configuración',
    
    # Seleccion / Selección
    'Seleccion': 'Selección',
    'seleccion': 'selección',
    
    # Campo Vectorial / Campo Vectorial (ya está bien)
    'Campo Vectorial': 'Campo Vectorial',
    'campo vectorial': 'campo vectorial',
    
    # Boton / Botón
    'Boton': 'Botón',
    'boton': 'botón',
    
    # Funcion / Función
    'Funcion': 'Función',
    'funcion': 'función',
    
    # Ecuacion / Ecuación
    'Ecuacion': 'Ecuación',
    'ecuacion': 'ecuación',
    
    # Velocidad / Velocidad (ya está bien)
    'Velocidad': 'Velocidad',
    'velocidad': 'velocidad',
    
    # Posicion / Posición
    'Posicion': 'Posición',
    'posicion': 'posición',
    
    # Direccion / Dirección
    'Direccion': 'Dirección',
    'direccion': 'dirección',
    
    # Aceleracion / Aceleración
    'Aceleracion': 'Aceleración',
    'aceleracion': 'aceleración',
    
    # Informacion / Información
    'Informacion': 'Información',
    'informacion': 'información',
    
    # Configuracion / Configuración
    'Configuracion': 'Configuración',
    'configuracion': 'configuración',
}

# Patrón para encontrar strings wxT("...")
WXSTRING_PATTERN = re.compile(r'wxT\("([^"]*)"\)')

def fix_file(filepath):
    """Arregla acentos en un archivo"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"Error leyendo {filepath}: {e}")
        return False
    
    original = content
    changes = []
    
    # Buscar y reemplazar en strings wxT()
    def replace_in_wxstring(match):
        text = match.group(1)
        new_text = text
        
        for old, new in REPLACEMENTS.items():
            if old in new_text:
                new_text = new_text.replace(old, new)
        
        if new_text != text:
            changes.append(f"  '{text}' -> '{new_text}'")
        
        return f'wxT("{new_text}")'
    
    content = WXSTRING_PATTERN.sub(replace_in_wxstring, content)
    
    # También buscar en comentarios
    for old, new in REPLACEMENTS.items():
        if old in content:
            content = content.replace(old, new)
            changes.append(f"  Comentario: {old} -> {new}")
    
    if content != original:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"✓ {filepath}")
            for change in changes[:5]:  # Mostrar primeros 5 cambios
                print(change)
            if len(changes) > 5:
                print(f"  ... y {len(changes)-5} cambios más")
            return True
        except Exception as e:
            print(f"✗ Error escribiendo {filepath}: {e}")
            return False
    
    return False

def main():
    print("Arreglando acentos en el proyecto...")
    print("=" * 50)
    
    files_fixed = 0
    
    for root, dirs, files in os.walk(PROJECT_DIR):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.hpp') or file.endswith('.h'):
                filepath = os.path.join(root, file)
                if fix_file(filepath):
                    files_fixed += 1
    
    print("=" * 50)
    print(f"Archivos modificados: {files_fixed}")
    print("Listo!")

if __name__ == "__main__":
    main()
