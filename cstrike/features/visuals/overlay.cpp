// used: [stl] vector
#include <vector>
// used: [stl] sort
#include <algorithm>

#include "overlay.h"

// used: cheat variables
#include "../../core/variables.h"

// used: entity
#include "../../sdk/entity.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/iengineclient.h"

// used: sdk variables
#include "../../core/sdk.h"

// used: l_print
#include "../../utilities/log.h"
// used: inputsystem
#include "../../utilities/inputsystem.h"
// used: draw system
#include "../../utilities/draw.h"

// used: mainwindowopened
#include "../../core/menu.h"

#include "../../core/interfaces.h"
#include "../../sdk/interfaces/igameresourceservice.h"

#include "../rage/lagcomp.h"

using namespace F::VISUALS;

void OVERLAY::OnFrameStageNotify(CCSPlayerController* pLocalController)
{
	if (!pLocalController)
		return;

	// only render when in-game
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;

	const int nHighestIndex = I::GameResourceService->pGameEntitySystem->GetHighestEntityIndex();

	std::vector<C_CSPlayerPawn> SortedEntities = {};

	for (int entity = 1; entity < I::Engine->GetMaxClients(); entity++)
	{
		C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(entity);
		if (!pEntity)
			continue;

		CCSPlayerController* pController = static_cast<CCSPlayerController*>(pEntity);
		if (!pController)
			continue;

		C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pController->GetPawnHandle());
		if (!pPawn)
			continue;

		if (!pController->IsPawnAlive())
			continue;

		Player(pLocalController, pController, 5);
	}

	//for (int entity = 64; entity < I::GameResourceService->pGameEntitySystem->GetHighestEntityIndex(); entity++) 
	//{
	//	C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(entity);
	//	if (!pEntity)
	//		continue;

	//	// todo: ESP for other entities
	//}
}

bool OVERLAY::GetEntityBoundingBox(C_CSPlayerPawn* pEntity, ImVec4* pVecOut)
{
	CCollisionProperty* pCollision = pEntity->GetCollision();
	if (pCollision == nullptr)
		return false;

	CGameSceneNode* pGameSceneNode = pEntity->GetGameSceneNode();
	if (pGameSceneNode == nullptr)
		return false;

	CTransform nodeToWorldTransform = pGameSceneNode->GetNodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.quatOrientation.ToMatrix(nodeToWorldTransform.vecPosition);

	const Vector_t vecMins = pCollision->GetMins();
	const Vector_t vecMaxs = pCollision->GetMaxs();

	pVecOut->x = pVecOut->y = std::numeric_limits<float>::max();
	pVecOut->z = pVecOut->w = -std::numeric_limits<float>::max();

	for (int i = 0; i < 8; ++i)
	{
		const Vector_t vecPoint{ 
			i & 1 ? vecMaxs.x : vecMins.x, 
			i & 2 ? vecMaxs.y : vecMins.y,
			i & 4 ? vecMaxs.z : vecMins.z 
		};
		ImVec2 vecScreen;
		if (!D::WorldToScreen(vecPoint.Transform(matTransform), &vecScreen))
			return false;

		pVecOut->x = MATH::Min(pVecOut->x, vecScreen.x);
		pVecOut->y = MATH::Min(pVecOut->y, vecScreen.y);
		pVecOut->z = MATH::Max(pVecOut->z, vecScreen.x);
		pVecOut->w = MATH::Max(pVecOut->w, vecScreen.y);
	}

	return true;
}

bool ComputeHitboxSurroundingBox(C_BaseEntity* baseEnt, Vector_t& min, Vector_t& max) {
	using fnComputeHitboxSurroundingBox = bool(CS_FASTCALL*)(void*, Vector_t&, Vector_t&);
	static auto ComputeHitboxSurroundingBox = reinterpret_cast<fnComputeHitboxSurroundingBox>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E9 ? ? ? ? F6 43 5B FD")), 1, 0));
	if (!ComputeHitboxSurroundingBox) {
		L_PRINT(LOG_ERROR) << CS_XOR("[C_BaseEntity::ComputeHitboxSurroundingBox] C_HOOK::ComputeHitboxSurroundingBox > null");
		return false;
	}
	return ComputeHitboxSurroundingBox(baseEnt, min, max);

}

