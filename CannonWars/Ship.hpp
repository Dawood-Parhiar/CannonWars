﻿#pragma once

class InputState;
class OutputMemoryStream;
class InputMemoryStream;

class Ship : public GameObject
{
public:
    CLASS_IDENTIFICATION('SHIP', GameObject)

        enum EShipReplicationState 
    {
        ESRS_Pose = 1 << 0,
        ESRS_Color = 1 << 1,
        ESRS_PlayerId = 1 << 2,
        ESRS_Health = 1 << 3,
        ESRS_CannonLeft = 1 << 4,
        ESRS_CannonRight = 1 << 5,
        //ESRS_Shooting = 1 << 4,

        ESRS_AllState = ESRS_Pose | ESRS_Color | ESRS_PlayerId | ESRS_Health | ESRS_CannonLeft | ESRS_CannonRight
    };

    static GameObject* StaticCreate() { return new Ship(); }

    virtual uint32_t GetAllStateMask() const override { return ESRS_AllState; }

    virtual	Ship* GetAsShip() override { return this; }
   /* virtual bool HandleCollisionWithShip(Ship* inShip) override
    {
        return true;
    }*/
    virtual void Update() override;


    void ProcessInput(float inDeltaTime, const InputState& inInputState);
    void SimulateMovement(float inDeltaTime);

    void ProcessCollisionsWithScreenWalls();
    void ProcessCollisions();

    void		SetPlayerId(uint32_t inPlayerId) { mPlayerId = inPlayerId; }
    uint32_t	GetPlayerId()						const { return mPlayerId; }

    void			SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }
    const Vector3& GetVelocity()						const { return mVelocity; }

    uint8_t& GetHealth() { return mHealth; };

    void SetHealth(uint8_t& health) { mHealth = health; }

    virtual uint32_t	Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) const override;

    float GetCannonRotation() const { return mCannonRotation; }
    void  SetCannonRotation(float r) { mCannonRotation = r; }


protected:
    Ship();  

    float mCannonRotation;
    float mMaxCannonRotationSpeed;

private:

    void TryMove(Vector3 p_move);

    void AdjustVelocityByThrust(float inDeltaTime);


    Vector3   mVelocity;

    float     mMaxLinearSpeed;
    float     mMaxRotationSpeed;

    uint32_t  mPlayerId;
	uint8_t   mAmmo;

    float				mWallRestitution;
    float				mShipRestitution;

protected:
    float     mLastMoveTimestamp;
    sf::Vector2f mThrustDir;
    uint8_t   mHealth;
    bool      mIsShooting;
    bool    mCannonLeft = false;    // true while the player is holding the “turn cannon left” key
    bool    mCannonRight = false;
};

typedef std::shared_ptr<Ship> ShipPtr;
