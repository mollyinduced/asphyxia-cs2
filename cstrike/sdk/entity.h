#pragma once

#include "interfaces/ischemasystem.h"
#include "datatypes/utlvector.h"
#include "datatypes/stronghandle.h"

// used: schema field
#include "../core/schema.h"
//#include "../core/interfaces.h"

// used: l_print
#include "../utilities/log.h"
// used: vector_t
#include "datatypes/vector.h"
// used: qangle_t
#include "datatypes/qangle.h"
// used: ctransformb
#include "datatypes/transform.h"
#include "datatypes/bullet.h"

// used: cbasehandle
#include "entity_handle.h"
// used: game's definitions
#include "const.h"
// used: entity vdata
#include "vdata.h"


using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;

enum BONEINDEX : int
{
	head = 6,
	neck_0 = 5,
	spine_1 = 4,
	spine_2 = 2,
	pelvis = 0,
	arm_upper_L = 8,
	arm_lower_L = 9,
	hand_L = 10,
	arm_upper_R = 13,
	arm_lower_R = 14,
	hand_R = 15,
	leg_upper_L = 22,
	leg_lower_L = 23,
	ankle_L = 24,
	leg_upper_R = 25,
	leg_lower_R = 26,
	ankle_R = 27
};

enum HITBOXES
{
	HEAD = 0,
	NECK = 1,
	PELVIS = 2,
	SPINE_0 = 3,
	SPINE_1 = 4,
	SPINE_2 = 5,
	SPINE_3 = 6,
	LEG_UPPER_L = 7,
	LEG_UPPER_R = 8,
	LEG_LOWER_L = 9,
	LEG_LOWER_R = 10,
	ANKLE_L = 11,
	ANKLE_R = 12,
	HAND_L = 13,
	HAND_R = 14,
	ARM_UPPER_L = 15,
	ARM_LOWER_L = 16,
	ARM_UPPER_R = 17,
	ARM_LOWER_R = 18,
	HITBOX_COUNT
};

enum e_hitgroups : std::uint32_t
{
	HITGROUP_GENERIC,
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_STOMACH,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG,
	HITGROUP_NECK,
	HITGROUP_GEAR = 10U
};

class CNetworkedQuantizedFloat
{
public:
	std::float_t m_Value;
	std::uint16_t m_nEncoder;
	bool m_bUnflattened;
	MEM_PAD(0x1);
};

class CNetworkVelocityVector
{
public:
	Vector_t m_vecValue;					// 0x0000
	CNetworkedQuantizedFloat m_vecX;	// 0x0010
	CNetworkedQuantizedFloat m_vecY;	// 0x0018
	CNetworkedQuantizedFloat m_vecZ;	// 0x0020
};

class CNetworkViewOffsetVector
{
public:
	Vector_t m_vecValue;					// 0x0000
	CNetworkedQuantizedFloat m_vecX;	// 0x0010
	CNetworkedQuantizedFloat m_vecY;	// 0x0018
	CNetworkedQuantizedFloat m_vecZ;	// 0x0020
};

class CNetworkOriginQuantizedVector
{
public:
	Vector_t m_vecValue;					// 0x0000
	CNetworkedQuantizedFloat m_vecX;	// 0x0010
	CNetworkedQuantizedFloat m_vecY;	// 0x0018
	CNetworkedQuantizedFloat m_vecZ;	// 0x0020
};

class CNetworkOriginCellCoordQuantizedVector
{
public:
	Vector_t m_vecValue;					// 0x0000
	std::uint16_t m_cellX;						// 0x0010
	std::uint16_t m_cellY;						// 0x0012
	std::uint16_t m_cellZ;						// 0x0014
	std::uint16_t m_nOutsideWorld;				// 0x0016
	CNetworkedQuantizedFloat m_vecX;	// 0x0018
	CNetworkedQuantizedFloat m_vecY;	// 0x0020
	CNetworkedQuantizedFloat m_vecZ;	// 0x0028
};

class CEntityInstance;

class CEntityIdentity
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityIdentity);

	// @note: handle index is not entity index
	SCHEMA_ADD_OFFSET(std::uint32_t, GetIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, GetDesignerName, "CEntityIdentity->m_designerName");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "CEntityIdentity->m_flags");

	[[nodiscard]] bool IsValid()
	{
		return GetIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;

		return GetIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		return GetIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* pInstance; // 0x00
};

class CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityInstance);

	void GetSchemaClassInfo(SchemaClassInfoData_t** pReturn)
	{
		return MEM::CallVFunc<void, 38U>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = GetIdentity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->GetFlags() & 1));
	}

	SCHEMA_ADD_FIELD(CEntityIdentity*, GetIdentity, "CEntityInstance->m_pEntity");
};

class CCollisionProperty
{
public:
	CS_CLASS_NO_INITIALIZER(CCollisionProperty);

	SCHEMA_ADD_FIELD(Vector_t, GetMins, "CCollisionProperty->m_vecMins");
	SCHEMA_ADD_FIELD(Vector_t, GetMaxs, "CCollisionProperty->m_vecMaxs");

	std::uint16_t CollisionMask() {
		return *reinterpret_cast<std::uint16_t*>((uintptr_t)(this) + 0x38);
	}
};

class CSkeletonInstance;

struct BoneData_t
{
	Vector_t vecPosition;
	float flScale;
	Vector4D_t vecRotation;
};

class CHitBox {
public:
	const char* Name() {
		return *(const char**)(this + 0x0);
	}

	Vector_t& VectorMin() {
		return *(Vector_t*)(this + 0x0020);
	}

	Vector_t& VectorMax() {
		return *(Vector_t*)(this + 0x002C);
	}
	float ShapeRadius() {
		return *(float*)(this + 0x30);
	}