bool CalculateBoundingBox(C_CSPlayerPawn* pPawn, ImVec4& out, bool compute_surrounding_box) {

	if (!pPawn || pPawn->GetHealth() <= 0)
		return false;

	CGameSceneNode* pGameSceneNode = pPawn->GetGameSceneNode();
	if (pGameSceneNode == nullptr)
		return false;

	CCollisionProperty* pCollision = pPawn->GetCollision();
	if (pCollision == nullptr) {
		return false;
	}

	Vector_t min{}, max{};

	CTransform nodeToWorldTransform = pGameSceneNode->GetNodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.quatOrientation.ToMatrix(nodeToWorldTransform.vecPosition);
	if (compute_surrounding_box) {
		if (!ComputeHitboxSurroundingBox(pPawn, min, max)) {
			return false;
		}
	}
	else {
		min = pCollision->GetMins();
		max = pCollision->GetMaxs();
	}

	out.x = out.y = std::numeric_limits<float>::max();
	out.z = out.w = -std::numeric_limits<float>::max();

	for (int i = 0; i < 8; ++i)
	{
		const Vector_t vecPoint{
			i & 1 ? max.x : min.x,
			i & 2 ? max.y : min.y,
			i & 4 ? max.z : min.z
		};
		ImVec2 vecScreen;
		if (!D::WorldToScreen(compute_surrounding_box ? vecPoint : vecPoint.Transform(matTransform), &vecScreen))
			return false;

		out.x = MATH::Min(out.x, vecScreen.x);
		out.y = MATH::Min(out.y, vecScreen.y);
		out.z = MATH::Max(out.z, vecScreen.x);
		out.w = MATH::Max(out.w, vecScreen.y);
	}

	return true;
}

ImVec4 Get2DBox(CCSPlayerController* PlayerEntity, C_CSPlayerPawn* pPawn)
{
	const float FixedPlayerHeight = 75.0f;
	Vector_t TopPos3D;

	TopPos3D = pPawn->GetSceneOrigin();
	TopPos3D.z += FixedPlayerHeight;

	ImVec2 GroundPos2D, TopPos2D;
	if (!D::WorldToScreen(pPawn->GetSceneOrigin(), &GroundPos2D) || !D::WorldToScreen(TopPos3D, &TopPos2D))
		return ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f };

	if (TopPos2D.y < 0.0f || GroundPos2D.y > ImGui::GetIO().DisplaySize.y)
		return ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f };


	ImVec2 Size, Pos;
	Size.y = std::abs(TopPos2D.y - GroundPos2D.y);
	Size.x = Size.y * 0.60;

	Pos.x = GroundPos2D.x - Size.x / 2;
	Pos.y = TopPos2D.y;

	return ImVec4{ Pos.x, Pos.y, Size.x, Size.y };
}

static void drawPlayerSkeleton(BoneData_t* bones) noexcept
{
	const auto color = g_Config::colSkeletonEsp;

	if (!bones)
		return;

	BoneData_t previous, current;
	for (auto i : BoneJointList::List)
	{
		previous.vecPosition = Vector_t(0, 0, 0);
		for (auto Index : i)
		{
			current = bones[Index];
			if (previous.vecPosition == Vector_t(0, 0, 0))
			{
				previous = current;
				//pGame->View->Gui->Text(Current.Name.c_str(), Current.ScreenPos, ImColor(255, 255, 0, 255));
				continue;
			}
			ImVec2 previousSP{}, currentSP{};
			if (D::WorldToScreen(previous.vecPosition, &previousSP) &&
				D::WorldToScreen(current.vecPosition, &currentSP))
			{
				ImGui::GetBackgroundDrawList()->AddLine(previousSP, currentSP, color.GetU32(), 1.f);
				//pGame->View->Gui->Text(Current.Name.c_str(), Current.ScreenPos, ImColor(255, 255, 0, 255));
			}
			previous = current;
		}
	}
}

