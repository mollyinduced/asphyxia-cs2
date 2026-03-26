#include "hooks.h"
#include <cmath>

// used: variables
#include "variables.h"

#include "../features/crc.h"

// used: game's sdk
#include "../sdk/interfaces/iswapchaindx11.h"
#include "../sdk/interfaces/iviewrender.h"
#include "../sdk/interfaces/cgameentitysystem.h"
#include "../sdk/interfaces/ccsgoinput.h"
#include "../sdk/interfaces/iinputsystem.h"
#include "../sdk/interfaces/iengineclient.h"
#include "../sdk/interfaces/inetworkclientservice.h"
#include "../sdk/interfaces/iglobalvars.h"
#include "../sdk/interfaces/imaterialsystem.h"
#include "../sdk/interfaces/igameresourceservice.h"
#include "../sdk/interfaces/igameeventsystem.h"
#include "../sdk/datatypes/grenade.h"

#include "../entitylist.h"


#include "../sdk/entity.h"


// used: convars
#include "convars.h"
#include "../sdk/interfaces/ienginecvar.h"

// used: viewsetup
#include "../sdk/datatypes/viewsetup.h"

// used: entity
#include "../sdk/entity.h"

// used: get virtual function, find pattern, ...
#include "../utilities/memory.h"
// used: inputsystem
#include "../utilities/inputsystem.h"
// used: draw
#include "../utilities/draw.h"

// used: features callbacks
#include "../features.h"

// used: game's interfaces
#include "interfaces.h"
#include "sdk.h"

// used: menu
#include "menu.h"

#include "../features/prediction/pred.h"
#include "../features/antiaim/antiaim.h"
#include "../features/visuals/nightmode.h"
#include "../features/misc/movement.h"
#include "../features/rage/autowall.h"
#include "../features/rage/lagcomp.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <DirectXMath.h>
#include "../features/rage/ragebot.h"
#include "../features/autobuy/autobuy.h"
#include "../features/misc/scope.h"
#include "../features/visuals.h"

#include "../globals.h"
#include "../features/rage/doubletap.h"

#define M_PI_F ((float)(M_PI))

#ifndef DEG2RAD
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#endif

