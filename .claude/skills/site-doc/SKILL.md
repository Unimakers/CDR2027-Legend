---
name: site-doc
description: Site de documentation Jekyll du projet (docs/, thème Just the Docs, GitHub Pages) — structure des pages, navigation, pages encore à rédiger, build local et CI. À charger pour créer ou modifier une page de documentation (hors journaux).
---

# Site de documentation

- Jekyll + thème `just-the-docs`, config `docs/_config.yml`. Publication sur GitHub Pages à chaque push sur `main` qui touche `docs/**` (`.github/workflows/pages.yml`). La CI (`ci.yml`) construit le site sur chaque PR.
- Build local : `cd docs && bundle install && bundle exec jekyll serve`.
- Les dossiers commençant par `_` ne sont pas publiés (`_sync/`, `_includes/`...).

## Pages

| Page | nav_order | État |
|:--|:--|:--|
| `index.md` Accueil | 1 | rédigé le 2026-11-19 (présentation, avancement, plan) ; lien Onshape et iframe réels |
| `objectifs.md` | 2 | rédigé (défi, règlement, approche) |
| `robot-principal.md` Le robot principal | 3 | rédigé (carte, encodeur, levage des murs) |
| `pami.md` Les PAMI | 4 | rédigé (carte, firmware, mécanique, points ouverts) |
| `journaux/` | 5 | rédigé (skill `journaux`) |

Le 2026-11-19, le site a été aligné sur celui du projet RATP 2025 (https://unimakers.fr/CDR-2025-I3-RATP/, même thème just-the-docs) : lien « Unimakers sur GitHub » sous le menu (`aux_links`) et pied de page dans `_config.yml`. Les pages du modèle (`premiers-pas/`, `etapes/`, `etudes.md`, `conception.md`, poster et vidéo d'exemple) ont été supprimées. Le `README.md` racine a été réécrit le 2026-11-19 : présentation courte, liens site et Onshape, carte du dépôt, avancement, commandes de démarrage. Il renvoie vers le site pour le détail, sans le recopier.

## Rédiger une page

- En-tête : `layout: default`, `title`, `nav_order`, et `parent` / `has_children` pour une sous-page.
- La doc décrit l'état **final** et les choix ; le cheminement va dans les journaux. Pour remplir `etudes.md` ou `conception.md`, partir des décisions du skill `etat-projet` et citer les logs.
- Médias dans `docs/images/`, moins de 50 Mo pour une vidéo.
- Encadrés Just the Docs : `{: .warning }` / `{: .note }` avant un bloc `>`.