static void drawPlayerName(ImVec4 vecbox, C_CSPlayerPawn* pPlayerPawn, CCSPlayerController* pPlayerController) noexcept
{
	const auto color = g_Config::colPlayerName;

	//Vector_t headBonePos = pPlayerPawn->GetGameSceneNode()->GetBonePosition(6);
	//ImVec2 screenHeadBonePos{};
	//D::WorldToScreen(Vector_t(vecbox.x, vecbox.y, vecbox.z), &screenHeadBonePos);

	/*ImVec2 screenTextPos = ImVec2(vecbox.x - (ImGui::CalcTextSize(pPlayerController->GetPlayerName()).x / 2), vecbox.y);

	ImGui::GetBackgroundDrawList()->AddText(screenTextPos, color.GetU32(), pPlayerController->GetPlayerName());*/

	ImVec2 TextSize = ImGui::CalcTextSize(pPlayerController->GetPlayerName());
	ImVec2 TextPos = { vecbox.x + vecbox.z / 2 - TextSize.x / 2, vecbox.y - 5 };

	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(TextPos.x - 1, TextPos.y), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerName());
	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(TextPos.x, TextPos.y - 1), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerName());
	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(TextPos.x + 1, TextPos.y), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerName());
	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(TextPos.x, TextPos.y + 1), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerName());

	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, TextPos, color.GetU32(), pPlayerController->GetPlayerName());
}

void DrawHealthBar(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, bool Horizontal = true)
{
	ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

	float Proportion = CurrentHealth / MaxHealth;

	ImColor FirstStageColor = ImColor(29, 179, 41, 220);
	ImColor SecondStageColor = ImColor(222, 174, 13, 220);
	ImColor ThirdStageColor = ImColor(214, 32, 32, 220);
	ImColor BackGroundColor = ImColor(5, 5, 5, 220);
	ImColor Color;

	if (g_Config::bOverrideHealthBarColor)
	{
		Color = g_Config::colHealthbarEsp.GetU32();
	}
	else
	{
		if (Proportion > 0.5)
			Color = FirstStageColor;
		else if (Proportion > 0.25)
			Color = SecondStageColor;
		else
			Color = ThirdStageColor;
	}

	DrawList->AddRectFilled(Pos, { Pos.x + 5, Pos.y + Size.y }, BackGroundColor);

	if (Horizontal)
	{
		DrawList->AddRectFilled(Pos, { Pos.x + 5 * Proportion, Pos.y + Size.y }, Color);
	}
	else
	{
		float healthHeight = Size.y * Proportion;
		DrawList->AddRectFilled({ Pos.x, Pos.y + Size.y - healthHeight }, { Pos.x + 5, Pos.y + Size.y }, Color);
	}

	ImColor BorderColor = ImColor(45, 45, 45, 220);
	DrawList->AddRect(Pos, { Pos.x + 5, Pos.y + Size.y }, BorderColor);
}

static void drawPlayerWeapon(ImVec4 vecbox, C_CSPlayerPawn* pPlayerPawn, CCSPlayerController* pPlayerController) noexcept
{
	if (!pPlayerController->GetPlayerWeapon(pPlayerPawn))
		return;

	if (!pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData())
		return;

	ImVec2 textSize = ImGui::CalcTextSize(pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData()->m_szName());
	ImVec2 textPosition;
	textPosition.x = vecbox.x + (vecbox.z - textSize.x) * 0.5f;
	textPosition.y = vecbox.y + vecbox.w;

	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(textPosition.x - 1, textPosition.y), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData()->m_szName());
	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(textPosition.x, textPosition.y - 1), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData()->m_szName());
	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(textPosition.x + 1, textPosition.y), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData()->m_szName());
	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, ImVec2(textPosition.x, textPosition.y + 1), ImColor(0, 0, 0, 200), pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData()->m_szName());

	ImGui::GetBackgroundDrawList()->AddText(FONT::pEsp, 11, textPosition, g_Config::colWeaponEsp.GetU32(), pPlayerController->GetPlayerWeapon(pPlayerPawn)->GetWeaponVData()->m_szName());
}

