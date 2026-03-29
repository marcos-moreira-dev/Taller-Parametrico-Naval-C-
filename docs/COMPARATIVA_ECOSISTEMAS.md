# Comparativa de Ecosistemas de Desarrollo

**C++ (wxWidgets/Ninja) vs Java (Maven) vs Python vs Node.js**

---

## Tabla Comparativa Rápida

| Aspecto                    | C++ (Nuestro Proyecto)   | Java (Maven)               | Python                    | Node.js                 |
| -------------------------- | ------------------------ | -------------------------- | ------------------------- | ----------------------- |
| **Compilación**            | Sí (a binario)           | Sí (a bytecode)            | No (interpretado)         | No (interpretado JIT)   |
| **Build Tool**             | CMake + Ninja            | Maven/Gradle               | setuptools/poetry         | npm/yarn/pnpm           |
| **Gestor de dependencias** | CMake FetchContent/vcpkg | Maven Central              | pip/PyPI                  | npm registry            |
| **Archivo de config**      | CMakeLists.txt           | pom.xml                    | setup.py/pyproject.toml   | package.json            |
| **Ejecutable**             | .exe (nativo)            | .jar (bytecode)            | .py (source)              | .js (source)            |
| **Velocidad**              | Muy rápido               | Rápido                     | Lento                     | Rápido (V8)             |
| **GUI Framework**          | wxWidgets/Qt/GTK         | JavaFX/Swing               | tkinter/PyQt/wxPython     | Electron/React          |
| **Tiempo build**           | Largo (compilación)      | Medio                      | Instantáneo               | Medio (bundling)        |
| **Portabilidad**           | Compilar en cada SO      | "Write once, run anywhere" | Necesita Python instalado | Necesita Node instalado |

---

## 1. C++ con CMake + Ninja + wxWidgets

### Flujo de Trabajo

```
Código Fuente (.cpp/.hpp)
    ↓
CMake (genera build.ninja)
    ↓
Ninja (coordina compilación)
    ↓
g++ (compila cada archivo)
    ↓
ld (linka librerías)
    ↓
TallerParametrico.exe (nativo)
```

### Archivos Clave

```
cplus/
├── CMakeLists.txt          # Define targets, dependencias, flags
├── build/
│   ├── build.ninja         # Script de build generado
│   └── bin/
│       └── TallerParametrico.exe
└── src/
    └── presentation/       # Código GUI wxWidgets
        └── main_window/
            └── MainWindow.cpp
```

### Comandos Esenciales

```bash
# Configurar
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Compilar
ninja

# Ejecutar
./bin/TallerParametrico.exe

# Instalar dependencias (wxWidgets)
cd build/wxwidgets && ninja && ninja install
```

### Características

- ✅ Ejecutable nativo (no necesita runtime)
- ✅ Rendimiento máximo
- ❌ Compilación lenta (20-40 min wxWidgets)
- ❌ Gestión manual de DLLs

---

## 2. Java con Maven + JavaFX

### Flujo de Trabajo

```
Código Fuente (.java)
    ↓
Maven (descarga dependencias)
    ↓
javac (compila a bytecode .class)
    ↓
Empaqueta en .jar
    ↓
java -jar app.jar (JVM ejecuta)
```

### Archivos Clave

```
java-project/
├── pom.xml                      # Dependencias y build config
├── src/
│   └── main/
│       ├── java/
│       │   └── com/taller/
│       │       └── Main.java    # Código con JavaFX
│       └── resources/
│           └── view.fxml        # UI declarativa
└── target/
    └── TallerParametrico-1.0.jar
```

### Ejemplo pom.xml

```xml
<project>
    <modelVersion>4.0.0</modelVersion>
    <groupId>com.taller</groupId>
    <artifactId>parametrico</artifactId>
    <version>1.0</version>

    <dependencies>
        <!-- JavaFX -->
        <dependency>
            <groupId>org.openjfx</groupId>
            <artifactId>javafx-controls</artifactId>
            <version>21</version>
        </dependency>
    </dependencies>

    <build>
        <plugins>
            <plugin>
                <groupId>org.apache.maven.plugins</groupId>
                <artifactId>maven-compiler-plugin</artifactId>
                <version>3.11</version>
                <configuration>
                    <release>17</release>
                </configuration>
            </plugin>
        </plugins>
    </build>
</project>
```

### Comandos Esenciales

```bash
# Descargar dependencias
mvn dependency:resolve

# Compilar
mvn compile

# Empaquetar
mvn package

# Ejecutar
mvn javafx:run
# o
java -jar target/TallerParametrico-1.0.jar
```

### Características

- ✅ "Write once, run anywhere" (con JVM)
- ✅ Maven maneja dependencias automáticamente
- ✅ JavaFX moderno y profesional
- ❌ Necesita JRE/JVM instalado
- ❌ Menor rendimiento que C++

---

## 3. Python (Aplicación Completa)

### Flujo de Trabajo

```
Código Fuente (.py)
    ↓
Python Interpreter (CPython)
    ↓
Importa módulos (dinámicamente)
    ↓
Ejecución directa (no hay build)
    ↓
Opcional: Empaquetar con PyInstaller
```

