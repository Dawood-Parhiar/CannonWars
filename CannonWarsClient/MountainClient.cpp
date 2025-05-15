#include "ClientPCH.hpp"


MountainClient::MountainClient()
{
    // Attach our SFML sprite component
    m_sprite.reset(new SFSpriteComponent(this));

    // Use the same texture ID you registered in SFTextureManager::StaticInit()
    auto tex = SFTextureManager::sInstance->GetTexture("mountain");
    if (tex)
    {
        m_sprite->SetTexture(tex);
    }
    else
    {
        // Fallback or log if the texture wasn't loaded
        std::cout << "MountainClient: failed to get 'mountain' texture";
    }
}
