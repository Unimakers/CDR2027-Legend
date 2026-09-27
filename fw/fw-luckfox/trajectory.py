import math
import struct
import json
import os

# ==========================================
# CONFIGURATION ET PARAMÈTRES FIXES
# ==========================================
POINTS_PER_SEGMENT = 15    # Densité de la courbe (nombre de points entre chaque P_i)
V_MAX              = 2.0   # Vitesse maximale en ligne droite (m/s)
K_CURVE            = 1.2   # Sensibilité de base au virage
MAX_ACCEL          = 0.5   # Accélération/Freinage max du robot (en m/s²)

# --- PARAMÈTRES DE SYMÉTRIE (MIROIR) ---
TABLE_WIDTH_X      = 2.0   # Largeur de la table en mètres
TABLE_LENGTH_Y     = 3.0   # Longueur de la table en mètres
MIRROR_AXIS        = 'Y'   # 'X' (Gauche/Droite) ou 'Y' (Haut/Bas) selon ton interface

SHOW_PLOT_BY_DEF   = True

try:
    import matplotlib.pyplot as plt
    PLOT_AVAILABLE = True
except ImportError:
    PLOT_AVAILABLE = False


def generate_full_path(waypoints, points_per_segment=POINTS_PER_SEGMENT, v_max=V_MAX, k_curve=K_CURVE, show_plot=False, segment_index=1):
    """
    Génère la trajectoire optimisée pour un tableau de waypoints (Tronçon).
    """
    if len(waypoints) < 2:
        return []

    def get_catmull_point(p0, p1, p2, p3, t):
        def calc(v0, v1, v2, v3, t):
            return 0.5 * ((2*v1) + (-v0+v2)*t + (2*v0-5*v1+4*v2-v3)*t**2 + (-v0+3*v1-3*v2+v3)*t**3)
        return (calc(p0[0], p1[0], p2[0], p3[0], t), calc(p0[1], p1[1], p2[1], p3[1], t))

    # Courbe ouverte (On clone les extrémités)
    extended = [waypoints[0]] + waypoints + [waypoints[-1]] + [waypoints[-1]]

    # 1. Génération de la courbe continue
    raw_path = []
    for i in range(1, len(extended) - 2):
        for j in range(points_per_segment):
            t = j / points_per_segment
            raw_path.append(get_catmull_point(extended[i-1], extended[i], extended[i+1], extended[i+2], t))

    # Ajout du point final absolu
    raw_path.append(waypoints[-1])
    n_points = len(raw_path)
    
    # 2. Premier passage : Vitesse théorique max locale
    local_speeds = []
    for i in range(n_points):
        curr = raw_path[i]
        prev = raw_path[i-1] if i > 0 else raw_path[0]
        nxt = raw_path[i+1] if i < n_points - 1 else raw_path[-1]
        
        v1 = (curr[0]-prev[0], curr[1]-prev[1])
        v2 = (nxt[0]-curr[0], nxt[1]-curr[1])
        mag1 = math.sqrt(v1[0]**2 + v1[1]**2) or 1e-6
        mag2 = math.sqrt(v2[0]**2 + v2[1]**2) or 1e-6
        
        dot = (v1[0]*v2[0] + v1[1]*v2[1]) / (mag1 * mag2)
        dot = max(-1.0, min(1.0, dot))
        angle = math.acos(dot)
        
        target_v = v_max * (1.0 - min(1.0, angle * k_curve))
        local_speeds.append(max(0.1, target_v))

    # Force la vitesse d'arrivée exacte à 0
    local_speeds[-1] = 0.0

    # 3. Deuxième passage : Filtre d'anticipation (Freinage)
    final_speeds = list(local_speeds)
    for _ in range(2):
        for i in range(n_points - 2, -1, -1):
            nxt_idx = i + 1
            
            dx = raw_path[nxt_idx][0] - raw_path[i][0]
            dy = raw_path[nxt_idx][1] - raw_path[i][1]
            dist = math.sqrt(dx**2 + dy**2)
            
            max_v_allowed = math.sqrt(final_speeds[nxt_idx]**2 + 2 * MAX_ACCEL * dist)
            
            if final_speeds[i] > max_v_allowed:
                final_speeds[i] = max_v_allowed

    # 4. Encodage binaire
    binary_data = bytearray()
    for i in range(n_points):
        binary_data.extend(struct.pack('fff', raw_path[i][0], raw_path[i][1], final_speeds[i]))
        
    # 5. Rendu Graphique
    if show_plot and PLOT_AVAILABLE:
        x_val = [p[0] for p in raw_path]
        y_val = [p[1] for p in raw_path]
        
        plt.figure(figsize=(10, 6))
        plt.gca().set_facecolor('#F8F8F8')
        
        scatter = plt.scatter(x_val, y_val, c=final_speeds, cmap='turbo', s=15)
        cbar = plt.colorbar(scatter)
        cbar.set_label('Vitesse (m/s)')
        
        wp_x = [w[0] for w in waypoints]
        wp_y = [w[1] for w in waypoints]
        plt.scatter(wp_x, wp_y, color='black', s=100, zorder=10, label='Waypoints')
        
        for idx, (x, y) in enumerate(waypoints):
            plt.text(x + 0.05, y + 0.05, f'P{idx}', fontsize=10, fontweight='bold')
        
        plt.title(f"Tronçon {segment_index} - Arrêt automatique à l'arrivée")
        plt.xlabel("X (m)"); plt.ylabel("Y (m)")
        plt.axis('equal'); plt.grid(True, linestyle='--', alpha=0.5)
        plt.show()
        
    return raw_path, final_speeds, binary_data

