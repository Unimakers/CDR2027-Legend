---
layout: default
title: Firmware
parent: Les PAMI
nav_order: 2
---

# Firmware des PAMI

Firmware de la carte V1 : ESP32-S3, PlatformIO (Arduino). Il sera adapté à la carte V2.

## Architecture

Code organisé en composants, un dossier par fonction : [`fw/fw-pami-carte-v1/lib`](https://github.com/Unimakers/CDR2027-Legend/tree/main/fw/fw-pami-carte-v1/lib).

*À compléter : tâches et échanges entre composants.*

## Fonctions

- **Odométrie** : pas des moteurs et cap du MPU9250.
- **Stratégies** : fichiers JSON stockés dans le robot, lancés depuis l'interface web.
- **Interface web** : télémétrie en direct, carte de la table.
- **Mise à jour sans fil** (OTA).
- **Liaison série** avec le Luckfox.

## Évitement d'obstacles

*À compléter.*

## Coprocesseur Luckfox

*À compléter : rôle, code Python ([`fw/fw-luckfox`](https://github.com/Unimakers/CDR2027-Legend/tree/main/fw/fw-luckfox)).*
