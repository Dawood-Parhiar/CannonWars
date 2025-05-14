#pragma once
enum ECatControlType
{
	ESCT_Human,
	ESCT_AI
};

class ShipServer : public Ship
{
public:
	static GameObjectPtr	StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn( new ShipServer() ); }
	virtual void HandleDying() override;

	virtual void Update();

	void SetCatControlType( ECatControlType inCatControlType ) { mCatControlType = inCatControlType; }

	void TakeDamage( int inDamagingPlayerId );

protected:
	ShipServer();

private:

	void HandleShooting();

	ECatControlType	mCatControlType;


	float		mTimeOfNextShot;
	float		mTimeBetweenShots;

};