# ==========================================
# FONCTION DE SYMÉTRIE (MIROIR)
# ==========================================
def apply_mirror_to_strategy(strategy, axis='X'):
    """
    Inverse les coordonnées et les angles en mémoire pour l'équipe adverse.
    """
    mirrored = []
    for step in strategy:
        new_step = step.copy() # Copie pour ne pas altérer l'original
        
        if new_step["type"] == "MOVE":
            if axis == 'X':
                # Symétrie Gauche/Droite
                new_step["x"] = round(TABLE_WIDTH_X - step["x"], 3)
                new_step["theta"] = (180 - step["theta"]) % 360
            elif axis == 'Y':
                # Symétrie Haut/Bas
                new_step["y"] = round(TABLE_LENGTH_Y - step["y"], 3)
                new_step["theta"] = (-step["theta"]) % 360
                
        mirrored.append(new_step)
    return mirrored

# ==========================================
# PARSEUR DE STRATÉGIE WEB (JSON)
# ==========================================
def process_strategy_file(filename="strategy.json", team_color="BLUE"):
    if not os.path.exists(filename):
        print(f"[ERREUR] Le fichier {filename} n'existe pas.")
        return

    with open(filename, 'r') as f:
        strat = json.load(f)

    # --- APPLICATION DYNAMIQUE DU MIROIR ---
    if team_color == "YELLOW":
        print(f"\n[STRATEGIE] Équipe JAUNE détectée : Application du miroir sur l'axe {MIRROR_AXIS}.")
        strat = apply_mirror_to_strategy(strat, axis=MIRROR_AXIS)
    else:
        print(f"\n[STRATEGIE] Équipe BLEUE détectée : Stratégie standard.")
    # ---------------------------------------

    print(f"Chargement de la séquence : {len(strat)} instructions au total.")

    current_chunk = []
    segment_counter = 1

    for step in strat:
        if step["type"] == "MOVE":
            current_chunk.append((step["x"], step["y"]))
            
        elif step["type"] == "ACTION":
            if len(current_chunk) >= 2:
                print(f"\n--- Calcul du Tronçon {segment_counter} ({len(current_chunk)} waypoints) ---")
                path, speeds, bindata = generate_full_path(current_chunk, show_plot=SHOW_PLOT_BY_DEF, segment_index=segment_counter)
                print(f">> Généré {len(path)} points interpolés. Vitesse finale = {speeds[-1]:.2f} m/s")
                segment_counter += 1
                
            print(f"\n[!] ARRÊT ROBOT - Exécution de l'action : {step['cmd']} (Param: {step.get('param', 0)})")
            
            # Le dernier point du tronçon devient le point de départ du suivant
            if len(current_chunk) > 0:
                current_chunk = [current_chunk[-1]]

    # S'il reste des points de déplacement après la dernière action (fin de match)
    if len(current_chunk) >= 2:
        print(f"\n--- Calcul du Tronçon Final {segment_counter} ({len(current_chunk)} waypoints) ---")
        path, speeds, bindata = generate_full_path(current_chunk, show_plot=SHOW_PLOT_BY_DEF, segment_index=segment_counter)
        print(f">> Généré {len(path)} points interpolés. Vitesse finale = {speeds[-1]:.2f} m/s")


if __name__ == "__main__":
    # Test avec un fichier factice
    test_json = [
        {"type": "MOVE", "x": 0.0, "y": 0.0, "theta": 0},
        {"type": "MOVE", "x": 1.0, "y": 0.5, "theta": 0},
        {"type": "MOVE", "x": 2.0, "y": 0.0, "theta": 0},
        {"type": "ACTION", "cmd": "DEPLOY_BRAS", "param": 100},
        {"type": "MOVE", "x": 2.5, "y": 1.0, "theta": 90},
        {"type": "MOVE", "x": 3.0, "y": 1.5, "theta": 90}
    ]
    
    with open("strategy.json", "w") as f:
        json.dump(test_json, f)
        
    # --- TEST DE LA LOGIQUE ---
    # Tu peux changer "YELLOW" par "BLUE" pour voir la différence dans les calculs
    process_strategy_file("strategy.json", team_color="YELLOW")