import math
import struct

# ==========================================
# CONFIGURATION ET PARAMÈTRES FIXES
# ==========================================
POINTS_PER_SEGMENT = 15    # Densité de la courbe (nombre de points entre chaque P_i)
V_MAX              = 2.0   # Vitesse maximale en ligne droite (m/s)
K_CURVE            = 1.2   # Sensibilité de base au virage
MAX_ACCEL          = 0.5   # Accélération/Freinage max du robot (en m/s²)

SHOW_PLOT_BY_DEF   = True

# Gestion de l'affichage optionnel (PC vs Luckfox)
try:
    import matplotlib.pyplot as plt
    PLOT_AVAILABLE = True
except ImportError:
    PLOT_AVAILABLE = False


def generate_full_path(waypoints, points_per_segment=POINTS_PER_SEGMENT, v_max=V_MAX, k_curve=K_CURVE, show_plot=SHOW_PLOT_BY_DEF):
    
    def get_catmull_point(p0, p1, p2, p3, t):
        def calc(v0, v1, v2, v3, t):
            return 0.5 * ((2*v1) + (-v0+v2)*t + (2*v0-5*v1+4*v2-v3)*t**2 + (-v0+3*v1-3*v2+v3)*t**3)
        return (calc(p0[0], p1[0], p2[0], p3[0], t), calc(p0[1], p1[1], p2[1], p3[1], t))

    # Extension pour la continuité de la boucle fermée
    extended = [waypoints[-1]] + waypoints + [waypoints[0]] + [waypoints[1]]

    # 1. Génération de la courbe continue
    raw_path = []
    for i in range(1, len(extended) - 2):
        for j in range(points_per_segment):
            t = j / points_per_segment
            raw_path.append(get_catmull_point(extended[i-1], extended[i], extended[i+1], extended[i+2], t))

    n_points = len(raw_path)
    
    # 2. Premier passage : Vitesse théorique max locale (liée à la courbure brute)
    local_speeds = []
    for i in range(n_points):
        curr = raw_path[i]
        prev = raw_path[i-1] if i > 0 else raw_path[-1]
        nxt = raw_path[(i+1) % n_points]
        
        v1 = (curr[0]-prev[0], curr[1]-prev[1])
        v2 = (nxt[0]-curr[0], nxt[1]-curr[1])
        mag1 = math.sqrt(v1[0]**2 + v1[1]**2) or 1e-6
        mag2 = math.sqrt(v2[0]**2 + v2[1]**2) or 1e-6
        
        dot = (v1[0]*v2[0] + v1[1]*v2[1]) / (mag1 * mag2)
        dot = max(-1.0, min(1.0, dot))
        angle = math.acos(dot)
        
        target_v = v_max * (1.0 - min(1.0, angle * k_curve))
        local_speeds.append(max(0.1, target_v))

    # 3. Deuxième passage : Filtre d'anticipation (Forward-Backward Pass)
    final_speeds = list(local_speeds)
    for _ in range(2):
        for i in range(n_points - 1, -1, -1):
            nxt_idx = (i + 1) % n_points
            
            dx = raw_path[nxt_idx][0] - raw_path[i][0]
            dy = raw_path[nxt_idx][1] - raw_path[i][1]
            dist = math.sqrt(dx**2 + dy**2)
            
            # Formule physique de freinage : Vf² = Vi² + 2*a*d
            max_v_allowed = math.sqrt(final_speeds[nxt_idx]**2 + 2 * MAX_ACCEL * dist)
            
            if final_speeds[i] > max_v_allowed:
                final_speeds[i] = max_v_allowed

    # 4. Encodage binaire pour l'ESP32
    binary_data = bytearray()
    for i in range(n_points):
        binary_data.extend(struct.pack('fff', raw_path[i][0], raw_path[i][1], final_speeds[i]))
        
    # 5. Rendu Graphique (Sur PC)
    if show_plot and PLOT_AVAILABLE:
        x_val = [p[0] for p in raw_path]
        y_val = [p[1] for p in raw_path]
        
        plt.figure(figsize=(12, 8))
        plt.gca().set_facecolor('#F8F8F8')
        
        # Affiche la courbe lissée colorée selon la vitesse (Dégradé Turbo)
        scatter = plt.scatter(x_val, y_val, c=final_speeds, cmap='turbo', s=15, label='Trajectoire lissée (Robot)')
        cbar = plt.colorbar(scatter)
        cbar.set_label('Vitesse cible anticipée (m/s)')
        
        # Filtrer les waypoints uniques pour éviter de superposer P1 et le point de fermeture
        unique_wps = waypoints[:-1] if waypoints[0] == waypoints[-1] else waypoints
        wp_x = [w[0] for w in unique_wps]
        wp_y = [w[1] for w in unique_wps]
        
        # Dessin des points de passage initiaux (Gros repères noirs)
        plt.scatter(wp_x, wp_y, color='black', s=100, zorder=10, label='Waypoints (Points de passage)')
        
        # Ajout des étiquettes P1, P2, P3...
        for idx, (x, y) in enumerate(unique_wps):
            plt.text(x + 0.15, y + 0.15, f'P{idx+1}', fontsize=10, fontweight='bold',
                     bbox=dict(facecolor='white', alpha=0.8, edgecolor='gray', boxstyle='round,pad=0.2'), zorder=11)
        
        plt.title(f"Profil de vitesse avec ANTICIPATION AUTOMATIQUE\nFreinage calculé selon MAX_ACCEL = {MAX_ACCEL} m/s²")
        plt.xlabel("X (m)")
        plt.ylabel("Y (m)")
        plt.axis('equal')
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.legend(loc='lower left')
        plt.show()
        
    return wp_x,wp_y

# ==========================================
# ZONE DE TEST (Circuit d'évaluation Robot)
# ==========================================
if __name__ == "__main__":
    # Circuit de test équilibré (21 points + 1 fermeture)
    # circuit_test_robot = [
    #     (0.0, 0.0), (3.0, 0.0), (6.0, 0.0), (9.0, 0.0), # Ligne droite (P1 à P4)
    #     (11.0, 1.0), (12.0, 3.0), (11.0, 5.0),          # Virage large (P5 à P7)
    #     (8.0, 5.0), (5.0, 5.0),                         # Intermédiaire (P8 à P9)
    #     (4.0, 6.0), (3.0, 7.0), (4.0, 8.0), (6.0, 8.0), # Chicane (P10 à P13)
    #     (8.0, 9.0), (8.5, 10.5), (7.0, 11.0),           # Épingle serrée (P14 à P16)
    #     (4.0, 11.0), (1.0, 10.0), (-1.0, 7.0), (-2.0, 3.0), (-1.0, 1.0) # Retour (P17 à P21)

    # ]

    # Simple courbe sans retour
    circuit_test_robot = [
        (0.0, 0.0), (3.0, 0.0), (6.0, 0.0), (9.0, 0.0), # Ligne droite (P1 à P4)
        (11.0, 1.0), (12.0, 3.0), (11.0, 5.0)             # Virage large (P5 à P7)
    ]

    data = generate_full_path(circuit_test_robot, show_plot=True)
    print(data)