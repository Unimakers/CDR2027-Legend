---
title: "MPU déplacé au-dessus des moteurs"
date: 2026-09-25
date_estimee: true
sujet: Capteurs
description: "Le MPU passe au-dessus des NEMA. Le champ magnétique des moteurs ne dégrade pas la précision, et un seuil de 1° limite le drift."
sources: ["ab4778c38bd2"]
---
**Décision** : le MPU a été déplacé au-dessus des NEMA.

**Crainte** : que le champ magnétique des moteurs fasse perdre en précision.

**Résultat** : après essai, il n'y a pas de souci. Pour réduire encore le drift, tout mouvement inférieur à 1° est compté comme du drift. La précision est relativement fiable depuis.
