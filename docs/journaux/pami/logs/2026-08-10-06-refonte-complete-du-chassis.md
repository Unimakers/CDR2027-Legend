---
title: "Refonte complète du châssis, moteurs encastrés"
date: 2026-08-10
sujet: Châssis
description: "Le châssis repart de zéro : moteurs encastrés dans une boîte, plateforme au-dessus des NEMA 17, MPU déplacé. Les capteurs ToF à 35° frôlent un pilier."
sources: ["85ec12de94f3", "9128d147b4c3", "8e5bc746c431", "3d54dbb8ace0", "ab4778c38bd2", "5800b7694f12"]
---

## Pourquoi repartir de zéro

**Problème** : les problèmes de fixation et de solidité se retrouvaient un peu partout sur la première version.

**Décision** : tout le châssis a été repris depuis le début, en ne gardant que ce qui marche. L'objectif est de finaliser l'ensemble, de réduire le nombre de vis et de simplifier la conception.

## Fixation des moteurs pas à pas

**Avant** : les moteurs étaient vissés à des piliers du châssis.

**Décision** : ils sont maintenant encastrés dans une boîte, fermée par une épaisseur d'acrylique qui verrouille le NEMA en position, l'arbre d'entraînement sortant de la boîte.

**Résultat** : le montage est bien plus droit et bien plus solide.

## Plateforme

La plateforme se fixe au-dessus des NEMA 17 et fait le lien entre la carte électronique et le châssis. Son épaisseur permet d'y loger 3 capteurs ToF.

## Support batterie

Il a été modifié pour être moins long.

## Position du MPU

**Décision** : le MPU a été déplacé au-dessus des NEMA.

**Crainte** : que le champ magnétique des moteurs fasse perdre en précision.

**Résultat** : après essai, il n'y a pas de souci. Pour réduire encore le drift, tout mouvement inférieur à 1° est compté comme du drift. La précision est relativement fiable depuis.

## Capteurs ToF à 35°

**Problème** : les capteurs orientés à 35° semblent frôler un pilier du châssis. La détection est faussée et le système d'évitement ne peut pas être testé en l'état.
