# GUI Layout JSON Schema (Nova3D)

Root:
- `version` (number)
- `widgets` (array)

Widget fields:
- `type`: registered factory type (e.g. `window`, `panel`, `button`, `meshviewer`)
- `id`: unique id
- `parent`: parent id (`""` for root)
- `text`: widget caption/text
- `rect`: `{ "x": n, "y": n, "w": n, "h": n }`
- `visible`: bool
- `enabled`: bool
- `styleClass`: style token/class
- `onClick`: action id string

Round-trip:
- Load with `NGUILayoutLoader::loadLayoutJson(...)`
- Save with `NGUILayoutSerializer::saveLayoutJson(...)`
