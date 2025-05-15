#pragma once
#include "GameObject.hpp"

class Mountain : public GameObject
{
public:
    CLASS_IDENTIFICATION('MOUN', GameObject)

        enum EMountainReplicationState : uint32_t
    {
        EMRS_Pose = 1 << 0,
        EMRS_AllState = EMRS_Pose
    };

    static GameObject* StaticCreate() { return new Mountain(); }

    Mountain();

    virtual uint32_t       GetAllStateMask() const override { return EMRS_AllState; }
    virtual uint32_t       Write(OutputMemoryBitStream& os, uint32_t dirtyState) const override;
    virtual void           Read(InputMemoryBitStream& is) override;
    virtual bool           HandleCollisionWithShip(Ship* inShip) override;

protected:
    bool picked = false;
    // no extra data needed
};
