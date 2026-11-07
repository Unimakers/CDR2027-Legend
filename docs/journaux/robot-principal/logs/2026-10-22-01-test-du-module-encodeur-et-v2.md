---
title: "Test du module encodeur : problèmes relevés et développement de la v2"
date: 2026-10-22
sujet: Mécanique
description: "Le test du module encodeur révèle cinq problèmes, dont un rail trop permissif et un axe trop fragile. Une v2 corrigée est développée."
vignette: 2026-10-22-mecanique-197ff54d45a1.jpg
sources: ["4d19b9801c9e", "197ff54d45a1", "8df73d4fc7b3"]
---
{% include media.html fichier="2026-10-22-mecanique-197ff54d45a1.jpg" legende="Le module encodeur imprimé : supports noirs, roue codeuse, axe de sortie et chariots de rail aux deux extrémités" %}

**Problème** : le test du module a mis en évidence plusieurs défauts :

- le rail est trop permissif ;
- il y a trop de tolérance, ce qui crée un frottement entre la roue et le support ;
- l'axe entre la roue et l'encodeur est trop fragile ;
- les ressorts sont mal positionnés ;
- le roulement ne peut pas être sorti manuellement.

**Décision** : les corrections retenues sont :

- passer sur un rail prismatique ;
- réduire les tolérances de la roue ;
- augmenter le diamètre de l'axe ;
- déplacer le ressort près de l'encodeur.

**Résultat** : une v2 corrigée a été développée.

{% include media.html fichier="2026-10-22-mecanique-8df73d4fc7b3.jpg" legende="CAO de la v2 du module encodeur" %}
