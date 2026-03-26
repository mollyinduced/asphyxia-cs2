#pragma once

#include "config.h"

#pragma region variables_combo_entries
using VisualOverlayBox_t = int;

enum EVisualOverlayBox : VisualOverlayBox_t
{
	VISUAL_OVERLAY_BOX_NONE = 0,
	VISUAL_OVERLAY_BOX_FULL,
	VISUAL_OVERLAY_BOX_CORNERS,
	VISUAL_OVERLAY_BOX_MAX
};

using VisualChamMaterial_t = int;
enum EVisualsChamMaterials : VisualChamMaterial_t
{
	VISUAL_MATERIAL_LATEX = 0,
	VISUAL_MATERIAL_BLOOM,
	VISUAL_MATERIAL_GLOW,
	VISUAL_MATERIAL_MAX
};

using MiscDpiScale_t = int;

enum EMiscDpiScale : MiscDpiScale_t
{
	MISC_DPISCALE_DEFAULT = 0,
	MISC_DPISCALE_125,
	MISC_DPISCALE_150,
	MISC_DPISCALE_175,
	MISC_DPISCALE_200,
	MISC_DPISCALE_MAX
};

#pragma endregion

#pragma region variables_multicombo_entries
using MenuAddition_t = unsigned int;
enum EMenuAddition : MenuAddition_t
{
	MENU_ADDITION_NONE = 0U,
	MENU_ADDITION_DIM_BACKGROUND = 1 << 0,
	MENU_ADDITION_BACKGROUND_PARTICLE = 1 << 1,
	MENU_ADDITION_GLOW = 1 << 2,
	MENU_ADDITION_ALL = MENU_ADDITION_DIM_BACKGROUND | MENU_ADDITION_BACKGROUND_PARTICLE | MENU_ADDITION_GLOW
};
#pragma endregion

namespace g_Config
{
#pragma region variables_visuals
    inline bool bVisualOverlay = false;
    inline bool bThirdPerson = false;
    inline float flThirdperson = 90.0f;
    inline float flSetViewModelFOV = 80.f;
    inline float flSetWorldFov = 90.0f;
    inline bool bThirdpersonNoInterp = true;
    inline bool bNameEsp = false;
    inline Color_t colPlayerName = Color_t(255,255,255);
    inline bool bHeadDotEsp = false;
    inline Color_t colHeadDot = Color_t(255, 255, 255);
    inline bool bSkeletonEsp = false;
    inline bool bDrawHistorySkeletonEsp = false;
    inline Color_t colSkeletonEsp = Color_t(255, 255, 255);
    inline bool bBoundingBox = false;    
    inline bool bBox = false;    
    inline Color_t colBoundingBox = Color_t(255, 255, 255);
    inline Color_t colBox = Color_t(255, 255, 255);
    inline bool bWeaponEsp = false;
    inline Color_t colWeaponEsp = Color_t(255, 255, 255);
    inline bool bHealthbarEsp = false;
    inline Color_t colHealthbarEsp = Color_t(255, 255, 255);
    inline bool bOverrideHealthBarColor = false;
    inline bool bNightmode = false;
    inline Color_t colNightmode = Color_t(80, 80, 100);
    inline Color_t colWorldColor = Color_t(80, 80, 100);
    inline bool bVisualChams = false;
    inline bool bVisualChamsIgnoreZ = false;
    inline int nVisualChamMaterial = VISUAL_MATERIAL_LATEX;
    inline Color_t colVisualChams = Color_t(0, 255, 0);
    inline Color_t colVisualChamsIgnoreZ = Color_t(255, 0, 0);
    inline bool bVisualChamsSelf = false;
    inline bool bWeaponChams = false;
    inline bool bVisualChamsSelfIgnoreZ = false;
    inline int nVisualSelfChamMaterial = VISUAL_MATERIAL_LATEX;
    inline Color_t colVisualChamsSelf = Color_t(0, 255, 0);
    inline Color_t colVisualChamsSelfIgnoreZ = Color_t(255, 0, 0);
    inline Color_t selfGlowColor = Color_t(255, 0, 0);
    inline bool selfGlow = false;
    inline Color_t espGlowColor = Color_t(255, 0, 0);
    inline bool espGlow = false;
    inline bool bRemoveScope = false;
    inline bool bDrawStaticCrosshair = false;
    inline int iScopeGap = 30;
    inline int iScopeThickness = 1;
    inline int iScopeLength = 150;
    inline Color_t cScope = Color_t(0, 0, 0);
    
