#pragma once

class MountainClient : public Mountain
{
public:
    static GameObjectPtr StaticCreate()
    {
        return GameObjectPtr(new MountainClient());
    }

protected:
    MountainClient();

private:
    SFSpriteComponentPtr m_sprite;
};
