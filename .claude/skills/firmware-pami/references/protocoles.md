# Protocoles d'échange

À lire seulement pour modifier l'interface web, la liaison Luckfox ou le format des stratégies.

## WebSocket (navigateur ↔ ESP32), `fw/lib/WEB_INTERFACE/`

Navigateur → ESP32 (JSON) :
- `{"cmd":"strategy","action":"save|load|rename|delete|list|play|stop","name":"...","newName":"...","steps":[...]}`. Réponses : `strategy_ack` (save), `strategy_data` (load), `strategy_list`.
- Commande composant : `{"cmd":"<composant>","id":<n>,"val":"<valeur>"}`, transmis à `executer_commande_web()` dans `main.cpp` :

| composant | id | valeur |
|:--|:--|:--|
| `servo` | 0 / 1 | angle |
| `dc` | 1 / 2 | vitesse -255..255 |
| `neopixel` | n° d'animation (0 = fixe) | `#RRGGBB` (`#000000` = éteint) |
| `nema` | n° moteur | `MODE\|vitesse\|accél\|cible`, MODE = `POS`, `CONT` ou `STOP` |
| `avoid` | — | `OFF`, `STOP` ou autre (= AVOID) |
| `luckfox` | — | message brut envoyé sur l'UART |

ESP32 → navigateur :
- Télémétrie toutes les 100 ms : `x`, `y` (m), `eL`, `eR`, `aL`, `aR`, `b` (V), `gz`, `ga`, `c1` (charge CPU %), `tof1..tof4`.
- `{"type":"strat_debug", ...}` pendant une stratégie.
- `{"type":"file_list","files":"..."}` (réponse du Luckfox).

## UART ESP32 ↔ Luckfox

UART1 (GPIO 18 RX / 8 TX), 115200 bauds. Messages encadrés `<NOM:données>` terminés par `\n`.

| Requête | Réponse |
|:--|:--|
| `PING` | `<ACK:PING>` |
| `FILE_LIST` | `<FILE_LIST_RESP:...>` |
| `FILE_DEL` / `FILE_REN` | `<FILE_STATUS:SUCCESS\|NOT_FOUND\|ERR_PROTECTED>` |
| `OTA_START` / `OTA_LINE` / `OTA_END` | `<OTA_STATUS:SUCCESS\|ERROR_SYNTAX\|ERROR_PROTECTED>` |

Au démarrage, le Luckfox envoie `<READY>`.

## Étapes de stratégie (JSON)

```json
{"type":"MOVE","x":1000,"y":500,"theta":90}
{"type":"ACTION","cmd":"ATTENTE","param":500}
```

Côté ESP32 : x, y en mm, theta en degrés (0° = +X, sens anti-horaire). `fw/fw-luckfox/strategy.json` utilise des mètres.
