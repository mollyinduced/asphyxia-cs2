#pragma once
#include "../../utilities/memory.h"

class IGameEvent {
public:
    const char* Name() {
        return MEM::CallVFunc<const char*, 1>(this);
    }
};