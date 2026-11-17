# CDR2027-Legend

Dépôt de l'équipe **Unimakers** (MakerSpace Amiens) pour la **Coupe de France de Robotique 2027** : un robot principal et des **PAMI** (petits robots autonomes). Dépôt : `Unimakers/CDR2027-Legend`, branche `main`. Tout se fait **en français**.

## Carte du dépôt

| Dossier | Contenu | Skill |
|:--|:--|:--|
| `fw/` | Firmware PAMI ESP32-S3 (PlatformIO, Arduino) + `fw-luckfox/` (Python, Linux) | `firmware-pami` |
| `ecad/pcb-pami/` | Carte PAMI sous KiCad | `electronique-pami` |
| `ecad/robot principal/` | Carte du robot principal sous KiCad (`head V1`) | `electronique-robot-principal` |
| `mcad/` | Exports seulement (STL de l'actionneur du robot principal) : la CAO est sur Onshape (lien dans `docs/index.md`) | `conception-3d` |
| `docs/` | Site Jekyll « Just the Docs », publié sur GitHub Pages | `site-doc` |
| `docs/journaux/` | Journaux de bord (PAMI, robot principal) | `journaux` |
| (hors dépôt) | Règlement CDR/Eurobot 2027 « Legend of Camelot » | `reglement-2027` |

Le robot principal n'a encore **pas de code** ; son schéma KiCad est commencé et son journal couvre le module encodeur, la carte et l'actionneur de levage des murs. Le firmware et le reste concernent les PAMI.

## Comment se contextualiser (économie de tokens)

1. Ce fichier suffit pour s'orienter. **Ne pas explorer le dépôt à l'aveugle.**
2. Charger **uniquement** le skill de `.claude/skills/` qui correspond à la demande. Pour « où en est-on / qu'est-ce qui marche ? », c'est `etat-projet`.
3. Lire un fichier source seulement si le skill ne répond pas, et seulement la partie utile.
4. Ne jamais lire : `fw/.pio/`, `ecad/**/*-backups/`, `ecad/**/Footprint/`, `ecad/**/3D/`, `docs/images/`, `docs/journaux/*/medias/`, `*.kicad_pcb` en entier (fichiers énormes).
5. La source de vérité reste le code et les journaux. Si un skill contredit le code, le code a raison : corriger le skill.

## Règles

- Commits au format `[TAG] Message` : `[FW]`, `[DOC]`, `[ECAD]`, `[GLOBAL]` (vus dans l'historique). Pas de commit ni de push sans demande explicite.
- Journaux : ne rien inventer, ne pas réécrire un log publié, médias < 2 Mo. Procédure : skill `journaux`.
- Ne pas exposer les identifiants Wi-Fi/SSH du dépôt hors du dépôt.

## Faire vivre le contexte

Après une modification qui change ce que décrit un skill (nouveau component, pin, protocole, décision, problème résolu), **mettre à jour le skill concerné dans le même travail** :

- `etat-projet` : ce qui marche, ce qui ne marche pas, décisions, points ouverts. Dater chaque changement (`AAAA-MM-JJ`).
- Les autres skills : faits stables (architecture, API, procédures).
- Rester court : un skill dépasse rarement 150 lignes. Pointer vers le fichier source plutôt que recopier du code.
- Nouveau domaine (robot principal, CAO, stratégie de match...) : créer `.claude/skills/<nom>/SKILL.md` et l'ajouter au tableau ci-dessus.

Commandes : `/verifier-journaux` (contrôle de forme des logs), `/synchro-journaux` (routine hebdo, lance l'agent `journaux`), `/avant-commit` (lance l'agent `relecteur`).

## Agents (économie de tokens)

Les agents de `.claude/agents/` travaillent dans leur propre contexte et ne renvoient qu'un rapport court. **Déléguer quand la tâche lit beaucoup de fichiers et renvoie peu ; faire en direct pour une petite tâche** (un agent repart de zéro). Les décisions de conception et d'architecture restent dans la conversation principale.

| Tâche | Agent | Modèle | Accès |
|:--|:--|:--|:--|
| Trouver un pin, un composant, une constante, un mot (`fichier:ligne`) | `chercheur` | haiku | lecture |
| Renommer / déplacer des fichiers, corriger les liens | `fichiers` | haiku | `git mv`, édition |
| Question de conformité au règlement 2027 | `reglement` | haiku | lecture |
| Mettre à jour un skill après un changement (avec date) | `skills-maj` | haiku | édition |
| Ajouter, corriger, synchroniser des logs | `journaux` | sonnet | édition, `journal.py` |
| Analyser ou comparer des fichiers KiCad sans les lire en entier | `kicad` | sonnet | lecture |
| Modifier `fw/` et vérifier que ça compile (`pio run`) | `firmware` | sonnet | édition, `pio run` |
| Relire les modifications avant un commit | `relecteur` | sonnet | lecture |

- Un agent **informe, il ne décide pas** et ne commit jamais : tout ce qu'il modifie est relu avant commit (`/avant-commit`).
- `firmware` ne flashe jamais (ni upload, ni env OTA).
- **Cohérence carte / firmware** : la carte est en cours de conception, des liaisons non terminées sont normales, et le firmware suit la carte avec retard (voir `etat-projet`). Si le firmware s'appuie sur un composant ou un signal absent de la carte et que `etat-projet` ne le documente pas comme décision, l'agent `kicad` le signale comme problème ouvert, sans corriger.
- Permissions et dossiers interdits : `.claude/settings.json`.
