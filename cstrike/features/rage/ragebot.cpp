#include "ragebot.h"
#include <DirectXMath.h>

void RAGEBOT::AutoStop(C_CSPlayerPawn* pLocal, C_CSWeaponBase* weapon, CUserCmd* cmd)
{
	if (!bAutoStop || CONVAR::weapon_accuracy_nospread->value.i1)
		return;

	if (cmd->nButtons.nValue & IN_DUCK || !pLocal || !weapon)
		return;

	if (!pLocal->IsValidMoveType() || !(SDK::LocalPawn->GetFlags() & FL_ONGROUND))
		return;

	CBaseUserCmdPB* pBaseCmd = cmd->csgoUserCmd.pBaseCmd;
	if (!pBaseCmd)
		return;

	pBaseCmd->SetBits(BASE_BITS_FORWARDMOVE);
	pBaseCmd->SetBits(BASE_BITS_LEFTMOVE);

	pBaseCmd->flSideMove = 0.0f;
	pBaseCmd->flForwardMove = pLocal->m_vecVelocity().Length2D() > 20.0f ? 1.0f : 0.0f;

	QAngle_t angViewAngles = I::Input->GetViewAngles();

	float flYaw = pLocal->m_vecVelocity().ToAngles().y + 180.0f;
	float flRotation = M_DEG2RAD(angViewAngles.y - flYaw);

	float flCosRotation = std::cos(flRotation);
	float flSinRotation = std::sin(flRotation);

	float flNewForwardMove = flCosRotation * pBaseCmd->flForwardMove - flSinRotation * pBaseCmd->flSideMove;
	float flNewSideMove = flSinRotation * pBaseCmd->flForwardMove + flCosRotation * pBaseCmd->flSideMove;

	pBaseCmd->flForwardMove = flNewForwardMove;
	pBaseCmd->flSideMove = -flNewSideMove;
}

bool RAGEBOT::IsAccurate(int nHitchance)
{
	if (nHitchance == 0 || CONVAR::weapon_accuracy_nospread->value.i1)
		return true;

	C_CSWeaponBase* pWeapon = cRagebot.ActiveWeapon;
	if (!pWeapon)
		return false;

	const int nWantedHitCount = 255;
	Vector_t vecForward = Vector_t(0, 0, 0), vecRight = Vector_t(0, 0, 0), vecUp = Vector_t(0, 0, 0);
	BestEntity.vecCalcAimPos.ToDirections(&vecForward, &vecRight, &vecUp);

	const Vector_t vecSource = SDK::LocalPawn->GetEyePosition();
	int nHits = 0;
	const int nNeededHits = static_cast<int>(nWantedHitCount * (nHitchance / 100.f));

	//pWeapon->GetInaccuracy();
	const float flWeaponSpread = pWeapon->GetSpread();
	const float flWeaponInaccuracy = pWeapon->GetInaccuracy();

	TraceFilter_t filter(0x1C3003, SDK::LocalPawn, nullptr, 4);

	for (int i = 0; i < nWantedHitCount; i++)
	{
		float a = MATH::fnRandomFloat(0.f, 1.f);
		float b = MATH::fnRandomFloat(0.f, 2.f * DirectX::XM_PI);
		const float c = MATH::fnRandomFloat(0.f, 1.f);
		const float d = MATH::fnRandomFloat(0.f, 2.f * DirectX::XM_PI);

		const float flInaccuracy = a * flWeaponInaccuracy;
		const float flSpread = c * flWeaponSpread;

		if (cRagebot.ActiveWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() == WEAPON_R8_REVOLVER)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector_t vecSpreadView((cos(b) * flInaccuracy) + (cos(d) * flSpread), (sin(b) * flInaccuracy) + (sin(d) * flSpread), 0), vecDirection = Vector_t(0, 0, 0);

		vecDirection.x = vecForward.x + (vecSpreadView.x * vecRight.x) + (vecSpreadView.y * vecUp.x);
		vecDirection.y = vecForward.y + (vecSpreadView.x * vecRight.y) + (vecSpreadView.y * vecUp.y);
		vecDirection.z = vecForward.z + (vecSpreadView.x * vecRight.z) + (vecSpreadView.y * vecUp.z);
		vecDirection = vecDirection.Normalized();

		QAngle_t angViewSpread{};
		MATH::VectorAnglesVQ(vecDirection, angViewSpread);
		angViewSpread = angViewSpread.Normalize();

		Vector_t vecViewForward;
		angViewSpread.ToDirections(&vecViewForward);
		vecViewForward.NormalizeInPlace();
		vecViewForward = vecSource + (vecViewForward * cRagebot.ActiveWeapon->GetWeaponVData()->m_flRange());

		Ray_t ray = Ray_t();
		GameTrace_t trace = GameTrace_t();
		I::EngineTrace->ClipRayToEntity(&ray, vecSource, vecViewForward, BestEntity.pEntity, &filter, &trace);

		if (trace.m_pHitEntity == BestEntity.pEntity/* && trace.GetHitgroup() == hitgroup <- TODO*/)
			nHits++;

		const int nHitChance = static_cast<int>((static_cast<float>(nHits) / nWantedHitCount) * 100.f);
		if (nHitChance >= nHitchance)
			return true;

		if ((nWantedHitCount - i + nHits) < nNeededHits)
			return false;
	}

	return false;
}

