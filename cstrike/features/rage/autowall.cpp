#include "autowall.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/igameresourceservice.h"
#include "../../core/sdk.h"
#include "../../common.h"
#include "ragebot.h"

SurfaceData_t* GameTrace_t::GetSurfaceData()
{
	using fnGetSurfaceData = std::uint64_t(__fastcall*)(void*);
	static auto oGetSurfaceData = reinterpret_cast<fnGetSurfaceData>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ? ? ? ? 48 85 C0 74 ? 44 38 60")), 0x1, 0x0));
	return reinterpret_cast<SurfaceData_t*>(oGetSurfaceData(m_pSurface));
}

int GameTrace_t::GetHitboxId()
{
	if (m_pHitboxData)
		return m_pHitboxData->m_nHitboxId;
	return 0;
}

int GameTrace_t::GetHitgroup()
{
	if (m_pHitboxData)
		return m_pHitboxData->m_nHitGroup;
	return 0;
}

bool GameTrace_t::DidHitWorld() const
{
	C_BaseEntity* pWorldEntity = I::GameResourceService->pGameEntitySystem->Get(0);
	if (!pWorldEntity)
		return false;

	return reinterpret_cast<C_BaseEntity*>(m_pHitEntity) == pWorldEntity;
}

TraceFilter_t::TraceFilter_t(std::uint32_t uMask, C_CSPlayerPawn* pSkip1, C_CSPlayerPawn* pSkip2, int nLayer)
{
	m_uTraceMask = uMask;
	m_v1[0] = m_v1[1] = 0;
	m_v2 = 7;
	m_v3 = nLayer;
	m_v4 = 0x49;
	m_v5 = 0;
	m_arrSkipHandles[0] = pSkip1->GetRefEHandle().GetEntryIndex();
	m_arrSkipHandles[1] = pSkip2 ? pSkip2->GetRefEHandle().GetEntryIndex() : 0;
	m_arrSkipHandles[2] = pSkip1->GetOwnerHandle().GetEntryIndex();
	m_arrSkipHandles[3] = pSkip2 ? pSkip2->GetOwnerHandle().GetEntryIndex() : 0;
	m_arrCollisions[0] = pSkip1->GetCollisionMask();
	m_arrCollisions[1] = pSkip2 ? pSkip2->GetCollisionMask() : 0;
}

bool GameTrace_t::IsVisible() const
{
	return (m_flFraction > 0.97f);
}

