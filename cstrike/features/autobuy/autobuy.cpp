#include "autobuy.h"
#include "../../globals.h"

void BuyCmd(const char* command)
{
	I::Engine->ExecuteClientCmdUnrestricted(command);
}

static bool bBought = false;

void BUYBOT::Buy()
{
	if (!I::Engine->IsConnected() || !I::Engine->IsInGame())
		return;

	if (!SDK::LocalController || !SDK::LocalPawn)
		return;

	if (!SDK::LocalController->IsPawnAlive())
		return;

	if (!g_Config::bAutobuy)
		return;

	if (!globals::m_bIsInFreezeTime)
		return;

	bBought = false;

	if (bBought)
		return;

	int primary = g_Config::iAutobuyPrimary;
	int secondary = g_Config::iAutobuySecondary;
	int misc = g_Config::iAutobuyMisc;
	/*L_PRINT(LOG_INFO) << primary;
	L_PRINT(LOG_INFO) << secondary;
	L_PRINT(LOG_INFO) << misc;*/

	switch (primary)
	{
	case 1:
		BuyCmd("buy ssg08");
		break;
	case 2:
		BuyCmd("buy awp");
		break;
	case 3:
		BuyCmd("buy g3sg1; buy scar20");
		break;
	}
	
	switch (secondary)
	{
	case 1:
		BuyCmd("buy deagle");
		break;
	case 2:
		BuyCmd("buy fiveseven; buy tec9");
		break;
	}

	switch (misc)
	{
	case 1:
		BuyCmd("buy vesthelm; buy helm");
		break;
	}

	bBought = true;
}