std::vector<Vector_t> Multipoints(Matrix4x2_t* bones, CHitBox* hitbox) {
	std::vector<Vector_t> points{};

	Vector_t center = bones->GetOrigin(HitboxToBone(hitbox->HitBoxIndex()));

	if (g_Config::iMultipoints == 1) {
		points.push_back(center);
	}
	else if (g_Config::iMultipoints == 2) {
		points.push_back(center);

		points.push_back(center + Vector_t{ 0.f, 0.f, hitbox->ShapeRadius() });
		points.push_back(center - Vector_t{ 0.f, 0.f, hitbox->ShapeRadius() });
	}
	else if (g_Config::iMultipoints == 3) {
		points.push_back(center);

		points.push_back(center + Vector_t{ 0.f, 0.f, hitbox->ShapeRadius() });
		points.push_back(center - Vector_t{ 0.f, 0.f, hitbox->ShapeRadius() });

		points.push_back(center + Vector_t{ 0.f, hitbox->ShapeRadius(), 0.f });
		points.push_back(center - Vector_t{ 0.f, hitbox->ShapeRadius(), 0.f });
	}
	else if (g_Config::iMultipoints == 4) {
		points.push_back(center);

		points.push_back(center + Vector_t{ 0.f, 0.f, hitbox->ShapeRadius() });
		points.push_back(center - Vector_t{ 0.f, 0.f, hitbox->ShapeRadius() });

		points.push_back(center + Vector_t{ 0.f, hitbox->ShapeRadius(), 0.f });
		points.push_back(center - Vector_t{ 0.f, hitbox->ShapeRadius(), 0.f });

		points.push_back(center + Vector_t{ hitbox->ShapeRadius(), 0.f, 0.f });
		points.push_back(center - Vector_t{ hitbox->ShapeRadius(), 0.f, 0.f });
	}

	return points;
}

ShootPoint RAGEBOT::GetShootPoint(LagRecord_t* pRecord, CModelState pModel)
{
	Vector_t vecAimPoint = SDK::LocalPawn->GetEyePosition();
	Vector_t vecAngle;
	float flMaxDamage = 0.f;

	for (int hitbox = HEAD; hitbox <= HITBOX_COUNT; ++hitbox)
	{
		if (!(MENU::selectedHitboxes & (1 << hitbox)))
			continue;

		auto points = Multipoints(pRecord->m_Matrix.data(), pModel.modelHandle->GetHitbox(static_cast<HITBOXES>(hitbox)));

		if (points.empty())
			continue;

		for (const auto& point : points)
		{
			float damage = autowall::calculate_damage(pRecord->m_pPawn, cRagebot.ActiveWeapon, point);

			if (damage > flMaxDamage && damage >= g_Config::iMinDamage)
			{
				flMaxDamage = damage;
				vecAngle = point;
				vecAimPoint = point;
			}
		}
	}

	ShootPoint shootPoint;
	shootPoint.flDamage = flMaxDamage;
	shootPoint.vecAimPos = vecAimPoint;
	shootPoint.pRecord = pRecord;

	return shootPoint;
}

