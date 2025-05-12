#pragma once

namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kGhost = 1 << 1,
		kParticleSystem = 1 << 2,
		kSoundEffect = 1 << 3,
		kPlayerRed = 1 << 4,
		kPlayerBlue = 1 << 5,
		kPlatform = 1 << 6,
		kRayRedPlayer = 1 << 9,
		kRayBluePlayer = 1 << 10,
		kTrap = 1 << 11,

		kPlayer = kPlayerRed | kPlayerBlue,
		kRay = kRayRedPlayer | kRayBluePlayer,
	};

	enum class ColorType
	{
		kBlue,
		kRed
	};

	enum class EPlatformType
	{
		kNone,
		kBackground,
		kAir
	};

	enum ETileType
	{
		kNon = -1,
		kBluePlayer = 5,
		kRedPlayer = 6,
		kHorizontalPlatformPart = 9,
		kHorizontalRedPlatformPart = 17,
		kHorizontalBluePlatformPart = 15,
		kVerticalPlatformPart = 10,
		kVerticalRedPlatformPart = 11,
		kVerticalBluePlatformPart = 8,
		kHorizontalImpactPlatformPart = 13,
		kVerticalImpactPlatformPart = 7,
		kHorizontalPulsePlatformPart = 16,
		kFinishPlatformPart = 18,
		kCheckpointPlatformPart = 14,
		kSpikes = 12
	};
}
