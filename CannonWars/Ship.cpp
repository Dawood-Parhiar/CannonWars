#include "SocketWrapperPCH.hpp"

// somewhere central
static constexpr float WORLD_WIDTH = 1920.f;
static constexpr float WORLD_HEIGHT = 5000.f;


Ship::Ship()
    : mMaxRotationSpeed(5.f),
	mMaxLinearSpeed(5000.f),
	mVelocity(Vector3::Zero),
	mWallRestitution(0.1f),
	mShipRestitution(0.1f),
	mThrustDir(sf::Vector2f(0.f, 0.f)),
	mPlayerId(0),
	mIsShooting(false),
	mHealth(10),
	mAmmo(20)

{
    SetCollisionRadius(20.f);
}


void Ship::ProcessInput(float inDeltaTime, const InputState& inInputState)
{
	//process our input....

	int rot = -1;
	int W = 0;
	int NW = 45;
	int N = 90;
	int NE = 135;
	int E = 180;
	int SE = 225;
	int S = 270;
	int SW = 315;


	//turning...
	//float newRotation = GetRotation() + inInputState.GetDesiredHorizontalDelta() * mMaxRotationSpeed * inDeltaTime;
	//SetRotation( newRotation );

	////moving...
	//float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
	//mThrustDir = inputForwardDelta;

	float inputHorizontalDelta = inInputState.GetDesiredHorizontalDelta();
	mThrustDir.x = inputHorizontalDelta;
	float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
	mThrustDir.y = -inputForwardDelta;


	if (mThrustDir.x == 1 && mThrustDir.y == 1)
		rot = NE;
	else if (mThrustDir.x == 1 && mThrustDir.y == -1)
		rot = NW;
	else if (mThrustDir.x == -1 && mThrustDir.y == 1)
		rot = SE;
	else if (mThrustDir.x == -1 && mThrustDir.y == -1)
		rot = SW;
	else if (mThrustDir.x == 1 && mThrustDir.y == 0)
		rot = N;
	else if (mThrustDir.x == -1 && mThrustDir.y == 0)
		rot = S;
	else if (mThrustDir.x == 0 && mThrustDir.y == 1)
		rot = E;
	else if (mThrustDir.x == 0 && mThrustDir.y == -1)
		rot = W;

	if (rot != -1)
		SetRotation(rot);

	mIsShooting = inInputState.IsShooting();
}


void Ship::SimulateMovement(float inDeltaTime)
{
	//simulate us...
	AdjustVelocityByThrust(inDeltaTime);

	// Replace with a "TryMove" that preemptively checks for collisions.
	TryMove(mVelocity * inDeltaTime);
	//SetLocation( GetLocation() + mVelocity * inDeltaTime );

	// Will encompass the collisions with everything except the walls.
	ProcessCollisions();
}

void Ship::ProcessCollisionsWithScreenWalls()
{
	Vector3 pos = GetLocation();
	float   r = GetCollisionRadius();

	// Clamp X so the ship’s bounding circle never crosses the left/right edges
	pos.mX = std::max(r, std::min(pos.mX, WORLD_WIDTH - r));
	pos.mY = std::max(r, std::min(pos.mY, WORLD_HEIGHT - r));

	SetLocation(pos);
}

//void Ship::ProcessCollisions()
//{
//	// 1) Bounce off screen edges
//	ProcessCollisionsWithScreenWalls();
//
//	// 2) Gather our radius & location
//	const float sourceRadius = GetCollisionRadius();
//	const Vector3 sourceLoc = GetLocation();
//
//	// 3) Brute-force check every other GameObject
//	for (auto& goPtr : World::sInstance->GetGameObjects())
//	{
//		Ship* other = goPtr->GetAsShip();
//		if (!other || other == this || other->DoesWantToDie())
//			continue;
//
//		// 4) Sphere-sphere test in 2D
//		const Vector3 otherLoc = other->GetLocation();
//		const float   otherRadius = other->GetCollisionRadius();
//
//		Vector3 delta = otherLoc - sourceLoc;
//		float   distSq = delta.LengthSq2D();
//		float   collideDist = sourceRadius + otherRadius;
//
//		if (distSq < (collideDist * collideDist))
//		{
//			// 5) Let the other ship decide if it wants to react (e.g. take damage)
//			if (other->HandleCollisionWithShip(this))
//			{
//				// 6) Push us out of collision
//				Vector3 dir = delta;
//				dir.Normalize2D();
//				SetLocation(otherLoc - dir * collideDist);
//
//				// 7) Compute relative velocity along dir
//				Vector3 relVel = mVelocity - other->mVelocity;
//				float   velAlong = Dot2D(relVel, dir);
//
//				if (velAlong > 0.f)
//				{
//					// 8) Simple impulse resolution
//					Vector3 impulse = velAlong * dir;
//					mVelocity -= impulse * mShipRestitution;
//					other->mVelocity += impulse * other->mShipRestitution;
//				}
//			}
//		}
//	}
//}