bool hooks::Setup()
{
	if (MH_Initialize() != MH_OK)
	{
		L_PRINT(LOG_ERROR) << CS_XOR("failed to initialize minhook");

		return false;
	}
	L_PRINT(LOG_INFO) << CS_XOR("minhook initialization completed");

	if (!hkPresent.Create(MEM::GetVFunc(I::SwapChain->pDXGISwapChain, VTABLE::D3D::PRESENT), reinterpret_cast<void*>(&Present)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"Present\" hook has been created");

	if (!hkResizeBuffers.Create(MEM::GetVFunc(I::SwapChain->pDXGISwapChain, VTABLE::D3D::RESIZEBUFFERS), reinterpret_cast<void*>(&ResizeBuffers)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"ResizeBuffers\" hook has been created");

	// creat swap chain hook
	IDXGIDevice* pDXGIDevice = NULL;
	I::Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));

	IDXGIAdapter* pDXGIAdapter = NULL;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);

	IDXGIFactory* pIDXGIFactory = NULL;
	pDXGIAdapter->GetParent(IID_PPV_ARGS(&pIDXGIFactory));

	if (!hkCreateSwapChain.Create(MEM::GetVFunc(pIDXGIFactory, VTABLE::DXGI::CREATESWAPCHAIN), reinterpret_cast<void*>(&CreateSwapChain)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateSwapChain\" hook has been created");

	pDXGIDevice->Release();
	pDXGIDevice = nullptr;
	pDXGIAdapter->Release();
	pDXGIAdapter = nullptr;
	pIDXGIFactory->Release();
	pIDXGIFactory = nullptr;

	// @ida: class CViewRender->OnRenderStart call GetMatricesForView
	if (!hkGetMatrixForView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 81 EC ? ? ? ? 49 8B C1")), reinterpret_cast<void*>(&GetMatrixForView)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"GetMatrixForView\" hook has been created");

	if (!hkSetViewModelFOV.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC 50 BA ? ? ? ?")), reinterpret_cast<float*>(&SetViewModelFOV)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"SetViewModelFOV\" hook has been created");

	if (!hkSetWorldFov.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 81 EC ? ? ? ? 48 8B D9 E8 ? ? ? ? 48 85 C0")), reinterpret_cast<float*>(&SetWorldFov)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"SetWorldFov\" hook has been created");

	if (!hkInputParser.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 4C 89 48 20 55 56 41 56 48 8D 68 B1 48 81 EC D0 00 00 00")), reinterpret_cast<void*>(&InputParser)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"InputParser\" hook has been created");

	// @ida: #STR: cl: CreateMove clamped invalid attack history index %d in frame history to -1. Was %d, frame history size %d.\n
	if (!hkCreateMove.Create(MEM::GetVFunc(I::Input, VTABLE::CLIENT::CREATEMOVE), reinterpret_cast<void*>(&CreateMove)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateMove\" hook has been created");

	if (!hkMouseInputEnabled.Create(MEM::GetVFunc(I::Input, VTABLE::CLIENT::MOUSEINPUTENABLED), reinterpret_cast<void*>(&MouseInputEnabled)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"MouseInputEnabled\" hook has been created");

	if (!hkFrameStageNotify.Create(MEM::GetVFunc(I::Client, VTABLE::CLIENT::FRAMESTAGENOTIFY), reinterpret_cast<void*>(&FrameStageNotify)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"FrameStageNotify\" hook has been created");

	// @ida: ClientModeShared -> #STR: "mapname", "transition", "game_newmap"
	if (!hkLevelInit.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 56 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B F2")), reinterpret_cast<void*>(&LevelInit)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LevelInit\" hook has been created");

	// @ida: ClientModeShared -> #STR: "map_shutdown"
	if (!hkLevelShutdown.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC 30 80 B9 ? ? ? ? ? 48 8B D9 0F 84 ? ? ? ? 48 8B 05 ? ? ? ?")), reinterpret_cast<void*>(&LevelShutdown)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LevelShutdown\" hook has been created");

	//CS_ASSERT(hkAllowCameraChange.Create(MEM::GetVFunc(I::Input, 7u), reinterpret_cast<void*>(&AllowCameraAngleChange)));

	// @note: seems to do nothing for now...
	// @ida: ClientModeCSNormal->OverrideView idx 15
	//v21 = flSomeWidthSize * 0.5;
	//v22 = *flSomeHeightSize * 0.5;
	//*(float*)(pSetup + 0x49C) = v21; // m_OrthoRight
	//*(float*)(pSetup + 0x494) = -v21; // m_OrthoLeft
	//*(float*)(pSetup + 0x498) = -v22; // m_OrthoTop
	//*(float*)(pSetup + 0x4A0) = v22; // m_OrthoBottom
	//if (!hkOverrideView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B FA E8")), reinterpret_cast<void*>(&OverrideView)))
	//	return false;

	//L_PRINT(LOG_INFO) << CS_XOR("\"OverrideView\" hook has been created");

	if (!hkDrawObject.Create(MEM::FindPattern(SCENESYSTEM_DLL, CS_XOR("48 8B C4 53 41 54 41 55 48 81 EC ? ? ? ? 4D 63 E1")), reinterpret_cast<void*>(&DrawObject))) // 48 8B C4 53 41 54 41 55 48 81 EC ? ? ? ? 4D 63 E1
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"DrawObject\" hook has been created");

	if (!hkAllowCameraChange.Create(MEM::GetVFunc(I::Input, 7u), reinterpret_cast<void*>(&AllowCameraAngleChange)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"AllowCameraChange\" hook has been created");

	if (!hkOverrideView.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 10 55 48 8D 6C 24 A9 48 81 EC B0 00 00 00 48 8B 01")), reinterpret_cast<void*>(&OverrideView)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"OverrideView\" hook has been created");

	if (!hkIsRelativeMouseMode.Create(MEM::GetVFunc(I::InputSystem, VTABLE::INPUTSYSTEM::ISRELATIVEMOUSEMODE), reinterpret_cast<void*>(&IsRelativeMouseMode)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"IsRelativeMouseMode\" hook has been created");

	if (!hkIsGlowing.Create(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ? ? ? ? 33 DB 84 C0 0F 84 ? ? ? ? 48 8B 4F 18")), 0x1), reinterpret_cast<void*>(&IsGlowing)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"IsGlowing\" hook has been created");

	if (!hkGetGlowColor.Create(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ? ? ? ? F3 0F 10 BE F8 0B 00 00")), 0x1), reinterpret_cast<void*>(&GetGlowColor)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"GetGlowColor\" hook has been created");

	if (!hkOnAddEntity.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 74 24 ? 57 48 83 EC ? 48 8B F9 41 8B C0 B9")), reinterpret_cast<void*>(&OnAddEntity)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"OnAddEntity\" hook has been created");

	if (!hkOnRemoveEntity.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 74 24 10 57 48 83 EC ? 48 8B F9 41 8B C0 25 ? ? ? ?")), reinterpret_cast<void*>(&OnRemoveEntity)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"OnRemoveEntity\" hook has been created");

	if (!hkViewmodelChanger.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC ? 49 8B E8 48 8B DA 48 8B F1")), reinterpret_cast<void*>(&ViewmodelChanger)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"ViewmodelChanger\" hook has been created");

	if (!hkFireClientEvent.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 56 57 41 54 48 83 EC 30 48 8B F2")), reinterpret_cast<void*>(&FireClientEvent)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"FireClientEvent\" hook has been created");

	if (!hkSubtickCheck.Create(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("E8 ? ? ? ? F3 0F 10 0D ? ? ? ? 45 33 C9")), 0x1), reinterpret_cast<void*>(&SubtickCheck)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"SubtickCheck\" hook has been created");

	if (!hkShouldRender.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("40 53 48 83 EC ? 48 8B D9 83 FA ? 75 ? 48 8B 0D ? ? ? ? 48 8D 54 24 ? 48 8B 01 FF 90 ? ? ? ? 8B 10")), reinterpret_cast<void*>(&ShouldRender)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"ShouldRender\" hook has been created");

	if (!hkLightModulation.Create(MEM::FindPattern(SCENESYSTEM_DLL, CS_XOR("48 89 54 24 ? 53 55 41 57")), reinterpret_cast<void*>(&LightModulation)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"LightModulation\" hook has been created");

	if (!hkOnRenderStart.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 10 48 89 6C 24 18 56 57 41 56 48 83 EC ? 4C 8B F1 48 8D 94 24 90 00 00 00")), reinterpret_cast<void*>(&OnRenderStart)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"CreateViewRender\" hook has been created");

	if (!hkDrawSmokeVertex.Create(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 40 48 8B 9C 24 88 00 00 00 4D")), reinterpret_cast<void*>(&DrawSmokeVertex)))
		return false;
	L_PRINT(LOG_INFO) << CS_XOR("\"DrawSmokeVertex\" hook has been created");

	SCOPE::Hook();

	return true;
}

void hooks::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	MH_Uninitialize();
}

HRESULT __stdcall hooks::Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags)
{
	const auto oPresent = hkPresent.GetOriginal();

	// recreate it if it's not valid
	if (I::RenderTargetView == nullptr)
		I::CreateRenderTarget();

	// set our render target
	if (I::RenderTargetView != nullptr)
		I::DeviceContext->OMSetRenderTargets(1, &I::RenderTargetView, nullptr);

	F::OnPresent();

	return oPresent(I::SwapChain->pDXGISwapChain, uSyncInterval, uFlags);
}

HRESULT CS_FASTCALL hooks::ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags)
{
	const auto oResizeBuffer = hkResizeBuffers.GetOriginal();

	auto hResult = oResizeBuffer(pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags);
	if (SUCCEEDED(hResult))
		I::CreateRenderTarget();

	return hResult;
}

HRESULT __stdcall hooks::CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	const auto oCreateSwapChain = hkCreateSwapChain.GetOriginal();

	I::DestroyRenderTarget();
	L_PRINT(LOG_INFO) << CS_XOR("render target view has been destroyed");

	return oCreateSwapChain(pFactory, pDevice, pDesc, ppSwapChain);
}

long hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!MENU::bMainWindowOpened)
	{
		if (uMsg == WM_KEYDOWN && wParam == g_Config::nAARightKey) g_Config::bAntiAimRight = !g_Config::bAntiAimRight;
		if (uMsg == WM_KEYDOWN && wParam == g_Config::nAALeftKey) g_Config::bAntiAimLeft = !g_Config::bAntiAimLeft;

		if (uMsg == WM_KEYDOWN && wParam == g_Config::nMinDmgOverride)
		{
			g_Config::iMinDamage = g_Config::iMinDamageOverride;
		}
	}

	if (D::OnWndProc(hWnd, uMsg, wParam, lParam))
		return 1L;

	return ::CallWindowProcW(IPT::pOldWndProc, hWnd, uMsg, wParam, lParam);
}

ViewMatrix_t* CS_FASTCALL hooks::GetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels)
{
	const auto oGetMatrixForView = hkGetMatrixForView.GetOriginal();
	ViewMatrix_t* matResult = oGetMatrixForView(pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels);

	// get view matrix
	SDK::ViewMatrix = *pOutWorldToProjection;
	// get camera position
	// @note: ida @GetMatrixForView(global_pointer, pRenderGameSystem + 16, ...)
	SDK::CameraPosition = pViewRender->vecOrigin;

	return matResult;
}

