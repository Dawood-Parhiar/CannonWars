#include "SocketWrapperPCH.hpp"


Mountain::Mountain()
{
    // size this to match your mountain sprite
    SetScale(GetScale() * 0.5f);
    SetCollisionRadius(64.f);

}

uint32_t Mountain::Write(OutputMemoryBitStream& os, uint32_t dirtyState) const
{
    uint32_t written = 0;
    if (dirtyState & EMRS_Pose)
    {
        os.Write(true);
        auto loc = GetLocation();
        os.Write(loc.mX);
        os.Write(loc.mY);
        os.Write(GetRotation());
        written |= EMRS_Pose;
    }
    else
        os.Write(false);

    return written;
}

void Mountain::Read(InputMemoryBitStream& is)
{
    bool flag;
    is.Read(flag);
    if (flag)
    {
        Vector3 loc;
        is.Read(loc.mX);
        is.Read(loc.mY);
        SetLocation(loc);
        float rot; is.Read(rot);
        SetRotation(rot);
    }
}

bool Mountain::HandleCollisionWithShip(Ship* inShip)
{

    // Bounce logic (same as server)
    Vector3 shipLoc = inShip->GetLocation();
    Vector3 mountainLoc = GetLocation();
    Vector3 dir = shipLoc - mountainLoc;
    dir.Normalize2D();

    float overlap = inShip->GetCollisionRadius() + GetCollisionRadius()
        - (shipLoc - mountainLoc).Length2D();
    if (overlap < 0.f) overlap = 0.f;

    inShip->SetLocation(shipLoc + dir * overlap);

    Vector3 vel = inShip->GetVelocity();
    float  vn = Dot2D(vel, dir);
    Vector3 velNorm = dir * vn;
    Vector3 velTang = vel - velNorm;
    float  restitution = 0.2f;
    inShip->SetVelocity(velTang - velNorm * restitution);

    // Play a thud sound
    SoundManager::sInstance->PlaySound(SoundManager::STP_Death);

    return false; // skip default bounce
}