void Ship::ProcessCollisions()
{
	//right now just bounce off the sides..
	//ProcessCollisionsWithScreenWalls();

	float sourceRadius = GetCollisionRadius();
	Vector3 sourceLocation = GetLocation();

	//now let's iterate through the world and see what we hit...
	//note: since there's a small number of objects in our game, this is fine.
	//but in a real game, brute-force checking collisions against every other object is not efficient.
	//it would be preferable to use a quad tree or some other structure to minimize the
	//number of collisions that need to be tested.
	for (auto goIt = World::sInstance->GetGameObjects().begin(), end = World::sInstance->GetGameObjects().end(); goIt != end; ++goIt)
	{
		GameObject* target = goIt->get();
		if (target->GetClassId() == 'SHIP')
		{
		}
		if (target != this && !target->DoesWantToDie())
		{
			//simple collision test for spheres- are the radii summed less than the distance?
			Vector3 targetLocation = target->GetLocation();
			float targetRadius = target->GetCollisionRadius();

			Vector3 delta = targetLocation - sourceLocation;
			float distSq = delta.LengthSq2D();
			float collisionDist = (sourceRadius + targetRadius);
			if (distSq < (collisionDist * collisionDist))
			{
				//first, tell the other guy there was a collision with a cat, so it can do something...

				if (target->HandleCollisionWithShip(this))
				{
					//okay, you hit something!
					//so, project your location far enough that you're not colliding
					Vector3 dirToTarget = delta;
					dirToTarget.Normalize2D();
					Vector3 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist;
					//important note- we only move this cat. the other cat can take care of moving itself
					SetLocation(targetLocation - acceptableDeltaFromSourceToTarget);


					Vector3 relVel = mVelocity;

					//if other object is a cat, it might have velocity, so there might be relative velocity...
					Ship* targetCat = target->GetAsShip();
					if (targetCat)
					{
						relVel -= targetCat->mVelocity;
					}

					//got vel with dir between objects to figure out if they're moving towards each other
					//and if so, the magnitude of the impulse ( since they're both just balls )
					float relVelDotDir = Dot2D(relVel, dirToTarget);

					if (relVelDotDir > 0.f)
					{
						Vector3 impulse = relVelDotDir * dirToTarget;

						if (targetCat)
						{
							mVelocity -= impulse;
							mVelocity *= mShipRestitution;
						}
						else
						{
							mVelocity -= impulse * 2.f;
							mVelocity *= mWallRestitution;
						}
					}
				}
			}
		}
	}

}


uint32_t Ship::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
	uint32_t writtenState = 0;

	if (inDirtyState & ESRS_PlayerId)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(GetPlayerId());

		writtenState |= ESRS_PlayerId;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}


	if (inDirtyState & ESRS_Pose)
	{
		inOutputStream.Write((bool)true);

		Vector3 velocity = mVelocity;
		inOutputStream.Write(velocity.mX);
		inOutputStream.Write(velocity.mY);

		Vector3 location = GetLocation();
		inOutputStream.Write(location.mX);
		inOutputStream.Write(location.mY);

		inOutputStream.Write(GetRotation());

		writtenState |= ESRS_Pose;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	//always write mThrustDir- it's just two bits
	if (mThrustDir != sf::Vector2f(0.f, 0.f))
	{
		inOutputStream.Write(true);
		inOutputStream.Write(mThrustDir.x > 0.f);
		inOutputStream.Write(mThrustDir.y > 0.f);
	}
	else
	{
		inOutputStream.Write(false);
	}

	if (inDirtyState & ESRS_Color)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(GetColor());

		writtenState |= ESRS_Color;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	if (inDirtyState & ESRS_Health)
	{
		inOutputStream.Write((bool)true);
		inOutputStream.Write(mHealth, 4);

		writtenState |= ESRS_Health;
	}
	else
	{
		inOutputStream.Write((bool)false);
	}

	return writtenState;
}

void Ship::Update()
{
	//GameObject::Update();
}

void Ship::TryMove(Vector3 p_move)
{
	Vector3 newPos = GetLocation() + p_move;

	// wrap X
	if (newPos.mX < 0.f)               newPos.mX += WORLD_WIDTH;
	else if (newPos.mX > WORLD_WIDTH) newPos.mX -= WORLD_WIDTH;

	// wrap Y
	if (newPos.mY < 0.f)                newPos.mY += WORLD_HEIGHT;
	else if (newPos.mY > WORLD_HEIGHT) newPos.mY -= WORLD_HEIGHT;

	SetLocation(newPos);
}

void Ship::AdjustVelocityByThrust(float inDeltaTime)
{
	//just set the velocity based on the thrust direction -- no thrust will lead to 0 velocity
	//simulating acceleration makes the client prediction a bit more complex
	Vector3 forwardVector = GetForwardVector();
	//mVelocity = forwardVector * (mThrustDir * inDeltaTime * mMaxLinearSpeed);
	mVelocity = Vector3(GetForwardVector().mX + (mThrustDir.x * inDeltaTime * mMaxLinearSpeed), GetForwardVector().mY + (mThrustDir.y * inDeltaTime * mMaxLinearSpeed), GetForwardVector().mZ);
}
