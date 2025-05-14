#include "ClientPCH.hpp"

std::unique_ptr<PlayerTextureGenerator> PlayerTextureGenerator::sInstance;

PlayerTextureGenerator::PlayerTextureGenerator()
{
	m_playerTextureIDs = {
		"yellow_ship", 
		"white_ship",
		"grey_ship",
		"red_ship",
		"green_ship",
		"blue_ship",
		"pirate_ship",
		"broken_ship"
	};
}

SFTexturePtr PlayerTextureGenerator::GetPlayerTexure(uint32_t p_id)
{
	
	return SFTextureManager::sInstance->GetTexture(ResolveID(p_id));
}

std::string PlayerTextureGenerator::ResolveID(uint32_t p_id)
{
	return m_playerTextureIDs[p_id % m_playerTextureIDs.size()];
}

bool PlayerTextureGenerator::StaticInit()
{
	sInstance.reset(new PlayerTextureGenerator());
	return true;
}
