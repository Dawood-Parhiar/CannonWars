#pragma once

namespace Category
{
	enum RecieverType

	{
		kNone = 0,
		kScene = 1 << 0,
		kPlayerShip = 1 << 1,
		kAlliedShip = 1 << 2,
		kEnemyShip = 1 << 3,
		kAlliedProjectile = 1 << 4,
		kEnemyProjectile = 1 << 5,
		kPickup = 1 << 6,
		kParticleSystem = 1 << 7,
		kSoundEffect = 1 << 8,
		kObstacle = 1 << 9,
		kCoin = 1 << 10,
		kCannon = 1 << 11,
		kNetwork = 1 << 12,

		kShip = kPlayerShip | kAlliedShip | kEnemyShip,
		kProjectile = kAlliedProjectile | kEnemyProjectile,
	};

	enum class ProjectileType
	{
		kMissile,
		kBullet
	};

	enum class EPlatformType
	{
		kNone,
		kBackground,
		kAir
	};

	enum Action
	{
		kMoveUp,
		kMoveDown,
		kRotateRight,
		kRotateLeft,
		kMissileFire,
		kRotateCannonLeft,
		kRotateCannonRight,
		kActionCount
	};

	enum class ShipType
	{
		kPlayerShip,
		kEnemyShip
	};
}