	uint8_t ShapeType() {
		return *(uint8_t*)(this + 0x3C);
	}

	uint16_t HitBoxIndex() {
		return *(uint16_t*)(this + 0x48);
	}
};

class c_drawcalls
{
public:
	__int32 m_nPrimitiveType; // 0x0000
	__int32 m_nBaseVertex;	  // 0x0004
	__int32 m_nVertexCount;	  // 0x0008
	__int32 m_nStartIndex;	  // 0x000C
	__int32 m_nIndexCount;	  // 0x0010
	float	m_flUvDensity;	  // 0x0014
	float	m_vTintColor[3];  // 0x0018
	float	m_flAlpha;		  // 0x0024
	char	pad_0x0028[0xC0]; // 0x0028

}; // Size=0x00E8

class c_attachments
{
public:
	char		pad_0x0000[0x10];		  // 0x0000
	const char* m_key;					  // 0x0010
	char		pad_0x0018[0x8];		  // 0x0018
	const char* m_name;					  // 0x0020
	const char* m_influenceNames[3];	  // 0x0028
	char		pad_0x0030[0x10];		  // 0x0030
	Vector4D_t		m_vInfluenceRotations[3]; // 0x0040
	Vector_t		m_vInfluenceOffsets[3];	  // 0x0070
	float		m_influenceWeights[3];	  // 0x0094
	char		pad_0x00A0[0x10];		  // 0x00A0
};

class c_bones
{
public:
	const char* m_boneName;		   // 0x0000
	const char* m_parentName;	   // 0x0008
	float		m_invBindPose[12]; // 0x0010
	Vector_t		m_vecCenter;	   // 0x0040
	Vector_t		m_vecSize;		   // 0x004C
	float		m_flSphereradius;  // 0x0058
	char		pad_0x005C[0x4];   // 0x005C
};

class c_hitboxsets
{
public:
	char	  pad_0x0000[0x20]; // 0x0000
	uint32_t  m_nNameHash;		// 0x0020
	char	  pad_0x0024[0x4];	// 0x0024
	__int32	  m_nHitboxCount;	// 0x0028
	char	  pad_0x002C[0x4];	// 0x002C
	CHitBox* m_hitbox;			// 0x0030
	char	  pad_0x0038[0x18]; // 0x0038
};

class c_rendermesh
{
public:
	char		   pad_0x0000[0x28];	// 0x0000
	Vector_t		   m_vMinBounds;		// 0x0028
	Vector_t		   m_vMaxBounds;		// 0x0034
	__int32		   m_drawCallsCount;	// 0x0040
	char		   pad_0x0044[0x4];		// 0x0044
	c_drawcalls* m_drawCalls;			// 0x0048
	char		   pad_0x0050[0x68];	// 0x0050
	__int32		   m_skeletoncount;		// 0x00B8
	char		   pad_0x00BC[0x4];		// 0x00BC
	BoneData_t* m_skeleton;			// 0x00C0
	char		   pad_0x00C8[0x8];		// 0x00C8
	__int32		   m_hashescount;		// 0x00D0
	char		   pad_0x00D4[0x4];		// 0x00D4
	uint32_t* m_hashes;			// 0x00D8
	char		   pad_0x00E0[0x20];	// 0x00E0
	__int32		   m_nBoneWeightCount;	// 0x0100
	char		   pad_0x0104[0xC];		// 0x0104
	c_attachments* m_attachments;		// 0x0110
	__int32		   m_attachments_count; // 0x0118
	char		   pad_0x011C[0x1C];	// 0x011C
	c_hitboxsets* m_hitboxsets;		// 0x0138
	__int32		   m_nHitboxSets;		// 0x0140
};

class c_rendermeshes
{
public:
	c_rendermesh* m_meshes;
};

class c_physmodel_bonenames
{
public:
	uint32_t	m_boneNames_count;
	char		padd[0x4];
	const char* m_boneNames[];
};

class c_physmodel
{
public:
	char				   padd[0x10];
	c_physmodel_bonenames* m_boneName;
	uint32_t* m_boneHash;
	uint32_t			   m_indexNames_count;
	char				   padd2[0x4];
	uint16_t* m_indexNames;
	uint32_t			   m_indexHash_count;
	char				   padd3[0x4];
	uint16_t* m_indexHash;
};

class CModel
{
public:
	CHitBox* GetHitbox(int index)
	{
		if (m_rendermesh_count <= 0)
			return nullptr;

		if (!m_rendermesh)
			return nullptr;

		c_rendermesh* meshes = m_rendermesh->m_meshes;
		if (!meshes)
			return nullptr;

		c_rendermesh mesh = meshes[0];

		c_hitboxsets* hithoxsets = mesh.m_hitboxsets;
		if (!hithoxsets)
			return nullptr;

		c_hitboxsets hithoxset = hithoxsets[0];
		if (hithoxset.m_nHitboxCount <= 0 || index > hithoxset.m_nHitboxCount)
			return nullptr;

		CHitBox* hitbox = hithoxset.m_hitbox;
		if (!hitbox)
			return nullptr;

		CHitBox* hbx0 = &hitbox[0];

		return hbx0 + 0x70 * index;
	}

