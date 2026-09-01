# WrathClassicAPI

Adds a set of convenience Lua functions to the 3.3.5a client, available to any
addon with no companion addon to install. The functions are registered directly
into the in-game Lua state as global functions and `C_*` namespaces.

Highlights:

- **`C_EncodingUtil`** — hex and Base64 encode/decode, compression, and JSON and
  CBOR serialization of Lua values.
- **Spell, unit, item, quest, talent and tooltip helpers** — small predicates and
  info accessors addons commonly reach for.
- **`C_Timer`-style scheduling** and per-frame tick utilities.

It installs through WarcraftXL's hook registry, so it runs alongside other
extensions that detour the same engine functions.
