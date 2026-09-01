# wxl-wrathclassicapi

A [WarcraftXL](https://warcraftxl.github.io/) extension that adds the WrathClassicAPI Lua
functions to the World of Warcraft 3.3.5a (build 12340) client. Addons call the
new globals and `C_*` namespaces directly — no companion addon required.

This repository is a thin front-end. The API implementations, client offsets and
Lua glue live in the shared [WrathClassicAPI](https://github.com/brues-code/WrathClassicAPI) core, which also
ships as a standalone LichLoader-injected DLL. This extension compiles that same
core in and installs its hooks through WarcraftXL's chained hook registry, so it
coexists with other extensions that hook the same engine functions.

## Building

The client is 32-bit, so the build is Win32 only. Two build-time dependencies are
resolved as siblings by default (neither is linked at runtime):

```
C:\Git\
├── wxl-core            (WarcraftXL — provides include/wxl/PluginApi.h)
├── WrathClassicAPI     (the shared core + its build recipe)
└── wxl-wrathclassicapi (this repository)
```

```powershell
# Configure and build (Release, Win32). Override the sibling paths with
# -DWXL_CORE_DIR=<path> and -DWRATHCLASSICAPI_DIR=<path> if they live elsewhere.
cmake -B build -A Win32
cmake --build build --config Release
```

The built DLL is `build/Release/wxl-wrathclassicapi.dll`. Pass
`-DCLIENT_PATH="C:\WoW\WoW 3.3.5"` at configure time to deploy it to
`<client>\Extensions\wxl-wrathclassicapi\wxl-wrathclassicapi.dll` after each build,
where WarcraftXL discovers it.

## Scope

Every in-game API from the core is available. Login-screen-only registrations
(developer-console commands) are **not** wired in this front-end: WarcraftXL loads
extensions earlier than the login screen, and no glue-ready signal is hooked yet.
Everything an addon calls after entering the world is unaffected.

## License

GPL-3.0-or-later. The shared WrathClassicAPI core is licensed separately (LGPL-3.0)
in its own repository.
