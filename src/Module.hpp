// Module identity, and the core service table every part of it reaches the host through.
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version. See <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>

#include "wxl/PluginApi.h"

// The single version input: the WrathClassicAPI release this extension ships.
// The build sets it from -DWRATHCLASSICAPI_TAG=vX.Y.Z via the core's
// cmake/WrathClassicAPICore.cmake (as major*10000 + minor*100 + patch). The
// fallback keeps a plain dev build (no tag) compiling.
#ifndef WRATHCLASSICAPI_VERSION_VALUE
#define WRATHCLASSICAPI_VERSION_VALUE 1
#endif

namespace wxlwca
{
    // Name the core logs this module under, and the folder it is deployed into
    // (Extensions/<name>/<name>.dll). Must match the target name in CMake and
    // the entry/id in wxl.json.
    inline constexpr const char* kModuleName = "wxl-wrathclassicapi";

    // The version reported to WarcraftXL (WXL_PluginInfo.pluginVersion, shown in
    // log lines). Tracks the WrathClassicAPI core version this extension ships,
    // so this number, the compiled WRATH_CLASSIC_API_VERSION global, and
    // wxl.json's version all move together from the one release tag.
    inline constexpr uint32_t kModuleVersion = WRATHCLASSICAPI_VERSION_VALUE;

    // Records the service table for the process lifetime. Called once, first
    // thing in WXL_Load; everything below is undefined before it.
    void SetApi(const WXL_Api* api);

    // The service table, or null before SetApi.
    const WXL_Api* Api();

    // Writes one line to the core's log under this module's name. The formatting
    // is done here because the ABI's Log is itself variadic with no va_list entry
    // to forward.
    void Log(int level, const char* format, ...);
}