void* CS_FASTCALL hooks::InputParser(CCSInputMessage* Input, CCSGOInputHistoryEntryPB* InputHistoryEntry, char a3, void* a4, void* a5, void* a6)
{
	auto original = hkInputParser.GetOriginal();

	auto result = original(Input, InputHistoryEntry, a3, a4, a5, a6);

	if (!SDK::Cmd)
		return result;

	
	if (I::Engine->IsConnected() || I::Engine->IsInGame() && SDK::LocalPawn && SDK::LocalController->IsPawnAlive())
	{
		if (g_Config::bDoubleTap)
		{
			DOUBLETAP::InputParser(Input, InputHistoryEntry);
		}
	}
	

	return result;
}

void CS_FASTCALL hooks::AllowCameraAngleChange(CCSGOInput* pCSGOInput, int a2)
{
	const auto Original = hkAllowCameraChange.GetOriginal();

	auto angOriginalAngle = pCSGOInput->GetViewAngles();

	CUserCmd* pCmd = pCSGOInput->GetUserCmd();
	//pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = F::ANTIAIM::angStoredViewBackup;

	Original(pCSGOInput, a2);

	//pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = angOriginalAngle;
	pCSGOInput->SetViewAngle(angOriginalAngle);
}


bool CS_FASTCALL hooks::CreateMove(CCSGOInput* pInput, int nSlot, CUserCmd* pPredictedCmd)
{
	const auto oCreateMove = hkCreateMove.GetOriginal();
	const bool bResult = oCreateMove(pInput, nSlot, pPredictedCmd);

	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return bResult;

	CUserCmd* pCmd = SDK::Cmd = pPredictedCmd;
	if (pCmd == nullptr)
		return bResult;

	CBaseUserCmdPB* pBaseCmd = pCmd->csgoUserCmd.pBaseCmd;
	if (pBaseCmd == nullptr)
		return bResult;

	//F::ANTIAIM::angStoredViewBackup = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;

	if (I::Engine->IsInGame() && EntityList::vecEntities.empty( ))
		EntityList::ForceUpdateEntityList( );

	SDK::LocalController = CCSPlayerController::GetLocalPlayerController();
	if (SDK::LocalController == nullptr)
		return bResult;

	SDK::LocalPawn = I::GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(SDK::LocalController->GetPawnHandle());
	if (SDK::LocalPawn == nullptr)
		return bResult;

	F::ANTIAIM::angStoredViewBackup = pInput->GetViewAngles();

	BUYBOT::Buy();

	if (I::Engine->IsConnected() || I::Engine->IsInGame() && SDK::LocalPawn && SDK::LocalPawn->GetHealth() > 0)
	{
		g_PredictionSystem->Begin(pInput, pCmd);
		{
			RAGEBOT::CreateMove(SDK::LocalPawn, pInput, pCmd);

			F::OnCreateMove(pCmd, SDK::LocalController);
		}
		g_PredictionSystem->End(pInput, pCmd);
	}

	// apply buttonstates
	pBaseCmd->pInButtonState->nValue = pCmd->nButtons.nValue;
	pBaseCmd->pInButtonState->nValueChanged = pCmd->nButtons.nValueChanged;
	pBaseCmd->pInButtonState->nValueScroll = pCmd->nButtons.nValueScroll;

	if (!CRC::CalculateCRC(pBaseCmd))
		L_PRINT(LOG_ERROR) << CS_XOR("Failed to calculate CRC ");

	return bResult;
}

