#include "ServerPCH.h"



MouseServer::MouseServer()
{
}

void MouseServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject(this);
}


bool MouseServer::HandleCollisionWithShip(Ship* inCat)
{
	if (!picked)
	{
		if (inCat->GetHealth() <= 15)
			inCat->GetHealth()++;
		picked = true;

		// Hacked in here.
		int ECRS_Health = 1 << 3;
		NetworkManagerServer::sInstance->SetStateDirty(inCat->GetNetworkId(), ECRS_Health);
	}
	//kill yourself!
	SetDoesWantToDie(true);
	return false;
}


