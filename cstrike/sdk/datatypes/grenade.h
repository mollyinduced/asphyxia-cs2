#pragma once
#include <cstdint>
#include "../entity.h"

class C_BaseCSGrenadeProjectile : public C_BaseGrenade
{
public:
	// MNetworkEnable
	Vector_t m_vInitialPosition; // 0x10b8	
	// MNetworkEnable
	Vector_t m_vInitialVelocity; // 0x10c4	
	// MNetworkEnable
	int32_t m_nBounces; // 0x10d0	
private:
	[[maybe_unused]] uint8_t __pad10d4[0x4]; // 0x10d4
public:
	// MNetworkEnable
	int32_t m_nExplodeEffectTickBegin; // 0x10e0	
	// MNetworkEnable
	Vector_t m_vecExplodeEffectOrigin; // 0x10e4	
	GameTime_t m_flSpawnTime; // 0x10f0	
	Vector_t vecLastTrailLinePos; // 0x10f4	
	GameTime_t flNextTrailLineTime; // 0x1100	
	bool m_bExplodeEffectBegan; // 0x1104	
	bool m_bCanCreateGrenadeTrail; // 0x1105	
private:
	[[maybe_unused]] uint8_t __pad1106[0x2]; // 0x1106
public:
	CUtlVector< Vector_t > m_arrTrajectoryTrailPoints; // 0x1118	
	CUtlVector< float > m_arrTrajectoryTrailPointCreationTimes; // 0x1130	
	float m_flTrajectoryTrailEffectCreationTime; // 0x1148	
};

class C_SmokeGrenadeProjectile : public C_BaseCSGrenadeProjectile
{
private:
	[[maybe_unused]] uint8_t __pad1150[0x8]; // 0x1150
public:
	// MNetworkEnable
	int32_t m_nSmokeEffectTickBegin; // 0x1158	
	// MNetworkEnable
	bool m_bDidSmokeEffect; // 0x115c	
private:
	[[maybe_unused]] uint8_t __pad115d[0x3]; // 0x115d
public:
	// MNetworkEnable
	int32_t m_nRandomSeed; // 0x1160	
	// MNetworkEnable
	Vector_t m_vSmokeColor; // 0x1164	
	// MNetworkEnable
	Vector_t m_vSmokeDetonationPos; // 0x1170	
private:
	[[maybe_unused]] uint8_t __pad117c[0x4]; // 0x117c
public:
	// MNetworkEnable
	// MNetworkChangeCallback "OnSimulationChanged"
	CUtlVector< uint8_t > m_VoxelFrameData; // 0x1180	
	bool m_bSmokeVolumeDataReceived; // 0x1198	
	bool m_bSmokeEffectSpawned; // 0x1199	
};