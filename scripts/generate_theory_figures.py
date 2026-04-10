from pathlib import Path
import math
import numpy as np
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / 'assets' / 'theory'


def save_fig(path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)
    plt.tight_layout()
    plt.savefig(path, dpi=180, bbox_inches='tight')
    plt.close()


def fig_uniform_field(path: Path):
    x, y = np.meshgrid(np.linspace(0, 10, 8), np.linspace(0, 6, 5))
    u = np.full_like(x, 2.0)
    v = np.zeros_like(y)
    plt.figure(figsize=(7.2, 4.2))
    plt.quiver(x, y, u, v, angles='xy', scale_units='xy', scale=1)
    plt.plot([1, 4.5], [3, 3.8], marker='o')
    plt.title('Campo uniforme y deriva lateral')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.grid(True, alpha=0.25)
    save_fig(path)


def fig_ode_parabola(path: Path):
    t = np.linspace(0, 4, 200)
    plt.figure(figsize=(7.2, 4.2))
    plt.plot(t, t**2, label='x(t)=t²')
    plt.scatter([0,1,2,3,4], [0,1,4,9,16])
    plt.title('Posición con aceleración constante')
    plt.xlabel('t')
    plt.ylabel('x(t)')
    plt.grid(True, alpha=0.25)
    plt.legend()
    save_fig(path)


def fig_euler(path: Path):
    t = np.linspace(0, 1, 200)
    exact = 1 + 2*t
    points_t = np.array([0.0, 0.5, 1.0])
    points_y = 1 + 2*points_t
    plt.figure(figsize=(7.2, 4.2))
    plt.plot(t, exact, label='Exacta')
    plt.plot(points_t, points_y, 'o--', label='Euler')
    plt.title('Euler frente a solución exacta')
    plt.xlabel('t')
    plt.ylabel('v(t)')
    plt.grid(True, alpha=0.25)
    plt.legend()
    save_fig(path)


def fig_heun(path: Path):
    t = np.linspace(0, 0.5, 200)
    exact = np.exp(t)
    pts_t = np.array([0.0, 0.1, 0.2, 0.3])
    pts_y = np.array([1.0, 1.105, 1.221, 1.349])
    plt.figure(figsize=(7.2, 4.2))
    plt.plot(t, exact, label='Exacta e^t')
    plt.plot(pts_t, pts_y, 'o--', label='Heun')
    plt.title('Heun en una EDO suave')
    plt.xlabel('t')
    plt.ylabel('y(t)')
    plt.grid(True, alpha=0.25)
    plt.legend()
    save_fig(path)


def fig_rk4(path: Path):
    t = np.linspace(0, 1.0, 200)
    exact = np.exp(-2*t)
    pts_t = np.array([0.0, 0.2, 0.4, 0.6, 0.8])
    pts_y = np.array([1.0, 0.6704, 0.4493, 0.3012, 0.2019])
    plt.figure(figsize=(7.2, 4.2))
    plt.plot(t, exact, label='Exacta')
    plt.plot(pts_t, pts_y, 'o--', label='RK4')
    plt.title('RK4 y decaimiento exponencial')
    plt.xlabel('t')
    plt.ylabel('y(t)')
    plt.grid(True, alpha=0.25)
    plt.legend()
    save_fig(path)


def fig_vector_sum(path: Path):
    plt.figure(figsize=(6.5, 4.6))
    origin = np.array([0, 0])
    vectors = [np.array([4, 0]), np.array([0, 1.5]), np.array([4, 1.5])]
    labels = ['Motor', 'Corriente', 'Resultado']
    for vec, label in zip(vectors, labels):
        plt.arrow(origin[0], origin[1], vec[0], vec[1], head_width=0.15, length_includes_head=True)
        plt.text(vec[0] + 0.1, vec[1] + 0.1, label)
    plt.xlim(-0.5, 5.5)
    plt.ylim(-0.5, 2.5)
    plt.axhline(0)
    plt.axvline(0)
    plt.grid(True, alpha=0.25)
    plt.title('Composición de velocidades')
    save_fig(path)