void OVERLAY::Player(CCSPlayerController* pLocal, CCSPlayerController* pPlayer, const float flDistance)
{
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocal->GetPawnHandle());
	C_CSPlayerPawn* pPlayerPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pPlayer->GetPawnHandle());

	if (pLocalPawn == nullptr || pPlayerPawn == nullptr)
		return;

	bool bIsEnemy = (pLocalPawn->IsOtherEnemy(pPlayerPawn));

	// @note: only enemy overlay for now
	if (!bIsEnemy)
		return;

	ImVec4 vecBox = Get2DBox(pLocal, pPlayerPawn);

	if (g_Config::bBox)
	{
		//GetEntityBoundingBox();

		ImVec2 p_min = ImVec2(vecBox.x, vecBox.y);
		ImVec2 p_max = ImVec2(vecBox.x + vecBox.z, vecBox.y + vecBox.w);

		ImGui::GetBackgroundDrawList()->AddRect(p_min, p_max, ImColor(0, 0, 0, 180), 0.f, 0, 2.2f);
		ImGui::GetBackgroundDrawList()->AddRect(p_min, p_max, g_Config::colBox.GetU32());
	}

	if (g_Config::bHealthbarEsp)
	{
		ImVec2 HealthBarPos, HealthBarSize; // ImVec2 HealthBarPos, HealthBarSize;
		ImVec2 Rect_Max = ImVec2(vecBox.x + vecBox.z, vecBox.y + vecBox.w);

		HealthBarPos = { vecBox.x - 4.0f, vecBox.y }; // HealthBarPos = { Rect.x - 7.0f, Rect.y };
		HealthBarSize = { 4.0f, vecBox.w }; // HealthBarSize = { 7.0f, Rect.w };

		DrawHealthBar(pPlayerPawn->GetMaxHealth(), pPlayerPawn->GetHealth(), HealthBarPos, HealthBarSize, false);
	}

	if (g_Config::bBoundingBox)
	{
		/*ImVec4 bb{};
		CalculateBoundingBox(pPlayerPawn, bb, true);

		ImVec2 bb_min = ImVec2(bb.x, bb.y);
		ImVec2 bb_max = ImVec2(bb.x + bb.z, bb.y + bb.w);

		ImGui::GetBackgroundDrawList()->AddRect(bb_min, bb_max, ImColor(0, 0, 0, 180), 0.f, 0, 2.2f);
		ImGui::GetBackgroundDrawList()->AddRect(bb_min, bb_max, g_Config::colBoundingBox.GetU32());*/
	}

	if (g_Config::bNameEsp)
	{
		drawPlayerName(vecBox, pPlayerPawn, pPlayer);
	}

	if (g_Config::bHeadDotEsp)
	{
		Vector_t headBonePos = pPlayerPawn->GetGameSceneNode()->GetBonePosition(6);
		ImVec2 screenHeadBonePos{};
		D::WorldToScreen(headBonePos, &screenHeadBonePos);
		ImGui::GetBackgroundDrawList()->AddCircleFilled(screenHeadBonePos, 2, g_Config::colHeadDot.GetU32());
	}

	if (g_Config::bSkeletonEsp)
	{
		if (LagRecord_t* pRecord = Lagcomp::m_arrRecords.at(pPlayer->GetRefEHandle().GetEntryIndex()).GetLastRecord(); pRecord != nullptr)
			drawPlayerSkeleton(reinterpret_cast<BoneData_t*>(pRecord->m_Matrix.data()));
					

		drawPlayerSkeleton(pPlayerPawn->GetGameSceneNode()->GetSkeletonInstance()->m_modelState().bones);
			
	}

	if (g_Config::bWeaponEsp)
	{
		drawPlayerWeapon(vecBox, pPlayerPawn, pPlayer);
	}

	//if (const auto& healthOverlayConfig = m_Config::overlayHealthBar; healthOverlayConfig.bEnable)
	//{
	//	// @note: pPlayerPawn->GetMaxHealth() sometime return 0.f
	//	const float flHealthFactor = pPlayerPawn->GetHealth() / 100.f;
	//	context.AddComponent(new CBarComponent(false, SIDE_LEFT, vecBox, flHealthFactor, m_Config::overlayHealthBar));
	//}

	/*if (const auto& armorOverlayConfig = m_Config::overlayArmorBar; armorOverlayConfig.bEnable)
	{
		const float flArmorFactor = pPlayerPawn->GetArmorValue() / 100.f;
		context.AddComponent(new CBarComponent(false, SIDE_BOTTOM, vecBox, flArmorFactor, m_Config::overlayArmorBar));
	}*/

}