	int HitboxCount()
	{
		if (m_rendermesh_count <= 0)
			return 0;

		if (!m_rendermesh)
			return 0;

		c_rendermesh* meshes = m_rendermesh->m_meshes;
		if (!meshes)
			return 0;

		c_rendermesh mesh = meshes[0];

		c_hitboxsets* hithoxsets = mesh.m_hitboxsets;
		if (!hithoxsets)
			return 0;

		c_hitboxsets hithoxset = hithoxsets[0];
		if (hithoxset.m_nHitboxCount <= 0)
			return 0;

		return hithoxset.m_nHitboxCount;
	}

private:
	char			pad_0x0000[0x8];				// 0x0000
	const char* m_name;							// 0x0008
	char			pad_0x0010[0x4];				// 0x0010
	__int32			m_nFlags;						// 0x0014
	Vector_t			m_vHullMin;						// 0x0018
	Vector_t			m_vHullMax;						// 0x0024
	Vector_t			m_vViewMin;						// 0x0030
	Vector_t			m_vViewMax;						// 0x003C
	char			pad_0x0048[0x28];				// 0x0048
	__int32			m_rendermesh_count;				// 0x0070
	char			pad_0x0074[0x4];				// 0x0074
	c_rendermeshes* m_rendermesh;					// 0x0078
	char			pad_0x0080[0x50];				// 0x0080
	c_physmodel* m_physmodel;					// 0x00D0
	__int32			m_animationgroupresource_count; // 0x00D8
	char			pad_0x00DC[0x4];				// 0x00DC
	void* m_animationgroupresource;		// 0x00E0
	char			pad_0x00E8[0x8];				// 0x00E8
	__int32			m_meshes_count;					// 0x00F0
	char			pad_0x00F4[0x4];				// 0x00F4
	void* m_meshes;						// 0x00F8
	char			pad_0x0100[0x28];				// 0x0100
	void* N00000135;						// 0x0128
	char			pad_0x0130[0x28];				// 0x0130
	__int32			m_names_count;					// 0x0158
	char			pad_0x015C[0x4];				// 0x015C
	const char** m_names;						// 0x0160
	char			pad_0x0168[0x8];				// 0x0168
	__int32			m_elementindexcount;			// 0x0170
	char			pad_0x0174[0x4];				// 0x0174
	uint16_t* m_elementindex;					// 0x0178
	char			pad_0x0180[0x8];				// 0x0180
	__int32			N00000141;						// 0x0188
	char			pad_0x018C[0x4];				// 0x018C
	void* N00000142;						// 0x0190
	char			pad_0x0198[0x8];				// 0x0198
	__int32			N00000144;						// 0x01A0
	char			pad_0x01A4[0x4];				// 0x01A4
	void* N00000145;						// 0x01A8
	char			pad_0x01B0[0x8];				// 0x01B0
	__int32			m_pos_count;					// 0x01B8
	char			pad_0x01BC[0x4];				// 0x01BC
	Vector_t* m_pos;							// 0x01C0
	char			pad_0x01C8[0x8];				// 0x01C8
	__int32			m_quat_count;					// 0x01D0
	char			pad_0x01D4[0x4];				// 0x01D4
	Vector4D_t* m_quat;							// 0x01D8
	char			pad_0x01E0[0x8];				// 0x01E0
	__int32			m_scale_count;					// 0x01E8
	char			pad_0x01EC[0x4];				// 0x01EC
	float* m_scale;						// 0x01F0
	char			pad_0x01F8[0x8];				// 0x01F8
	__int32			N00000150;						// 0x0200
	char			pad_0x0204[0x4];				// 0x0204
	void* N00000151;						// 0x0208
	char			pad_0x0210[0x8];				// 0x0210
	__int32			m_somecount2;					// 0x0218
	char			pad_0x021C[0x4];				// 0x021C
	uint16_t* m_somearray2;						// 0x0220
	char			pad_0x0228[0x8];				// 0x0228
	__int32			m_somecount1;					// 0x0230
	char			pad_0x0234[0x4];				// 0x0234
	uint16_t* m_somearray1;					// 0x0238
	char			pad_0x0240[0x20];				// 0x0240
	__int32			N0000015C;						// 0x0260
	char			pad_0x0264[0x4];				// 0x0264
	void* N0000015D;						// 0x0268
	char			pad_0x0270[0x8];				// 0x0270
	__int32			N0000015F;						// 0x0278
	char			pad_0x027C[0x4];				// 0x027C
	void* N00000160;						// 0x0280
	char			pad_0x0288[0x40];				// 0x0288
	__int32			N00000169;						// 0x02C8
	char			pad_0x02CC[0x4];				// 0x02CC
	void* N0000016A;						// 0x02D0
	char			pad_0x02D8[0x8];				// 0x02D8
	__int32			m_somecount3;					// 0x02E0
	char			pad_0x02E4[0x4];				// 0x02E4
	uint16_t* m_somearray3;					// 0x02E8
	char			pad_0x02F0[0x58];				// 0x02F0
	void* N00000179;						// 0x0348
	char			pad_0x0350[0x8];				// 0x0350
	__int32			N0000017B;						// 0x0358
	char			pad_0x035C[0x4];				// 0x035C
	void* N0000017C;						// 0x0360
	char			pad_0x0368[0x70];				// 0x0368
	__int32			N0000018B;						// 0x03D8
	char			pad_0x03DC[0x4];				// 0x03DC
	void* N0000018C;						// 0x03E0
	char			pad_0x03E8[0x30];				// 0x03E8
	__int32			N00000193;						// 0x0418
	char			pad_0x041C[0x4];				// 0x041C
	void* N00000194;						// 0x0420
	char			pad_0x0428[0x5D8];				// 0x0428
};


class CModelState
{
public:
	std::uint8_t padding_0[0x80];
	BoneData_t* bones;
	std::uint8_t padding_1[0x18];
	CStrongHandle<CModel> modelHandle;
};

class CGameSceneNode
{
public:
	CS_CLASS_NO_INITIALIZER(CGameSceneNode);