bool CS_FASTCALL hooks::MouseInputEnabled(void* pThisptr)
{
	const auto oMouseInputEnabled = hkMouseInputEnabled.GetOriginal();
	return MENU::bMainWindowOpened ? false : oMouseInputEnabled(pThisptr);
}

void CS_FASTCALL hooks::FrameStageNotify(void* rcx, int nFrameStage)
{
	const auto oFrameStageNotify = hkFrameStageNotify.GetOriginal();
	F::OnFrameStageNotify(nFrameStage);

	oFrameStageNotify(rcx, nFrameStage);

	NightMode::fsn(nFrameStage);

	if (nFrameStage == FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE)
		Lagcomp::Run();
}

__int64* CS_FASTCALL hooks::LevelInit(void* pClientModeShared, const char* szNewMap)
{
	const auto oLevelInit = hkLevelInit.GetOriginal();
	
	// if global variables are not captured during I::Setup or we join a new game, recapture it
	if (I::GlobalVars == nullptr)
		I::GlobalVars = *reinterpret_cast<IGlobalVars**>(MEM::ResolveRelativeAddress(MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 0D ? ? ? ? 48 89 41")), 0x3, 0x7));

	I::PVS->Set(false);

	return oLevelInit(pClientModeShared, szNewMap);
}

__int64 CS_FASTCALL hooks::LevelShutdown(void* pClientModeShared)
{
	const auto oLevelShutdown = hkLevelShutdown.GetOriginal();
	// reset global variables since it got discarded by the game
	I::GlobalVars = nullptr;

	return oLevelShutdown(pClientModeShared);
}

Vector_t TransformVector(Vector_t src, Vector_t angle, float magnitude) {
	Vector_t newPos;
	newPos.x = src.x + (cosf(DEG2RAD(angle.y)) * magnitude);
	newPos.y = src.y + (sinf(DEG2RAD(angle.y)) * magnitude);
	newPos.z = src.z - (tanf(DEG2RAD(angle.x)) * magnitude);
	return newPos;
}

inline QAngle_t CalcAngles(Vector_t viewPos, Vector_t aimPos)
{
	QAngle_t angle = { 0, 0, 0 };

	Vector_t delta = aimPos - viewPos;

	angle.x = -asin(delta.z / delta.Length()) * (180.0f / 3.141592654f);
	angle.y = atan2(delta.y, delta.x) * (180.0f / 3.141592654f);

	return angle;
}

Vector_t CalculateCameraPosition(Vector_t anchorPos, float distance, Vector_t viewAngles)
{
	float yaw = DirectX::XMConvertToRadians(viewAngles.y);
	float pitch = DirectX::XMConvertToRadians(viewAngles.x);

	float x = anchorPos.x + distance * cosf(yaw) * cosf(pitch);
	float y = anchorPos.y + distance * sinf(yaw) * cosf(pitch);
	float z = anchorPos.z + distance * sinf(pitch);

	return Vector_t{ x, y, z };
}

void CS_FASTCALL hooks::OverrideView(void* pClientModeCSNormal, CViewSetup* pSetup)
{
	const auto oOverrideView = hkOverrideView.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oOverrideView(pClientModeCSNormal, pSetup);

	if (!SDK::LocalController->IsPawnAlive() || !SDK::LocalController || !pSetup)
		return oOverrideView(pClientModeCSNormal, pSetup);

	oOverrideView(pClientModeCSNormal, pSetup);

	if (g_Config::bThirdPerson)
	{
		Vector_t adjusted_cam_view_angle = I::Input->currentMoveData.vecViewAngle;
		adjusted_cam_view_angle.x = -adjusted_cam_view_angle.x;
		pSetup->vecOrigin = CalculateCameraPosition(SDK::LocalPawn->GetEyePosition(), -g_Config::flThirdperson, adjusted_cam_view_angle); // calculates the camera position based on the view angles, distance and anchor point

		Ray_t ray{};
		GameTrace_t trace{};
		TraceFilter_t filter{ 0x1C3003, SDK::LocalPawn, NULL, 4 };

		if (I::EngineTrace->TraceShape(&ray, SDK::LocalPawn->GetEyePosition(), pSetup->vecOrigin, &filter, &trace)) {
			if (trace.DidHitWorld( )) 
			{
				pSetup->vecOrigin = trace.m_vecPosition;
			}
		}

		QAngle_t p = CalcAngles(pSetup->vecOrigin, SDK::LocalPawn->GetEyePosition()).Normalize();
		pSetup->angView = QAngle_t{ p.x, p.y };
	}
}

float CS_FASTCALL hooks::SetViewModelFOV()
{
	const auto oSetViewModelFOV = hkSetViewModelFOV.GetOriginal();

	if (g_Config::flSetViewModelFOV == 0.f)
		return oSetViewModelFOV();

	return g_Config::flSetViewModelFOV;
}

float CS_FASTCALL hooks::SetWorldFov()
{
	const auto oSetWorldFov = hkSetWorldFov.GetOriginal();

	return g_Config::flSetWorldFov;
}

void CS_FASTCALL hooks::DrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2)
{
	const auto oDrawObject = hkDrawObject.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (SDK::LocalController == nullptr || SDK::LocalPawn == nullptr)
		return oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);

	if (!F::OnDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2))
		oDrawObject(pAnimatableSceneObjectDesc, pDx11, arrMeshDraw, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2);
}

