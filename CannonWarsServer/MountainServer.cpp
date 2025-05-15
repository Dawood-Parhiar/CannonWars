#include "ServerPCH.h"


MountainServer::MountainServer()
{
    // If your base Mountain ctor didn’t already set a radius, do it here:
    SetCollisionRadius(64.f);
}

bool MountainServer::HandleCollisionWithShip(Ship* inShip)
{
   
    uint8_t h = inShip->GetHealth();
    h = (h > 0? h - 0.1 : 0);
    inShip->SetHealth(h);

    // Mark that ship’s health dirty so it replicates to clients
    const uint32_t ECRS_Health = 1 << 3;
    NetworkManagerServer::sInstance
		->SetStateDirty(inShip->GetNetworkId(), ECRS_Health);
    if (h == 0)
    {
        inShip->SetDoesWantToDie(true);
    }

    //Compute bounce direction & overlap
    Vector3 shipLoc = inShip->GetLocation();
    Vector3 mountainLoc = GetLocation();
    Vector3 dir = shipLoc - mountainLoc;
    dir.Normalize2D();

    float overlap = inShip->GetCollisionRadius() + GetCollisionRadius()
        - (shipLoc - mountainLoc).Length2D();
    if (overlap < 0.f) overlap = 0.f;

    //Push the ship out so it just touches the mountain
    Vector3 newLoc = shipLoc + dir * overlap;
    inShip->SetLocation(newLoc);

    // Reflect the ship’s velocity along the collision normal
    Vector3 vel = inShip->GetVelocity();
    float  vn = RoboMath::Dot2D(vel, dir);
    Vector3 velNorm = dir * vn;
    Vector3 velTang = vel - velNorm;
    float  restitution = 0.2f;  // 0=bull-sticky, 1=perfectly elastic
    Vector3 newVel = velTang - velNorm * restitution;
    inShip->SetVelocity(newVel);

    // return false so your Ship::ProcessCollisions bounce logic still applies / true otherwise
    return true;
}
