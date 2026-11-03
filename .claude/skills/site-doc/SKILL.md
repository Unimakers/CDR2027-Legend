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
| `index.md` Accueil | 1 | modèle (« projet XY », poster et vidéo d'exemple) ; lien Onshape réel |
| `premiers-pas/` | 2 | guides du modèle (Jekyll, Markdown, Git) : à garder |
| `objectifs.md` | 3 | lorem ipsum |
| `etudes.md` Études et choix techniques | 4 | vide |
| `conception.md` Conception et prototypage | 5 | vide |
| `etapes/` Étapes de fabrication | 6 | exemple générique |
| `journaux/` | 7 | rédigé (skill `journaux`) |

Le `README.md` racine est aussi celui du modèle.

## Rédiger une page

- En-tête : `layout: default`, `title`, `nav_order`, et `parent` / `has_children` pour une sous-page.
- La doc décrit l'état **final** et les choix ; le cheminement va dans les journaux. Pour remplir `etudes.md` ou `conception.md`, partir des décisions du skill `etat-projet` et citer les logs.
- Médias dans `docs/images/`, moins de 50 Mo pour une vidéo.
- Encadrés Just the Docs : `{: .warning }` / `{: .note }` avant un bloc `>`.