    inline float flAspectRatio = 1.33f;
    inline int iArFlags = 2;
    inline bool bAspectRatio = false;

    inline bool bSubtickAutostrafeExploit = false;

    inline bool bRemoveSmoke = false;

    inline bool bModulateLight = false;
    inline Color_t colLightColor = Color_t(0, 0, 0);
    inline int iLightIntensity = 1;
    inline int iRagebotFov = 30;
#pragma gma endregion
#pragma gma region variables_misc
    inline bool bAntiUntrusted = true;
    inline bool bAutoBHop = false;
    inline bool bEdgeJump = false;
    inline bool bJumpbug = false;
    inline int nAutoBHopChance = 100;
    inline bool bAutostrafeAssistance = false;
    inline bool bAutoStrafe = false;
    inline unsigned int bAutostrafeMode = 0;
    inline float autostrafe_smooth = 80;
    inline int iAutobuyPrimary = 0;
    inline int iAutobuySecondary = 0;
    inline int iAutobuyMisc = 0;
    inline bool bAutobuy = false;
#pragma gma endregion
#pragma gma region variables_menu
    inline unsigned int nMenuKey = VK_INSERT;
    inline unsigned int nPanicKey = VK_END;
    inline int nDpiScale = 0;
    inline /*
    inline  * color navigation:
    inline  * [definition N][purpose]
    inline  * 1. primitive:
    inline  * - primtv 0 (text)
    inline  * - primtv 1 (background)
    inline  * - primtv 2 (disabled)
    inline  * - primtv 3 (control bg)
    inline  * - primtv 4 (border)
    inline  * - primtv 5 (hover)
    inline  *
    inline  * 2. accents:
    inline  * - accent 0 (main)
    inline  * - accent 1 (dark)
    inline  * - accent 2 (darker)
    inline  */
    inline unsigned int bMenuAdditional = MENU_ADDITION_ALL;
    inline ColorPickerVar_t colPrimtv0 = ColorPickerVar_t(255, 255, 255); // (text)
    inline ColorPickerVar_t colPrimtv1 = ColorPickerVar_t(50, 55, 70); // (background)
    inline ColorPickerVar_t colPrimtv2 = ColorPickerVar_t(190, 190, 190); // (disabled)
    inline ColorPickerVar_t colPrimtv3 = ColorPickerVar_t(20, 20, 30); // (control bg)
    inline ColorPickerVar_t colPrimtv4 = ColorPickerVar_t(0, 0, 0); // (border)
    inline ColorPickerVar_t colAccent0 = ColorPickerVar_t(85, 90, 160); // (main)
    inline ColorPickerVar_t colAccent1 = ColorPickerVar_t(100, 105, 175); // (dark)
    inline ColorPickerVar_t colAccent2 = ColorPickerVar_t(115, 120, 190); // (darker)
#pragma gma endregion
#pragma gma region antiaim
    inline bool bAntiAim = false;
    inline bool bAntiAimLeft = false;
    inline bool bAntiAimRight = false;
    inline bool bAngleNormalize = false;
    inline bool bJitter = false;
    inline int iJitterAmount = 0;
    inline int iRollJitterAmount = 0;
    inline int iBaseYawType = 1;
    inline int iPitchType = 1;
    inline float flAntiAimPitch = 0.f;
    inline float flAntiAimYaw = 0.f;
    inline bool bStopIfUsing = false;
    inline bool bStopIfAttacking = false;
    inline unsigned int uniStopIf = 0;
#pragma gma endregion
#pragma gma region ragebot
    inline bool bRagebot = false;
    inline bool bDoubleTap = false;
    inline bool bAutostop = false;
    inline bool bAutoscope = true;
    inline bool bHideShots = false;
    inline bool bAutoshoot = true;
    inline bool bCompensateRecoil = false;
    inline int iMinDamage = 10;
    inline int iMinDamageOverride = 1;
    inline int iHitchance = 65;
    inline int iMultipoints = 3;
    inline int flPointscale = 1.f;
    inline int iBacktrackTicks = 3;
    inline bool bManualAntiAim = true;
    inline unsigned int nAARightKey = VK_NUMPAD1;
    inline unsigned int nAALeftKey = VK_NUMPAD2;
    inline unsigned int nMinDmgOverride = VK_NUMPAD3;
#pragma endregion
};
