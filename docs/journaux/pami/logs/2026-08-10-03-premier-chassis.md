---
title: "Premier châssis : placement des éléments, toit et capteurs ToF"
date: 2026-08-10
sujet: Châssis
description: "Châssis sans murs pour placer les éléments, support batterie, plateforme de fixation de la carte, toit et orientation des capteurs ToF. Plusieurs retouches restent à faire."
sources: ["cae4bac4f7af", "318aa3a9d6bb", "00171152ba0c", "100ea45ca498", "c68d9cd791f7", "8102254dadec", "c72617ab6d6d", "5f003514c796"]
---

## Châssis sans murs

**Constat** : un premier châssis a été réalisé sans les murs, uniquement pour positionner les éléments.

**Problème** : les alignements posaient problème.

**Résultat** : il a fallu plusieurs versions pour optimiser les placements.

## Support batterie

Le support a été optimisé pour être moins large et moins haut, avec des barrettes métalliques pour faire contact. Il a ensuite été mis à jour pour recevoir le convertisseur buck d'un côté et le MPU de l'autre.

## Plateforme de fixation de la carte

**Problème** : deux des vis de la carte tombaient dans le vide à cause des roues.

**Décision** : une plateforme fixée sur le châssis permet à la carte de se fixer solidement. Elle a ensuite été mise à jour pour intégrer les capteurs ToF.

## Toit

Le toit ferme le robot et porte l'écran.

**Problème** : il était trop haut au départ.

**Décision** : sa hauteur a été réduite de 1 cm, ce qui complique le câble management.

**À prévoir** : ajouter 3 mm dans toutes les directions au support écran pour cacher les bords de l'écran.

## Hauteur du PAMI

**Problème** : le PAMI est globalement haut et le haut de la batterie tape contre le PCB.

**À prévoir** : rehausser le support batterie d'environ 1 cm, sans toucher à la hauteur du buck et du MPU.

## Orientation des capteurs ToF

**Problème** : on touchait les capteurs en serrant les vis.

**Décision** : la plateforme a été modifiée pour changer leur orientation et les reculer de 5 mm. Des trous dans les supports ToF permettent cette orientation à 180°. Un système a été ajouté pour bloquer les câbles toujours au même endroit.

## Solidité des supports capteurs

**Problème** : malgré cette mise à jour, le support des capteurs n'est pas assez solide et les vis extérieures sont impossibles à visser.

**À prévoir** : modifier la plateforme.
