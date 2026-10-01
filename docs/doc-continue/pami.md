---
layout: default
title: PAMI
parent: Documentation continue
nav_order: 2
---

# Documentation continue : PAMI

Journal de conception des PAMI. Les entrées sont regroupées par sous-système et, dans chaque partie, données dans l'ordre où elles sont arrivées.

## Électronique

**Première carte ESP32-S3.** Le PCB reprend la base des cartes des années précédentes. La nouveauté est que la batterie passe au travers du PCB.

**Câblage.** Tous les câbles ont été sertis puis branchés sur le PCB. Leur nombre rend le câble management compliqué. Il faudrait prévoir de quoi orienter les câbles.

**Connecteur NEMA cassé.** Le connecteur d'un des moteurs a cassé. Le câble a été resserti avec une gaine pour que le connecteur tienne mieux dans le temps.

**Nouvelle carte en préparation.** Elle ne garde que le strict nécessaire et passe en 4 couches. L'objectif est de gagner de la place et de faciliter le câble management.

## Firmware

**Architecture du code.** La première version était écrite sans classes, en séparant bien les fichiers. Il y avait trop de fichiers, et l'ensemble était trop complexe à maintenir et à faire évoluer. Le code a été réorganisé en « components », ce qui rend le développement nettement plus simple.

**Configuration de la carte.** La configuration matérielle (GPIO, etc.) est regroupée dans le fichier `pamiboard.h`.

## Châssis : première version

**Châssis sans murs.** Un premier châssis a été réalisé sans les murs, uniquement pour positionner les éléments. Les alignements posaient problème, et il a fallu plusieurs versions pour optimiser les placements.

**Support batterie.** Le support a été optimisé pour être moins large et moins haut, avec des barrettes métalliques pour faire contact. Il a ensuite été mis à jour pour recevoir le convertisseur buck d'un côté et le MPU de l'autre.

**Plateforme de fixation de la carte.** Deux des vis de la carte tombaient dans le vide à cause des roues. Une plateforme fixée sur le châssis permet à la carte de se fixer solidement. Elle a ensuite été mise à jour pour intégrer les capteurs ToF.

**Toit.** Le toit ferme le robot et porte l'écran. Il était trop haut au départ et sa hauteur a été réduite de 1 cm, ce qui complique le câble management. À prévoir : ajouter 3 mm dans toutes les directions au support écran pour cacher les bords de l'écran.

**Hauteur du PAMI.** Le PAMI est globalement haut et le haut de la batterie tape contre le PCB. À prévoir : rehausser le support batterie d'environ 1 cm, sans toucher à la hauteur du buck et du MPU.

**Orientation des capteurs ToF.** On touchait les capteurs en serrant les vis. La plateforme a été modifiée pour changer leur orientation et les reculer de 5 mm. Des trous dans les supports ToF permettent cette orientation à 180°. Un système a été ajouté pour bloquer les câbles toujours au même endroit.

**Solidité des supports capteurs.** Malgré cette mise à jour, le support des capteurs n'est pas assez solide et les vis extérieures sont impossibles à visser. La plateforme est à modifier.

## Châssis : refonte complète

**Pourquoi repartir de zéro.** Les problèmes de fixation et de solidité se retrouvaient un peu partout sur la première version. Tout le châssis a été repris depuis le début, en ne gardant que ce qui marche. L'objectif est de finaliser l'ensemble, de réduire le nombre de vis et de simplifier la conception.

**Fixation des moteurs pas à pas.** Les moteurs étaient vissés à des piliers du châssis. Ils sont maintenant encastrés dans une boîte, fermée par une épaisseur d'acrylique qui verrouille le NEMA en position, l'arbre d'entraînement sortant de la boîte. Le montage est bien plus droit et bien plus solide.

**Plateforme.** La plateforme se fixe au-dessus des NEMA 17 et fait le lien entre la carte électronique et le châssis. Son épaisseur permet d'y loger 3 capteurs ToF.

**Support batterie.** Il a été modifié pour être moins long.

**Position du MPU.** Le MPU a été déplacé au-dessus des NEMA. La crainte était que le champ magnétique des moteurs fasse perdre en précision. Après essai, il n'y a pas de souci. Pour réduire encore le drift, tout mouvement inférieur à 1° est compté comme du drift. La précision est relativement fiable depuis.

**Capteurs ToF à 35°.** Les capteurs orientés à 35° semblent frôler un pilier du châssis. La détection est faussée et le système d'évitement ne peut pas être testé en l'état.

## Roues et appui au sol

**Moulage des roues.** Un premier moule a été créé pour mouler les roues, puis mis en attente.

**Moyeu et nouveau moule.** Le moyeu n'attrapait pas assez fermement le pneu, qui se retirait facilement. Le problème de tolérance a été corrigé et le moule a été repris de zéro pour limiter les fuites et obtenir une roue de meilleure qualité. Des chambres ont été ajoutées dans le moyeu pour que le liquide fusionne avec lui.

**Support « bille folle ».** Un support en PLA a été testé : il a été détruit en 5 minutes de fonctionnement, laissant le châssis traîner sur les têtes de vis. Ce n'est finalement pas une mauvaise idée : les têtes de vis frottent très peu comparé au PLA, ne s'usent pas sur le vinyle et sont très simples à mettre en place.

## Alimentation

**Autonomie.** La batterie semble se vider rapidement. Une mesure à l'ampèremètre ne montre pas de consommation excessive. La batterie est peut-être en fin de vie.

**Choix de la batterie.** Il est encore en cours. Le critère de décision sera la différence de courant instantané (consommation du PAMI) et le couple des NEMA 17. Si la différence est négligeable, on pourrait passer à une batterie d'appareil photo en 7 V rechargeable en USB-C, pour ne pas avoir à acheter un chargeur d'origine.

## Catapulte

**Concept.** L'idée est d'équiper chaque PAMI d'une catapulte en son centre. Elle se charge à la main (ressort) et un servomoteur libère l'énergie du ressort. Si le PAMI est dans les douves, un tir à environ 70° montant à plus de 40 cm de haut devrait atteindre l'objectif. Si possible, le système devra permettre d'ajuster mécaniquement la force de la catapulte, pour le cas où le PAMI serait bloqué avant.

## Points ouverts

| Sujet | État |
|:--|:--|
| Câble management | Trop de câbles. Prévoir un guidage, la nouvelle carte doit aussi aider. |
| Nouvelle carte 4 couches | En préparation. |
| Supports capteurs ToF | Pas assez solides sur la première version, vis extérieures inaccessibles. |
| Capteurs ToF à 35° | Frôlent un pilier du châssis, bloque les tests d'évitement. |
| Rehausse du support batterie (environ 1 cm) | À faire. |
| Support écran élargi de 3 mm | À faire. |
| Autonomie de la batterie | Pas de consommation excessive mesurée, batterie en fin de vie à confirmer. |
| Choix de la batterie | En cours, selon courant instantané et couple des NEMA 17. |
| Appui au sol | Têtes de vis à la place de la bille folle : idée à valider. |
| Catapulte | Au stade de concept. |
