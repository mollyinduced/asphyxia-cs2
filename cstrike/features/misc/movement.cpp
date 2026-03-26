#include "movement.h"

// used: sdk entity
#include "../../sdk/entity.h"
// used: cusercmd
#include "../../sdk/datatypes/usercmd.h"

// used: convars
#include "../../core/convars.h"
#include "../../sdk/interfaces/ienginecvar.h"

// used: cheat variables
#include "../../core/variables.h"

#include <algorithm>
#include "../antiaim/antiaim.h"
#include "../prediction/pred.h"

#include <array>
#include <algorithm>
#include <DirectXMath.h>

// movement correction angles
static QAngle_t angCorrectionView = {};

void F::MISC::MOVEMENT::OnMove(CUserCmd* pCmd, CCSPlayerController* pLocalController, C_CSPlayerPawn* pLocalPawn)
{
	if (!pLocalController->IsPawnAlive())
		return;

	CBaseUserCmdPB* pBaseCmd = pCmd->csgoUserCmd.pBaseCmd;
	if (pBaseCmd == nullptr)
		return;

	// check if player is in noclip or on ladder or in water
	if (const int32_t nMoveType = pLocalPawn->GetActualMoveType(); nMoveType == MOVETYPE_NOCLIP || nMoveType == MOVETYPE_LADDER || pLocalPawn->GetWaterLevel() >= WL_WAIST)
		return;

	AutoStrafe(pBaseCmd, pLocalPawn);

	F::ANTIAIM::RunAA(pCmd, I::Input);

	MovementFix(pBaseCmd, F::ANTIAIM::angStoredViewBackup);

	for (int nSubTick = 0; nSubTick < pCmd->csgoUserCmd.inputHistoryField.pRep->nAllocatedSize; nSubTick++)
	{
		CCSGOInputHistoryEntryPB* pInputEntry = pCmd->GetInputHistoryEntry(nSubTick);
		if (pInputEntry == nullptr)
			continue;

		// save view angles for movement correction
		angCorrectionView = pInputEntry->pViewAngles->angValue;

		// movement correction & anti-untrusted
		ValidateUserCommand(pCmd, pBaseCmd, pInputEntry);
	}
}

void AngleQangles(const QAngle_t& angles, QAngle_t* forward, QAngle_t* right, QAngle_t* up)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	// Convert angles from degrees to radians
	angle = angles.y * (MATH::_PI / 180.0);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles.x * (MATH::_PI / 180.0);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles.z * (MATH::_PI / 180.0);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void F::MISC::MOVEMENT::MovementFix(CBaseUserCmdPB* pBaseCmd, QAngle_t& angDesiredViewPoint)
{
	Vector2D_t vecMove = Vector2D_t(pBaseCmd->flForwardMove, pBaseCmd->flSideMove);
	Vector2D_t vecMoveBackup = vecMove;
	
	if (!pBaseCmd->pViewAngles)
		return;

	const QAngle_t& angCurrentAngle = pBaseCmd->pViewAngles->angValue;
	const float flDelta = remainderf(angDesiredViewPoint.y - angCurrentAngle.y, 360.f);
	const float flYaw = M_DEG2RAD(flDelta);

	vecMove.x = vecMoveBackup.x * std::cos(flYaw) - vecMoveBackup.y * std::sin(flYaw);
	vecMove.y = vecMoveBackup.x * std::sin(flYaw) + vecMoveBackup.y * std::cos(flYaw);

	float flMultiply = 1.f;
	if (std::fabsf(vecMove.x) > 1.f)
		flMultiply = 1.f / std::fabsf(vecMove.x);
	else if (std::fabsf(vecMove.y) > 1.f)
		flMultiply = 1.f / std::fabsf(vecMove.y);

	vecMove.x *= flMultiply;
	vecMove.y *= flMultiply;

	I::Input->currentMoveData.flForwardMove = vecMove.x;
	I::Input->currentMoveData.flSideMove = -vecMove.y;

	pBaseCmd->SetBits(PROTOSLOT_6 | PROTOSLOT_7);
	pBaseCmd->flForwardMove = vecMove.x;
	pBaseCmd->flSideMove = vecMove.y;
}