void* hooks::IsRelativeMouseMode(void* pThisptr, bool bActive)
{
	const auto oIsRelativeMouseMode = hkIsRelativeMouseMode.GetOriginal();

	MENU::bMainActive = bActive;

	if (MENU::bMainWindowOpened)
		return oIsRelativeMouseMode(pThisptr, false);

	return oIsRelativeMouseMode(pThisptr, bActive);
}

bool CS_FASTCALL hooks::IsGlowing(CGlowProperty* glow) {
	if (!I::Engine->IsInGame())
		return false;

	CBasePlayerController* con = SDK::LocalController;

	if (!con)
		return false;

	C_CSPlayerPawn* lp = SDK::LocalPawn;

	if (!lp)
		return false;

	if (!glow)
		return false;

	if (!glow->Owner())
		return false;

	if (glow->m_bGlowing())
		return true;

	CEntityIdentity* identify = reinterpret_cast<CEntityInstance*>(glow->Owner())->GetIdentity();

	if (!identify)
		return false;

	const char* class_name = identify->GetDesignerName();
	if (!class_name)
		return false;

	std::string sName = class_name;
	if (sName.find("player") != std::string::npos && (g_Config::espGlow) || g_Config::selfGlow) {
		C_CSPlayerPawn* pawn = reinterpret_cast<C_CSPlayerPawn*>(glow->Owner());

		if (pawn == lp) {
			if (g_Config::selfGlow && g_Config::bThirdPerson)
				return true;
			return false;
		}

		if (pawn->GetHealth() > 0.f && (pawn->GetTeam() != lp->GetTeam()))
			return true;
	}
	//else if (sName.find("weapon") != std::string::npos && settings::visuals::esp::items::glow) {
	//	CBasePlayerWeapon* weapon = reinterpret_cast<CBasePlayerWeapon*>(glow->m_pOwner);
	//
	//	if (weapon->GetHandleOwner() == 0xFFFFFFFF)
	//		return true;
	//}

	return false;
}

