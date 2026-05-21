#pragma once

#include <juce_core/juce_core.h>

#include <optional>

namespace rsat::settings
{
    std::optional<int> loadDefaultThemeIndex();
    void saveDefaultThemeIndex(int themeIndex);
}
