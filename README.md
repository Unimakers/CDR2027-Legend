# CDR 2027 - Legend

Dépôt de l'équipe **Unimakers** (MakerSpace Amiens) pour la **Coupe de France de Robotique 2027**, thème « The Legend of Camelot ».

Nous préparons un **robot principal** et des **PAMI** (petits robots autonomes qui partent de l'écurie en fin de match).

- **Documentation : [unimakers.fr/CDR2027-Legend](https://unimakers.fr/CDR2027-Legend/)** (objectifs, robots, journaux de bord)
- **CAO : [projet Onshape](https://cad.onshape.com/documents/a1dd7e986cf35e5710cc0af1/v/c28c25944dc0cfd69f9db5c7/e/192be7f926e66b5b2fba6ecb)**

## Contenu du dépôt

| Dossier | Contenu |
|:--|:--|
| [`fw/`](fw/) | Firmware des PAMI : ESP32-S3 (PlatformIO, Arduino) et coprocesseur Luckfox (Python) |
| [`ecad/`](ecad/) | Cartes électroniques KiCad : PAMI V1 (en service), PAMI V2 (en conception), robot principal |
| [`mcad/`](mcad/) | Exports de pièces (STL) ; la CAO elle-même est sur Onshape |
| [`docs/`](docs/) | Site de documentation (Jekyll, thème Just the Docs), publié sur GitHub Pages |

## Avancement

- **PAMI** : carte V1 câblée et firmware fonctionnel (odométrie, stratégies en JSON, interface web, mise à jour sans fil). Carte V2 routée, pas encore fabriquée.
- **Robot principal** : carte électronique en cours sous KiCad, module encodeur testé, prototype de l'actionneur de levage des murs. Pas encore de code.

Le détail, semaine par semaine, est dans les [journaux de bord](https://unimakers.fr/CDR2027-Legend/journaux/).

## Démarrer

```sh
# Firmware PAMI (carte V1)
cd fw/fw-pami-carte-v1
pio run

# Site de documentation en local
cd docs
bundle install
bundle exec jekyll serve
```

## Contribuer

Les commits suivent le format `[TAG] Message`, avec les tags `[FW]`, `[DOC]`, `[ECAD]` et `[GLOBAL]`.
