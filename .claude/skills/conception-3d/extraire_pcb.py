"""Cotes mécaniques d'une carte KiCad, sans ouvrir le .kicad_pcb en entier.

Usage : python extraire_pcb.py [chemin.kicad_pcb]   (défaut : ecad/pcb-pami/pcb-pami.kicad_pcb)
Affiche : contour (Edge.Cuts), épaisseur, nombre de couches cuivre, trous de fixation
et connecteurs, en mm, origine = coin haut-gauche du contour, Y vers le bas (repère KiCad).
"""
import re
import sys
from pathlib import Path

RACINE = Path(__file__).resolve().parents[3]
chemin = Path(sys.argv[1]) if len(sys.argv) > 1 else RACINE / "ecad/pcb-pami/pcb-pami.kicad_pcb"
s = chemin.read_text(encoding="utf-8")

xs, ys = [], []
for m in re.finditer(r'\((?:gr_line|gr_rect|gr_arc|gr_circle)\b(.*?)\(layer "Edge\.Cuts"\)', s, re.S):
    for x, y in re.findall(r'\((?:start|end|mid|center) ([-\d.]+) ([-\d.]+)\)', m.group(1)):
        xs.append(float(x))
        ys.append(float(y))
if not xs:
    sys.exit("Pas de contour Edge.Cuts trouvé.")
ox, oy = min(xs), min(ys)
L, H = max(xs) - ox, max(ys) - oy

epaisseur = re.search(r'\(general\s*\(thickness ([\d.]+)', s)
couches = len(re.findall(r'\(\d+ "[^"]*\.Cu" signal', s))
print(f"Carte : {chemin.name}")
print(f"Contour : {L:.1f} x {H:.1f} mm, épaisseur {epaisseur.group(1) if epaisseur else '?'} mm, {couches} couches cuivre")

motif = r'\(footprint "([^"]+)"\s.*?\(at ([-\d.]+) ([-\d.]+)(?: ([-\d.]+))?\).*?\(property "Reference" "([^"]+)"'
lignes = []
for lib, x, y, rot, ref in re.findall(motif, s, re.S):
    if not re.search(r"Hole|Mount|Conn|Header|Socket|JST|XT30|USB|TMC|ESP|Terminal", lib, re.I):
        continue
    px, py = float(x) - ox, float(y) - oy
    hors = "" if 0 <= px <= L and 0 <= py <= H else "  (hors contour : non placé)"
    lignes.append((0 if "Hole" in lib else 1, ref, lib.split(":")[-1], px, py, rot or "0", hors))
for _, ref, lib, px, py, rot, hors in sorted(lignes):
    print(f"{ref:7} {lib[:45]:45} x={px:6.1f} y={py:6.1f} rot={rot}{hors}")