	SCHEMA_ADD_FIELD(CTransform, GetNodeToWorld, "CGameSceneNode->m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, GetOwner, "CGameSceneNode->m_pOwner");

	SCHEMA_ADD_FIELD(Vector_t, GetAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector_t, GetRenderOrigin, "CGameSceneNode->m_vRenderOrigin");
	SCHEMA_ADD_FIELD(Vector_t, m_vecOrigin, "CGameSceneNode->m_vecOrigin");

	SCHEMA_ADD_FIELD(QAngle_t, GetAngleRotation, "CGameSceneNode->m_angRotation");
	SCHEMA_ADD_FIELD(QAngle_t, GetAbsAngleRotation, "CGameSceneNode->m_angAbsRotation");

	SCHEMA_ADD_FIELD(bool, IsDormant, "CGameSceneNode->m_bDormant");


	CSkeletonInstance* GetSkeletonInstance()
	{
		return MEM::CallVFunc<CSkeletonInstance*, 8U>(this);
	}

	Vector_t GetBonePosition(const std::int32_t eBoneIndex);
};

class CSkeletonInstance : public CGameSceneNode
{
public:
public:
	MEM_PAD(0x1CC); //0x0000
	int m_nBones; //0x01BC 
	MEM_PAD(0x18); //0x01C0
	int m_nMask; //0x01D8 
	MEM_PAD(0x4); //0x01DC
	Matrix3x4_t* m_pBoneCache;

	SCHEMA_ADD_FIELD(CModelState, m_modelState, "CSkeletonInstance->m_modelState");
	SCHEMA_ADD_FIELD(std::uint8_t, m_nHitboxSet, "CSkeletonInstance->m_nHitboxSet");

	void CalcWorldSpaceBones(int bone_mask)
	{
		using fn_t = void(__fastcall*)(CSkeletonInstance*, int);
		static fn_t fn = (fn_t)MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 4C 8B AF ? ? ? ?"), 0x1); // 40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC D0
		return fn(this, bone_mask);
	}
};

inline Vector_t CGameSceneNode::GetBonePosition(const std::int32_t eBoneIndex)
{
	CSkeletonInstance* pSkeleton = GetSkeletonInstance();
	if (pSkeleton == nullptr)
		return Vector_t(0, 0, 0);

	const CModelState modelState = pSkeleton->m_modelState();
	const BoneData_t* pBoneArray = modelState.bones;
	if (pBoneArray == nullptr)
		return Vector_t(0, 0, 0);

	return pBoneArray[eBoneIndex].vecPosition;
}

class C_BaseEntity : public CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseEntity);

	[[nodiscard]] bool IsBasePlayerController()
	{
		SchemaClassInfoData_t* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerController");
	}

	[[nodiscard]] bool IsWeapon()
	{
		static SchemaClassInfoData_t* pWeaponBaseClass = nullptr;
		if (pWeaponBaseClass == nullptr)
			I::SchemaSystem->FindTypeScopeForModule(CS_XOR("client.dll"))->FindDeclaredClass(&pWeaponBaseClass, CS_XOR("C_CSWeaponBase"));

		SchemaClassInfoData_t* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return (pClassInfo->InheritsFrom(pWeaponBaseClass));
	}

	[[nodiscard]] bool IsValidMoveType() noexcept
	{
		if (!this)
			return false;

		const auto move_type = this->GetActualMoveType();
		return move_type != MOVETYPE_NOCLIP && move_type != MOVETYPE_LADDER;
	}

	void SetAbsOrigin(const Vector_t& vecOrigin)
	{
		using fnSetAbsOrigin = void(__fastcall*)(C_BaseEntity*, Vector_t);
		static const auto oSetAbsOrigin = reinterpret_cast<fnSetAbsOrigin>(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 57 48 83 EC 40 48 8B 01 48 8B FA")));

		#ifdef CS_PARANOID
		CS_ASSERT(oSetAbsOrigin != nullptr);
		#endif

		return oSetAbsOrigin(this, vecOrigin);
	}

	static C_BaseEntity* GetLocalPlayer();

	// get entity origin on scene
	[[nodiscard]] const Vector_t& GetSceneOrigin();

	SCHEMA_ADD_FIELD(CGameSceneNode*, GetGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, GetCollision, "C_BaseEntity->m_pCollision");
	SCHEMA_ADD_FIELD(std::int32_t, GetTeam, "C_BaseEntity->m_iTeamNum");
	SCHEMA_ADD_FIELD(CBaseHandle, GetOwnerHandle, "C_BaseEntity->m_hOwnerEntity");
	SCHEMA_ADD_FIELD(Vector_t, GetBaseVelocity, "C_BaseEntity->m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector_t, GetAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(Vector_t, m_vecVelocity, "C_BaseEntity->m_vecVelocity");
	SCHEMA_ADD_FIELD(bool, IsTakingDamage, "C_BaseEntity->m_bTakesDamage");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "C_BaseEntity->m_fFlags");
	SCHEMA_ADD_FIELD(std::int32_t, GetEflags, "C_BaseEntity->m_iEFlags");
	SCHEMA_ADD_FIELD(std::int32_t, GetMoveType, "C_BaseEntity->m_MoveType");
	SCHEMA_ADD_FIELD(std::int32_t, GetActualMoveType, "C_BaseEntity->m_nActualMoveType");
	SCHEMA_ADD_FIELD(std::uint8_t, GetLifeState, "C_BaseEntity->m_lifeState");
	SCHEMA_ADD_FIELD(std::int32_t, GetHealth, "C_BaseEntity->m_iHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetMaxHealth, "C_BaseEntity->m_iMaxHealth");
	SCHEMA_ADD_FIELD(float, GetWaterLevel, "C_BaseEntity->m_flWaterLevel");
	SCHEMA_ADD_FIELD_OFFSET(void*, GetVData, "C_BaseEntity->m_nSubclassID", 0x8);
	SCHEMA_ADD_FIELD(float, m_flSimulationTime, "C_BaseEntity->m_flSimulationTime");
};


class CGlowProperty {
public:
	CS_CLASS_NO_INITIALIZER(CGlowProperty);

	SCHEMA_ADD_FIELD(Vector_t, m_fGlowColor, "CGlowProperty->m_fGlowColor");
	SCHEMA_ADD_FIELD(int, m_iGlowType, "CGlowProperty->m_iGlowType");
	SCHEMA_ADD_FIELD(int, m_iGlowTeam, "CGlowProperty->m_iGlowTeam");
	SCHEMA_ADD_FIELD(int, m_nGlowRange, "CGlowProperty->m_nGlowRange");
	SCHEMA_ADD_FIELD(int, m_nGlowRangeMin, "CGlowProperty->m_nGlowRangeMin");
	SCHEMA_ADD_FIELD(Color_t, m_glowColorOverride, "CGlowProperty->m_glowColorOverride");
	SCHEMA_ADD_FIELD(bool, m_bFlashing, "CGlowProperty->m_bFlashing");
	SCHEMA_ADD_FIELD(float, m_flGlowTime, "CGlowProperty->m_flGlowTime");
	SCHEMA_ADD_FIELD(float, m_flGlowStartTime, "CGlowProperty->m_flGlowStartTime");
	SCHEMA_ADD_FIELD(bool, m_bEligibleForScreenHighlight, "CGlowProperty->m_bEligibleForScreenHighlight");
	SCHEMA_ADD_FIELD(bool, m_bGlowing, "CGlowProperty->m_bGlowing");

	C_BaseEntity* Owner() {
		return *(C_BaseEntity**)(this + 0x18);
	}
};

class C_BaseModelEntity : public C_BaseEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseModelEntity);

	SCHEMA_ADD_FIELD(CCollisionProperty, GetCollisionInstance, "C_BaseModelEntity->m_Collision");
	SCHEMA_ADD_FIELD(CGlowProperty, GetGlowProperty, "C_BaseModelEntity->m_Glow");
	SCHEMA_ADD_FIELD(Vector_t, GetViewOffset, "C_BaseModelEntity->m_vecViewOffset");
	SCHEMA_ADD_FIELD(GameTime_t, GetCreationTime, "C_BaseModelEntity->m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, GetCreationTick, "C_BaseModelEntity->m_nCreationTick");
	SCHEMA_ADD_FIELD(CBaseHandle, GetMoveParent, "C_BaseModelEntity->m_hOldMoveParent");
	SCHEMA_ADD_FIELD(std::float_t, GetAnimTime, "C_BaseModelEntity->m_flAnimTime");
};

