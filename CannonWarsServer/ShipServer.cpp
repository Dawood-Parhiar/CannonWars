#include "ServerPCH.h"

ShipServer::ShipServer() :
	mCatControlType( ESCT_Human ),
	mTimeOfNextShot( 0.f ),
	mTimeBetweenShots( 0.2f )
{}

void ShipServer::HandleDying()
{
	NetworkManagerServer::sInstance->UnregisterGameObject( this );
}

void ShipServer::Update()
{
	Ship::Update();
	
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();
	float oldRotation = GetRotation();

	//are you controlled by a player?
	//if so, is there a move we haven't processed yet?
	if( mCatControlType == ESCT_Human )
	{
		ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy( GetPlayerId() );
		if( client )
		{
			MoveList& moveList = client->GetUnprocessedMoveList();
			for( const Move& unprocessedMove : moveList )
			{
				const InputState& currentState = unprocessedMove.GetInputState();

				float deltaTime = unprocessedMove.GetDeltaTime();

				ProcessInput( deltaTime, currentState );
				SimulateMovement( deltaTime );

				//LOG( "Server Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", unprocessedMove.GetTimestamp(), deltaTime, GetRotation() );
				
				NetworkManagerServer::sInstance
					->SetStateDirty(GetNetworkId(), ESRS_CannonLeft | ESRS_CannonRight);


			}

			moveList.Clear();
		}
	}
	else
	{
		//do some AI stuff
		SimulateMovement( Timing::sInstance.GetDeltaTime() );
	}

	HandleShooting();

	if( !RoboMath::Is2DVectorEqual( oldLocation, GetLocation() ) ||
		!RoboMath::Is2DVectorEqual( oldVelocity, GetVelocity() ) ||
		oldRotation != GetRotation() )
	{
		NetworkManagerServer::sInstance->SetStateDirty( GetNetworkId(), ESRS_Pose );
	}

}

//void ShipServer::HandleShooting()
//{
//	float time = Timing::sInstance.GetFrameStartTime();
//	if( mIsShooting && Timing::sInstance.GetFrameStartTime() > mTimeOfNextShot )
//	{
//		//not exact, but okay
//		mTimeOfNextShot = time + mTimeBetweenShots;
//
//		//fire!
//		YarnPtr yarn = std::static_pointer_cast< Yarn >( GameObjectRegistry::sInstance->CreateGameObject( 'YARN' ) );
//		yarn->InitFromShooter( this );
//	}
//}


void ShipServer::HandleShooting()
{
	float now = Timing::sInstance.GetFrameStartTime();
	if (mIsShooting && now > mTimeOfNextShot)
	{
		mTimeOfNextShot = now + mTimeBetweenShots;

		// Compute turret’s absolute angle:
		float  aimAngleDeg = GetRotation() + mCannonRotation;
		float  aimRad = aimAngleDeg * (3.14159265f / 180.f);
		Vector3 dir{ std::cos(aimRad), std::sin(aimRad), 0.f };

		// spawn
		YarnPtr yarn = std::static_pointer_cast<Yarn>(
			GameObjectRegistry::sInstance->CreateGameObject('YARN'));
		NetworkManagerServer::sInstance->RegisterGameObject(yarn);

		// place at cannon tip
		yarn->SetLocation(GetLocation() + dir * GetCollisionRadius());

		float muzzleSpeed = 400.f;
		// 2) fire in the cannon direction
		yarn->SetVelocity(dir * muzzleSpeed);

		// 3) replicate color, player‐id, rotation if you need them
		yarn->SetColor(GetColor());
		yarn->SetPlayerId(GetPlayerId());
		yarn->SetRotation(GetRotation() + mCannonRotation);
	}
}


void ShipServer::TakeDamage( int inDamagingPlayerId )
{
	mHealth--;
	if( mHealth <= 0.f )
	{
		//score one for damaging player...
		ScoreBoardManager::sInstance->IncScore( inDamagingPlayerId, 1 );

		//and you want to die
		SetDoesWantToDie( true );

		//tell the client proxy to make you a new cat
		ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy( GetPlayerId() );
		if( clientProxy )
		{
			clientProxy->HandleCatDied();
		}
	}

	//tell the world our health dropped
	NetworkManagerServer::sInstance->SetStateDirty( GetNetworkId(), ESRS_Health );
}
