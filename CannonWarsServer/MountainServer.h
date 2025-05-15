#pragma once


class MountainServer : public Mountain
{
public:
    /// Factory method that registers with the network manager
    static GameObjectPtr StaticCreate()
    {
        return NetworkManagerServer::sInstance->RegisterAndReturn(new MountainServer());
    }

    /// When a ship runs into a MountainServer, apply damage
    virtual bool HandleCollisionWithShip(Ship* inShip) override;

protected:
    MountainServer();
};
