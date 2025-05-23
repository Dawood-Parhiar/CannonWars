#include "ClientPCH.hpp"
#include <cassert>

void ReplicationManagerClient::Read( InputMemoryBitStream& inInputStream )
{
	while( inInputStream.GetRemainingBitCount() >= 32 )
	{
		//read the network id...
		int networkId; inInputStream.Read( networkId );
		
		//only need 2 bits for action...
		uint8_t action; inInputStream.Read( action, 2 );

		switch( action )
		{
		case RA_Create:
			ReadAndDoCreateAction( inInputStream, networkId );
			break;
		case RA_Update:
			ReadAndDoUpdateAction( inInputStream, networkId );
			break;
		case RA_Destroy:
			ReadAndDoDestroyAction( inInputStream, networkId );
			break;
	/*	case RA_RPC:
			ReadAndDoRPCAction(inInputStream, networkId);
			break;*/
		}

	}

}

void ReplicationManagerClient::ReadAndDoCreateAction( InputMemoryBitStream& inInputStream, int inNetworkId )
{
	//need 4 cc
	uint32_t fourCCName;
	inInputStream.Read( fourCCName );

	//we might already have this object- could happen if our ack of the create got dropped so server resends create request 
	//( even though we might have created )
	GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject( inNetworkId );
	if( !gameObject )
	{
		//create the object and map it...
		gameObject = GameObjectRegistry::sInstance->CreateGameObject( fourCCName );
		gameObject->SetNetworkId( inNetworkId );
		NetworkManagerClient::sInstance->AddToNetworkIdToGameObjectMap( gameObject );

		if (fourCCName == 'SHIP')
		{
			auto ship = static_cast<ShipClient*>(gameObject.get());
			if (ship->GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
			{
				SFRenderManager::sInstance->SetLocalShipSpawned(true);
			}
		}

		//it had really be the rigth type...
		assert( gameObject->GetClassId() == fourCCName );
	}

	//and read state
	gameObject->Read( inInputStream );
	if (gameObject->GetClassId() == 'SHIP')
	{
		SoundManager::sInstance->PlaySound(SoundManager::SoundToPlay::STP_Join);
	}
	if (gameObject->GetClassId() == 'YARN')
	{
		auto loc = SFRenderManager::sInstance->FindCatCentre();
		sf::Listener::setPosition(loc.x, loc.y, 0);

		SoundManager::sInstance->PlaySoundAtLocation(SoundManager::SoundToPlay::STP_Shoot, sf::Vector3f(gameObject->GetLocation().mX, gameObject->GetLocation().mY, 0));
		
		// Test the attenuation volume by playing the sounds from a fixed point instead.
		//SoundManager::sInstance->PlaySoundAtLocation(SoundManager::SoundToPlay::STP_Shoot, sf::Vector3f(0, 0, 0));
}
}

void ReplicationManagerClient::ReadAndDoUpdateAction( InputMemoryBitStream& inInputStream, int inNetworkId )
{
	//need object
	GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject( inNetworkId );

	//gameObject MUST be found, because create was ack'd if we're getting an update...
	//and read state
	gameObject->Read( inInputStream );
}

void ReplicationManagerClient::ReadAndDoDestroyAction( InputMemoryBitStream& inInputStream, int inNetworkId )
{
	//if something was destroyed before the create went through, we'll never get it
	//but we might get the destroy request, so be tolerant of being asked to destroy something that wasn't created
	GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject( inNetworkId );
	if( gameObject )
	{
		if (gameObject->GetClassId() == 'SHIP')
		{
			SoundManager::sInstance->PlaySound(SoundManager::SoundToPlay::STP_Death);
		}
		gameObject->SetDoesWantToDie( true );
		NetworkManagerClient::sInstance->RemoveFromNetworkIdToGameObjectMap( gameObject );
	}

}

void ReplicationManagerClient::ReadAndDoRPCAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
	SoundManager::SoundToPlay sound;
	inInputStream.Read(sound);
	SoundManager::sInstance->PlaySound(sound);
	Log("Sound Played");
}