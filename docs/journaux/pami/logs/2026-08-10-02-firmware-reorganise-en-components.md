---
title: "Firmware réorganisé en components"
date: 2026-08-10
sujet: Firmware
description: "La première version sans classes multipliait les fichiers. Le code passe en « components » et la configuration de la carte est regroupée dans pamiboard.h."
sources: ["a1b194a93166"]
---

## Architecture du code

**Constat** : la première version était écrite sans classes, en séparant bien les fichiers.

**Problème** : il y avait trop de fichiers, et l'ensemble était trop complexe à maintenir et à faire évoluer.

**Décision** : le code a été réorganisé en « components ».

**Résultat** : le développement est nettement plus simple.

## Configuration de la carte

La configuration matérielle (GPIO, etc.) est regroupée dans le fichier `pamiboard.h`.
