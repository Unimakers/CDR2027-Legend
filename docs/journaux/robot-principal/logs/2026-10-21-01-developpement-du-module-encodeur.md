---
title: "Développement du module encodeur pour l'asservissement en position"
date: 2026-10-21
sujet: Mécanique
description: "Premier module encodeur du robot : un encodeur AMT 102-V, une roue codeuse et deux rails igus qui maintiennent le contact avec le sol."
vignette: 2026-10-21-mecanique-13af19fbc9af.jpg
sources: ["36e200c473e8", "13af19fbc9af"]
---
**Objectif** : développer le module encodeur qui sert à l'asservissement en position du robot.

**Constat** : le module comprend :

- un encodeur AMT 102-V ;
- deux rails igus avec leurs chariots, pour le glissement vertical qui garantit un contact permanent entre le sol et la roue codeuse ;
- une roue codeuse avec un joint torique pour assurer l'adhérence au sol, et un axe qui la relie à l'encodeur ;
- deux pièces de support qui assemblent ces éléments et intègrent deux roulements chargés de supporter la roue codeuse.

{% include media.html fichier="2026-10-21-mecanique-13af19fbc9af.jpg" legende="CAO du module encodeur : la roue codeuse et son joint torique entre les deux pièces de support, l'encodeur à l'arrière et les deux rails de chaque côté" %}