def radial_field(path: Path, rotational=False, title='Campo radial'):
    x, y = np.meshgrid(np.linspace(-3, 3, 11), np.linspace(-3, 3, 11))
    if rotational:
        u, v = -y, x
    else:
        u, v = x, y
    plt.figure(figsize=(6.6, 6.0))
    plt.quiver(x, y, u, v)
    theta = np.linspace(0, 2*np.pi, 200)
    if rotational:
        plt.plot(2*np.cos(theta), 2*np.sin(theta), '--')
    plt.title(title)
    plt.xlabel('x')
    plt.ylabel('y')
    plt.axis('equal')
    plt.grid(True, alpha=0.25)
    save_fig(path)


def fig_energy(path: Path):
    v = np.linspace(0, 6, 200)
    plt.figure(figsize=(7.2, 4.2))
    plt.plot(v, 5*v**2)
    plt.title('Energía cinética en función de la velocidad')
    plt.xlabel('v')
    plt.ylabel('E_k')
    plt.grid(True, alpha=0.25)
    save_fig(path)


def fig_power(path: Path):
    v = np.linspace(0, 6, 200)
    plt.figure(figsize=(7.2, 4.2))
    plt.plot(v, 30*v)
    plt.title('Potencia con fuerza constante')
    plt.xlabel('v')
    plt.ylabel('P')
    plt.grid(True, alpha=0.25)
    save_fig(path)


def fig_parametric(path: Path):
    t = np.linspace(0, 4, 200)
    x = t
    y = (t**2)/2
    plt.figure(figsize=(7.2, 4.6))
    plt.plot(x, y)
    pts = np.linspace(0, 4, 9)
    plt.scatter(pts, (pts**2)/2)
    plt.title('Curva paramétrica de referencia')
    plt.xlabel('x(t)')
    plt.ylabel('y(t)')
    plt.grid(True, alpha=0.25)
    save_fig(path)


def fig_line_integral(path: Path):
    x, y = np.meshgrid(np.linspace(0, 6, 8), np.linspace(0, 4, 5))
    u = np.full_like(x, 1.5)
    v = np.zeros_like(y)
    plt.figure(figsize=(7.2, 4.6))
    plt.quiver(x, y, u, v)
    plt.plot([0,5], [0,0], marker='o', label='Trayectoria 1')
    plt.plot([0,2,5], [2,2,2], marker='o', label='Trayectoria 2')
    plt.title('Trabajo sobre trayectorias en campo uniforme')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.legend()
    plt.grid(True, alpha=0.25)
    save_fig(path)


def fig_circulation(path: Path):
    radial_field(path, rotational=True, title='Contorno cerrado y circulación')


def fig_divergence(path: Path):
    radial_field(path, rotational=False, title='Divergencia positiva: fuente')


def fig_curl(path: Path):
    radial_field(path, rotational=True, title='Rotor positivo')


GENERATORS = {
    'vector_fields': fig_uniform_field,
    'ode': fig_ode_parabola,
    'euler': fig_euler,
    'heun': fig_heun,
    'rk4': fig_rk4,
    'forces': fig_vector_sum,
    'radial_field': lambda p: radial_field(p, rotational=False, title='Campo radial'),
    'rotational_field': lambda p: radial_field(p, rotational=True, title='Campo rotacional'),
    'energy_work': fig_energy,
    'power': fig_power,
    'trajectories': fig_parametric,
    'line_integral': fig_line_integral,
    'circulation': fig_circulation,
    'divergence': fig_divergence,
    'curl': fig_curl,
}

if __name__ == '__main__':
    for topic, generator in GENERATORS.items():
        target = ASSETS / topic / 'figure_main.png'
        generator(target)
        print(f'generated: {target}')
