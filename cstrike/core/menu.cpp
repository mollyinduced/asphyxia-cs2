#define IMGUI_DEFINE_MATH_OPERATORS

#include "menu.h"

// used: config variables
#include "variables.h"
#include "fataFont.h"

// used: iinputsystem
#include "interfaces.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetworkclientservice.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/ienginecvar.h"

// used: overlay's context
#include "../features/visuals/overlay.h"

// used: notifications
#include "../utilities/notify.h"
#include "../features/rage/ragebot.h"
#include "../configsystem/configsystem.h"
#include "../globals.h"

#pragma region menu_array_entries
static constexpr const char* arrMiscDpiScale[] = {
	"100%",
	"125%",
	"150%",
	"175%",
	"200%"
};

static constexpr const char* arrMenuAddition[] = {
	"dim",
	"particle",
	"glow"
};

static constexpr const char* arrMovementStrafer[] = {
	"Adjust mouse",
	"Directional"
};

static constexpr const char* arrStopIf[] = {
	"Attacking",
	"Using"
};

static constexpr const char* arrAutobuyPrimary[] = {
	"None",
	"Scout",
	"AWP",
	"Auto"
};

static constexpr const char* arrAutobuySecondary[] = {
	"None",
	"Deagle",
	"Fiveseven/Tec9"
};

static constexpr const char* arrAutobuyMisc[] = {
	"None",
	"Armor"
};

#pragma endregion

void RenderTabs(int currTab)
{
	if (currTab == 0)
	{
		T::RageBot;
	}
	else if (currTab == 1)
	{
		T::AntiAim;
	}
	else if (currTab == 2)
	{
		T::Visuals;
	}
	else if (currTab == 3)
	{
		T::Miscellaneous;
	}
}

void MENU::RenderMainWindow()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuiStyle& style = ImGui::GetStyle();

	// @test: we should always update the animation?
	//animMenuDimBackground.Update(io.DeltaTime, style.AnimationSpeed);
	if (!bMainWindowOpened)
		return;

	const ImVec2 vecScreenSize = io.DisplaySize;
	const float flBackgroundAlpha = animMenuDimBackground.GetValue(1.f);
	flDpiScale = D::CalculateDPI(g_Config::nDpiScale);

	// @note: we call this every frame because we utilizing rainbow color as well! however it's not really performance friendly?
	UpdateStyle(&style);

	//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0);

	ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(600 * flDpiScale, 470 * flDpiScale), ImGuiCond_Always);

	// handle main window get out of screen bound
	// @note: we call this here so it will override the previous SetNextWindowPos
	//if (ImGuiWindow* pMainWindow = ImGui::FindWindowByName(CS_XOR("asphyxia")); pMainWindow != nullptr)
	//{
	//	bool bRequireClamp = false;
	//	ImVec2 vecWindowPos = pMainWindow->Pos;
	//	if (pMainWindow->Pos.x < 0.0f)
	//	{
	//		bRequireClamp = true;
	//		vecWindowPos.x = 0.0f;
	//	}
	//	else if (pMainWindow->Size.x + pMainWindow->Pos.x > vecScreenSize.x)
	//	{
	//		bRequireClamp = true;
	//		vecWindowPos.x = vecScreenSize.x - pMainWindow->Size.x;
	//	}
	//	if (pMainWindow->Pos.y < 0.0f)
	//	{
	//		bRequireClamp = true;
	//		vecWindowPos.y = 0.0f;
	//	}
	//	else if (pMainWindow->Size.y + pMainWindow->Pos.y > vecScreenSize.y)
	//	{
	//		bRequireClamp = true;
	//		vecWindowPos.y = vecScreenSize.y - pMainWindow->Size.y;
	//	}

	//	if (bRequireClamp) // Necessary to prevent window from constantly undocking itself if docked.
	//		ImGui::SetNextWindowPos(vecWindowPos, ImGuiCond_Always);
	//}

	// render main window
	ImGui::Begin(CS_XOR("gamerware"), &bMainWindowOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
	{
		const ImVec2 vecMenuPos = ImGui::GetWindowPos();
		const ImVec2 vecMenuSize = ImGui::GetWindowSize();
		ImDrawList* pDrawList = ImGui::GetWindowDrawList();

		float rectHeight = 3.5f;
		ImVec2 rectMin = vecMenuPos; 
		ImVec2 rectMax = ImVec2(vecMenuPos.x + vecMenuSize.x, vecMenuPos.y + rectHeight); 

		pDrawList->AddRectFilled(rectMin, rectMax, IM_COL32(236, 174, 255, 255));

		/*const ImVec2 vecTitleSize = FONT::pMenu[g_Config::nDpiScale]->CalcTextSizeA(12.f * flDpiScale, FLT_MAX, -1.f, CS_XOR("ohook"));
		pDrawList->AddText(ImVec2(vecMenuPos.x + vecMenuSize.x - style.WindowPadding.x - vecTitleSize.x, vecMenuPos.y + style.WindowPadding.y), ImGui::GetColorU32(ImGuiCol_Text), CS_XOR("ohook"));*/

		static const CTab arrTabs[] = {
			{ "Ragebot", &T::RageBot },
			{ "Anti Aim", &T::AntiAim },
			{ "Visuals", &T::Visuals },
			{ "Misc", &T::Miscellaneous },
			//{ "skins changer", &T::SkinsChanger }
		};

		T::Render(CS_XOR("##main_tabs"), arrTabs, CS_ARRAYSIZE(arrTabs), &nCurrentMainTab);

		//RenderTabs(iCurrTab);
	}
	ImGui::End();

	//ImGui::PopStyleVar();
}

