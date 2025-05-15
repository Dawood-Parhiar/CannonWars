#pragma once

class ShipClient : public Ship
{
public:
	static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr( new ShipClient() ); }

	virtual void Update();
	virtual void	HandleDying() override;

	virtual void	Read( InputMemoryBitStream& inInputStream ) override;

	void DoClientSidePredictionAfterReplicationForLocalCat( uint32_t inReadState );
	void DoClientSidePredictionAfterReplicationForRemoteCat( uint32_t inReadState );



protected:
	ShipClient();


private:
	sf::Vector2f		mShipInfo;
	void InterpolateClientSidePrediction( float inOldRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteCat );

	float				mTimeLocationBecameOutOfSync;
	float				mTimeVelocityBecameOutOfSync;
	
	// Either use a fake time that is based on ticks instead of time.
	// or, do an internal timer contained within the class.
	sf::Time m_gameTime;
	SFSpriteComponentPtr m_sprite, m_cannonSprite;
	SFHealthSpriteComponentPtr m_healthSprite;

	bool m_textureIsDirty;
};