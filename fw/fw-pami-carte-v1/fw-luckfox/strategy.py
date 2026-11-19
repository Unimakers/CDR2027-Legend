import os
import select

import trajectory

def gerer_message(name,data):
    if name == "TRAJ":
        data = trajectory.generate_full_path(data, show_plot=False)
    elif name == "LOG":
        pass



gerer_message("TRAJ", [
        (0.0, 0.0), (3.0, 0.0), (6.0, 0.0), (9.0, 0.0), # Ligne droite (P1 à P4)
        (11.0, 1.0), (12.0, 3.0), (11.0, 5.0)             # Virage large (P5 à P7)
    ])