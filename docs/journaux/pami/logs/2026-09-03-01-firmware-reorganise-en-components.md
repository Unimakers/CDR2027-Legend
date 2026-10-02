---
title: "Firmware réorganisé en components"
date: 2026-09-03
sujet: Firmware
description: "La première version sans classes multipliait les fichiers. Le code passe en « components », un dossier par fonction, et la config de la carte va dans pamiboard.h."
vignette: 2026-09-03-firmware-21ab34209395.jpg
sources: ["a1b194a93166", "21ab34209395", "441a4e3e17d8", "f2fa42c22a86"]
---
## Architecture du code

**Constat** : la première version était écrite sans classes, en séparant bien les fichiers.

**Problème** : il y avait trop de fichiers, et l'ensemble était trop complexe à maintenir et à faire évoluer.

**Décision** : le code a été réorganisé en « components ».

**Résultat** : le développement est nettement plus simple.

{% include media.html fichier="2026-09-03-firmware-21ab34209395.jpg" legende="Le dossier des components : un dossier par fonction" %}

{% include media.html fichier="2026-09-03-firmware-441a4e3e17d8.jpg" legende="Un component = une fonction, découpée en .cpp et .h" %}

## Configuration de la carte

La configuration matérielle (GPIO, etc.) est regroupée dans le fichier `pamiboard.h`.
