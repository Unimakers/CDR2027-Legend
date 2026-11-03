---
name: reglement-2027
description: Règlement de la Coupe de France de Robotique / Eurobot 2027 « The Legend of Camelot » — dimensions et masse des robots et PAMI, sécurité (BAU, batteries, lasers), table, éléments de jeu (pierres, boulets, Graal), actions, points, déroulement du match, pénalités. À charger pour toute question de conformité, de stratégie ou de dimensionnement. Évite de rouvrir les PDF.
---

# Règlement 2027 : The Legend of Camelot

Sources : « Règlement général Eurobot », version officielle 1.4, et « Règlement Eurobot et Eurobot Junior 2027 », version officielle 1.0. Résumé fait le 2026-11-03. Les PDF ne sont pas dans le dépôt.
La FAQ (www.eurobot.org/faq/) et les documents complémentaires peuvent modifier ces règles. Si une règle change, mettre à jour ce skill et dater la modification. En cas de doute, écrire à referee@planete-sciences.org.

## Match

- Préparation : **3 min** pour tout installer. Ensuite, plus aucun contact avec les robots, sauf pour appuyer sur le BAU.
- Match : **100 s**. À la fin, tous les actionneurs doivent être arrêtés (les afficheurs peuvent rester allumés).
- Départ : uniquement par un **cordon d'au moins 500 mm** que l'on tire et qui ne reste pas attaché. Aucun autre démarrage manuel n'est autorisé. Le cordon peut équiper le robot, un PAMI, une balise fixe ou le dispositif de calcul.
- Aucune communication avec un système hors de la table pendant le match. Wi-Fi 5 GHz conseillé.
- Batteries : prévoir **3 matchs d'affilée**, attente comprise, et un jeu de rechange chargé.
- Homologation : statique (taille, masse, mécanismes visibles) puis dynamique (en 100 s, le robot **et** les PAMI sortent de leur zone, le robot valide au moins une action, l'évitement est contrôlé). Toute modification après homologation doit être signalée.

## Table et éléments de jeu

- Table **3000 × 2000 mm**, bordures de 70 mm de haut et 22 mm d'épaisseur, vinyle antidérapant. Il peut y avoir des jointures entre les plaques.
- Zones :

  | Zone | Rôle | Dimensions |
  |:--|:--|:--|
  | Salle du trône | départ et arrivée du robot | 50 × 50 cm |
  | Cour du château | entoure la salle du trône | — |
  | Remparts : murs | zone de construction de l'équipe | rectangle 200 × 400 mm |
  | Remparts : tours | zone de construction de l'équipe | cercle Ø 200 mm |
  | Douves | autour des remparts | — |
  | Écurie | départ des PAMI | **200 × 300 mm** |
  | Carrières | réserves de pierres | — |

  Les lignes de couleur font partie de leur zone. Pour une zone de départ, le bord de table accolé aussi.
- **Pierre** : boîte en carton de **320 × 110 × 110 mm**, environ 90 g (RAJA BLFA01). Il y en a 30, empilées par 3 à l'horizontale dans les carrières, avec un tag ArUco 13 sur les 4 grandes faces.
- **Boulet** : balle en mousse compressible de **Ø 45 mm** (balle de magicien). 10 par équipe.
- **Graal** : fabriqué par l'équipe. Il respecte les règles de construction d'un PAMI, mais doit seulement être **plus grand qu'un cube de 60 mm**. Il est posé dans la salle du trône pendant la préparation, sans être tenu par le robot.
- Couleurs : bleu RAL 5017, jaune RAL 1023, gris RAL 7032. Le plan coté de la table est en annexe G.1 du PDF 2027 (ses coordonnées n'ont pas été reprises ici).

## Actions et points

| Action | Points |
|:--|:--|
| **Construction** : pierre posée dans une zone de construction | 1 / pierre |
| Mur : 3 pierres à l'horizontale, empilées, dans une zone mur | +5 |
| Tour : 1 pierre debout, dans une zone tour | +5 |
| Porte : 2 pierres debout + 1 à l'horizontale dessus, dans une zone mur (1 seule par château) | +20 |
| **Graal** présent dans le château | 5 |
| Graal surélevé : niveau 1 = 1 pierre à l'horizontale, 2 = 2 pierres, 3 = 3 pierres à l'horizontale ou 1 debout, 4 = sur le toit du robot (> 30 cm), libre de tout actionneur | +5 / niveau |
| **Retour du roi** : robot principal dans la salle du trône à la fin, partiellement / complètement (PAMI exclus) | 5 / 10 |
| **Assaut** : douve du château adverse occupée par un PAMI | 5 / douve |
| Au moins un PAMI qui touche un PAMI adverse | 5 |
| Au moins 1 boulet dans la cour du château adverse | 5 |
| Au moins 3 boulets dans la cour du château adverse | +15 |

Règles associées :
- Un élément encore tenu par un robot à la fin ne compte pas.
- On peut stocker jusqu'à 3 pierres dans sa cour.
- On peut voler des pierres au château adverse seulement quand toutes les carrières sont vides, et sans le détruire. Sinon, pénalité d'au moins 50 points.
- Les pierres et le Graal peuvent dépasser la hauteur limite dans les zones de construction et dans le château, mais pas les actionneurs.
- Le Graal ne sort jamais de la cour et des remparts. Il est interdit de toucher le Graal adverse.

**Assaut** (PAMI et boulets) :
- La **phase d'attaque** va de **85 s à la fin du match**. Une action faite en dehors ne compte pas.
- **Un PAMI qui sort de l'écurie avant 85 s est retiré de la table.**
- Un PAMI compte « en douve » si une partie de sa projection au sol est dans une douve du château adverse à la fin du match.
- Un PAMI compte « en attaque » si l'un de ses actionneurs touche un PAMI adverse à la fin. Cet actionneur doit être **d'une couleur différente** du reste du PAMI, ne doit pas abîmer l'adversaire, et c'est la seule partie autorisée à le toucher.
- Les boulets sont chargés pendant la préparation, **1 boulet au maximum par PAMI** (le robot principal peut en prendre plus). Un boulet compte si sa projection au sol est dans la cour adverse à la fin. Pas de limite de hauteur pendant le vol. Interdit de dévier les boulets adverses.

## PAMI (règlement général F.7 et règlement 2027)

- **Taille minimale : plus grand qu'un cylindre Ø 80 mm × 150 mm de haut.**
- Périmètre (enveloppe convexe au sol) : **≤ 600 mm** au départ, **≤ 700 mm** une fois déployé.
- Hauteur : **≤ 175 mm** en déploiement.
- Masse : **≤ 1,5 kg**. Zone libre de **30 × 30 mm** pour l'autocollant du numéro de stand.
- Il repose uniquement sur la table au départ, joue quelle que soit la couleur de l'équipe, et n'est commandé par rien d'extérieur à la table.
- Mêmes obligations que le robot : **BAU**, **évitement**, sac ignifuge pour une LiPo, règles laser. Pas de mât ni de support de balise.
- Il doit tenir dans l'écurie de 200 × 300 mm avec les autres PAMI de l'équipe.
- Lancement par cordon de 50 cm, ou par le robot pendant le match. Le PAMI doit se déplacer par lui-même.

## Robot principal (règlement général F.3 et F.6)

- Périmètre ≤ **1200 mm** au départ, ≤ **1400 mm** déployé. Hauteur ≤ **350 mm** à tout instant (le BAU est toléré jusqu'à 375 mm).
- Il doit tenir entièrement dans la salle du trône (50 × 50 cm) à la fin de la préparation.
- **Support de balise** obligatoire :
  - dessus plat à **430 ± 5 mm** du sol, recouvert de Velcro côté crochets ;
  - plein et opaque, section entre un cercle Ø 70 mm et un carré de 100 mm ;
  - dans un cercle de Ø 20 cm autour du centre du robot ;
  - capable de porter 400 g sans fléchir.
- Espace libre de **100 × 70 mm** sur une face verticale pour les autocollants. Il est conseillé de fermer les ouvertures du robot.
- Système d'évitement des robots et des PAMI adverses obligatoire.

## Sécurité

- **BAU** : rouge, Ø ≥ 20 mm, au sommet, actionnable d'un coup de poing. Il peut dépasser la hauteur de 25 mm. Il coupe les actionneurs ; la commande, l'affichage et l'évitement peuvent rester alimentés. Un appui par l'équipe entraîne un forfait.
- Énergie stockée autorisée : batteries, piles CE, **ressorts et élastiques**, air comprimé ≤ 4 bar, gravité. Tension ≤ **48 V**.
- Lithium :
  - chargeur à présenter, sac ignifuge permanent, détection de sous-charge fortement conseillée ;
  - **exemptés** : batteries avec BMS intégré et boîtier rigide (USB, téléphone, portable, outillage), non démontées et utilisées comme prévu par le fabricant, ainsi que les LiFePO4 ;
  - batteries non modifiées uniquement.
- Lasers : classes 1 et 1M sans restriction ; classe 2 seulement si le faisceau reste sur l'aire et s'éteint quand le robot ne touche pas la table ; 2M et plus interdites.
- Son ≤ 80 dBA à 1 m. Fusibles conseillés près des batteries. Aucune pièce saillante ou pointue.
- Tags ArUco 4x4 n° 0 à 50 interdits aux équipes. Utiliser 51 à 70 pour l'équipe bleue, 71 à 90 pour l'équipe jaune.

## Pénalités utiles à la conception

- Perte d'une pièce du robot sur la table : **-20**.
- Évitement non fonctionnel : -30.
- Un robot qui bouge encore après la fin : -50.
- Faux départ : -50.
- Dimensions non respectées : **forfait**.
- Évitement désactivé volontairement, ou robot trop proche de celui d'une autre équipe : disqualification.

## Conséquences pour nos PAMI (à vérifier, 2026-11-03)

- **Catapulte** :
  - le projectile de match est le **boulet en mousse Ø 45 mm**, 1 par PAMI ;
  - un projectile imprimé qui reste sur la table compterait comme une pièce perdue (-20) ;
  - il faut atteindre la **cour** du château adverse, et la douve ne rapporte des points qu'au PAMI qui y est garé ;
  - le tir ne compte qu'**après 85 s** ;
  - le ressort est une source d'énergie autorisée.
- Le PAMI reste dans l'écurie jusqu'à 85 s, puis dispose de 15 s pour atteindre une douve adverse.
- La taille minimale (Ø 80 × 150 mm de haut) et la hauteur maximale déployée (175 mm) encadrent la hauteur du PAMI et de la catapulte armée.
- Le BAU est obligatoire sur chaque PAMI : prévoir sa place sur le toit.
- Batterie : une batterie d'appareil photo USB-C avec BMS semble entrer dans l'exemption lithium. À confirmer, puisqu'il faut un usage « prévu par le fabricant ».
- Un actionneur d'attaque d'une couleur distincte peut rapporter 5 points de plus.
