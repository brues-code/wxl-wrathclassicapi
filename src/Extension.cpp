// The two entry points WarcraftXL resolves out of this DLL, and the WXL-backed
// hook host it installs the shared WrathClassicAPI core through.
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version. See <https://www.gnu.org/licenses/>.

#include "wxl/PluginApi.h"

#include "Game.h"        // the shared WrathClassicAPI core (IHookHost, InstallCoreHooks, ...)
#include "Module.hpp"

// WXL_EXTENSION, which makes PluginApi.h declare WXL_Query and WXL_Load as exports, comes from the
// build (see CMakeLists.txt) rather than from here, so the whole target inherits it.

namespace
{
    // Bridges the core's loader-agnostic Game::IHookHost onto WarcraftXL's chained
    // hook registry. Each Install becomes one HookAttach at default priority; WXL
    // arms the detour immediately and chains it alongside any other extension
    // hooking the same address, so there is no batch/commit step (unlike the
    // LichLoader front-end's MinHook host, which queues then applies once).
    class WxlHookHost final : public Game::IHookHost
    {
    public:
        explicit WxlHookHost(const WXL_Api* api) : api_(api) {}

        bool Install(uintptr_t target, void* detour, void** original) override
        {
            return api_->HookAttach(wxlwca::kModuleName, target, detour, original,
                                    WXL_HOOK_DEFAULT_PRIORITY) != 0;
        }

    private:
        const WXL_Api* api_;
    };

    const WXL_PluginInfo g_info = {
        sizeof(WXL_PluginInfo),
        WXL_API_VERSION,
        wxlwca::kModuleName,
        wxlwca::kModuleVersion,
        WXL_CLIENT_BUILD,
    };
}

const WXL_PluginInfo* __cdecl WXL_Query(void)
{
    return &g_info;
}

int __cdecl WXL_Load(const WXL_Api* api)
{
    if (!api || api->apiVersion != WXL_API_VERSION) return 0;
    wxlwca::SetApi(api);

    WxlHookHost host(api);

    // The same install the LichLoader front-end runs, through WXL's hook registry
    // instead of MinHook:
    //   * InstallCoreHooks arms the in-game bootstrap (fires module registration
    //     once the in-game Lua state is ready), the reload-teardown hook, and the
    //     glue (CGlueMgr::Initialize) hook that fires login-screen registrations
    //     — so developer-console commands register here too — and performs the
    //     closure-pointer gate write;
    //   * RunHookRegistrations arms every feature module's own hook.
    // WXL_Load runs early in engine init — before the file reader and the world —
    // so all of these detours are armed before the engine reaches them (including
    // the glue hook, which then fires at the first login screen). The gate write
    // is re-asserted from the in-game bootstrap detour (see the core's
    // Bootstrap.cpp) so it stays correct despite this early-arming.
    bool ok = Game::InstallCoreHooks(host);
    ok = Game::RunHookRegistrations(host) && ok;

    if (ok)
        wxlwca::Log(WXL_LOG_INFO, "loaded");
    else
        wxlwca::Log(WXL_LOG_WARN, "loaded with errors; some hooks failed to attach");

    // Loaded either way: a hook that failed disables only what depended on it, and
    // the failure has already been logged.
    return 1;
}
