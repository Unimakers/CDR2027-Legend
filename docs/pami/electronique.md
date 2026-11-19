---
layout: default
title: Électronique
parent: Les PAMI
nav_order: 1
---

# Électronique des PAMI

## Architecture

*À compléter : schéma d'ensemble (ESP32-S3, Luckfox, moteurs, capteurs).*

## Carte V1 (en service)

- ESP32-S3, PCB 2 couches, 95 × 95 mm.
- Deux moteurs pas à pas NEMA 17 pilotés par des TMC2209.
- Servomoteurs, LED NeoPixel, capteurs de distance ToF.
- Coprocesseur Luckfox Pico Mini B pour les calculs lourds (recherche de chemin).
- Projet KiCad : [`ecad/pcb-pami-v1`](https://github.com/Unimakers/CDR2027-Legend/tree/main/ecad/pcb-pami-v1)

## Carte V2 (en conception)

- 60 × 50 mm, 2 couches.
- Un seul connecteur ToF, à l'avant ; module radio retiré.
- Projet KiCad : [`ecad/pcb-pami-v2`](https://github.com/Unimakers/CDR2027-Legend/tree/main/ecad/pcb-pami-v2)

## Choix des composants

*À compléter.*