void MENU::RenderOverlayPreviewWindow()
{
	//using namespace F::VISUALS::OVERLAY;

	//ImGuiStyle& style = ImGui::GetStyle();
	//// @note: call this function inside rendermainwindow, else expect a crash...
	//const ImVec2 vecMenuPos = ImGui::GetWindowPos();
	//const ImVec2 vecMenuSize = ImGui::GetWindowSize();

	//const ImVec2 vecOverlayPadding = ImVec2(30.f * flDpiScale, 50.f * flDpiScale);

	//ImGui::SetNextWindowPos(ImVec2(vecMenuPos.x + vecMenuSize.x + style.WindowPadding.x, vecMenuPos.y), ImGuiCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(175 * flDpiScale, vecMenuSize.y), ImGuiCond_Always);
	//ImGui::Begin(CS_XOR("preview"), &bMainWindowOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	//{
	//	const ImVec2 vecWindowPos = ImGui::GetWindowPos();
	//	const ImVec2 vecWindowSize = ImGui::GetWindowSize();

	//	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	//	Context_t context;

	//	ImVec4 vecBox = {
	//		vecWindowPos.x + vecOverlayPadding.x,
	//		vecWindowPos.y + vecOverlayPadding.y,
	//		vecWindowPos.x + vecWindowSize.x - vecOverlayPadding.x,
	//		vecWindowPos.y + vecWindowSize.y - vecOverlayPadding.y
	//	};

	//	if (const auto& boxOverlayConfig = m_Config::overlayBox; boxOverlayConfig.bEnable)
	//	{
	//		const bool bHovered = context.AddBoxComponent(pDrawList, vecBox, 1, boxOverlayConfig.flThickness, boxOverlayConfig.flRounding, boxOverlayConfig.colPrimary, boxOverlayConfig.colOutline);
	//		if (bHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	//			ImGui::OpenPopup(CS_XOR("context##box.component"));
	//	}

	//	if (const auto& nameOverlayConfig = m_Config::overlayName; nameOverlayConfig.bEnable)
	//		context.AddComponent(new CTextComponent(true, SIDE_TOP, DIR_TOP, FONT::pVisual, CS_XOR("asphyxia"), m_Config::overlayName));

	//	if (const auto& healthOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayHealthBar); healthOverlayConfig.bEnable)
	//	{
	//		const float flFactor = M_SIN(ImGui::GetTime() * 5.f) * 0.55f + 0.45f;
	//		context.AddComponent(new CBarComponent(true, SIDE_LEFT, vecBox, flFactor, Vars.overlayHealthBar));
	//	}

	//	if (const auto& armorOverlayConfig = C_GET(BarOverlayVar_t, Vars.overlayArmorBar); armorOverlayConfig.bEnable)
	//	{
	//		const float flArmorFactor = M_SIN(ImGui::GetTime() * 5.f) * 0.55f + 0.45f;
	//		context.AddComponent(new CBarComponent(false, SIDE_BOTTOM, vecBox, flArmorFactor, Vars.overlayArmorBar));
	//	}

	//	// only render context preview if overlay is enabled
	//	context.Render(pDrawList, vecBox);

	//	if (ImGui::BeginPopup(CS_XOR("context##box.component"), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	//	{
	//		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

	//		ImGui::ColorEdit4(CS_XOR("primary color##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).colPrimary);
	//		ImGui::ColorEdit4(CS_XOR("outline color##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).colOutline);
	//		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
	//		ImGui::SliderFloat(CS_XOR("thickness##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).flThickness, 1.f, 5.f, CS_XOR("%.1f"), ImGuiSliderFlags_AlwaysClamp);
	//		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.75f);
	//		ImGui::SliderFloat(CS_XOR("rounding##box.component"), &C_GET(FrameOverlayVar_t, Vars.overlayBox).flRounding, 1.f, 5.f, CS_XOR("%.1f"), ImGuiSliderFlags_AlwaysClamp);

	//		ImGui::PopStyleVar();

	//		ImGui::EndPopup();
	//	}
	//}
	//ImGui::End();
}