void __fastcall hooks::GetGlowColor(CGlowProperty* glow, float* pColorOut) {
	const auto oGetGlowColor = hkGetGlowColor.GetOriginal();
	if (!glow)
		return oGetGlowColor(glow, pColorOut);

	CEntityIdentity* identify = reinterpret_cast<CEntityInstance*>(glow->Owner())->GetIdentity();

	if (!identify)
		return oGetGlowColor(glow, pColorOut);

	const char* class_name = identify->GetDesignerName();
	if (!class_name)
		return oGetGlowColor(glow, pColorOut);

	std::string sName = class_name;
	if (sName.find("player") != std::string::npos && (g_Config::espGlow)) {
		C_CSPlayerPawn* pawn = reinterpret_cast<C_CSPlayerPawn*>(glow->Owner());

		if (pawn == SDK::LocalPawn)
			return;

		if (pawn->GetHealth() > 0.f) {
			pColorOut[0] = g_Config::espGlowColor.r / 255.f;
			pColorOut[1] = g_Config::espGlowColor.g / 255.f;
			pColorOut[2] = g_Config::espGlowColor.b / 255.f;
			pColorOut[3] = g_Config::espGlowColor.a / 255.f;
		}
	}
	else if (sName.find("player") != std::string::npos && g_Config::selfGlow)
	{
		C_CSPlayerPawn* pawn = reinterpret_cast<C_CSPlayerPawn*>(glow->Owner());

		if (pawn != SDK::LocalPawn)
			return;

		if (pawn->GetHealth() > 0.f) {
			pColorOut[0] = g_Config::selfGlowColor.r / 255.f;
			pColorOut[1] = g_Config::selfGlowColor.g / 255.f;
			pColorOut[2] = g_Config::selfGlowColor.b / 255.f;
			pColorOut[3] = g_Config::selfGlowColor.a / 255.f;
		}
	}
	else {
		return oGetGlowColor(glow, pColorOut);
	}



	//else if (sName.find("weapon") != std::string::npos && settings::visuals::esp::items::glow) {
	//	CBasePlayerWeapon* weapon = reinterpret_cast<CBasePlayerWeapon*>(glow->m_pOwner);
	//
	//	if (weapon->GetHandleOwner() == 0xFFFFFFFF)
	//		memcpy(pColorOut, settings::visuals::esp::items::glow_color, sizeof(float) * 4);
	//}
}

