#pragma once
#include "../../core/sdk.h"
#include "../../core/config.h"
#include "../../core/variables.h"
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/iengineclient.h"
#include <minhook/minhook.h>

namespace SCOPE
{
	void DrawCrosshair();
	void Hook();
	void Run();
}