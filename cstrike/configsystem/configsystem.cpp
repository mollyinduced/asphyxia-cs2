#include "configsystem.h"
#include "../core/config.h"
#include "../core/variables.h"
#include "../core/menu.h"

void to_json(nlohmann::json& j, const Color_t& p) {
    j = nlohmann::json{ {"r", p.r}, {"g", p.g}, {"b", p.b}, {"a", p.a} };
}

void from_json(const nlohmann::json& j, Color_t& p) {
    j.at("r").get_to(p.r);
    j.at("g").get_to(p.g);
    j.at("b").get_to(p.b);
    j.at("a").get_to(p.a);
}

void configsystem::readconfig(const std::string& filePath)
{
    std::ifstream inputFile("C:\\ohook\\configs\\" + filePath);

    if (!inputFile.is_open()) {
        L_PRINT(LOG_INFO) << "Could not open the file for reading!";
        return;
    }

    // Parse the JSON data from the file
    json cfg;
    inputFile >> cfg;

    // Close the file
    inputFile.close();

    CFG_LOAD(MENU::selectedHitboxes);

    CFG_LOAD(g_Config::bHeadDotEsp);
    CFG_LOAD(g_Config::colHeadDot);
    CFG_LOAD(g_Config::bBox);
    CFG_LOAD(g_Config::colBox);
    CFG_LOAD(g_Config::bBoundingBox);
    CFG_LOAD(g_Config::colBoundingBox);
    CFG_LOAD(g_Config::bSkeletonEsp);
    CFG_LOAD(g_Config::colSkeletonEsp);
    CFG_LOAD(g_Config::bNameEsp);
    CFG_LOAD(g_Config::colPlayerName);
    CFG_LOAD(g_Config::bWeaponEsp);
    CFG_LOAD(g_Config::colWeaponEsp);
    CFG_LOAD(g_Config::bDrawHistorySkeletonEsp);
    CFG_LOAD(g_Config::bHealthbarEsp);
    CFG_LOAD(g_Config::colHealthbarEsp);

    CFG_LOAD(g_Config::bThirdPerson);
    CFG_LOAD(g_Config::flThirdperson);
    CFG_LOAD(g_Config::flSetViewModelFOV);
    CFG_LOAD(g_Config::flSetWorldFov);
    CFG_LOAD(g_Config::bThirdpersonNoInterp);
    CFG_LOAD(g_Config::bNightmode);
    CFG_LOAD(g_Config::colNightmode);
    CFG_LOAD(g_Config::colWorldColor);
    CFG_LOAD(g_Config::bVisualChams);
    CFG_LOAD(g_Config::bVisualChamsIgnoreZ);
    CFG_LOAD(g_Config::nVisualChamMaterial);
    CFG_LOAD(g_Config::colVisualChams);
    CFG_LOAD(g_Config::colVisualChamsIgnoreZ);
    CFG_LOAD(g_Config::bVisualChamsSelf);
    CFG_LOAD(g_Config::bWeaponChams);
    CFG_LOAD(g_Config::bVisualChamsSelfIgnoreZ);
    CFG_LOAD(g_Config::nVisualSelfChamMaterial);
    CFG_LOAD(g_Config::colVisualChamsSelf);
    CFG_LOAD(g_Config::colVisualChamsSelfIgnoreZ);
    CFG_LOAD(g_Config::selfGlowColor);
    CFG_LOAD(g_Config::selfGlow);
    CFG_LOAD(g_Config::espGlowColor);
    CFG_LOAD(g_Config::espGlow);
    CFG_LOAD(g_Config::bRemoveScope);
    CFG_LOAD(g_Config::bDrawStaticCrosshair);
    CFG_LOAD(g_Config::iScopeGap);
    CFG_LOAD(g_Config::iScopeThickness);
    CFG_LOAD(g_Config::iScopeLength);
    CFG_LOAD(g_Config::cScope);
    CFG_LOAD(g_Config::bAntiUntrusted);
    CFG_LOAD(g_Config::bAutoBHop);
    CFG_LOAD(g_Config::bJumpbug);
    CFG_LOAD(g_Config::nAutoBHopChance);
    CFG_LOAD(g_Config::bAutostrafeAssistance);
    CFG_LOAD(g_Config::bAutoStrafe);
    CFG_LOAD(g_Config::bAutostrafeMode);
    CFG_LOAD(g_Config::autostrafe_smooth);
    CFG_LOAD(g_Config::iAutobuyPrimary);
    CFG_LOAD(g_Config::iAutobuySecondary);
    CFG_LOAD(g_Config::iAutobuyMisc);
    CFG_LOAD(g_Config::bAutobuy);
    CFG_LOAD(g_Config::bAntiAim);
    CFG_LOAD(g_Config::bAntiAimLeft);
    CFG_LOAD(g_Config::bAntiAimRight);
    CFG_LOAD(g_Config::bAngleNormalize);
    CFG_LOAD(g_Config::bJitter);
    CFG_LOAD(g_Config::iJitterAmount);
    CFG_LOAD(g_Config::iBaseYawType);
    CFG_LOAD(g_Config::iPitchType);
    CFG_LOAD(g_Config::flAntiAimPitch);
    CFG_LOAD(g_Config::flAntiAimYaw);
    CFG_LOAD(g_Config::bStopIfUsing);
    CFG_LOAD(g_Config::bStopIfAttacking);
    CFG_LOAD(g_Config::uniStopIf);
    CFG_LOAD(g_Config::bRagebot);
    CFG_LOAD(g_Config::bDoubleTap);
    CFG_LOAD(g_Config::bAutostop);
    CFG_LOAD(g_Config::bAutoscope);
    CFG_LOAD(g_Config::bHideShots);
    CFG_LOAD(g_Config::bAutoshoot);
    CFG_LOAD(g_Config::bCompensateRecoil);
    CFG_LOAD(g_Config::iMinDamage);
    CFG_LOAD(g_Config::iMinDamageOverride);
    CFG_LOAD(g_Config::iHitchance);
    CFG_LOAD(g_Config::iMultipoints);
    CFG_LOAD(g_Config::iBacktrackTicks);
    CFG_LOAD(g_Config::bManualAntiAim);
    CFG_LOAD(g_Config::nAARightKey);
    CFG_LOAD(g_Config::nAALeftKey);
    CFG_LOAD(g_Config::nMinDmgOverride);
    CFG_LOAD(g_Config::flPointscale);
    CFG_LOAD(g_Config::bOverrideHealthBarColor);
    CFG_LOAD(g_Config::bSubtickAutostrafeExploit);
    CFG_LOAD(g_Config::iRagebotFov);



    L_PRINT(LOG_INFO) << "Config data read from file successfully: " << filePath.c_str();
}