class CPlayer_ItemServices {
public:
	CS_CLASS_NO_INITIALIZER(CPlayer_ItemServices);

	SCHEMA_ADD_FIELD(bool, m_bHasDefuser, "CCSPlayer_ItemServices->m_bHasDefuser");
	SCHEMA_ADD_FIELD(bool, m_bHasHelmet, "CCSPlayer_ItemServices->m_bHasHelmet");
	SCHEMA_ADD_FIELD(bool, m_bHasHeavyArmor, "CCSPlayer_ItemServices->m_bHasHeavyArmor");
};
class CPlayer_CameraServices;
class CUserCmd;
class CPlayer_MovementServices
{
public:
	CS_CLASS_NO_INITIALIZER(CPlayer_MovementServices);

	SCHEMA_ADD_FIELD(int32_t, m_nImpulse, "CPlayer_MovementServices->m_nImpulse");
	SCHEMA_ADD_FIELD(uint64_t, m_nQueuedButtonDownMask, "CPlayer_MovementServices->m_nQueuedButtonDownMask");
	SCHEMA_ADD_FIELD(uint64_t, m_nQueuedButtonChangeMask, "CPlayer_MovementServices->m_nQueuedButtonChangeMask");
	SCHEMA_ADD_FIELD(uint64_t, m_nButtonDoublePressed, "CPlayer_MovementServices->m_nButtonDoublePressed");
	SCHEMA_ADD_FIELD(uint32_t[64], m_pButtonPressedCmdNumber, "CPlayer_MovementServices->m_pButtonPressedCmdNumber");
	SCHEMA_ADD_FIELD(uint32_t, m_nLastCommandNumberProcessed, "CPlayer_MovementServices->m_nLastCommandNumberProcessed");
	SCHEMA_ADD_FIELD(uint64_t, m_nToggleButtonDownMask, "CPlayer_MovementServices->m_nToggleButtonDownMask");
	SCHEMA_ADD_FIELD(float, m_flMaxspeed, "CPlayer_MovementServices->m_flMaxspeed");
	SCHEMA_ADD_FIELD(float[4], m_arrForceSubtickMoveWhen, "CPlayer_MovementServices->m_arrForceSubtickMoveWhen");
	SCHEMA_ADD_FIELD(float, m_flForwardMove, "CPlayer_MovementServices->m_flForwardMove");
	SCHEMA_ADD_FIELD(float, m_flLeftMove, "CPlayer_MovementServices->m_flLeftMove");
	SCHEMA_ADD_FIELD(float, m_flUpMove, "CPlayer_MovementServices->m_flUpMove");
	SCHEMA_ADD_FIELD(Vector_t, m_vecLastMovementImpulses, "CPlayer_MovementServices->m_vecLastMovementImpulses");
	SCHEMA_ADD_FIELD(QAngle_t, m_vecOldViewAngles, "CPlayer_MovementServices->m_vecOldViewAngles");

	void SetPredictionCommand(CUserCmd* pCmd) 
	{
		MEM::CallVFunc<void*, 34U>(this);
	}

	void ResetPredictionCommand() 
	{
		MEM::CallVFunc<void*, 35U>(this);
	}
};