void CS_FASTCALL hooks::OnAddEntity(void* rcx, CEntityInstance* inst, CBaseHandle handle)
{
	const auto oOnAddEntity = hkOnAddEntity.GetOriginal();

	EntityList::OnAddEntity(inst, handle);

	return oOnAddEntity(rcx, inst, handle);
}

void CS_FASTCALL hooks::OnRemoveEntity(void* rcx, CEntityInstance* inst, CBaseHandle handle)
{
	const auto oOnRemoveEntity = hkOnRemoveEntity.GetOriginal();

	EntityList::OnRemoveEntity(inst, handle);

	return oOnRemoveEntity(rcx, inst, handle);
}

void* CS_FASTCALL hooks::ViewmodelChanger(float* unk1, float* viewmodelOffsets, float* viewmodelFov)
{
	const auto oViewmodelChanger = hkViewmodelChanger.GetOriginal();

	float tmp = 0.f;
	float vmoffsets[3]{ 0 };

	float vmx = vmoffsets[0];
	float vmy = vmoffsets[1];
	float vmz = vmoffsets[2];
	float vmfov = g_Config::flSetViewModelFOV;

	viewmodelOffsets[0] = vmx;
	viewmodelOffsets[1] = vmy;
	viewmodelOffsets[2] = vmz;
	*viewmodelFov = vmfov;

	return oViewmodelChanger(unk1, vmoffsets, &tmp);
}


