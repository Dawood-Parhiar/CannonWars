﻿#include "ClientPCH.hpp"



ShipClient::ShipClient() :
	mTimeLocationBecameOutOfSync( 0.f ),
	mTimeVelocityBecameOutOfSync( 0.f ),
	m_textureIsDirty(true)
{
	m_sprite.reset(new SFSpriteComponent(this));
	m_cannonSprite.reset(new SFSpriteComponent(this));

	m_cannonSprite->SetTexture(SFTextureManager::sInstance->GetTexture("cannon"));

	// --------------- name text setup ---------------
	auto& font = *FontManager::sInstance->GetFont("bebas");
	m_nameText.setFont(font);
	m_nameText.setCharacterSize(18);
	m_nameText.setFillColor(sf::Color::White);

	SoundManager::sInstance->PlayMusic();
	m_healthSprite.reset(new SFHealthSpriteComponent(this));
}

void ShipClient::HandleDying()
{
	Ship::HandleDying();

	//and if we're local, tell the hud so our health goes away!
	if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
	{
		HUD::sInstance->SetPlayerHealth( 0 );
	}
}



void ShipClient::HandleCannonSpriteText()
{
	sf::Vector2f worldPos{ GetLocation().mX, GetLocation().mY };

	sf::Sprite& cs = m_cannonSprite->GetSprite();
	cs.setPosition(worldPos);
	cs.setRotation(GetRotation() + mCannonRotation);

	// 3) Set the name string
	std::string name;
	if (GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId())
	{
		name = ConnectionDetails::sInstance->GetClientName();
	}
	else
	{
		// remote player
		auto entry = ScoreBoardManager::sInstance->GetEntry(GetPlayerId());
		name = entry ? entry->GetPlayerName() : "??";
	}
	m_nameText.setString(name);

	// center‐origin the text
	auto bounds = m_nameText.getLocalBounds();
	m_nameText.setOrigin(bounds.width / 2.f, bounds.height);

	// position it just above the cannon
	m_nameText.setPosition(worldPos.x, worldPos.y - cs.getGlobalBounds().height / 2 - 4.f);

	// 4) Draw it
	SFWindowManager::sInstance->draw(m_nameText);
}

void ShipClient::Update()
{
	// Check if we need to set the texture.
	if (m_textureIsDirty)
	{
		m_sprite->SetTexture(PlayerTextureGenerator::sInstance->GetPlayerTexure(GetPlayerId()));
		m_textureIsDirty = false;
	}

	


	//is this the cat owned by us?
	if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
	{
		const Move* pendingMove = InputManager::sInstance->GetAndClearPendingMove();
		//in theory, only do this if we want to sample input this frame / if there's a new move ( since we have to keep in sync with server )
		if( pendingMove ) //is it time to sample a new move...
		{
			float deltaTime = pendingMove->GetDeltaTime();
			
			//apply that input
			
			ProcessInput( deltaTime, pendingMove->GetInputState() );

			//and simulate!

			SimulateMovement( deltaTime );
			
			//LOG( "Client Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", latestMove.GetTimestamp(), deltaTime, GetRotation() );
		}
		HandleCannonSpriteText();

	}
	else
	{
		SimulateMovement( Timing::sInstance.GetDeltaTime() );

		if( RoboMath::Is2DVectorEqual( GetVelocity(), Vector3::Zero ) )
		{
			//we're in sync if our velocity is 0
			mTimeLocationBecameOutOfSync = 0.f;
		}
	}

	
}

void ShipClient::Read( InputMemoryBitStream& inInputStream )
{
	bool stateBit;
	
	uint32_t readState = 0;

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		uint32_t playerId;
		inInputStream.Read( playerId );
		SetPlayerId( playerId );
		readState |= ESRS_PlayerId;
	}

	float oldRotation = GetRotation();
	Vector3 oldLocation = GetLocation();
	Vector3 oldVelocity = GetVelocity();

	float replicatedRotation;
	Vector3 replicatedLocation;
	Vector3 replicatedVelocity;

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		inInputStream.Read( replicatedVelocity.mX );
		inInputStream.Read( replicatedVelocity.mY );

		SetVelocity( replicatedVelocity );

		inInputStream.Read( replicatedLocation.mX );
		inInputStream.Read( replicatedLocation.mY );

		SetLocation( replicatedLocation );

		inInputStream.Read( replicatedRotation );
		SetRotation( replicatedRotation );

		readState |= ESRS_Pose;
	}

	inInputStream.Read( stateBit );
	if (stateBit)
	{
		inInputStream.Read(stateBit);
		mThrustDir.x = stateBit ? 1.f : -1.f;
		inInputStream.Read(stateBit);
		mThrustDir.y = stateBit ? 1.f : -1.f;
	}
	else
	{
		mThrustDir.x = 0.f; 
		mThrustDir.y = 0.f;
	}

	// --- CANNON INPUTS ---
	// Cannon Left
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		inInputStream.Read(mCannonLeft);
		readState |= ESRS_CannonLeft;
	}
	else
	{
		mCannonLeft = false;
	}

	// Cannon Right
	inInputStream.Read(stateBit);
	if (stateBit)
	{
		inInputStream.Read(mCannonRight);
		readState |= ESRS_CannonRight;
	}
	else
	{
		mCannonRight = false;
	}

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		Vector3 color;
		inInputStream.Read( color );
		SetColor( color );
		readState |= ESRS_Color;
	}

	inInputStream.Read( stateBit );
	if( stateBit )
	{
		mHealth = 0;
		inInputStream.Read( mHealth, 4 );
		readState |= ESRS_Health;
	}

	

	if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
	{
		//did we get health? if so, tell the hud!
		if( ( readState & ESRS_Health ) != 0 )
		{
			HUD::sInstance->SetPlayerHealth( mHealth );
		}

		DoClientSidePredictionAfterReplicationForLocalCat( readState );

		//if this is a create packet, don't interpolate
		if( ( readState & ESRS_PlayerId ) == 0 )
		{
			InterpolateClientSidePrediction( oldRotation, oldLocation, oldVelocity, false );
		}
	}
	else
	{
		DoClientSidePredictionAfterReplicationForRemoteCat( readState );

		//will this smooth us out too? it'll interpolate us just 10% of the way there...
		if( ( readState & ESRS_PlayerId ) == 0 )
		{
			InterpolateClientSidePrediction( oldRotation, oldLocation, oldVelocity, true );
		}

	}

	
}




