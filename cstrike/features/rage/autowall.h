#pragma once
#include "../../sdk/entity.h"
#include <array>
#include "../../sdk/datatypes/vector.h"

struct Ray_t
{
    Vector_t vecStart;
    Vector_t vecEnd;
    Vector_t vecMins;
    Vector_t vecMaxs;
    char _pad[0x4];
    std::uint8_t UnkType;
};

struct SurfaceData_t
{
    char _pad1[0x8];
    float m_flPenetrationModifier;
    float m_flDamageModifier;
    char _pad[0x4];
    int m_iMaterial;
};

struct TraceHitboxData_t
{
    char _pad1[0x38];
    int m_nHitGroup;
    char _pad2[0x4];
    int m_nHitboxId;
};

struct GameTrace_t
{
    GameTrace_t() = default;
    SurfaceData_t* GetSurfaceData();

    int GetHitboxId();
    int GetHitgroup();
    bool DidHitWorld() const;
    bool IsVisible() const;

    void* m_pSurface;
    C_BaseEntity* m_pHitEntity;
    TraceHitboxData_t* m_pHitboxData;
    MEM_PAD(0x38);
    std::uint32_t m_uContents;
    MEM_PAD(0x24);
    Vector_t m_vecStartPos;
    Vector_t m_vecEndPos;
    Vector_t m_vecNormal;
    Vector_t m_vecPosition;
    MEM_PAD(0x4);
    float m_flFraction;
    MEM_PAD(0x6);
    bool m_bAllSolid;
    MEM_PAD(0x4D);
}; // Size: 0xB8

static_assert(sizeof(GameTrace_t) == 0x108);


struct TraceFilter_t
{
    char _pad[0x8];
    std::int64_t m_uTraceMask;
    std::array<std::int64_t, 2> m_v1;
    std::array<std::int32_t, 4> m_arrSkipHandles;
    std::array<std::int16_t, 2> m_arrCollisions;
    std::int16_t m_v2;
    std::uint8_t m_v3;
    std::uint8_t m_v4;
    std::uint8_t m_v5;

    TraceFilter_t() = default;
    TraceFilter_t(std::uint32_t uMask, C_CSPlayerPawn* pSkip1, C_CSPlayerPawn* pSkip2, int nLayer);
};

struct UpdateValue_t {
    float m_previous_length_mod{ }, m_current_length_mod{ };
    MEM_PAD(0x8)
        std::int16_t m_handle_idx{ };
    MEM_PAD(0x6)
};

struct TraceArrElement_t {
    MEM_PAD(0x30)
};

struct TraceData_t {
    std::int32_t m_uk1{ };
    float m_uk2{ 52.0f };
    void* m_arr_pointer{ };
    std::int32_t m_uk3{ 128 };
    std::int32_t m_uk4{ static_cast<std::int32_t>(0x80000000) };
    std::array< TraceArrElement_t, 0x80 > m_arr = { };
    MEM_PAD(0x8)
        std::int64_t m_num_update{ };
    void* m_pointer_update_value{ };
    MEM_PAD(0xC8)
        Vector_t m_start{ }, m_end{ };
    MEM_PAD(0x50)
};

class C_EngineTrace
{
public:
    bool TraceShape(Ray_t* pRay, Vector_t vecStart, Vector_t vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace);

    void ClipTraceToPlayers(Vector_t vecStart, Vector_t vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace, float flUnknown, float flUnknown2, float flUnknown3);

    void CreateTrace(TraceData_t* const trace, const Vector_t start, const Vector_t end, const TraceFilter_t& filler, const int penetration_count);

    void InitTraceInfo(GameTrace_t* hit);

    void GetTraceInfo(TraceData_t* trace, GameTrace_t* hit, float unk_flt, void* unk_ptr);

    void InitFilter(TraceFilter_t& filter, C_CSPlayerPawn* skip, uint64_t mask, uint8_t layer, uint16_t unk);

    bool ClipRayToEntity(Ray_t* ray, const Vector_t& start, const Vector_t& end, C_CSPlayerPawn* player, TraceFilter_t* filter, GameTrace_t* trace);
};

namespace autowall {
	float GetHitgroupDamageMultiplier(float HeadshotMultiplier, int Hitgroup);

	int ScaleDamage(int curDmg, int hitgroup, C_CSPlayerPawn* ent, CCSWeaponBaseVData* wpData);

	int firebullet(Vector_t Start, Vector_t End, C_CSPlayerPawn* lp, C_CSPlayerPawn* target, CCSWeaponBaseVData* wpData);

	float calculate_damage(C_CSPlayerPawn* target, C_CSWeaponBaseGun* gun, Vector_t targetPos);
}