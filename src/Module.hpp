// Module identity, and the core service table every part of it reaches the host through.
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version. See <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>

#include "wxl/PluginApi.h"

namespace wxlwca
{
    // Name the core logs this module under, and the folder it is deployed into
    // (Extensions/<name>/<name>.dll). Must match the target name in CMake and
    // the entry/id in wxl.json.
    inline constexpr const char* kModuleName = "wxl-wrathclassicapi";

    // This module's own version, opaque to the core and only ever shown in a log
    // line. Encoded as major * 10000 + minor * 100 + patch so releases stay
    // distinguishable and ordered.
    inline constexpr uint32_t kModuleVersion = 10000;

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