### Archivos Clave

```
python-project/
├── pyproject.toml           # Config moderna (PEP 518)
├── setup.py                 # Config tradicional
├── requirements.txt         # Dependencias
├── src/
│   └── taller_parametrico/
│       ├── __init__.py
│       ├── __main__.py      # Punto de entrada
│       ├── gui/
│       │   └── main_window.py  # tkinter/PyQt/wxPython
│       └── simulation/
└── dist/                    # Generado por PyInstaller
    └── TallerParametrico.exe
```

### Ejemplo pyproject.toml

```toml
[build-system]
requires = ["setuptools", "wheel"]
build-backend = "setuptools.build_meta"

[project]
name = "taller-parametrico"
version = "1.0"
dependencies = [
    "wxPython>=4.2",
    "numpy",
    "matplotlib"
]

[project.gui-scripts]
taller-parametrico = "taller_parametrico.__main__:main"
```

### Ejemplo Código Python (wxPython)

```python
import wx

class MainWindow(wx.Frame):
    def __init__(self):
        super().__init__(None, title="Taller Parametrico")
        panel = wx.Panel(self)

        # Botones, controles, etc.
        btn = wx.Button(panel, label="Simular")
        btn.Bind(wx.EVT_BUTTON, self.on_simulate)

    def on_simulate(self, event):
        print("Simulando...")

if __name__ == "__main__":
    app = wx.App()
    frame = MainWindow()
    frame.Show()
    app.MainLoop()
```

### Comandos Esenciales

```bash
# Instalar dependencias
pip install -r requirements.txt

# Ejecutar directamente
python -m taller_parametrico

# O empaquetar como ejecutable
pip install pyinstaller
pyinstaller --onefile --windowed src/taller_parametrico/__main__.py

# Instalar localmente
pip install -e .
```

### Características

- ✅ Desarrollo ultrarrápido
- ✅ Muchas librerías disponibles
- ✅ Sintaxis simple
- ❌ Necesita Python instalado (o empaquetar)
- ❌ Rendimiento lento (interpretado)
- ❌ GIL limita threading

---

## 4. Node.js con Electron (Aplicación Desktop)

### Flujo de Trabajo

```
Código Fuente (.js/.html/.css)
    ↓
npm install (descarga node_modules)
    ↓
npm run build (webpack/vite bundle)
    ↓
Empaqueta con Electron
    ↓
TallerParametrico.exe (con Chromium embebido)
```

### Archivos Clave

```
nodejs-project/
├── package.json             # Dependencias y scripts
├── vite.config.js           # Bundler config
├── electron-builder.json    # Config de empaquetado
├── src/
│   ├── main/
│   │   └── main.js          # Proceso principal Electron
│   ├── renderer/
│   │   ├── App.jsx          # React/Vue componentes
│   │   └── index.html       # UI web
│   └── simulation/
│       └── physics.js       # Lógica de simulación
└── dist/                    # Build generado
```

### Ejemplo package.json

```json
{
  "name": "taller-parametrico",
  "version": "1.0.0",
  "main": "src/main/main.js",
  "scripts": {
    "start": "electron .",
    "build": "vite build && electron-builder",
    "dev": "vite"
  },
  "dependencies": {
    "electron": "^28.0.0",
    "react": "^18.0.0",
    "three": "^0.160.0"
  },
  "devDependencies": {
    "vite": "^5.0.0",
    "electron-builder": "^24.0.0"
  }
}
```

### Ejemplo Código (Electron + React)

```javascript
// src/main/main.js - Proceso principal
const { app, BrowserWindow } = require("electron");

function createWindow() {
  const win = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
    },
  });

  win.loadFile("dist/index.html");
}

app.whenReady().then(createWindow);
```

```jsx
// src/renderer/App.jsx - UI con React
import React, { useState } from "react";

function App() {
  const [running, setRunning] = useState(false);

  return (
    <div className="app">
      <h1>Taller Parametrico</h1>
      <button onClick={() => setRunning(!running)}>
        {running ? "Detener" : "Simular"}
      </button>
      <canvas id="simulation"></canvas>
    </div>
  );
}

export default App;
```

### Comandos Esenciales

```bash
# Instalar dependencias
npm install

# Desarrollo
npm run dev

# Build
npm run build

# Empaquetar
npx electron-builder

# Ejecutar
npm start
```

### Características

- ✅ UI web moderna (HTML/CSS/JS)
- ✅ Muchos frameworks disponibles
- ✅ Hot reload en desarrollo
- ❌ Ejecutable muy grande (>100MB por Chromium)
- ❌ Alto consumo de memoria
- ❌ Más lento que nativo

---

## Comparación de Build Tools

### C++: CMake + Ninja

```cmake
# CMakeLists.txt
add_executable(TallerParametrico WIN32
    src/main.cpp
    src/Window.cpp
)

target_link_libraries(TallerParametrico
    wxWidgets
    # Librerías del sistema
)
```

**Filosofía**: "Configurar una vez, compilar en cualquier plataforma"

### Java: Maven