void MENU::UpdateStyle(ImGuiStyle* pStyle)
{
	ImGuiStyle& style = pStyle != nullptr ? *pStyle : ImGui::GetStyle();

	
}

#pragma region menu_tabs

void T::Render(const char* szTabBar, const CTab* arrTabs, const unsigned long long nTabsCount, int* nCurrentTab, ImGuiTabBarFlags flags)
{
	ImVec2 buttonAreaSize(120, 0); // Width of 120, height fills available space
	const ImVec4 activeColor = ImColor(236, 174, 255); // Darker color for active button
	const ImVec4 inactiveColor = ImColor(89, 89, 89); // Lighter color for inactive buttons

	// Start child window for buttons
	ImGui::BeginChild("ButtonArea", buttonAreaSize, true);

	for (std::size_t i = 0U; i < nTabsCount; i++)
	{
		bool isActive = (*nCurrentTab == static_cast<int>(i));
		ImVec4 buttonColor = isActive ? activeColor : inactiveColor;
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, activeColor);
		// Change color if this is the active tab for visual feedback
		if (isActive)
		{
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Button, ImColor(236, 174, 255).Value); // Active color
		}

		// Create the button for each tab
		if (ImGui::Button(arrTabs[i].szName, ImVec2(100, 30))) // Width: 100, Height: 30
		{
			// Set the current tab index when the button is clicked
			*nCurrentTab = static_cast<int>(i);
		}

		ImGui::PopStyleColor(2);

		// Add some vertical spacing between buttons
		ImGui::Spacing();
	}

	ImGui::EndChild(); // End the button area child window

	ImGui::SameLine(); // Move to the right for content

	// Start child window for content
	ImGui::BeginChild("ContentArea", ImVec2(0, 0), false); // Fill the remaining space

	// Render the content of the currently active tab
	if (arrTabs[*nCurrentTab].pRenderFunction != nullptr)
	{
		arrTabs[*nCurrentTab].pRenderFunction();
	}

	ImGui::EndChild(); // End the group for the content
}

