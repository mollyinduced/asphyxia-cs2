#pragma once
#include <fstream>
#include "../utilities/log.h"
#include <json.hpp>

#define CFG_LOAD(var) loadcfg(#var, var, cfg)
#define CFG_SAVE(var) cfg[#var] = var
#define CFG_SET_DEFAULT(var) var = decltype(var){}

using json = nlohmann::json;

template <typename T>
void loadcfg(const std::string& key, T& var, const json& cfg) {
    if (cfg.contains(key)) {
        var = cfg.at(key).get<T>();
    }
}

namespace configsystem
{
	void writeconfig(const std::string& filePath);
	void readconfig(const std::string& filePath);
}

namespace Config {
    inline bool exampleBool1 = true;
    inline std::string exampleString1 = "example string";
    inline float exampleFloat1 = 3.14f;
    inline int exampleInt1 = 42;
    // Add more configuration variables as needed
}