bool RAGEBOT::GetEntities()
{
	RageEntities.clear();

	std::vector<RageEntity> TempEntities;
	for (int entity = 0; entity < I::Engine->GetMaxClients(); entity++)
	{
		C_BaseEntity* pEntity = I::GameResourceService->pGameEntitySystem->Get(entity);
		if (!pEntity)
			continue;

		SchemaClassInfoData_t* data = new SchemaClassInfoData_t{};
		pEntity->GetSchemaClassInfo(&data);
		if (!data)
			continue;

		if (std::string(data->szName) != "CCSPlayerController")
			continue;

		CCSPlayerController* pController = static_cast<CCSPlayerController*>(pEntity);
		if (!pController)
			continue;

		C_CSPlayerPawn* pPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pController->GetPawnHandle());
		if (!pPawn)
			continue;

		if (pPawn->GetHealth() <= 0)
			continue;

		if (!pPawn->IsOtherEnemy(SDK::LocalPawn))
			continue;

		if (pPawn->GetTeam() == SDK::LocalPawn->GetTeam())
			continue;

		if (!pController->IsPawnAlive())
			continue;

		if (Lagcomp::m_arrRecords.at(entity).m_deqRecords.empty())
			continue;

		CModelState pModel = pPawn->GetGameSceneNode()->GetSkeletonInstance()->m_modelState();


		LagRecord_t* pFirstRecord = Lagcomp::m_arrRecords.at(entity).GetFirstRecord();
		if (!pFirstRecord)
			continue;

		LagRecord_t* pLastRecord = Lagcomp::m_arrRecords.at(entity).GetLastRecord();
		if (!pLastRecord)
			continue;

		ShootPoint TempShootPoint;

		std::unique_ptr<LagRecord_t> pBackupRecord = std::make_unique<LagRecord_t>(pPawn);

		pFirstRecord->Apply(pPawn);
		ShootPoint FirstAimPos = GetShootPoint(pFirstRecord, pModel);

		pLastRecord->Apply(pPawn);
		ShootPoint LastAimPos = GetShootPoint(pLastRecord, pModel);

		pBackupRecord->Restore(pPawn);

		if (FirstAimPos.flDamage <= 0.0f && LastAimPos.flDamage <= 0.0f)
			continue;

		if (FirstAimPos.flDamage > LastAimPos.flDamage)
			TempShootPoint = FirstAimPos;
		else
			TempShootPoint = LastAimPos;

		RageEntity TempEntity;
		TempEntity.pEntity = pPawn;
		TempEntity.flHealth = pPawn->GetHealth();
		TempEntity.flDamage = TempShootPoint.flDamage;
		TempEntity.vecCalcAimPos = CalcAngle(SDK::LocalPawn->GetEyePosition(), TempShootPoint.vecAimPos);
		TempEntity.vecAimPosNonCalc = TempShootPoint.vecAimPos;
		TempEntity.pRecord = TempShootPoint.pRecord;
		RAGEBOT::RageEntities.push_back(TempEntity);
	}

	return true;
}