void T::RageBot()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, CS_XOR("##ragebot_collumns"), false);
	{
		static float flOverlayChildSize = 0.f;
		ImGui::BeginChild(CS_XOR("ragebot.main"), ImVec2(ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("Main");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("Enable"), &g_Config::bRagebot);
			ImGui::Checkbox(CS_XOR("Hide Shots"), &g_Config::bHideShots);
			ImGui::SliderInt(CS_XOR("Min Damage"), &g_Config::iMinDamage, 0, 250);
			ImGui::SliderInt(CS_XOR("Min Damage Override"), &g_Config::iMinDamageOverride, 0, 250);
			ImGui::HotKey(CS_XOR("Min Damage Override Hotkey"), &g_Config::nMinDmgOverride);

			ImGui::SliderInt(CS_XOR("Backtrack Ticks"), &g_Config::iBacktrackTicks, 1, 20);

			ImGui::PopStyleVar();

			flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
		}
		ImGui::EndChild();

		ImGui::BeginChild(CS_XOR("ragebot.scan"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("Scan");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::MultiCombo(CS_XOR("Hitboxes"), &MENU::selectedHitboxes, MENU::HitboxNames, HITBOX_COUNT);
			ImGui::SliderInt(CS_XOR("Multipoints"), &g_Config::iMultipoints, 1, 5);
			//ImGui::SliderInt(CS_XOR("Pointscale"), &g_Config::flPointscale, 0.1f, 20.f);
			ImGui::SliderInt(CS_XOR("Hit Chance"), &g_Config::iHitchance, 0, 100);

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		static float flOverlayChildSize = 0.f;
		ImGui::BeginChild(CS_XOR("ragebot.other"), ImVec2(ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("Other");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("Auto Shoot"), &g_Config::bAutoshoot);
			ImGui::Checkbox(CS_XOR("Auto Stop"), &g_Config::bAutostop);
			ImGui::Checkbox(CS_XOR("Auto Scope"), &g_Config::bAutoscope);

			ImGui::PopStyleVar();

			flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
}

void T::AntiAim()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::BeginChild(CS_XOR("antiaim.main"), ImVec2(0.f, -1), true, ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::TextUnformatted("Antiaim");
			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

		ImGui::Checkbox(CS_XOR("Enable"), &g_Config::bAntiAim);
		if (g_Config::bAntiAim)
		{
			const char* PitchTypes[4]{ CS_XOR("Off"), CS_XOR("Down"), CS_XOR("Up"), CS_XOR("Zero") };
			ImGui::Combo(CS_XOR("Pitch"), &g_Config::iPitchType, PitchTypes, IM_ARRAYSIZE(PitchTypes), 4);

			const char* BaseYawTypes[2]{ CS_XOR("Off"), CS_XOR("Backwards") };
			ImGui::Combo(CS_XOR("Base Yaw"), &g_Config::iBaseYawType, BaseYawTypes, IM_ARRAYSIZE(BaseYawTypes), 2);

			ImGui::Checkbox(CS_XOR("Jitter"), &g_Config::bJitter);
			if (g_Config::bJitter)
				ImGui::SliderInt(CS_XOR("Jitter Amount"), &g_Config::iJitterAmount, 0, 180);

			ImGui::HotKey(CS_XOR("Override Left"), &g_Config::nAALeftKey);
			ImGui::HotKey(CS_XOR("Override Right"), &g_Config::nAARightKey);
		}

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
}

void T::Visuals()
{
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, CS_XOR("##visuals_collumns"), false);
	{
		static float flOverlayChildSize = 0.f;
		ImGui::BeginChild(CS_XOR("visuals.overlay"), ImVec2(0.f, flOverlayChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("Visuals");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			//ImGui::Checkbox(CS_XOR("Enemy ESP"), &g_Config::bVisualOverlay);

			/*ImGui::Checkbox(CS_XOR("Bounding Box"), &g_Config::bBoundingBox);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##colBoundingBox"), &g_Config::colBoundingBox);*/

			ImGui::Checkbox(CS_XOR("Box"), &g_Config::bBox);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##colBox"), &g_Config::colBox);

			ImGui::Checkbox(CS_XOR("Head Dot"), &g_Config::bHeadDotEsp);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##headColor"), &g_Config::colHeadDot);

			ImGui::Checkbox(CS_XOR("Skeleton"), &g_Config::bSkeletonEsp);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##skeletonColor"), &g_Config::colSkeletonEsp);

			ImGui::Checkbox(CS_XOR("History Skeleton"), &g_Config::bDrawHistorySkeletonEsp);

			ImGui::Checkbox(CS_XOR("Health Bar"), &g_Config::bHealthbarEsp);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##healthbarColor"), &g_Config::colHealthbarEsp);

			ImGui::Checkbox(CS_XOR("Name"), &g_Config::bNameEsp);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##nameColor"), &g_Config::colPlayerName);

			ImGui::Checkbox(CS_XOR("Weapon"), &g_Config::bWeaponEsp);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##weaponColor"), &g_Config::colWeaponEsp);

			//ImGui::Checkbox(CS_XOR("name"), &C_GET(TextOverlayVar_t, Vars.overlayName).bEnable);
			//ImGui::Checkbox(CS_XOR("Health Bar"), &m_Config::overlayHealthBar.bEnable);
			//ImGui::Checkbox(CS_XOR("armor bar"), &C_GET(BarOverlayVar_t, Vars.overlayArmorBar).bEnable);
			/*ImGui::Checkbox(CS_XOR("Enemy Glow"), &m_Config::espGlow);
			if (m_Config::espGlow)
				ImGui::ColorEdit4(CS_XOR("Enemy Glow Color"), &m_Config::espGlowColor);

			ImGui::Checkbox(CS_XOR("Self Glow"), &m_Config::selfGlow);
			if (m_Config::espGlow)
				ImGui::ColorEdit4(CS_XOR("Self Glow Color"), &m_Config::selfGlowColor);*/

			ImGui::Checkbox(CS_XOR("Remove Scope"), &g_Config::bRemoveScope);
			if (g_Config::bRemoveScope)
			{
				ImGui::Checkbox(CS_XOR("Draw Static Scope"), &g_Config::bDrawStaticCrosshair);
				if (g_Config::bDrawStaticCrosshair)
				{
					ImGui::ColorEdit4(CS_XOR("Scope Color"), &g_Config::cScope);
					ImGui::SliderInt(CS_XOR("Scope Length"), &g_Config::iScopeLength, 0, 250);
					ImGui::SliderInt(CS_XOR("Scope Thickness"), &g_Config::iScopeThickness, 0, 50);
					ImGui::SliderInt(CS_XOR("Scope Gap"), &g_Config::iScopeGap, 0, 50);
				}
			}
				

			ImGui::PopStyleVar();

			flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
		}
		ImGui::EndChild();

		ImGui::BeginChild(CS_XOR("visuals.chams"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("Chams");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("Enemy Chams"), &g_Config::bVisualChams);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##visible"), &g_Config::colVisualChams);

			ImGui::Checkbox(CS_XOR("Chams Ignore Z"), &g_Config::bVisualChamsIgnoreZ);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##invisible"), &g_Config::colVisualChamsIgnoreZ);

			ImGui::Combo(CS_XOR("Material"), &g_Config::nVisualChamMaterial, CS_XOR("Latex\0Bloom\0Glow\0"));

			ImGui::Separator();
			
			ImGui::Checkbox(CS_XOR("Weapon Chams"), &g_Config::bWeaponChams);
			ImGui::Checkbox(CS_XOR("Self Chams"), &g_Config::bVisualChamsSelf);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##self_visible"), &g_Config::colVisualChamsSelf);

			ImGui::Checkbox(CS_XOR("Self Chams Ignore Z"), &g_Config::bVisualChamsSelfIgnoreZ);
			ImGui::SameLine(160);
			ImGui::ColorEdit4(CS_XOR("##self_invisible"), &g_Config::colVisualChamsSelfIgnoreZ);

			ImGui::Combo(CS_XOR("Self Material"), &g_Config::nVisualSelfChamMaterial, CS_XOR("Latex\0Bloom\0Glow\0"));

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		static float flOverlayChildSize = 0.f;
		ImGui::BeginChild(CS_XOR("visuals.extras"), ImVec2(0.f, flOverlayChildSize), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted("Extras");
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("Thirdperson"), &g_Config::bThirdPerson);
			if (g_Config::bThirdPerson)
			{
				ImGui::SliderFloat(CS_XOR("Thirdperson distance"), &g_Config::flThirdperson, 0.f, 150.f);
			}

			ImGui::Checkbox(CS_XOR("Color Modulation"), &g_Config::bNightmode);
			if (g_Config::bNightmode)
			{
				ImGui::ColorEdit4(CS_XOR("Skybox Color"), &g_Config::colNightmode);
				ImGui::ColorEdit4(CS_XOR("World Color"), &g_Config::colWorldColor);
			}

			ImGui::Checkbox(CS_XOR("Light Modulation"), &g_Config::bModulateLight);
			if (g_Config::bModulateLight)
			{
				ImGui::ColorEdit4(CS_XOR("Light Color"), &g_Config::colLightColor);
				ImGui::SliderInt(CS_XOR("Intensity"), &g_Config::iLightIntensity, 0.f, 100.f);
			}

			ImGui::Checkbox(CS_XOR("Aspect Ratio"), &g_Config::bAspectRatio);
			if (g_Config::bAspectRatio)
			{
				ImGui::SliderFloat(CS_XOR("Aspect Ratio Amount"), &g_Config::flAspectRatio, 0.f, 10.f);
				ImGui::SliderInt(CS_XOR("Aspect Ratio Flag"), &g_Config::iArFlags, 0, 20);
			}

			ImGui::Checkbox(CS_XOR("Remove Smoke"), &g_Config::bRemoveSmoke);

			ImGui::SliderFloat(CS_XOR("Viewmodel FOV"), &g_Config::flSetViewModelFOV, 0.f, 150.f);
			ImGui::SliderFloat(CS_XOR("World FOV"), &g_Config::flSetWorldFov, 0.f, 150.f);

			ImGui::PopStyleVar();

			flOverlayChildSize = ImGui::GetCursorPosY() + style.ItemSpacing.y;
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
}

void T::Miscellaneous()
{
	if (MENU::configFiles.empty())
	{
		MENU::LoadConfigFiles();
	}

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Columns(2, CS_XOR("##misc_collumns"), false);
	{
		/*ImGui::BeginChild(CS_XOR("misc.general"), ImVec2(0, ImGui::GetContentRegionAvail().y / 2.f), true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("general"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
			if (ImGui::Button(CS_XOR("unlock hidden cvars"), ImVec2(-1, 15 * MENU::flDpiScale)))
			{
				I::Cvar->UnlockHiddenCVars();
				NOTIFY::Push({ N_TYPE_INFO, CS_XOR("unlocked all hidden cvars") });
			}

			ImGui::PopStyleColor();

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();*/

		ImGui::BeginChild(CS_XOR("misc.movement"), ImVec2{}, true, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("Movement"));
				ImGui::EndMenuBar();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, 0));

			ImGui::Checkbox(CS_XOR("Edge Jump"), &g_Config::bEdgeJump);
			ImGui::Checkbox(CS_XOR("Auto Jump"), &g_Config::bAutoBHop);
			if (g_Config::bAutoBHop)
			{
				ImGui::Checkbox("Jump Bug", &g_Config::bJumpbug);
				//ImGui::SliderInt(CS_XOR("Chance"), &m_Config::nAutoBHopChance), 0, 100, CS_XOR("%d%%"));
				//ImGui::SliderFloat(CS_XOR("Smoothness"), &g_Config::autostrafe_smooth, 0, 100);
			}
				

			ImGui::Checkbox(CS_XOR("Auto Strafe"), &g_Config::bAutoStrafe);
			ImGui::Checkbox(CS_XOR("Sub-tick Mode"), &g_Config::bSubtickAutostrafeExploit);


			ImGui::Checkbox(CS_XOR("Autobuy"), &g_Config::bAutobuy);
			if (g_Config::bAutobuy)
			{
				ImGui::Combo(CS_XOR("Primary"), &g_Config::iAutobuyPrimary, arrAutobuyPrimary, IM_ARRAYSIZE(arrAutobuyPrimary));
				ImGui::Combo(CS_XOR("Secondary"), &g_Config::iAutobuySecondary, arrAutobuySecondary, IM_ARRAYSIZE(arrAutobuySecondary));
				ImGui::Combo(CS_XOR("Misc"), &g_Config::iAutobuyMisc, arrAutobuyMisc, IM_ARRAYSIZE(arrAutobuyMisc));
			}
			//ImGui::MultiCombo(CS_XOR("Strafe modes"), &C_GET(unsigned int, Vars.bAutostrafeMode), arrMovementStrafer, CS_ARRAYSIZE(arrMovementStrafer));

			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		static int nCurrentSettingSubTab = 0;
		ImGui::BeginChild(CS_XOR("misc.settings"), ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar); // ImVec2(0, ImGui::GetContentRegionAvail().y / 2.f)
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextUnformatted(CS_XOR("Configs"));
				ImGui::EndMenuBar();
			}

			ImGui::Text("Config Name: ");
			ImGui::InputText("##configName", globals::strConfigNameInput, sizeof(globals::strConfigNameInput));

			ImGui::Text("Select a config:");
			/*if (ImGui::ListBox("##configList", &MENU::currentConfigIndex, [](void* data, int idx, const char** out_text) {
				auto& files = *static_cast<std::vector<std::string>*>(data);
				*out_text = files[idx].c_str();
				return true;
				}, static_cast<void*>(&MENU::configFiles), MENU::configFiles.size(), 5))
			{
				if (MENU::currentConfigIndex != -1)
				{
					strncpy(globals::strConfigNameInput, MENU::configFiles[MENU::currentConfigIndex].c_str(), sizeof(globals::strConfigNameInput) - 1);
					globals::strConfigNameInput[sizeof(globals::strConfigNameInput) - 1] = '\0';
				}
			}*/

			ImGui::BeginListBox("##configList", ImVec2(300, 300));
			{
				for (int i = 0; i < MENU::configFiles.size(); i++)
				{
					if (ImGui::Selectable(MENU::configFiles[i].c_str(), MENU::currentConfigIndex == i))
					{
						MENU::currentConfigIndex = i;
						strncpy(globals::strConfigNameInput, MENU::configFiles[MENU::currentConfigIndex].c_str(), sizeof(globals::strConfigNameInput) - 1);
						globals::strConfigNameInput[sizeof(globals::strConfigNameInput) - 1] = '\0';
					}
				}
			}
			ImGui::EndListBox();

			if (ImGui::Button("Save Config", ImVec2(-1, 25)))
			{
				configsystem::writeconfig(globals::strConfigNameInput);
			}
			if (ImGui::Button("Load Config", ImVec2(-1, 25)))
			{
				configsystem::readconfig(globals::strConfigNameInput);
			}
			if (ImGui::Button("Refresh Configs", ImVec2(-1, 25)))
			{
				MENU::LoadConfigFiles();
			}
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
}

void T::SkinsChanger()
{

}

#pragma endregion