uint64_t last_pressed = 0;
uint64_t last_buttons = 0;
float m_old_yaw;
bool m_switch_value = false;

void F::MISC::MOVEMENT::AutoStrafe(CBaseUserCmdPB* pUserCmd, C_CSPlayerPawn* pLocalPawn) 
{
	if (!g_Config::bAutoStrafe)
		return;

	/*CSubtickMoveStep* subtick = pUserCmd->AddSubTick();
	if (!subtick)
		return;

	subtick->flWhen = 0.f;*/

	//subtick-> = i * .1f;

	//auto mov_serv = SDK::LocalPawn->m_pMovementServices();

	if (g_Config::bSubtickAutostrafeExploit)
	{
		I::Cvar->Find(FNV1A::HashConst("sv_cheats"))->value.i1 = true;
		I::Cvar->Find(FNV1A::HashConst("sv_air_max_wishspeed"))->value.fl = 60.f;
		I::Cvar->Find(FNV1A::HashConst("sv_airaccelerate"))->value.fl = 35.f;
	}
	else
	{
		I::Cvar->Find(FNV1A::HashConst("sv_air_max_wishspeed"))->value.fl = 30.f;
		I::Cvar->Find(FNV1A::HashConst("sv_airaccelerate"))->value.fl = 12.f;
	}

	if (SDK::LocalPawn->GetMoveType() == MOVETYPE_LADDER || SDK::LocalPawn->GetMoveType() == MOVETYPE_NOCLIP || SDK::LocalPawn->GetFlags() & FL_ONGROUND)
		return;

	CPlayer_MovementServices* pMovementServices = SDK::LocalPawn->m_pMovementServices();
	if (!pMovementServices)
		return;

	const float flSpeed = SDK::LocalPawn->GetAbsVelocity().Length2D();
	auto CalculateDelta = [=]() // https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Proper_auto-strafer
	{
		const static float flMaxSpeed = pMovementServices->m_flMaxspeed();
		const float sv_airaccelerate = CONVAR::sv_airaccelerate->value.fl;
		const float flTerm = 30.0f / sv_airaccelerate / flMaxSpeed * 100.0f / flSpeed;

		if (flTerm < 1.0f && flTerm > -1.0f)
			return std::acos(flTerm);

		return 0.0f;
	};

	const float flDeltaAir = CalculateDelta();
	if (flDeltaAir != 0.0f)
	{
		const QAngle_t angViewAngles = I::Input->GetViewAngles();
		const float flYaw = M_DEG2RAD(angViewAngles.y);
		const Vector_t vecVelocity = SDK::LocalPawn->GetAbsVelocity();
		const float flVelocityDirection = std::atan2(vecVelocity.y, vecVelocity.x) - flYaw;
		const float flBestAngleMove = std::atan2(pUserCmd->flSideMove, pUserCmd->flForwardMove);

		auto GetDeltaAngle = [](float flFirst, float flSecond)
		{
			const float flDelta = flFirst - flSecond;
			float flResult = std::isfinite(flDelta) ? std::remainder(flDelta, DirectX::XM_2PI) : 0.0f;

			if (flFirst > flSecond)
			{
				if (flResult >= DirectX::XM_PI)
					flResult -= DirectX::XM_2PI;
			}
			else
			{
				if (flResult <= -DirectX::XM_PI)
					flResult += DirectX::XM_2PI;
			}

			return flResult;
		};

		const float flDelta = GetDeltaAngle(flVelocityDirection, flBestAngleMove);
		const float flFinalMove = flDelta < 0.0f ? flVelocityDirection + flDeltaAir : flVelocityDirection - flDeltaAir;

		pUserCmd->SetBits(BASE_BITS_FORWARDMOVE);
		pUserCmd->SetBits(BASE_BITS_LEFTMOVE);

		pUserCmd->flForwardMove = std::cos(flFinalMove);
		pUserCmd->flSideMove = std::sin(flFinalMove);

		I::Input->currentMoveData.flForwardMove = std::cos(flFinalMove);
		I::Input->currentMoveData.flSideMove = std::sin(flFinalMove);
	}
}

