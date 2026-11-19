---
name: site-doc
description: Site de documentation Jekyll du projet (docs/, thème Just the Docs, GitHub Pages) — structure des pages, navigation, pages encore à rédiger, build local et CI. À charger pour créer ou modifier une page de documentation (hors journaux).
---

# Site de documentation

- Jekyll + thème `just-the-docs`, config `docs/_config.yml`. Publication sur GitHub Pages à chaque push sur `main` qui touche `docs/**` (`.github/workflows/pages.yml`). La CI (`ci.yml`) construit le site sur chaque PR.
- Build local : `cd docs && bundle install && bundle exec jekyll serve`.
- Les dossiers commençant par `_` ne sont pas publiés (`_sync/`, `_includes/`...).

- Site publié : https://unimakers.fr/CDR2027-Legend/ (`url` dans `_config.yml`, `baseurl` fourni par la CI).

## Arborescence (2026-11-19)

Structure posée le 2026-11-19 : des squelettes (titres, puces courtes, `*À compléter.*`), à remplir.

| Section (nav_order) | Dossier | Sous-pages |
|:--|:--|:--|
| Accueil (1) | `index.md` | — ; **seul endroit de l'avancement** (+ README en deux lignes) |
| Le projet (2) | `projet/` | `competition.md` → `reglement.md` (3e niveau, `grand_parent: Le projet`), `equipe.md` |
| Le robot principal (3) | `robot-principal/` | `electronique.md`, `mecanique.md`, `logiciel.md` (pas de code) |
| Les PAMI (4) | `pami/` | `electronique.md` (V1, V2), `firmware.md`, `mecanique.md` |
| Journaux (5) | `journaux/` | skill `journaux` |

Les titres « Le robot principal » / « Les PAMI » sont distincts de « Robot principal » / « PAMI » des journaux : ne pas les renommer, la navigation se fait par `parent` (titre). Idées non retenues pour l'instant : page Ressources (PDF, BOM), mode d'emploi PAMI, stratégie de match.

Le 2026-11-19, le site a été aligné sur celui du projet RATP 2025 (https://unimakers.fr/CDR-2025-I3-RATP/, même thème just-the-docs). Le `README.md` racine renvoie vers le site pour le détail, sans le recopier.

## Rédiger une page

- En-tête : `layout: default`, `title`, `nav_order`, et `parent` / `has_children` pour une sous-page (`grand_parent` au 3e niveau).
- La doc décrit l'état **final** et les choix, sans dates ; l'avancement va sur l'accueil, le cheminement dans les journaux. Partir des décisions du skill `etat-projet` et citer les logs.
- Liens internes en `.md` relatifs (`../pami/index.md`) ; liens vers le code en URL GitHub `tree/main/...`.
- Médias dans `docs/images/`, moins de 50 Mo pour une vidéo.
- Encadrés Just the Docs : `{: .warning }` / `{: .note }` avant un bloc `>`.
