# LibraryDocs — KC868_A16_EnIP

Knowledge extraction package for ImplCache / agent retrieval covering the Kincony KC868-A16 EtherNet/IP adapter firmware (ESP32 + OpENer).

## Quick links

| Doc | Purpose |
|-----|---------|
| [INDEX.md](INDEX.md) | Routing table for all components |
| [project/COMPONENT_INVENTORY.md](project/COMPONENT_INVENTORY.md) | Authoritative component list |
| [project/architecture/system-overview.md](project/architecture/system-overview.md) | Startup and data flows |
| [VALIDATION.md](VALIDATION.md) | Strict validation report |
| [CREATE_LIBRARYDOCS.md](CREATE_LIBRARYDOCS.md) | How this package was produced |

## Layout

```text
LibraryDocs/
├── libraries/     # Reusable drivers and OpENer port
├── project/       # Subsystems, architecture, recipes
├── platform/      # Build, memory, EDS/identity
└── artifacts/     # Interfaces, patterns, data, build excerpts
```
