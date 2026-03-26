#pragma once

// used: [stl] vector
#include <vector>

#include "../common.h"

// used: [ext] imgui, draw, animation
#include "../utilities/draw.h"

#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

#define MENU_MAX_BACKGROUND_PARTICLES 100

class CTab
{
public:
	const char* szName;
	void (*pRenderFunction)();
};

namespace MENU
{
	inline const char* HitboxNames[] = {
	"Head", "Neck", "Pelvis", "Spine 0", "Spine 1", "Spine 2", "Spine 3",
	"Upper Leg L", "Upper Leg R", "Lower Leg L", "Lower Leg R",
	"Ankle L", "Ankle R", "Hand L", "Hand R",
	"Upper Arm L", "Lower Arm L", "Upper Arm R", "Lower Arm R"
	};

	inline unsigned int selectedHitboxes = 0;

	void RenderMainWindow();
	void RenderOverlayPreviewWindow();

	void UpdateStyle(ImGuiStyle* pStyle = nullptr);

	inline bool bMainWindowOpened = false;
	inline bool bMainActive = false;
	inline int nCurrentMainTab = 0;
	inline int iCurrTab = 0;
	inline AnimationHandler_t animMenuDimBackground;
	inline float flDpiScale = 1.f;
	inline std::vector<std::string> configFiles;
	inline static int currentConfigIndex = -1;

	inline void LoadConfigFiles()
	{
		configFiles.clear();
		std::string configDirectory = "C:\\ohook\\configs\\";
		for (const auto& entry : fs::directory_iterator(configDirectory))
			if (entry.is_regular_file() && entry.path().extension() == ".json")
				configFiles.push_back(entry.path().filename().string());
		if (configFiles.empty())
			currentConfigIndex = -1;
	}
}

namespace T
{
	/* @section: main */
	void Render(const char* szTabBar, const CTab* arrTabs, const unsigned long long nTabsCount, int* nCurrentTab, ImGuiTabBarFlags flags = ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip);

	/* @section: tabs */
	void RageBot();
	void AntiAim();
	void LegitBot();
	void Visuals();
	void Miscellaneous();
	void SkinsChanger();

	/* @section: values */
	// user-defined configuration filename in miscellaneous tab
	inline char szConfigFile[256U] = {};
	// current selected configuration in miscellaneous tab
	inline unsigned long long nSelectedConfig = ~1U;
	// current sub tab overlay in visuals tab
	inline int nCurrentOverlaySubtab = 0;
}
