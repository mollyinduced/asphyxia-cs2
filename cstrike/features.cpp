#include "features.h"

// used: draw callbacks
#include "utilities/draw.h"
// used: notify
#include "utilities/notify.h"

// used: cheat variables
#include "core/variables.h"
// used: menu
#include "core/menu.h"

// used: features callbacks
#include "features/visuals.h"
#include "features/misc.h"

// used: interfaces
#include "core/interfaces.h"
#include "sdk/interfaces/iengineclient.h"
#include "sdk/interfaces/cgameentitysystem.h"
#include "sdk/datatypes/usercmd.h"
#include "sdk/entity.h"
#include "features/misc/scope.h"
#include "globals.h"
#include "features/visuals/overlay.h"

#include "core/interfaces.h"
#include "sdk/interfaces/igameresourceservice.h"

bool F::Setup()
{
	if (!VISUALS::Setup())
	{
		L_PRINT(LOG_ERROR) << CS_XOR("failed to setup visuals");
		return false;
	}

	return true;
}

void F::Destroy()
{
	VISUALS::OnDestroy();
}

void F::OnPresent()
{
	if (!D::bInitialized)
		return;

	D::NewFrame();
	{
		// main window
		ImGui::PushFont(FONT::pMenu[g_Config::nDpiScale]);
		// @note: here you can draw your stuff
		MENU::RenderMainWindow();
		SCOPE::Run();

		F::VISUALS::OVERLAY::OnFrameStageNotify(SDK::LocalController);

		//ImGui::GetBackgroundDrawList()->AddRect(ImVec2(25, 25), ImVec2(25, 25), ImColor(255, 255, 255));

		// render notifications
		NOTIFY::Render();
		ImGui::PopFont();
	}
	D::Render();
	globals::m_bReadyToRender = true;
}

void F::OnFrameStageNotify(int nStage)
{
	F::VISUALS::OnFrame(nStage);
}

void F::OnCreateMove(CUserCmd* pCmd, CCSPlayerController* pLocalController)
{
	C_CSPlayerPawn* pLocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(pLocalController->GetPawnHandle());
	if (pLocalPawn == nullptr)
		return;

	F::MISC::OnMove(pCmd, pLocalController, pLocalPawn);
}

bool F::OnDrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	return VISUALS::OnDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}
