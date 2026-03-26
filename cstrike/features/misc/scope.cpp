#include "scope.h"
#include "../../globals.h"

void SCOPE::DrawCrosshair()
{
    if (!SDK::LocalController->IsPawnAlive())
        return;

    if (!SDK::LocalPawn->IsScoped())
        return;

    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    ImVec2 screen = ImGui::GetIO().DisplaySize;
    ImVec2 center = ImVec2(screen.x * 0.5f, screen.y * 0.5f);

    Color_t color = g_Config::cScope;
    ImU32 color_transparent = IM_COL32(0, 0, 0, 0);

    int gap = g_Config::iScopeGap;
    int thickness = g_Config::iScopeThickness;
    int line_length = g_Config::iScopeLength;

    // Top line
    draw->AddRectFilledMultiColor(
        ImVec2(center.x - thickness * 0.5f, center.y - gap * 0.5f - line_length),
        ImVec2(center.x + thickness * 0.5f, center.y - gap * 0.5f),
        color_transparent, color_transparent, color.GetU32(), color.GetU32()
    );

    // Bottom line
    draw->AddRectFilledMultiColor(
        ImVec2(center.x - thickness * 0.5f, center.y + gap * 0.5f),
        ImVec2(center.x + thickness * 0.5f, center.y + gap * 0.5f + line_length),
        color.GetU32(), color.GetU32(), color_transparent, color_transparent
    );

    // Left line
    draw->AddRectFilledMultiColor(
        ImVec2(center.x - gap * 0.5f - line_length, center.y - thickness * 0.5f),
        ImVec2(center.x - gap * 0.5f, center.y + thickness * 0.5f),
        color_transparent, color.GetU32(), color.GetU32(), color_transparent
    );

    // Right line
    draw->AddRectFilledMultiColor(
        ImVec2(center.x + gap * 0.5f, center.y - thickness * 0.5f),
        ImVec2(center.x + gap * 0.5f + line_length, center.y + thickness * 0.5f),
        color.GetU32(), color_transparent, color_transparent, color.GetU32()
    );
}


__int64(__fastcall* o52BEE0)(C_CSPlayerPawn* pawn, __int64 unk);
__int64 __fastcall hk52BEE0(C_CSPlayerPawn* pawn, __int64 unk) {
    if (!pawn)
        return o52BEE0(pawn, unk);

    if (pawn->GetRefEHandle().GetEntryIndex() != SDK::LocalPawn->GetRefEHandle().GetEntryIndex())
        return o52BEE0(pawn, unk);

    if (g_Config::bRemoveScope && pawn->IsScoped() && I::Engine->IsInGame() && I::Engine->IsConnected())
        return NULL;

    return o52BEE0(pawn, unk);
}

void SCOPE::Hook()
{
    void* func = MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 80 7C 24 34 ? 74 ?"), 0x1);
    if (MH_CreateHook(func, hk52BEE0, (LPVOID*)&o52BEE0) != MH_OK)
        return;

    if (MH_EnableHook(func) != MH_OK)
        return;
}

void SCOPE::Run()
{
    if (!globals::m_bReadyToRender)
        return;

    if (!SDK::LocalPawn)
        return;

    if (g_Config::bRemoveScope && I::Engine->IsInGame() && I::Engine->IsConnected() && g_Config::bDrawStaticCrosshair)
    {
        SCOPE::DrawCrosshair();
    }
}