```xml
<!-- pom.xml -->
<dependencies>
    <dependency>
        <groupId>org.openjfx</groupId>
        <artifactId>javafx-controls</artifactId>
        <version>21</version>
    </dependency>
</dependencies>
```

**Filosofía**: "Convención sobre configuración"

### Python: pip + setuptools

```toml
# pyproject.toml
[project]
dependencies = [
    "wxPython>=4.2",
]

[project.scripts]
app = "module:main"
```

**Filosofía**: "Simplicidad y legibilidad"

### Node.js: npm + package.json

```json
{
  "dependencies": {
    "electron": "^28.0.0"
  },
  "scripts": {
    "build": "electron-builder"
  }
}
```

**Filosofía**: "Todo es un módulo"

---

## Comparación de GUIs

### wxWidgets (C++) vs JavaFX (Java) vs wxPython (Python) vs Electron (Node)

| Característica           | wxWidgets     | JavaFX         | wxPython         | Electron        |
| ------------------------ | ------------- | -------------- | ---------------- | --------------- |
| **Look & Feel**          | Nativo del SO | Moderno/Custom | Nativo (wrapper) | Web (Chromium)  |
| **Rendimiento**          | Excelente     | Bueno          | Regular          | Medio           |
| **Tamaño app**           | Pequeño       | Medio          | Grande (Python)  | Muy grande      |
| **Curva de aprendizaje** | Media         | Media          | Baja             | Baja            |
| **Personalización**      | Limitada      | Alta           | Limitada         | Ilimitada (CSS) |

---

## Contexto Consola vs GUI

### C++

```cpp
// CONSOLA
int main(int argc, char* argv[]) {
    std::cout << "Taller Parametrico - Consola" << std::endl;
    // Lógica de simulación...
    return 0;
}

// GUI (wxWidgets)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    wxApp app;
    MainWindow* frame = new MainWindow();
    frame->Show();
    return app.MainLoop();
}
```

### Java

```java
// CONSOLA
public class Main {
    public static void main(String[] args) {
        System.out.println("Taller Parametrico - Consola");
        // Lógica...
    }
}

// GUI (JavaFX)
public class Main extends Application {
    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("Taller Parametrico");
        primaryStage.setScene(new Scene(new MainView()));
        primaryStage.show();
    }
}
```

### Python

```python
# CONSOLA
def main():
    print("Taller Parametrico - Consola")
    # Lógica...

# GUI (wxPython)
class MainWindow(wx.Frame):
    def __init__(self):
        super().__init__(None, title="Taller Parametrico")
        self.Show()

if __name__ == "__main__":
    app = wx.App()
    MainWindow()
    app.MainLoop()
```

### Node.js

```javascript
// CONSOLA (Node.js puro)
console.log("Taller Parametrico - Consola");

// GUI (Electron)
const { app, BrowserWindow } = require("electron");

app.whenReady().then(() => {
  new BrowserWindow({
    title: "Taller Parametrico",
    width: 1200,
  }).loadFile("index.html");
});
```

---

## ¿Cuándo usar cada uno?

### Usa C++ cuando:

- ✅ Necesitas máximo rendimiento
- ✅ Acceso a hardware de bajo nivel
- ✅ Ejecutable autocontenido sin runtime
- ✅ Aplicación de simulación científica

### Usa Java cuando:

- ✅ Equipo familiarizado con JVM
- ✅ Necesitas portabilidad entre SO sin recompilar
- ✅ Empresas (banca, grandes corporaciones)
- ✅ Aplicaciones empresariales complejas

### Usa Python cuando:

- ✅ Prototipado rápido
- ✅ Integración con ciencia de datos/ML
- ✅ Scripts de automatización
- ✅ Web backends (Django/Flask)

### Usa Node.js cuando:

- ✅ Quieres UI web en desktop
- ✅ Equipo de desarrollo web
- ✅ Hot reload rápido en desarrollo
- ✅ Multiplataforma con look moderno

---

## Conclusión

**Nuestro Proyecto (C++ + wxWidgets + Ninja) es ideal porque**:

1. **Rendimiento**: Simulación física necesita velocidad
2. **Independencia**: Un solo .exe sin dependencias de runtime
3. **Nativo**: Look y feel de Windows nativo
4. **Control**: Acceso completo al hardware

**El costo**: Complejidad en build, compilación lenta inicial, gestión manual de DLLs.

**Alternativas válidas**:

- **JavaFX**: Si necesitas portabilidad JVM
- **Python + PyInstaller**: Si quieres desarrollo rápido
- **Electron**: Si prefieres UI web moderna

**Regla de oro**: "Usa la herramienta adecuada para el trabajo, no la más hypeada."

---

## Recursos Adicionales

- **C++**: [cppreference.com](https://cppreference.com), [cmake.org](https://cmake.org)
- **Java**: [maven.apache.org](https://maven.apache.org), [openjfx.io](https://openjfx.io)
- **Python**: [docs.python.org](https://docs.python.org), [packaging.python.org](https://packaging.python.org)
- **Node.js**: [nodejs.org](https://nodejs.org), [electronjs.org](https://electronjs.org)