void configsystem::writeconfig(const std::string& filePath)
{
    json cfg;



    CFG_SAVE(MENU::selectedHitboxes);

    CFG_SAVE(g_Config::bHeadDotEsp);
    CFG_SAVE(g_Config::colHeadDot);
    CFG_SAVE(g_Config::bBoundingBox);
    CFG_SAVE(g_Config::colBoundingBox);
    CFG_SAVE(g_Config::bBox);
    CFG_SAVE(g_Config::colBox);
    CFG_SAVE(g_Config::bSkeletonEsp);
    CFG_SAVE(g_Config::colSkeletonEsp);
    CFG_SAVE(g_Config::bNameEsp);
    CFG_SAVE(g_Config::colPlayerName);
    CFG_SAVE(g_Config::bWeaponEsp);
    CFG_SAVE(g_Config::colWeaponEsp);
    CFG_SAVE(g_Config::bDrawHistorySkeletonEsp);
    CFG_SAVE(g_Config::bHealthbarEsp);
    CFG_SAVE(g_Config::colHealthbarEsp);

    CFG_SAVE(g_Config::bThirdPerson);
    CFG_SAVE(g_Config::flThirdperson);
    CFG_SAVE(g_Config::flSetViewModelFOV);
    CFG_SAVE(g_Config::flSetWorldFov);
    CFG_SAVE(g_Config::bThirdpersonNoInterp);
    CFG_SAVE(g_Config::bNightmode);
    CFG_SAVE(g_Config::colNightmode);
    CFG_SAVE(g_Config::colWorldColor);
    CFG_SAVE(g_Config::bVisualChams);
    CFG_SAVE(g_Config::bVisualChamsIgnoreZ);
    CFG_SAVE(g_Config::nVisualChamMaterial);
    CFG_SAVE(g_Config::colVisualChams);
    CFG_SAVE(g_Config::colVisualChamsIgnoreZ);
    CFG_SAVE(g_Config::bVisualChamsSelf);
    CFG_SAVE(g_Config::bWeaponChams);
    CFG_SAVE(g_Config::bVisualChamsSelfIgnoreZ);
    CFG_SAVE(g_Config::nVisualSelfChamMaterial);
    CFG_SAVE(g_Config::colVisualChamsSelf);
    CFG_SAVE(g_Config::colVisualChamsSelfIgnoreZ);
    CFG_SAVE(g_Config::selfGlowColor);
    CFG_SAVE(g_Config::selfGlow);
    CFG_SAVE(g_Config::espGlowColor);
    CFG_SAVE(g_Config::espGlow);
    CFG_SAVE(g_Config::bRemoveScope);
    CFG_SAVE(g_Config::bDrawStaticCrosshair);
    CFG_SAVE(g_Config::iScopeGap);
    CFG_SAVE(g_Config::iScopeThickness);
    CFG_SAVE(g_Config::iScopeLength);
    CFG_SAVE(g_Config::cScope);
    CFG_SAVE(g_Config::bAntiUntrusted);
    CFG_SAVE(g_Config::bAutoBHop);
    CFG_SAVE(g_Config::bJumpbug);
    CFG_SAVE(g_Config::nAutoBHopChance);
    CFG_SAVE(g_Config::bAutostrafeAssistance);
    CFG_SAVE(g_Config::bAutoStrafe);
    CFG_SAVE(g_Config::bAutostrafeMode);
    CFG_SAVE(g_Config::autostrafe_smooth);
    CFG_SAVE(g_Config::iAutobuyPrimary);
    CFG_SAVE(g_Config::iAutobuySecondary);
    CFG_SAVE(g_Config::iAutobuyMisc);
    CFG_SAVE(g_Config::bAutobuy);
    // Anti-am_Config::im settings
    CFG_SAVE(g_Config::bAntiAim);
    CFG_SAVE(g_Config::bAntiAimLeft);
    CFG_SAVE(g_Config::bAntiAimRight);
    CFG_SAVE(g_Config::bAngleNormalize);
    CFG_SAVE(g_Config::bJitter);
    CFG_SAVE(g_Config::iJitterAmount);
    CFG_SAVE(g_Config::iBaseYawType);
    CFG_SAVE(g_Config::iPitchType);
    CFG_SAVE(g_Config::flAntiAimPitch);
    CFG_SAVE(g_Config::flAntiAimYaw);
    CFG_SAVE(g_Config::bStopIfUsing);
    CFG_SAVE(g_Config::bStopIfAttacking);
    CFG_SAVE(g_Config::uniStopIf);
    // Ragebom_Config::t settings
    CFG_SAVE(g_Config::bRagebot);
    CFG_SAVE(g_Config::bDoubleTap);
    CFG_SAVE(g_Config::bAutostop);
    CFG_SAVE(g_Config::bAutoscope);
    CFG_SAVE(g_Config::bHideShots);
    CFG_SAVE(g_Config::bAutoshoot);
    CFG_SAVE(g_Config::bCompensateRecoil);
    CFG_SAVE(g_Config::iMinDamage);
    CFG_SAVE(g_Config::iMinDamageOverride);
    CFG_SAVE(g_Config::iHitchance);
    CFG_SAVE(g_Config::iMultipoints);
    CFG_SAVE(g_Config::iBacktrackTicks);
    CFG_SAVE(g_Config::bManualAntiAim);
    CFG_SAVE(g_Config::nAARightKey);
    CFG_SAVE(g_Config::nAALeftKey);
    CFG_SAVE(g_Config::nMinDmgOverride);
    CFG_SAVE(g_Config::flPointscale);
    CFG_SAVE(g_Config::bOverrideHealthBarColor);
    CFG_SAVE(g_Config::bSubtickAutostrafeExploit);
    CFG_SAVE(g_Config::iRagebotFov);


    std::ofstream outputFile("C:\\ohook\\configs\\" + filePath);

    if (!outputFile.is_open()) {
        L_PRINT(LOG_INFO) << "Could not open the file for writing: " << filePath.c_str();
        return;
    }

    outputFile << cfg.dump(4);
    outputFile.close();

    L_PRINT(LOG_INFO) << "Config data written to file successfully: " << filePath.c_str();
}