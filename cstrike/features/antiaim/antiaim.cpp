#include "antiaim.h"
#include <ctime>
#include <cmath>
#include "../../core/variables.h"
#include "../../globals.h"
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/igameresourceservice.h"

class IGameResourceService;

void F::ANTIAIM::RunAA(CUserCmd* pCmd, CCSGOInput* pCSGOInput)
{
	if (!g_Config::bAntiAim)
		return;

	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;

	if (!SDK::LocalController || !SDK::LocalController->IsPawnAlive())
		return;

	if (pCmd->nButtons.nValue & ECommandButtons::IN_USE)
		return;

	/*if (SDK::LocalController->GetActualMoveType() == MOVETYPE_LADDER)
		return;*/

	if (int32_t nMoveType = SDK::LocalController->GetActualMoveType(); nMoveType == MOVETYPE_NOCLIP || nMoveType == MOVETYPE_LADDER)
		return;

	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(SDK::LocalController->GetPawnHandle());

	if (pLocalPawn == nullptr)
		return;

	if (globals::m_bIsInFreezeTime)
		return;
	
	if (!g_Config::bHideShots)
		if (pCmd->nButtons.nValue & IN_ATTACK)
			return;

	QAngle_t angModifiedAngle = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;

	switch (g_Config::iBaseYawType)
	{
	case 0:
		break;
	case 1:
		angModifiedAngle.y += 179.f; // Backwards
		break;
	default:
		break;
	}

	switch (g_Config::iPitchType)
	{
	case 0:
		break;
	case 1: // Down
		angModifiedAngle.x = 89.f;
		break;
	case 2: // Up
		angModifiedAngle.x = -89.f;
		break;
	case 3: // Zero
		angModifiedAngle.x = 0.f;
		break;
	default:
		break;
	}

	static float yawJitterMult = 0.f;
	if (g_Config::bJitter) {
		if (yawJitterMult == 0.f)
			yawJitterMult = 1.f;

		yawJitterMult *= -1.f;

		angModifiedAngle.y += g_Config::iJitterAmount * yawJitterMult;
	}

	//static float rollJitterMult = 0.f;
	//if (g_Config::bJitter) {
	//	if (rollJitterMult == 0.f)
	//		rollJitterMult = 1.f;

	//	rollJitterMult *= -1.f;

	//	flRoll += g_Config::iRollJitterAmount * rollJitterMult;
	//}

	if (g_Config::bAntiAimLeft)
	{
		g_Config::bAntiAimRight = false;
		angModifiedAngle.y -= 90;
	}

	if (g_Config::bAntiAimRight)
	{
		g_Config::bAntiAimLeft = false;
		angModifiedAngle.y += 90;
	}

	//L_PRINT(LOG_INFO) << globals::m_bIsInFreezeTime;

	pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = angModifiedAngle.Normalize();
}
