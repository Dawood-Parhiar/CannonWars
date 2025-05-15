#include "ClientPCH.hpp"

std::unique_ptr<TexturedWorld> TexturedWorld::sInstance;

void TexturedWorld::StaticInit()
{
	sInstance.reset(new TexturedWorld());
	sInstance->Load();
}

bool TexturedWorld::Load()
{
	if (!loadSprites())
		return false;
	return true;
}

std::vector<sf::Sprite>& TexturedWorld::getTexturedWorld()
{
	return m_spriteTiles;
}
//
//bool TexturedWorld::loadSprites()
//{
//
//	if (!mWaterTex.loadFromFile("../Assets/maps/water.png"))
//		return false;
//	mWaterTex.setRepeated(true);
//	sf::Sprite water;
//	water.setTexture(mWaterTex);
//	water.setPosition(0, 0);
//	
//	m_spriteTiles.push_back(water);
//
//	return true;
//}
bool TexturedWorld::loadSprites()
{
    // 1) WATER: repeat the water texture to cover the whole world
    sf::Texture& waterTex = *SFTextureManager::sInstance->GetTexture("water");
    waterTex.setRepeated(true);

    // Assume your world is pixel‐based: e.g. 1920×1080 or whatever
    int worldW = 1920;
    int worldH = 5000;

    sf::Sprite water;
    water.setTexture(waterTex);

    // Tell SFML to tile the texture across this rectangle:
    water.setTextureRect({ 0, 0, worldW, worldH });
    water.setPosition(0, 0);

    m_spriteTiles.push_back(water);


    // 2) MOUNTAINS: pick a texture name and some positions
    // Make sure you’ve loaded "mountain" into SFTextureManager in StaticInit.
    //auto mountainTex = SFTextureManager::sInstance->GetTexture("mountain");
    //if (!mountainTex)
      //  return false;

    // Hard-code a few island/mountain centers (in pixels)
    //std::vector<sf::Vector2f> mountainPositions = {
    //    { 400, 200 },
    //    { 1600, 150 },
    //    { 800, 500 },
    //    { 300, 900 },
    //    { 1400, 750 },
    //};

    //for (auto& pos : mountainPositions)
    //{
    //    sf::Sprite m;
    //    m.setTexture(*mountainTex);
    //    // center the sprite on the position (optional)
    //    auto ts = mountainTex->getSize();
    //    m.setOrigin(ts.x / 2.f, ts.y / 2.f);
    //    m.setPosition(pos);
    //    m_spriteTiles.push_back(m);
    //}

    return true;
}
