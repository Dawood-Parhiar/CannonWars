#include "SocketWrapperPCH.hpp"

// somewhere central
static constexpr float WORLD_WIDTH = 2000.f;
static constexpr float WORLD_HEIGHT = 1200.f;


Ship::Ship()
    : mVelocity(0.f,0.f,0.f)
    , mMaxLinearSpeed(500.f)
    , mMaxRotationSpeed(3.f)
	, mAmmo(20)
    , mPlayerId(0)
    , mLastMoveTimestamp(0.f)
    , mThrustDir(0.f, 0.f)
    , mHealth(100)
    , mIsShooting(false)
	, mWallRestitution(0.1f)
	, mShipRestitution(0.1f)
{
    SetCollisionRadius(20.f);
}

void Ship::Update()
{
    //GameObject::Update();
}

//void Ship::ProcessInput(float inDeltaTime, const InputState& inInputState)
//{
//	//process our input....
//
//	int rot = -1;
//	int W = 0;
//	int NW = 45;
//	int N = 90;
//	int NE = 135;
//	int E = 180;
//	int SE = 225;
//	int S = 270;
//	int SW = 315;
//
//
//	//turning...
//	//float newRotation = GetRotation() + inInputState.GetDesiredHorizontalDelta() * mMaxRotationSpeed * inDeltaTime;
//	//SetRotation( newRotation );
//
//	////moving...
//	//float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
//	//mThrustDir = inputForwardDelta;
//
//	float inputHorizontalDelta = inInputState.GetDesiredHorizontalDelta();
//	mThrustDir.x = inputHorizontalDelta;
//	float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
//	mThrustDir.y = -inputForwardDelta;
//
//
//	if (mThrustDir.x == 1 && mThrustDir.y == 1)
//		rot = NE;
//	else if (mThrustDir.x == 1 && mThrustDir.y == -1)
//		rot = NW;
//	else if (mThrustDir.x == -1 && mThrustDir.y == 1)
//		rot = SE;
//	else if (mThrustDir.x == -1 && mThrustDir.y == -1)
//		rot = SW;
//	else if (mThrustDir.x == 1 && mThrustDir.y == 0)
//		rot = N;
//	else if (mThrustDir.x == -1 && mThrustDir.y == 0)
//		rot = S;
//	else if (mThrustDir.x == 0 && mThrustDir.y == 1)
//		rot = E;
//	else if (mThrustDir.x == 0 && mThrustDir.y == -1)
//		rot = W;
//
//	if (rot != -1)
//		SetRotation(rot);
//
//	mIsShooting = inInputState.IsShooting();
//}

void Ship::ProcessInput(float inDeltaTime, const InputState& inInputState)
{
	// 1) Get raw axis input (-1, 0, +1)
	float ix = inInputState.GetDesiredHorizontalDelta();  // A = -1, D = +1
	float iy = inInputState.GetDesiredVerticalDelta();    // S = -1, W = +1

	// 2) Build a vector; invert Y if your world Y+ is down (SFML), else drop the '* -1'
	sf::Vector2f inVec(ix, -iy);

	// 3) If there is any input, normalize and compute angle; otherwise zero thrust
	if (inVec.x != 0.f || inVec.y != 0.f)
	{
		// normalize
		float len = std::sqrt(inVec.x * inVec.x + inVec.y * inVec.y);
		inVec.x /= len;
		inVec.y /= len;

		// store thrust direction
		mThrustDir = inVec;

		// compute angle in radians, then to degrees
		// atan2(y, x): zero is along +X axis, positive rotates toward +Y
		float angleRad = std::atan2(inVec.y, inVec.x);
		float angleDeg = angleRad * (180.f / 3.14159265f);

		SetRotation(angleDeg);
	}
	else
	{
		mThrustDir = { 0.f, 0.f };
	}

	// 4) Shooting remains as before
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

void Ship::ProcessCollisions()
{
	// 1) Bounce off screen edges
	ProcessCollisionsWithScreenWalls();

	// 2) Gather our radius & location
	const float sourceRadius = GetCollisionRadius();
	const Vector3 sourceLoc = GetLocation();

	// 3) Brute-force check every other GameObject
	for (auto& goPtr : World::sInstance->GetGameObjects())
	{
		Ship* other = goPtr->GetAsShip();
		if (!other || other == this || other->DoesWantToDie())
			continue;

		// 4) Sphere-sphere test in 2D
		const Vector3 otherLoc = other->GetLocation();
		const float   otherRadius = other->GetCollisionRadius();

		Vector3 delta = otherLoc - sourceLoc;
		float   distSq = delta.LengthSq2D();
		float   collideDist = sourceRadius + otherRadius;

		if (distSq < (collideDist * collideDist))
		{
			// 5) Let the other ship decide if it wants to react (e.g. take damage)
			if (other->HandleCollisionWithShip(this))
			{
				// 6) Push us out of collision
				Vector3 dir = delta;
				dir.Normalize2D();
				SetLocation(otherLoc - dir * collideDist);

				// 7) Compute relative velocity along dir
				Vector3 relVel = mVelocity - other->mVelocity;
				float   velAlong = Dot2D(relVel, dir);

				if (velAlong > 0.f)
				{
					// 8) Simple impulse resolution
					Vector3 impulse = velAlong * dir;
					mVelocity -= impulse * mShipRestitution;
					other->mVelocity += impulse * other->mShipRestitution;
				}
			}
		}
	}
}

uint32_t Ship::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const
{
    return GameObject::Write(inOutputStream, inDirtyState);
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
