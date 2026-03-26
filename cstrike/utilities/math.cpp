#include "math.h"
#include "../sdk/datatypes/qangle.h"
//used: getexportaddr
#include "memory.h"

bool MATH::Setup()
{
	bool bSuccess = true;

	const void* hTier0Lib = MEM::GetModuleBaseHandle(TIER0_DLL);
	if (hTier0Lib == nullptr)
		return false;

	fnRandomSeed = reinterpret_cast<decltype(fnRandomSeed)>(MEM::GetExportAddress(hTier0Lib, CS_XOR("RandomSeed")));
	bSuccess &= (fnRandomSeed != nullptr);

	fnRandomFloat = reinterpret_cast<decltype(fnRandomFloat)>(MEM::GetExportAddress(hTier0Lib, CS_XOR("RandomFloat")));
	bSuccess &= (fnRandomFloat != nullptr);

	fnRandomFloatExp = reinterpret_cast<decltype(fnRandomFloatExp)>(MEM::GetExportAddress(hTier0Lib, CS_XOR("RandomFloatExp")));
	bSuccess &= (fnRandomFloatExp != nullptr);

	fnRandomInt = reinterpret_cast<decltype(fnRandomInt)>(MEM::GetExportAddress(hTier0Lib, CS_XOR("RandomInt")));
	bSuccess &= (fnRandomInt != nullptr);

	fnRandomGaussianFloat = reinterpret_cast<decltype(fnRandomGaussianFloat)>(MEM::GetExportAddress(hTier0Lib, CS_XOR("RandomGaussianFloat")));
	bSuccess &= (fnRandomGaussianFloat != nullptr);

	return bSuccess;
}

void MATH::VectorAnglesVQ(const Vector_t& vecForward, QAngle_t& vecAngles) {
	Vector_t vecView;
	if (vecForward.y == 0 && vecForward.x == 0)
	{
		vecView.x = 0.f;
		vecView.y = 0.f;
	}
	else
	{
		vecView.y = atan2(vecForward.y, vecForward.x) * 180.f / 3.14f;

		if (vecView.y < 0.f)
			vecView.y += 360.f;

		auto tmp = vecForward.Length2D();

		vecView.x = atan2(-vecForward.z, tmp) * 180.f / 3.14f;

		if (vecView.x < 0.f)
			vecView.x += 360.f;
	}

	vecAngles.x = vecView.x;
	vecAngles.y = vecView.y;
	vecAngles.z = 0.f;
}
