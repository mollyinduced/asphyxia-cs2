#pragma once

#include "../../common.h"

// used: draw system
#include "../../utilities/draw.h"
#include "../../sdk/entity.h"

class CCSPlayerController;
class C_BaseEntity;
class C_CSPlayerPawn;

namespace BoneJointList
{
    inline std::vector<DWORD> Trunk = { BONEINDEX::neck_0,BONEINDEX::spine_2, BONEINDEX::pelvis };
    inline std::vector<DWORD> LeftArm = { BONEINDEX::neck_0,  BONEINDEX::arm_upper_L, BONEINDEX::arm_lower_L, BONEINDEX::hand_L };
    inline std::vector<DWORD> RightArm = { BONEINDEX::neck_0, BONEINDEX::arm_upper_R,BONEINDEX::arm_lower_R, BONEINDEX::hand_R };
    inline std::vector<DWORD> LeftLeg = { BONEINDEX::pelvis, BONEINDEX::leg_upper_L , BONEINDEX::leg_lower_L, BONEINDEX::ankle_L };
    inline std::vector<DWORD> RightLeg = { BONEINDEX::pelvis, BONEINDEX::leg_upper_R , BONEINDEX::leg_lower_R, BONEINDEX::ankle_R };
    inline std::vector<std::vector<DWORD>> List = { Trunk, LeftArm, RightArm, LeftLeg, RightLeg };
}

namespace F::VISUALS::OVERLAY
{
	void OnFrameStageNotify(CCSPlayerController* pLocalController);
	bool GetEntityBoundingBox(C_CSPlayerPawn* pEntity, ImVec4* pVecOut);
	void Player(CCSPlayerController* pLocal, CCSPlayerController* pPlayer, const float flDistance);
}
