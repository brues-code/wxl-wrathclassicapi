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

A WarcraftXL extension is a 32-bit DLL, so the build is **Win32 only** — Visual
Studio with the C++ desktop workload, and CMake ≥ 3.20.

This repository is a thin front-end. It compiles in the shared
[WrathClassicAPI](https://github.com/brues-code/WrathClassicAPI) core and builds
against the [WarcraftXL SDK](https://github.com/WarcraftXL/wxl-core) headers, so
both are separate checkouts. The build resolves them as siblings of this repo by
default:

```
C:\Git\
├── wxl-core            (WarcraftXL SDK — provides include/wxl/PluginApi.h)
├── WrathClassicAPI     (the shared core + its build recipe and vendored libs)
└── wxl-wrathclassicapi (this repository)
```

Clone all three into a common parent, then configure and build:

```powershell
git clone https://github.com/WarcraftXL/wxl-core
git clone --recursive https://github.com/brues-code/WrathClassicAPI
git clone https://github.com/brues-code/wxl-wrathclassicapi
cd wxl-wrathclassicapi

cmake -B build -A Win32
cmake --build build --config Release
```

`--recursive` matters: the core vendors picojson and tinycbor as submodules and
the build compiles them. If you already cloned WrathClassicAPI without it, run
`git submodule update --init --recursive` in that checkout. `wxl-core` is
header-only — a plain clone is enough.

If the dependencies live elsewhere, point the build at them with
`-DWXL_CORE_DIR=<path>` and `-DWRATHCLASSICAPI_DIR=<path>` at configure time.

The built DLL is `build/Release/wxl-wrathclassicapi.dll`. Install it by copying it
to `<client>\Extensions\wxl-wrathclassicapi\wxl-wrathclassicapi.dll`, or pass
`-DCLIENT_PATH="C:\WoW\WoW 3.3.5"` at configure time to deploy it there after
every build — that's where WarcraftXL discovers it.

## Scope

The full core is available — every in-game API plus the login-screen
developer-console commands. WXL_Load runs before the login screen, but the core
arms a `CGlueMgr::Initialize` hook that fires the login-screen registrations once
glue is ready, so the console commands register here just as they do under the
LichLoader front-end.

## License

GPL-3.0-or-later, same as the shared WrathClassicAPI core (licensed in its own
repository).