void F::MISC::MOVEMENT::BunnyHop(CUserCmd* pCmd, CBaseUserCmdPB* pUserCmd, C_CSPlayerPawn* pLocalPawn)
{
	if (pCmd->nButtons.nValue & IN_JUMP && SDK::LocalPawn->GetFlags() & FL_ONGROUND)
	{
		pCmd->nButtons.nValue &= ~IN_JUMP;
	}
}

void F::MISC::MOVEMENT::JumpBug(CUserCmd* pCmd, CBaseUserCmdPB* pUserCmd, C_CSPlayerPawn* pLocalPawn)
{
	if (!g_Config::bJumpbug)
		return;

	if (pCmd->nButtons.nValue & IN_JUMP && SDK::LocalPawn->GetFlags() & FL_ONGROUND)
	{
		pCmd->nButtons.nValue |= IN_DUCK;
		pCmd->nButtons.nValue &= ~IN_JUMP;
	}
}

void F::MISC::MOVEMENT::EdgeJump(CUserCmd* pCmd)
{
	if (!g_Config::bEdgeJump)
		return;

	if (g_PredictionSystem->GetOldFlags( ) & FL_ONGROUND && !(SDK::LocalPawn->GetFlags() & FL_ONGROUND))
		pCmd->nButtons.nValue |= IN_JUMP;
}

void F::MISC::MOVEMENT::ValidateUserCommand(CUserCmd* pCmd, CBaseUserCmdPB* pUserCmd, CCSGOInputHistoryEntryPB* pInputEntry)
{
	if (pUserCmd == nullptr)
		return;

	// clamp angle to avoid untrusted angle
	if (g_Config::bAntiUntrusted)
	{
		if (pInputEntry->pViewAngles->angValue.IsValid())
		{
			pInputEntry->pViewAngles->angValue.Clamp();
			pInputEntry->pViewAngles->angValue.z = 0.f;
		}
		else
		{
			pInputEntry->pViewAngles->angValue = {};
			L_PRINT(LOG_WARNING) << CS_XOR("view angles have a NaN component, the value is reset");
		}
	}

	// correct movement buttons while player move have different to buttons values
	// clear all of the move buttons states	
	pCmd->nButtons.nValue &= (~IN_FORWARD | ~IN_BACK | ~IN_LEFT | ~IN_RIGHT);

	// re-store buttons by active forward/side moves
	if (pUserCmd->flForwardMove > 0.0f)
		pCmd->nButtons.nValue |= IN_FORWARD;
	else if (pUserCmd->flForwardMove < 0.0f)
		pCmd->nButtons.nValue |= IN_BACK;

	if (pUserCmd->flSideMove > 0.0f)
		pCmd->nButtons.nValue |= IN_RIGHT;
	else if (pUserCmd->flSideMove < 0.0f)
		pCmd->nButtons.nValue |= IN_LEFT;
		
	if (!pInputEntry->pViewAngles->angValue.IsZero())
	{
		const float flDeltaX = std::remainderf(pInputEntry->pViewAngles->angValue.x - angCorrectionView.x, 360.f);
		const float flDeltaY = std::remainderf(pInputEntry->pViewAngles->angValue.y - angCorrectionView.y, 360.f);

		float flPitch = CONVAR::m_pitch->value.fl;
		float flYaw = CONVAR::m_yaw->value.fl;

		float flSensitivity = CONVAR::sensitivity->value.fl;
		if (flSensitivity == 0.0f)
			flSensitivity = 1.0f;

		pUserCmd->nMousedX = static_cast<short>(flDeltaX / (flSensitivity * flPitch));
		pUserCmd->nMousedY = static_cast<short>(-flDeltaY / (flSensitivity * flYaw));
	}
}

void F::MISC::MOVEMENT::PostPrediction(CUserCmd* pCmd)
{
	BunnyHop(pCmd, pCmd->csgoUserCmd.pBaseCmd, SDK::LocalPawn);
	JumpBug(pCmd, pCmd->csgoUserCmd.pBaseCmd, SDK::LocalPawn);
	EdgeJump(pCmd);
}