// entries 12 | size 0x218
class CPlayer_MovementServices_Humanoid : public CPlayer_MovementServices
{
public:
	SCHEMA_ADD_FIELD(bool, m_bDucking, "CPlayer_MovementServices_Humanoid->m_bDucking");
};

class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid
{
public:
	void RunCommand(CUserCmd* pCmd)
	{
		MEM::CallVFunc<void, 23U>(this, pCmd);
	}

	void SetPredictionCommand(CUserCmd* pCmd)
	{
		MEM::CallVFunc<void, 34U>(this, pCmd);
	}

	void ResetPredictionCommand()
	{
		MEM::CallVFunc<void, 35U>(this);
	}

public:
	SCHEMA_ADD_FIELD(std::float_t, m_flDuckAmount, "CCSPlayer_MovementServices->m_flDuckAmount");
};

class CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(CBaseHandle, GetActiveWeapon, "CPlayer_WeaponServices->m_hActiveWeapon");
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(GameTime_t, GetNextAttack, "CCSPlayer_WeaponServices->m_flNextAttack");
};

class CCSInputMessage
{
public:
	int32_t m_frame_tick_count; //0x0000
	float m_frame_tick_fraction; //0x0004
	int32_t nPlayerTickCount; //0x0008
	float m_player_tick_fraction; //0x000C
	Vector_t m_view_angles; //0x0010
	Vector_t m_shoot_position; //0x001C
	int32_t m_target_index; //0x0028
	Vector_t m_target_head_position; //0x002C
	Vector_t m_target_abs_origin; //0x0038
	Vector_t m_target_angle; //0x0044
	int32_t m_sv_show_hit_registration; //0x0050
	int32_t m_entry_index_max; //0x0054
	int32_t m_index_idk; //0x0058
}; //Size: 0x005C

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerPawn);

	SCHEMA_ADD_FIELD(CBaseHandle, GetControllerHandle, "C_BasePlayerPawn->m_hController");
	SCHEMA_ADD_FIELD(CCSPlayer_WeaponServices*, GetWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");
	SCHEMA_ADD_FIELD(CPlayer_ItemServices*, GetItemServices, "C_BasePlayerPawn->m_pItemServices");
	SCHEMA_ADD_FIELD(CPlayer_CameraServices*, GetCameraServices, "C_BasePlayerPawn->m_pCameraServices");
	SCHEMA_ADD_FIELD(CCSPlayer_MovementServices*, m_pMovementServices, "C_BasePlayerPawn->m_pMovementServices");
	
	SCHEMA_ADD_FIELD(float, m_flOldSimulationTime, "C_BasePlayerPawn->m_flOldSimulationTime");
};

class CCSPlayer_ViewModelServices;

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawnBase);

	SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, GetViewModelServices, "C_CSPlayerPawnBase->m_pViewModelServices");
	SCHEMA_ADD_FIELD(float, GetLowerBodyYawTarget, "C_CSPlayerPawnBase->m_flLowerBodyYawTarget");
	SCHEMA_ADD_FIELD(float, GetFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
	SCHEMA_ADD_FIELD(float, GetFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");
	SCHEMA_ADD_FIELD(Vector_t, GetLastSmokeOverlayColor, "C_CSPlayerPawnBase->m_vLastSmokeOverlayColor");
	SCHEMA_ADD_FIELD(int, GetSurvivalTeam, "C_CSPlayerPawnBase->m_nSurvivalTeam"); // danger zone
	SCHEMA_ADD_FIELD(std::string, m_pClippingWeapon, "C_CSPlayerPawnBase->m_pClippingWeapon"); // danger zone
	SCHEMA_ADD_FIELD(QAngle_t, m_angEyeAngles, "C_CSPlayerPawnBase->m_angEyeAngles");
};

class C_CSWeaponBaseGun;

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawn);

	[[nodiscard]] Vector_t GetEyePosition();

	[[nodiscard]] bool IsOtherEnemy(C_CSPlayerPawn* pOther);
	[[nodiscard]] int GetAssociatedTeam();
	[[nodiscard]] bool CanAttack(const float flServerTime);

	SCHEMA_ADD_FIELD(bool, m_pBulletServices, "C_CSPlayerPawn->m_pBulletServices");
	SCHEMA_ADD_FIELD(bool, IsScoped, "C_CSPlayerPawn->m_bIsScoped");
	SCHEMA_ADD_FIELD(bool, m_bResumeZoom, "C_CSPlayerPawn->m_bResumeZoom");
	SCHEMA_ADD_FIELD(bool, IsDefusing, "C_CSPlayerPawn->m_bIsDefusing");
	SCHEMA_ADD_FIELD(bool, IsGrabbingHostage, "C_CSPlayerPawn->m_bIsGrabbingHostage");
	SCHEMA_ADD_FIELD(bool, IsWaitForNoAttack, "C_CSPlayerPawn->m_bWaitForNoAttack");
	SCHEMA_ADD_FIELD(int, GetShotsFired, "C_CSPlayerPawn->m_iShotsFired");
	SCHEMA_ADD_FIELD(CUtlVector< QAngle_t >, m_aimPunchCache, "C_CSPlayerPawn->m_aimPunchCache");
	SCHEMA_ADD_FIELD(std::int32_t, GetArmorValue, "C_CSPlayerPawn->m_ArmorValue");

	CS_INLINE C_CSPlayerBulletServices* BulletServices() {
		return *reinterpret_cast<C_CSPlayerBulletServices**>(reinterpret_cast<std::uint64_t>(this) + this->m_pBulletServices());
	};

	std::uint16_t GetCollisionMask() {
		if (GetCollision())
			return GetCollision()->CollisionMask();  // Collision + 0x38
		return 0;
	}

	float speed_move()
	{
		return *(float*)(std::uintptr_t(this) + 0x3D8);
	}

	int get_bone_index(const char* name)
	{
		using fn_t = int(__fastcall*)(void* _this, const char* name);
		static fn_t fn = (fn_t)MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 84 DB 74 3A"), 0x1);
		return fn(this, name);
	}
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBasePlayerController);


	SCHEMA_ADD_FIELD(std::uint64_t, GetSteamId, "CBasePlayerController->m_steamID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetTickBase, "CBasePlayerController->m_nTickBase");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPawnHandle, "CBasePlayerController->m_hPawn");
	SCHEMA_ADD_FIELD(bool, IsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");
};

// forward decleration
class C_CSWeaponBaseGun;
class C_BasePlayerWeapon;
class C_CSWeaponBase;

class CCSPlayerController : public CBasePlayerController
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayerController);

	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();

	bool IsThrowingGrenade(C_CSWeaponBase* pBaseWeapon);

	// @note: always get origin from pawn not controller
	[[nodiscard]] const Vector_t& GetPawnOrigin();

	[[nodiscard]] C_CSWeaponBase* GetPlayerWeapon(C_CSPlayerPawn* pPlayer);

	SCHEMA_ADD_FIELD(std::uint32_t, GetPing, "CCSPlayerController->m_iPing");
	SCHEMA_ADD_FIELD(const char*, GetPlayerName, "CCSPlayerController->m_sSanitizedPlayerName");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnHealth, "CCSPlayerController->m_iPawnHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnArmor, "CCSPlayerController->m_iPawnArmor");
	SCHEMA_ADD_FIELD(bool, IsPawnHasDefuser, "CCSPlayerController->m_bPawnHasDefuser");
	SCHEMA_ADD_FIELD(bool, IsPawnHasHelmet, "CCSPlayerController->m_bPawnHasHelmet");
	SCHEMA_ADD_FIELD(bool, IsPawnAlive, "CCSPlayerController->m_bPawnIsAlive");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPlayerPawnHandle, "CCSPlayerController->m_hPlayerPawn");
	SCHEMA_ADD_FIELD(uint32_t, m_nTickBase, "CBasePlayerController->m_nTickBase");
	SCHEMA_ADD_FIELD_OFFSET(CUserCmd*, GetCurrentCommand, "CBasePlayerController->m_steamID", -0x8);

	void PreThink()
	{
		// @ida: Function at start of PhysicsRunThink caller
		MEM::CallVFunc<void, 212U>(this);
	}

	void PostThink()
	{
		// @ida: Function under PhysicsRunThink
		MEM::CallVFunc<void, 213U>(this);
	}
};

