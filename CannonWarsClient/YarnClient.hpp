#pragma once

class YarnClient : public Yarn
{
public:
	static	GameObjectPtr	StaticCreate() { return GameObjectPtr(new YarnClient()); }

	virtual void		Read(InputMemoryBitStream& inInputStream) override;
	virtual bool		HandleCollisionWithShip(Ship* inCat) override;

protected:
	YarnClient();

private:
	SFSpriteComponentPtr m_sprite;
};