bool C_EngineTrace::TraceShape(Ray_t* pRay, Vector_t vecStart, Vector_t vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
{
	using fnTraceShape = bool(__fastcall*)(void*, void*, const Vector_t&, const Vector_t&, void*, void*);
	static auto oTraceShape = reinterpret_cast<fnTraceShape>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ? ? ? ? 80 7D ? ? 75 ? F3 0F 10 45")), 0x1, 0x0));

#ifdef CS_PARANOID
	CS_ASSERT(oTraceShape != nullptr);
#endif

	return oTraceShape(this, pRay, vecStart, vecEnd, pFilter, pGameTrace);
}

void C_EngineTrace::ClipTraceToPlayers(Vector_t vecStart, Vector_t vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace, float flUnknown, float flUnknown2, float flUnknown3)
{
	using fnClipTraceToPlayers = void(__fastcall*)(const Vector_t&, const Vector_t&, TraceFilter_t*, GameTrace_t*, float, float, float);
	static auto oClipTraceToPlayers = reinterpret_cast<fnClipTraceToPlayers>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 55 56 48 8D ? ? FF FF FF 48 81 EC")));

#ifdef CS_PARANOID
	CS_ASSERT(oClipTraceToPlayers != nullptr);
#endif

	oClipTraceToPlayers(vecStart, vecEnd, pFilter, pGameTrace, flUnknown, flUnknown2, flUnknown3);
}

void C_EngineTrace::CreateTrace(TraceData_t* const trace, const Vector_t start, const Vector_t end, const TraceFilter_t& filler,
	const int penetration_count) {
	using func_t = void(__fastcall*)(TraceData_t*, Vector_t, Vector_t, TraceFilter_t, void*, void*, void*, void*, int);

	static func_t fn = (func_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 40 F2 0F 10 02"));

	return fn(trace, start, end, filler, nullptr, nullptr, nullptr, nullptr, penetration_count);
}

void C_EngineTrace::InitTraceInfo(GameTrace_t* hit) {
	using func_t = void(__fastcall*)(GameTrace_t*);

	static func_t fn = (func_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 57 48 83 EC ? 48 8B D9 33 FF 48 8B 0D ? ? ? ?"));

	return fn(hit);
}

void C_EngineTrace::GetTraceInfo(TraceData_t* trace, GameTrace_t* hit, float unk_flt, void* unk_ptr) {
	using func_t = void(__fastcall*)(TraceData_t*, GameTrace_t*, float, void*);

	static func_t fn = (func_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC ? 48 8B E9 0F 29 74 24 50"));

	return fn(trace, hit, unk_flt, unk_ptr);
}

void C_EngineTrace::InitFilter(TraceFilter_t& filter, C_CSPlayerPawn* skip, uint64_t mask, uint8_t layer, uint16_t idk)
{
	using func_t = TraceFilter_t * (__fastcall*)(TraceFilter_t&, void*, uint64_t, uint8_t, uint16_t);

	static func_t fn = (func_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 0F B6 41 37 33 F6"));

	fn(filter, skip, mask, layer, idk);
}

bool C_EngineTrace::ClipRayToEntity(Ray_t* ray, const Vector_t& start, const Vector_t& end, C_CSPlayerPawn* player, TraceFilter_t* filter, GameTrace_t* trace) {
	using function_t = bool(__fastcall*)(C_EngineTrace*, Ray_t*, const Vector_t&, const Vector_t&, C_CSPlayerPawn*, TraceFilter_t*, GameTrace_t*);
	static function_t fn = (function_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 56 41 57 48 81 EC C0 00 00 00 48 8B 9C"));
	return fn(this, ray, start, end, player, filter, trace);
}

bool handle_bullet_penetration(TraceData_t* trace, void* stats, UpdateValue_t* const mod_value, bool draw_showimpacts = false) {
	using fn_t = bool(__fastcall*)(TraceData_t*, void*, UpdateValue_t*, void*, void*, void*, void*, void*, bool);
	static fn_t fn = (fn_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 44 89 48 20 55 57 41 55"));
	return fn(trace, stats, mod_value, nullptr, nullptr, nullptr, nullptr, nullptr, draw_showimpacts);
}

float autowall::GetHitgroupDamageMultiplier(float HeadshotMultiplier, int Hitgroup)
{
	float Multiplier = 1.F;
	switch (Hitgroup)
	{
	case HITGROUP_HEAD:
		Multiplier = HeadshotMultiplier;
		break;

	case HITGROUP_STOMACH:
		Multiplier = 1.25F;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		Multiplier = 0.75F;
		break;
	}
	return Multiplier;
}

int autowall::ScaleDamage(int curDmg, int hitgroup, C_CSPlayerPawn* ent, CCSWeaponBaseVData* wpData) {
	curDmg *= GetHitgroupDamageMultiplier(wpData->m_flHeadshotMultiplier(), hitgroup);

	if (!ent) {
		return curDmg;
	}

	if (ent->GetArmorValue() > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (ent->GetItemServices()->m_bHasHelmet())
			{
				curDmg *= (wpData->m_flArmorRatio() * 0.5F);
			}
		}
		else
		{
			curDmg *= (wpData->m_flArmorRatio() * 0.5F);
		}
	}

	return curDmg;
}

int autowall::firebullet(Vector_t Start, Vector_t End, C_CSPlayerPawn* lp, C_CSPlayerPawn* target, CCSWeaponBaseVData* wpData) {
	int damage = wpData->m_nDamage();

	TraceData_t trace_data{};
	trace_data.m_arr_pointer = &trace_data.m_arr;

	const Vector_t direction = End - Start;
	const Vector_t end_pos = direction * wpData->GetRange();

	TraceFilter_t filter(0x1C300Bu, lp, nullptr, 3);

	I::EngineTrace->CreateTrace(&trace_data, Start, end_pos, filter, 4);

	struct handle_bullet_data_t {
		handle_bullet_data_t(const float dmg_mod, const float pen, const float range_mod, const float range,
			const int pen_count, const bool failed) :
			m_dmg(dmg_mod),
			m_pen(pen),
			m_range_mod(range_mod),
			m_range(range),
			m_pen_count(pen_count),
			m_failed(failed) {}

		float m_dmg{ }, m_pen{ }, m_range_mod{ }, m_range{ };
		int m_pen_count{ };
		bool m_failed{ };
	} handle_bullet_data(static_cast<float>(wpData->m_nDamage()),
		wpData->m_flPenetration(), wpData->GetRange(),
		wpData->m_flRangeModifier(), 4, false);

	if (trace_data.m_num_update > 0) {
		for (int i{ }; i < trace_data.m_num_update; i++) {
			UpdateValue_t* value = reinterpret_cast<UpdateValue_t* const>(
				reinterpret_cast<std::uintptr_t>(trace_data.m_pointer_update_value)
				+ i * sizeof(UpdateValue_t));
			GameTrace_t game_trace{ };

			I::EngineTrace->InitTraceInfo(&game_trace);

			I::EngineTrace->GetTraceInfo(&trace_data, &game_trace, 0.f,
				reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(trace_data.m_arr.data()) + sizeof(TraceArrElement_t) * (value->m_handle_idx & 0x7fff)));

			if (game_trace.m_pHitEntity == target) 
			{
				if (damage > 0.0f) // huh?
					damage = ScaleDamage(damage, game_trace.m_pHitboxData->m_nHitGroup, (C_CSPlayerPawn*)game_trace.m_pHitEntity, wpData);
				return damage;
			}

			if (handle_bullet_penetration(&trace_data, &handle_bullet_data, value, false))
				return 0;

			damage = handle_bullet_data.m_dmg;
		}
	}

	return 0;
}

float autowall::calculate_damage(C_CSPlayerPawn* target, C_CSWeaponBaseGun* gun, Vector_t targetPos) {
	float dmg = firebullet(SDK::LocalPawn->GetEyePosition(), targetPos, SDK::LocalPawn, target, gun->GetWeaponVData());

	static bool penetrateWalls = true;
	if (!penetrateWalls) {
		Ray_t ray{};
		GameTrace_t trace{};
		TraceFilter_t filter{ 0x1C3003, SDK::LocalPawn, NULL, 4 };

		if (!I::EngineTrace->TraceShape(&ray, SDK::LocalPawn->GetEyePosition(), targetPos, &filter, &trace))
			dmg = 0.f;

		if (trace.m_pHitEntity != target)
			dmg = 0.f;
	}

	return dmg;
}