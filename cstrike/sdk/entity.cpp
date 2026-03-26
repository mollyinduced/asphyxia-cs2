// used: convars
#include "../core/convars.h"
#include "interfaces/cgameentitysystem.h"
#include "interfaces/ienginecvar.h"
#include "interfaces/iengineclient.h"
#include "../core/interfaces.h"
#include "../sdk/interfaces/igameresourceservice.h"

// used: game's definitions, enums
#include "const.h"
#include "../core/sdk.h"

#include "entity.h"


// global empty vector for when we can't get the origin
static Vector_t vecEmpty = Vector_t(0, 0, 0);

Vector_t C_CSPlayerPawn::GetEyePosition() {
	auto game_scene_node = GetGameSceneNode();

	return game_scene_node->GetAbsOrigin() + GetViewOffset();
}

CCSPlayerController* CCSPlayerController::GetLocalPlayerController()
{
	const int nIndex = I::Engine->GetLocalPlayer();
	return I::GameResourceService->pGameEntitySystem->Get<CCSPlayerController>(nIndex);
}

const Vector_t& CCSPlayerController::GetPawnOrigin()
{
	CBaseHandle hPawn = this->GetPawnHandle();
	if (!hPawn.IsValid())
		return vecEmpty;

	C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(hPawn);
	if (pPawn == nullptr)
		return vecEmpty;

	return pPawn->GetSceneOrigin();
}

C_CSWeaponBase* CCSPlayerController::GetPlayerWeapon(C_CSPlayerPawn* pPlayer)
{
	if (!pPlayer || !pPlayer->GetWeaponServices())
		return nullptr;

	CBaseHandle hActiveWeapon = pPlayer->GetWeaponServices()->GetActiveWeapon();

	if (!hActiveWeapon.IsValid())
		return nullptr;

	C_CSWeaponBase* pWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hActiveWeapon);

	return pWeapon;
}

C_BaseEntity* C_BaseEntity::GetLocalPlayer()
{
	const int nIndex = I::Engine->GetLocalPlayer();
	return I::GameResourceService->pGameEntitySystem->Get(nIndex);
}

const Vector_t& C_BaseEntity::GetSceneOrigin()
{
	if (this->GetGameSceneNode())
		return GetGameSceneNode()->GetAbsOrigin();

	return vecEmpty;
}

bool CCSPlayerController::IsThrowingGrenade(C_CSWeaponBase* pBaseWeapon)
{
	if (!pBaseWeapon)
		return false;

	if (!SDK::LocalController->IsPawnAlive() || !SDK::LocalController)
		return false;

	const float flServerTime = TICKS_TO_TIME(this->m_nTickBase());
	const short nDefinitionIndex = pBaseWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex();
	CCSWeaponBaseVData* pWeaponBaseVData = pBaseWeapon->GetWeaponVData();
	if (!pWeaponBaseVData)
		return false;

	if (pWeaponBaseVData->GetWeaponType() == WEAPONTYPE_GRENADE)
	{
		C_BaseCSGrenade* pGrenade = reinterpret_cast<C_BaseCSGrenade*>(pBaseWeapon);
		if (pGrenade != nullptr)
		{
			return !pGrenade->IsPinPulled() && pGrenade->GetThrowTime() > 0.f && pGrenade->GetThrowTime() < flServerTime;
		}
	}

	return false;
}

bool C_CSPlayerPawn::IsOtherEnemy(C_CSPlayerPawn* pOther)
{
	// check are other player is invalid or we're comparing against ourselves
	if (pOther == nullptr || this == pOther)
		return false;

	if (CONVAR::game_type->value.i32 == GAMETYPE_FREEFORALL && CONVAR::game_mode->value.i32 == GAMEMODE_FREEFORALL_SURVIVAL)
		// check is not teammate
		return (this->GetSurvivalTeam() != pOther->GetSurvivalTeam());

	// @todo: check is deathmatch
	if (CONVAR::mp_teammates_are_enemies->value.i1)
		return true;

	return this->GetAssociatedTeam() != pOther->GetAssociatedTeam();
}

int C_CSPlayerPawn::GetAssociatedTeam()
{
	const int nTeam = this->GetTeam();

	// @todo: check is coaching, currently cs2 doesnt have sv_coaching_enabled, so just let it be for now...
	//if (CONVAR::sv_coaching_enabled->GetBool() && nTeam == TEAM_SPECTATOR)
	//	return this->GetCoachingTeam();

	return nTeam;
}

bool C_CSPlayerPawn::CanAttack(const float flServerTime)
{
	// check is player ready to attack
	if (CCSPlayer_WeaponServices* pWeaponServices = this->GetWeaponServices(); pWeaponServices != nullptr)
		if (this->IsWaitForNoAttack() || pWeaponServices->GetNextAttack() > flServerTime)
			return false;

	return true;
}

bool C_CSWeaponBaseGun::CanPrimaryAttack(const int nWeaponType, const float flServerTime)
{
	// check are weapon support burst mode and it's ready to attack
	if (this->IsBurstMode())
	{
		// check is it ready to attack
		if (this->GetBurstShotsRemaining() > 0 /*&& this->GetNextBurstShotTime() <= flServerTime*/)
			return true;
	}

	// check is weapon ready to attack
	if (this->GetNextPrimaryAttackTick() > TIME_TO_TICKS(flServerTime))
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->GetClip1() <= 0)
		return false;

	const ItemDefinitionIndex_t nDefinitionIndex = this->GetAttributeManager()->GetItem()->GetItemDefinitionIndex();

	// check for revolver cocking ready
	if (nDefinitionIndex == WEAPON_R8_REVOLVER && this->GetPostponeFireReadyFrac() > flServerTime)
		return false;

	return true;
}

bool C_CSWeaponBaseGun::CanSecondaryAttack(const int nWeaponType, const float flServerTime)
{
	// check is weapon ready to attack
	if (this->GetNextSecondaryAttackTick() > TIME_TO_TICKS(flServerTime))
		return false;

	// we doesn't need additional checks for knives
	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	// check do weapon have ammo
	if (this->GetClip1() <= 0)
		return false;

	// only revolver is allowed weapon for secondary attack
	if (this->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() != WEAPON_R8_REVOLVER)
		return false;

	return true;
}