void ShipClient::DoClientSidePredictionAfterReplicationForLocalCat( uint32_t inReadState )
{
	if( ( inReadState & ESRS_Pose ) != 0 )
	{
		//simulate pose only if we received new pose- might have just gotten thrustDir
		//in which case we don't need to replay moves because we haven't warped backwards

		//all processed moves have been removed, so all that are left are unprocessed moves
		//so we must apply them...
		const MoveList& moveList = InputManager::sInstance->GetMoveList();

		for( const Move& move : moveList )
		{
			float deltaTime = move.GetDeltaTime();
			ProcessInput( deltaTime, move.GetInputState() );

			SimulateMovement( deltaTime );
		}
	}

	

}


void ShipClient::InterpolateClientSidePrediction( float inOldRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteCat )
{
	if( inOldRotation != GetRotation() && !inIsForRemoteCat )
	{
		LOG( "ERROR! Move replay ended with incorrect rotation!", 0 );
	}

	float roundTripTime = NetworkManagerClient::sInstance->GetRoundTripTime();

	if( !RoboMath::Is2DVectorEqual( inOldLocation, GetLocation() ) )
	{
		//LOG( "ERROR! Move replay ended with incorrect location!", 0 );

		//have we been out of sync, or did we just become out of sync?
		float time = Timing::sInstance.GetFrameStartTime();
		if( mTimeLocationBecameOutOfSync == 0.f )
		{
			mTimeLocationBecameOutOfSync = time;
		}

		float durationOutOfSync = time - mTimeLocationBecameOutOfSync;
		if( durationOutOfSync < roundTripTime )
		{
			SetLocation( Lerp( inOldLocation, GetLocation(), inIsForRemoteCat ? ( durationOutOfSync / roundTripTime ) : 0.1f ) );
		}
	}
	else
	{
		//we're in sync
		mTimeLocationBecameOutOfSync = 0.f;
	}


	if( !RoboMath::Is2DVectorEqual( inOldVelocity, GetVelocity() ) )
	{
		//LOG( "ERROR! Move replay ended with incorrect velocity!", 0 );

		//have we been out of sync, or did we just become out of sync?
		float time = Timing::sInstance.GetFrameStartTime();
		if( mTimeVelocityBecameOutOfSync == 0.f )
		{
			mTimeVelocityBecameOutOfSync = time;
		}

		//now interpolate to the correct value...
		float durationOutOfSync = time - mTimeVelocityBecameOutOfSync;
		if( durationOutOfSync < roundTripTime )
		{
			SetVelocity( Lerp( inOldVelocity, GetVelocity(), inIsForRemoteCat ? ( durationOutOfSync / roundTripTime ) : 0.1f ) );
		}
		//otherwise, fine...
		
	}
	else
	{
		//we're in sync
		mTimeVelocityBecameOutOfSync = 0.f;
	}
	
}


//so what do we want to do here? need to do some kind of interpolation...

void ShipClient::DoClientSidePredictionAfterReplicationForRemoteCat( uint32_t inReadState )
{
	if( ( inReadState & ESRS_Pose ) != 0 )
	{

		//simulate movement for an additional RTT
		float rtt = NetworkManagerClient::sInstance->GetRoundTripTime();
		//LOG( "Other cat came in, simulating for an extra %f", rtt );

		//let's break into framerate sized chunks though so that we don't run through walls and do crazy things...
		float deltaTime = 1.f / 30.f;

		while( true )
		{
			if( rtt < deltaTime )
			{
				SimulateMovement( rtt );
				break;
			}
			else
			{
				SimulateMovement( deltaTime );
				rtt -= deltaTime;
			}
		}
	}

}