char __fastcall hooks::FireClientEvent(void* unk, IGameEvent* event) {

	const auto oFireClientEvent = hkFireClientEvent.GetOriginal();

	if (FNV1A::Hash(event->Name()) == FNV1A::HashConst("round_freeze_end"))
		globals::m_bIsInFreezeTime = false;

	if (FNV1A::Hash(event->Name()) == FNV1A::HashConst("round_officially_ended"))
		globals::m_bIsInFreezeTime = true;

	return oFireClientEvent(unk, event);
}

void* CS_FASTCALL hooks::SubtickCheck(void* a1, int a2) {

	const auto oSubtickCheck = hkSubtickCheck.GetOriginal();
	return oSubtickCheck(a1, a2);
}

bool CS_FASTCALL hooks::ShouldRender(C_CSPlayerPawn* pawn, int splitscreen)
{
	const auto oShouldRender = hkShouldRender.GetOriginal();
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return oShouldRender(pawn, splitscreen);

	if (splitscreen != -1)
		return oShouldRender(pawn, splitscreen);

	// local
	if (pawn == SDK::LocalPawn)
		return false;

	return oShouldRender(pawn, splitscreen);
}

void CS_FASTCALL hooks::LightModulation(__int64 a1, CAggregateSceneObject* sceneObject, __int64 a3)
{
	const auto oLightingModulation = hkLightModulation.GetOriginal();

	if (!g_Config::bModulateLight)
		return oLightingModulation(a1, sceneObject, a3);

	sceneObject->red = g_Config::colLightColor.GetVec4().x * g_Config::iLightIntensity;
	sceneObject->green = g_Config::colLightColor.GetVec4().y * g_Config::iLightIntensity;
	sceneObject->blue = g_Config::colLightColor.GetVec4().z * g_Config::iLightIntensity;

	oLightingModulation(a1, sceneObject, a3);
}

void* CS_FASTCALL hooks::OnRenderStart(CViewRender* pViewRender)
{
	const auto oCreateViewRender = hkOnRenderStart.GetOriginal();

	void* ret = oCreateViewRender(pViewRender);

	if (g_Config::bAspectRatio)
	{
		pViewRender->m_viewSetup.flAspectRatio = g_Config::flAspectRatio;
		pViewRender->m_viewSetup.nSomeFlags |= g_Config::iArFlags;
	}
	else
	{
		pViewRender->m_viewSetup.nSomeFlags &= ~2;
	}

	return ret;
}

void* CS_FASTCALL hooks::DrawSmokeVertex(void* a1, void* a2, int a3, int a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10)
{
	const auto original = hkDrawSmokeVertex.GetOriginal();

	if (g_Config::bRemoveSmoke)
	{
		return nullptr;
	}
	
	return original(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}