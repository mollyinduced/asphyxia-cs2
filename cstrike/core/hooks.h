#pragma once

// used: [d3d] api
#include <d3d11.h>
#include <dxgi1_2.h>

// used: chookobject
#include "../utilities/detourhook.h"

// used: viewmatrix_t
#include "../sdk/datatypes/matrix.h"
#include "../sdk/entity.h"

#include "../sdk/datatypes/viewsetup.h"

inline float dist2points(const Vector_t& p1, const Vector_t& p2) {
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	return std::sqrt(dx * dx + dy * dy);
}

namespace VTABLE
{
	namespace D3D
	{
		enum
		{
			PRESENT = 8U,
			RESIZEBUFFERS = 13U,
			RESIZEBUFFERS_CSTYLE = 39U,
		};
	}

	namespace DXGI
	{
		enum
		{
			CREATESWAPCHAIN = 10U,
		};
	}

	namespace CLIENT
	{
		enum
		{
			MOUSEINPUTENABLED = 16U,
			CREATEMOVE = 18U,
			FRAMESTAGENOTIFY = 36U,
		};
	}

	namespace INPUTSYSTEM
	{
		enum
		{
			ISRELATIVEMOUSEMODE = 78U,
		};
	}
}

class CRenderGameSystem;
class IViewRender;
class CCSGOInput;
class CViewSetup;
class CMeshData;

class CNetInputMessage;
class CRenderGameSystem;
class IViewRender;
class CCSGOInput;
class CViewSetup;
class IGameEvent;
class C_BaseModelEntity;
class material_data_t;
class CGCClientSystem;
class CGCClientSharedObjectTypeCache;
class CCSGOInputHistoryEntryPB;
class CCSInputMessage;
class CGameSceneNode;

class CAggregateSceneObject
{
public:
	unsigned char pad_0[0xE4];
	float red;
	float green;
	float blue;
};

struct CViewRender
{
	MEM_PAD(0x8); // 0x0000
	CViewSetup m_viewSetup; // 0x0008

	virtual void Function0();
};


namespace hooks
{
	bool Setup();
	void Destroy();

	/* @section: handlers */
	// d3d11 & wndproc
	HRESULT WINAPI Present(IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags);
	HRESULT CS_FASTCALL ResizeBuffers(IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags);
	HRESULT WINAPI CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);
	long CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// game's functions
	ViewMatrix_t* CS_FASTCALL GetMatrixForView(CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels);
	void* CS_FASTCALL InputParser(CCSInputMessage* Input, CCSGOInputHistoryEntryPB* InputHistoryEntry, char a3, void* a4, void* a5, void* a6);
	void CS_FASTCALL AllowCameraAngleChange(CCSGOInput* pCSGOInput, int a2);
	bool CS_FASTCALL CreateMove(CCSGOInput* pInput, int nSlot, CUserCmd* pPredictedCmd);
	bool CS_FASTCALL MouseInputEnabled(void* pThisptr);
	void CS_FASTCALL FrameStageNotify(void* rcx, int nFrameStage);
	__int64* CS_FASTCALL LevelInit(void* pClientModeShared, const char* szNewMap);
	__int64 CS_FASTCALL LevelShutdown(void* pClientModeShared);
	void CS_FASTCALL OverrideView(void* pClientModeCSNormal, CViewSetup* pSetup);
	void CS_FASTCALL DrawObject(void* pAnimatableSceneObjectDesc, void* pDx11, CMeshData* arrMeshDraw, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2);
	float CS_FASTCALL SetViewModelFOV();
	float CS_FASTCALL SetWorldFov();

	void CS_FASTCALL GetGlowColor(CGlowProperty* glow, float* pColorOut);
	bool CS_FASTCALL IsGlowing(CGlowProperty* glow);
	void CS_FASTCALL OnAddEntity(void* rcx, CEntityInstance* inst, CBaseHandle handle);
	void CS_FASTCALL OnRemoveEntity(void* rcx, CEntityInstance* inst, CBaseHandle handle); 

	void* CS_FASTCALL ViewmodelChanger(float* unk1, float* viewmodelOffsets, float* viewmodelFov);
	char CS_FASTCALL FireClientEvent(void* unk, IGameEvent* event);
	void* CS_FASTCALL SubtickCheck(void* a1, int a2);
	bool CS_FASTCALL ShouldRender(C_CSPlayerPawn* pawn, int splitscreen);
	void CS_FASTCALL LightModulation(__int64 a1, CAggregateSceneObject* sceneObject, __int64 a3);

	void* CS_FASTCALL OnRenderStart(CViewRender* pViewRender);

	// sdl functions
	void* IsRelativeMouseMode(void* pThisptr, bool bActive);
	void* CS_FASTCALL DrawSmokeVertex(void* a1, void* a2, int a3, int a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10);


	/* @section: managers */
	inline CBaseHookObject<decltype(&Present)> hkPresent = {};
	inline CBaseHookObject<decltype(&ResizeBuffers)> hkResizeBuffers = {};
	inline CBaseHookObject<decltype(&CreateSwapChain)> hkCreateSwapChain = {};
	inline CBaseHookObject<decltype(&WndProc)> hkWndProc = {};

	inline CBaseHookObject<decltype(&GetMatrixForView)> hkGetMatrixForView = {};
	inline CBaseHookObject<decltype(&SetViewModelFOV)> hkSetViewModelFOV = {};
	inline CBaseHookObject<decltype(&SetWorldFov)> hkSetWorldFov = {};
	inline CBaseHookObject<decltype(&CreateMove)> hkCreateMove = {};
	inline CBaseHookObject<decltype(&MouseInputEnabled)> hkMouseInputEnabled = {};
	inline CBaseHookObject<decltype(&IsRelativeMouseMode)> hkIsRelativeMouseMode = {};
	inline CBaseHookObject<decltype(&FrameStageNotify)> hkFrameStageNotify = {};
	inline CBaseHookObject<decltype(&LevelInit)> hkLevelInit = {};
	inline CBaseHookObject<decltype(&LevelShutdown)> hkLevelShutdown = {};
	inline CBaseHookObject<decltype(&OverrideView)> hkOverrideView = {};
	inline CBaseHookObject<decltype(&InputParser)> hkInputParser = {};

	inline CBaseHookObject<decltype(&AllowCameraAngleChange)> hkAllowCameraChange = {};

	inline CBaseHookObject<decltype(&DrawObject)> hkDrawObject = {};

	inline CBaseHookObject<decltype(&GetGlowColor)> hkGetGlowColor = {};
	inline CBaseHookObject<decltype(&IsGlowing)> hkIsGlowing = {};
	inline CBaseHookObject<decltype(&OnAddEntity)> hkOnAddEntity = {};
	inline CBaseHookObject<decltype(&OnRemoveEntity)> hkOnRemoveEntity = {};
	inline CBaseHookObject<decltype(&ViewmodelChanger)> hkViewmodelChanger = {};
	inline CBaseHookObject<decltype(&FireClientEvent)> hkFireClientEvent = {};
	inline CBaseHookObject<decltype(&SubtickCheck)> hkSubtickCheck = {};
	inline CBaseHookObject<decltype(&ShouldRender)> hkShouldRender = {};
	inline CBaseHookObject<decltype(&LightModulation)> hkLightModulation = {};
	inline CBaseHookObject<decltype(&OnRenderStart)> hkOnRenderStart = {};
	inline CBaseHookObject<decltype(&DrawSmokeVertex)> hkDrawSmokeVertex = {};
}
