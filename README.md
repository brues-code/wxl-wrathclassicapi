# wxl-wrathclassicapi

A [WarcraftXL](https://warcraftxl.github.io/) extension that adds the WrathClassicAPI Lua
functions to the World of Warcraft 3.3.5a (build 12340) client. Addons call the
new globals and `C_*` namespaces directly — no companion addon required. The
per-function reference is in the core's
[docs/API.md](https://github.com/brues-code/WrathClassicAPI/blob/main/docs/API.md).

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

The full core is available — every in-game API plus the login-screen
developer-console commands. WXL_Load runs before the login screen, but the core
arms a `CGlueMgr::Initialize` hook that fires the login-screen registrations once
glue is ready, so the console commands register here just as they do under the
LichLoader front-end.

## License

GPL-3.0-or-later, same as the shared WrathClassicAPI core (licensed in its own
repository).