void RAGEBOT::CreateMove(C_CSPlayerPawn* pLocal, CCSGOInput* pInput, CUserCmd* pCmd)
{
	if (!g_Config::bRagebot)
		return;

	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;

	if (!SDK::LocalController || !SDK::LocalPawn)
		return;

	if (!pInput || !pCmd)
		return;

	if (!SDK::LocalController->IsPawnAlive())
		return;

	cRagebot.ActiveWeapon = I::GameResourceService->pGameEntitySystem->Get<C_CSWeaponBaseGun>(SDK::LocalPawn->GetWeaponServices()->GetActiveWeapon());

	if (bAutoStop)
		RAGEBOT::AutoStop(pLocal, cRagebot.ActiveWeapon, pCmd);

	bAutoStop = false;

	if (!cRagebot.ActiveWeapon)
		return;

	if (cRagebot.ActiveWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() == WEAPON_ZEUS_X27 || cRagebot.ActiveWeapon->GetAttributeManager()->GetItem()->GetItemDefinitionIndex() == WEAPONTYPE_KNIFE)
		return;

	if (cRagebot.ActiveWeapon->IsKnife())
		return;

	if (!cRagebot.ActiveWeapon->CanPrimaryAttack(cRagebot.ActiveWeapon->GetWeaponVData()->GetWeaponType(), TICKS_TO_TIME(SDK::LocalController->m_nTickBase())))
		return;

	if (!RAGEBOT::GetEntities())
		return;

	if (RageEntities.empty())
		return;

	BestEntity = RageEntities[0];

	for (const auto& pEntity : RageEntities) 
	{
		if (pEntity.flDamage > BestEntity.flDamage) 
		{
			BestEntity = pEntity;
		}
	}

	if (BestEntity.flDamage < g_Config::iMinDamage)
		return;

	if (!BestEntity.pEntity)
		return;

	std::unique_ptr<LagRecord_t> pBackupRecord = std::make_unique<LagRecord_t>(BestEntity.pEntity);
	BestEntity.pRecord->Apply(BestEntity.pEntity);
	const bool bIsAccurate = RAGEBOT::IsAccurate(g_Config::iHitchance);
	pBackupRecord->Restore(BestEntity.pEntity);

	if (!bIsAccurate)
	{
		if (g_Config::bAutoscope && cRagebot.ActiveWeapon->GetWeaponVData()->GetWeaponType() == WEAPONTYPE_SNIPER_RIFLE)
		{
			if (!SDK::LocalPawn->IsScoped() && !SDK::LocalPawn->m_bResumeZoom())
				pCmd->nButtons.nValue |= IN_ZOOM;
		}

		bAutoStop = g_Config::bAutostop;
		RAGEBOT::AutoStop(pLocal, cRagebot.ActiveWeapon, pCmd);

		return;
	}

	if (g_Config::bAutoshoot)
	{
		pCmd->csgoUserCmd.nAttack1StartHistoryIndex = pCmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1;
		pCmd->csgoUserCmd.CheckAndSetBits(PROTOSLOT_5);

		if (BestEntity.pRecord == nullptr)
			return;

		int nTickCount = TIME_TO_TICKS(BestEntity.pRecord->m_flSimulationTime) + 2;

		for (std::int32_t i = 0; i < pCmd->csgoUserCmd.inputHistoryField.nCurrentSize; i++)
		{
			CCSGOInputHistoryEntryPB* iHistory = pCmd->GetInputHistoryEntry(i);
			if (!iHistory)
				continue;

			iHistory->SetBits(PROTOSLOT_10);
			iHistory->nRenderTickCount = nTickCount;

			iHistory->SetBits(PROTOSLOT_12);
			iHistory->nPlayerTickCount = SDK::LocalController->m_nTickBase();

			if (iHistory->cl_interp)
			{
				iHistory->cl_interp->SetBits(PROTOSLOT_1 | PROTOSLOT_2 | PROTOSLOT_3);
				iHistory->cl_interp->nSrcTick = nTickCount;
				iHistory->cl_interp->nDstTick = nTickCount - 1;
			}

			if (iHistory->sv_interp0)
			{
				iHistory->sv_interp0->SetBits(PROTOSLOT_1 | PROTOSLOT_2 | PROTOSLOT_3);
				iHistory->sv_interp0->nSrcTick = nTickCount;
				iHistory->sv_interp0->nDstTick = nTickCount - 1;
			}

			if (iHistory->sv_interp1)
			{
				iHistory->sv_interp1->SetBits(PROTOSLOT_1 | PROTOSLOT_2 | PROTOSLOT_3);
				iHistory->sv_interp1->nSrcTick = nTickCount;
				iHistory->sv_interp1->nDstTick = nTickCount - 1;
			}
		}

		pCmd->nButtons.nValue |= IN_ATTACK;
	}

	pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = BestEntity.vecCalcAimPos;
	pCmd->SetSubTickAngle(BestEntity.vecCalcAimPos);

	if (!g_Config::bHideShots)
		I::Input->SetViewAngle(BestEntity.vecCalcAimPos);
}