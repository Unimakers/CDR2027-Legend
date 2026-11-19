---
layout: default
nav_order: 3
title: Le robot principal
---

# Le robot principal

Le robot principal n'a pas encore de code. Ce qui existe aujourd'hui : une carte électronique en cours de routage et deux éléments mécaniques développés en prototype. Le détail est dans le [journal du robot principal](journaux/robot-principal/index.md).

## Carte électronique

Le schéma KiCad a été commencé le 7 novembre 2026. Il compte huit feuilles : puissance, drivers, actionneurs, IHM, capteurs, Luckfox et USB. Le routage du PCB a commencé le 16 novembre 2026 : la logique, les moteurs, les alimentations et le bus Dynamixel sont routés, il reste la partie actionneurs. La carte fait environ 170 mm, avec un trou central.

## Module encodeur

Le module encodeur (capteur AMT 102-V, roue codeuse, rails) a été testé le 22 octobre 2026. Une version 2, corrigée, a été développée, sans test noté. Le roulement n'est pas démontable à la main et aucune correction n'est notée pour l'instant.

## Actionneur de levage des murs

Un prototype est monté : un servo FS5115M et un réducteur entraînent une crémaillère, le tout sur un rail igus fixé à un MakerBeam. Il coulisse le long du rail. Un servo SG90 de rotation des murs est positionné, mais l'aspiration et la rotation ne sont pas encore conçues. Les fichiers STL sont dans le dépôt, dossier `mcad/robot-principal/actionneur/`.