class CBaseAnimGraph : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBaseAnimGraph);

	SCHEMA_ADD_FIELD(bool, IsClientRagdoll, "CBaseAnimGraph->m_bClientRagdoll");
};

class C_BaseFlex : public CBaseAnimGraph
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseFlex);
	/* not implemented */
};

class C_EconItemView
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconItemView);

	SCHEMA_ADD_FIELD(std::uint16_t, GetItemDefinitionIndex, "C_EconItemView->m_iItemDefinitionIndex");
	SCHEMA_ADD_FIELD(std::uint64_t, GetItemID, "C_EconItemView->m_iItemID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetItemIDHigh, "C_EconItemView->m_iItemIDHigh");
	SCHEMA_ADD_FIELD(std::uint32_t, GetItemIDLow, "C_EconItemView->m_iItemIDLow");
	SCHEMA_ADD_FIELD(std::uint32_t, GetAccountID, "C_EconItemView->m_iAccountID");
	SCHEMA_ADD_FIELD(char[161], GetCustomName, "C_EconItemView->m_szCustomName");
	SCHEMA_ADD_FIELD(char[161], GetCustomNameOverride, "C_EconItemView->m_szCustomNameOverride");
};

class CAttributeManager
{
public:
	CS_CLASS_NO_INITIALIZER(CAttributeManager);
	virtual ~CAttributeManager() = 0;
};

static_assert(sizeof(CAttributeManager) == 0x8);

class C_AttributeContainer : public CAttributeManager
{
public:
	CS_CLASS_NO_INITIALIZER(C_AttributeContainer);

	SCHEMA_ADD_PFIELD(C_EconItemView, GetItem, "C_AttributeContainer->m_Item");
};

class C_EconEntity : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconEntity);

	SCHEMA_ADD_PFIELD(C_AttributeContainer, GetAttributeManager, "C_EconEntity->m_AttributeManager");
	SCHEMA_ADD_FIELD(std::uint32_t, GetOriginalOwnerXuidLow, "C_EconEntity->m_OriginalOwnerXuidLow");
	SCHEMA_ADD_FIELD(std::uint32_t, GetOriginalOwnerXuidHigh, "C_EconEntity->m_OriginalOwnerXuidHigh");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackPaintKit, "C_EconEntity->m_nFallbackPaintKit");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackSeed, "C_EconEntity->m_nFallbackSeed");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackWear, "C_EconEntity->m_flFallbackWear");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackStatTrak, "C_EconEntity->m_nFallbackStatTrak");
	SCHEMA_ADD_FIELD(CBaseHandle, GetViewModelAttachmentHandle, "C_EconEntity->m_hViewmodelAttachment");
};

class C_EconWearable : public C_EconEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconWearable);

	SCHEMA_ADD_FIELD(std::int32_t, GetForceSkin, "C_EconWearable->m_nForceSkin");
	SCHEMA_ADD_FIELD(bool, IsAlwaysAllow, "C_EconWearable->m_bAlwaysAllow");
};

