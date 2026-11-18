---
title: "Routage des moteurs du robot principal et trois LED NeoPixel"
date: 2026-11-18
sujet: Électronique
description: "Après la logique, les quatre modules moteurs, les alimentations et la masse sont routés. L'IHM passe à trois LED NeoPixel pour transmettre plus d'informations."
vignette: 2026-11-18-electronique-74e2f8ac5b2f.jpg
sources: ["d398a5dc21f1", "74e2f8ac5b2f", "e6be0f3d2429"]
---
**Résultat** : après la logique, le routage avance sur la partie puissance :

- les quatre modules moteurs, avec leurs deux empreintes de driver (TMC2209 et EZ2209), leurs connecteurs moteurs et les signaux d'activation EN 1 et EN 2 ;
- les alimentations 24 V (avant et après le relais d'arrêt d'urgence) et 5 V ;
- les plans de masse.

Les Dynamixel sont déjà routés, mais leur routage risque encore de bouger.

{% include media.html fichier="2026-11-18-electronique-74e2f8ac5b2f.jpg" legende="Vue d'ensemble du PCB : les modules moteurs de part et d'autre du trou central, la logique en bas à gauche" %}

{% include media.html fichier="2026-11-18-electronique-e6be0f3d2429.jpg" legende="Les quatre modules moteurs routés, deux de chaque côté du trou central" %}

**Nouveauté** : deux LED WS2812B (D4, D5) sont ajoutées sur la feuille IHM, chaînées derrière la première (D3). Il restait de la place sur la carte, et trois LED NeoPixel permettent de transmettre plus d'informations par témoins lumineux.

**À prévoir** : router toute la partie actionneurs (pompes, électrovannes et servomoteurs), à l'exception des Dynamixel.
