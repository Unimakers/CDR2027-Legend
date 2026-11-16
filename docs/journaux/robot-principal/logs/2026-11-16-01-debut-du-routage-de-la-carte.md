---
title: "Début du routage de la carte du robot principal, logique presque finie"
date: 2026-11-16
sujet: Électronique
description: "Le routage du PCB commence. Après une première session, il repart de zéro par la logique, désormais presque finie. La carte mesure environ 170 mm, avec un trou central."
vignette: 2026-11-16-electronique-d9783a351609.jpg
sources: ["9b76e5b35a7e", "285d826929cc", "8a5a408140b4", "8cfadc41876a", "d9783a351609"]
---
## Première session

**Constat** : au début du routage, les composants sont éparpillés et les liaisons s'entremêlent. Le schéma en feuilles hiérarchiques permet de démêler la carte bloc par bloc.

<div class="log-galerie">
{% include media.html fichier="2026-11-16-electronique-285d826929cc.jpg" legende="Début du routage, avant le démêlage" %}
{% include media.html fichier="2026-11-16-electronique-8a5a408140b4.jpg" legende="Après le démêlage : les composants regroupés par bloc" %}
</div>

**Résultat** : les modules driver sont routés, avec les deux empreintes de driver (EZ2209 et TMC2209).

{% include media.html fichier="2026-11-16-electronique-8cfadc41876a.jpg" legende="Routage d'un module driver : connecteur moteur, condensateur 100 µF et les deux empreintes de driver superposées" %}

## Deuxième session

**Décision** : le routage repart de zéro, en commençant par la logique. Cette partie est désormais presque finie.

**Mesure** : pour l'instant, la carte mesure environ 170 mm. Un trou central permet de faire passer les câbles par le cœur du robot.

**Nouveauté** : deux connecteurs de ventilateur, « ventilo 1 » et « ventilo 2 » (J35, J36), sont ajoutés sur la feuille puissance.

**À prévoir** : router les moteurs, puis les actionneurs (pompes, servomoteurs et Dynamixel).

{% include media.html fichier="2026-11-16-electronique-d9783a351609.jpg" legende="Deuxième session : la logique routée autour de l'ESP32, avec le trou central au-dessus" %}
