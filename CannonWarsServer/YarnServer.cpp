#include "ServerPCH.h"


YarnServer::YarnServer()
{
	//yarn lives a second...
	mTimeToDie = Timing::sInstance.GetFrameStartTime() + 1.f;
}

void YarnServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject( this );
}


void YarnServer::Update()
{
	Yarn::Update();

	if( Timing::sInstance.GetFrameStartTime() > mTimeToDie )
	{
		SetDoesWantToDie( true );
	}

}

bool YarnServer::HandleCollisionWithShip( Ship* inCat )
{
	if( inCat->GetPlayerId() != GetPlayerId() )
	{
		//kill yourself!
		SetDoesWantToDie( true );

		static_cast< ShipServer* >( inCat )->TakeDamage( GetPlayerId() );

	}

	return false;
}


