---
name: conception-3d
description: Conception mécanique / CAO 3D des robots CDR2027 (PAMI et robot principal) — où sont les modèles (Onshape), pièces existantes, cotes d'interface (carte, NEMA 17, servo, ToF, batterie, catapulte), contraintes d'impression, méthode pour modifier une pièce. À charger pour concevoir, modifier ou coter une pièce, un support ou un châssis.
---

# Conception 3D

## Où sont les modèles

- **Source de vérité : Onshape** (lien dans `docs/index.md`, bouton « Notre projet sur Onshape »). Claude n'a pas d'accès à Onshape.
- `mcad/` ne contient qu'un `README.md` (« TODO lien Onshape3D »). **Aucun STEP/STL dans le dépôt** : ne pas chercher ailleurs.
- `ecad/pcb-pami/3D/` contient seulement les modèles des composants de la carte (bouton, ESP32, TMC2209, USB-C). Ne pas les lire.

Pour travailler sur une pièce, demander à l'utilisateur, dans cet ordre de préférence :
1. les **cotes** utiles (ou une capture cotée d'Onshape) ;
2. un export **STEP** de la pièce seule, à ranger dans `mcad/<robot>/<piece>/` (STL seulement pour l'impression) ;
3. une photo de la pièce imprimée.

## Ce que Claude peut produire

- Des cotes, jeux, tolérances et un plan de modification à reporter dans Onshape (cas le plus courant).
- Une pièce paramétrique neuve en script (CadQuery ou OpenSCAD) dans `mcad/<robot>/<piece>/`, avec ses paramètres en tête de fichier, exportable en STEP pour être importée dans Onshape.
- Ne jamais présenter une cote comme mesurée si elle vient d'un standard ou d'une estimation : le dire.

## Cotes d'interface connues

### Carte PAMI (`ecad/pcb-pami`)

Relire à jour avec `python .claude/skills/conception-3d/extraire_pcb.py` (sous Windows, prendre le Python qui a Pillow/pip, cf. skill `journaux`).
État au 2026-11-03 du fichier du dépôt :
- **90 × 66 mm**, 1,6 mm, déclarée **4 couches** (vérifier avec l'équipe si c'est bien la nouvelle carte, cf. `electronique-pami`).
- 4 trous **M3** (Ø 3,2) à 3,5 mm des bords : entraxe **83 × 59 mm**.
- Bord gauche (x ≈ 4) : USB-C `J1` (y 13,4), XT30 `J15` (y 26,1). Autres XT30 : `J10` (32,8 ; 36), `J6` (58,5 ; 36).
- TMC2209 `U9` (12,1 ; 48,2) et `U8` (82,1 ; 48,2) : modules debout, prévoir la hauteur et l'air.
- Repère : origine coin haut-gauche, Y vers le bas, face composants vue de dessus.
- Laisser l'accès aux connecteurs et au câblage (câble management = point ouvert).

### Moteurs et actionneurs

- **NEMA 17** (2 par PAMI) : valeurs standard, à confirmer sur le modèle réel : face 42,3 mm, trous M3 entraxe 31 mm, centrage Ø 22 mm, arbre Ø 5 mm. Montage actuel : encastrés dans une boîte du châssis fermée par une plaque d'acrylique (log 2026-09-19).
- **Servo de la catapulte** : micro-servo bleu transparent (photo du 2026-11-03), modèle exact non noté (SG90 probable, à confirmer avant de coter). Broches servo : GPIO 9 et 10.
- Moteurs DC : prévus sur la carte, pas de pièce mécanique connue.

### Capteurs, énergie, appui

- ToF VL53L0X : 3 logés dans l'épaisseur de la plateforme au-dessus des NEMA, latéraux à ±35°. **Le faisceau à 35° frôle un pilier** (log 2026-10-01) : vérifier le dégagement du cône de détection, pas seulement le boîtier.
- MPU : au-dessus des NEMA, sans souci magnétique (log 2026-09-25).
- Batterie : choix non tranché (piste : batterie d'appareil photo 7 V USB-C). Support déjà rehaussé de 1 cm puis raccourci : ne pas figer ses cotes.
- Appui au sol : **têtes de vis**, pas de bille folle (PLA détruit en 5 min).
- Écran : dans le toit ; support à élargir de 3 mm dans toutes les directions (à faire).

### Catapulte

Au centre du PAMI, ressort chargé à la main, libéré par le servo. Tir visé : ~70°, plus de 40 cm de haut. Pièces (photo `docs/journaux/pami/medias/2026-11-03-catapulte-48f3812029d2.jpg`) : tube fendu, servo sur son support, ressort, loquet, projectile avec ergot.
Résultats du 2026-11-03 : portée insuffisante selon le ressort ; **l'encoche de retenue du projectile a cassé** lors d'un déclenchement à la main. Pistes : renforcer (congé, épaisseur), changer l'orientation d'impression pour que l'effort ne soit pas entre les couches, garder un réglage de force (demandé dans le log du 2026-09-29).

## Contraintes du règlement

Dimensions, masse, BAU, batteries, boulet Ø 45 mm, écurie 200 × 300 mm : skill `reglement-2027` (à charger avant de dimensionner un PAMI ou le robot principal).

## Règles de conception (tirées des logs)

- **Accès aux vis** : prévoir le passage du tournevis (vis extérieures impossibles à visser en 2026-09 ; capteurs touchés au serrage).
- Moins de vis, moins de pièces (objectif de la refonte du 2026-09-19).
- Les vis de la carte ne doivent pas tomber au-dessus des roues (log 2026-08-21).
- PLA : pas de pièce de frottement ; pour une pièce qui reprend un effort (loquet, encoche), orienter les couches dans le sens de l'effort.
- Hauteurs : le toit a été abaissé de 1 cm et la batterie touchait la carte : toujours vérifier l'empilement vertical complet.

## Avant de modifier une pièce

1. Charger `etat-projet` (ce qui a déjà été essayé) et, selon l'interface, `electronique-pami` ou `firmware-pami`.
2. Lire uniquement le log du sujet (`docs/journaux/pami/logs/*<sujet>*`), pas tout le journal.
3. Lister les cotes d'interface touchées ; demander celles qui manquent au lieu de les deviner.
4. Après la modification : log dans le journal (skill `journaux`), mise à jour de ce skill si une cote ou une pièce change.

## Robot principal

Module encodeur (AMT 102-V, roue codeuse à joint torique, rail, ressort) : v2 développée le 2026-10-22, détails dans `docs/journaux/robot-principal/logs/`. Lien de la CAO et cotes non notés.

## À compléter

- Matière et réglages d'impression utilisés (imprimante, buse, remplissage).
- Liste des pièces Onshape (nom de l'onglet, version) et lien de chaque export dans `mcad/`.