class C_BasePlayerWeapon : public C_EconEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerWeapon);

	SCHEMA_ADD_FIELD(GameTick_t, GetNextPrimaryAttackTick, "C_BasePlayerWeapon->m_nNextPrimaryAttackTick");
	SCHEMA_ADD_FIELD(float, GetNextPrimaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextPrimaryAttackTickRatio");
	SCHEMA_ADD_FIELD(GameTick_t, GetNextSecondaryAttackTick, "C_BasePlayerWeapon->m_nNextSecondaryAttackTick");
	SCHEMA_ADD_FIELD(float, GetNextSecondaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextSecondaryAttackTickRatio");
	SCHEMA_ADD_FIELD(std::int32_t, GetClip1, "C_BasePlayerWeapon->m_iClip1");
	SCHEMA_ADD_FIELD(std::int32_t, GetClip2, "C_BasePlayerWeapon->m_iClip2");
	SCHEMA_ADD_FIELD(std::int32_t[2], GetReserveAmmo, "C_BasePlayerWeapon->m_pReserveAmmo");
};

#define GET_INACCURACY_SIG CS_XOR("48 89 5C 24 ? 55 56 57 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ?")
#define GET_SPREAD_SIG CS_XOR("48 83 EC ? 48 63 91 00 17 00 00")

class C_CSWeaponBase : public C_BasePlayerWeapon
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSWeaponBase);

	SCHEMA_ADD_FIELD(bool, IsInReload, "C_CSWeaponBase->m_bInReload");
	SCHEMA_ADD_FIELD(float, m_flRecoilIndex, "C_CSWeaponBase->m_flRecoilIndex");
	SCHEMA_ADD_OFFSET(int32_t, ZoomLevel, 0x1A70);

	__forceinline CCSWeaponBaseVData* GetWeaponVData()
	{
		return static_cast<CCSWeaponBaseVData*>(GetVData());
	}

	bool IsKnife() {
		auto defidx = GetWeaponVData()->GetWeaponType();
		return defidx == WEAPONTYPE_KNIFE;
	}

	__forceinline float GetMaxSpeed()
	{
		using original_fn = float(__fastcall*)(void*);
		return (*(original_fn**)this)[340](this);
	}

	[[nodiscard]] float GetSpread()
	{
		return MEM::CallVFunc<float, 357>(this);
	}

	[[nodiscard]] float GetInaccuracy()
	{
		float x = 0.0f, y = 0.0f;
		return MEM::CallVFunc<float, 403>(this, &x, &y);
	}
};

class C_CSWeaponBaseGun : public C_CSWeaponBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSWeaponBaseGun);

	SCHEMA_ADD_FIELD(std::int32_t, GetZoomLevel, "C_CSWeaponBaseGun->m_zoomLevel");
	SCHEMA_ADD_FIELD(std::int32_t, GetBurstShotsRemaining, "C_CSWeaponBaseGun->m_iBurstShotsRemaining");
	SCHEMA_ADD_FIELD(bool, IsBurstMode, "C_CSWeaponBase->m_bBurstMode");
	SCHEMA_ADD_FIELD(float, GetPostponeFireReadyFrac, "C_CSWeaponBase->m_flPostponeFireReadyFrac");
	SCHEMA_ADD_FIELD(float, m_fLastShotTime, "C_CSWeaponBase->m_fLastShotTime");

	[[nodiscard]] bool CanPrimaryAttack(const int nWeaponType, const float flServerTime);
	[[nodiscard]] bool CanSecondaryAttack(const int nWeaponType, const float flServerTime);

	C_EconItemView* GetEconItemView()
	{
		return  reinterpret_cast<C_EconItemView*>(std::uintptr_t(this) + 0x10E8);
	}
};

class C_BaseCSGrenade : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(bool, IsHeldByPlayer, "C_BaseCSGrenade->m_bIsHeldByPlayer");
	SCHEMA_ADD_FIELD(bool, IsPinPulled, "C_BaseCSGrenade->m_bPinPulled");
	SCHEMA_ADD_FIELD(GameTime_t, GetThrowTime, "C_BaseCSGrenade->m_fThrowTime");
	SCHEMA_ADD_FIELD(float, GetThrowStrength, "C_BaseCSGrenade->m_flThrowStrength");
};

class C_EnvSky
{
public:
	CS_CLASS_NO_INITIALIZER(C_EnvSky);

	SCHEMA_ADD_FIELD(bool, m_bStartDisabled, "C_EnvSky->m_bStartDisabled");
	SCHEMA_ADD_FIELD(Color_t, m_vTintColor, "C_EnvSky->m_vTintColor");
	SCHEMA_ADD_FIELD(ImColor, m_vTintColorIm, "C_EnvSky->m_vTintColor");
	SCHEMA_ADD_FIELD(Color_t, m_vTintColorLightingOnly, "C_EnvSky->m_vTintColorLightingOnly");
	SCHEMA_ADD_FIELD(float, m_flBrightnessScale, "C_EnvSky->m_flBrightnessScale");
	SCHEMA_ADD_FIELD(int, m_nFogType, "C_EnvSky->m_nFogType");
	SCHEMA_ADD_FIELD(float, m_flFogMinStart, "C_EnvSky->m_flFogMinStart");
	SCHEMA_ADD_FIELD(float, m_flFogMinEnd, "C_EnvSky->m_flFogMinEnd");
	SCHEMA_ADD_FIELD(float, m_flFogMaxStart, "C_EnvSky->m_flFogMaxStart");
	SCHEMA_ADD_FIELD(float, m_flFogMaxEnd, "C_EnvSky->m_flFogMaxEnd");
	SCHEMA_ADD_FIELD(bool, m_bEnabled, "C_EnvSky->m_bEnabled");
};

class C_BaseGrenade : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseGrenade);
};

