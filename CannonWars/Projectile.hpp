#pragma once

class Projectile
{
	
};

//
//class Projectile : public GameObject
//{
//public:
//	CLASS_IDENTIFICATION('PROJ', GameObject)
//
//		Projectile(int ownerId, Vector3 startPos, Vector3 velocity);
//
//	uint32_t GetAllStateMask() const override { return Ship::kPos | Ship::kVel | Ship::kCoins; }
//
//	void Update(float dt) override;
//	
//	unsigned GetCategory() const override { return Category::kProjectile; }
//
//	int     GetOwner() const { return _owner; }
//
//private:
//	int     _owner;
//	Vector3 _pos, _vel;
//};
