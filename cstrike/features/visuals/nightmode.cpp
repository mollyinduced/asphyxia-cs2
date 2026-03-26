#include <string>
// used: game's interfaces
#include "../../core/interfaces.h"
#include "../../sdk/interfaces/imaterialsystem.h"
#include "../../sdk/interfaces/igameresourceservice.h"
#include "../../sdk/interfaces/cgameentitysystem.h"
#include "../../sdk/interfaces/iresourcesystem.h"
#include "../../core/sdk.h"
#include "../../sdk/entity.h"

// used: original call in hooked function
#include "../../core/hooks.h"

// used: cheat variables
#include "../../core/variables.h"
namespace NightMode
{
	void SkyBoxChanger(Color_t targetColor)
	{
		if (!g_Config::bNightmode)
			targetColor = { 255, 255, 255 };
		else
			targetColor = targetColor;

		for (int i = 64; i < I::GameResourceService->pGameEntitySystem->GetHighestEntityIndex(); i++)
		{
			C_BaseEntity* cEnt = (C_BaseEntity*)I::GameResourceService->pGameEntitySystem->Get(i);

			if (cEnt == nullptr)
			{
				continue;
			}

			CEntityIdentity* pEntity = cEnt->GetIdentity();

			if (!pEntity)
			{
				continue;
			}

			if (!pEntity->GetDesignerName())
			{
				continue;
			}
			std::string classname = pEntity->GetDesignerName();

			if (classname == CS_XOR("env_sky"))
			{
				C_EnvSky* sky = I::GameResourceService->pGameEntitySystem->Get<C_EnvSky>(i);

				Color_t curCol = sky->m_vTintColor();

				if (curCol == targetColor)
				{
					return;
				}

				sky->m_vTintColor() = targetColor;

				MEM::fnUpdateSky(sky);
			}
		}
	}

	void WorldColorChanger(Color_t targetColor)
	{
		targetColor = Color_t(69, 69, 69);

		if (!g_Config::bNightmode)
			targetColor = { 255, 255, 255 };

		static Color_t currentColor{ 69, 69, 69 };
		static uint64_t oldArrCount = 0;
		ResourceArray_t arr{};

		arr.m_nCount = 0;
		arr.m_aResources = nullptr;
		I::ResourceSystem->EnumMaterials(0x74616D76, &arr, 2);

		if (targetColor == currentColor && oldArrCount == arr.m_nCount)
			return;

		if (!arr.m_nCount)
			return;

		oldArrCount = arr.m_nCount;
		currentColor = targetColor;

		for (int i = 0; i < arr.m_nCount; i++)
		{
			CMaterial2** pmat = arr.m_aResources[i];

			if (!pmat)
				continue;

			CMaterial2* mat = *pmat;
			if (!mat)
				continue;

			std::string name = mat->GetName();

			if (name.find("char") != std::string::npos)
				continue;

			if (name.find("weapon") != std::string::npos)
				continue;

			material_paramater* param = mat->FindParameter("g_vColorTint");

			if (!param)
				continue;

			param->vec_value = Vector4D_t{ targetColor.r / 255.f, targetColor.g / 255.f, targetColor.b / 255.f, 1.f };
			mat->UpdateParameter();
		}
	}

	void fsn(int stage)
	{
		if (stage == 6)
		{
			if (g_Config::bNightmode)
			{
				SkyBoxChanger(g_Config::colNightmode);
				WorldColorChanger(g_Config::colWorldColor);
			}
			else
			{
				SkyBoxChanger(Color_t(255,255,255));
				WorldColorChanger(Color_t(255, 255, 255));
			}

		}
	}
}
