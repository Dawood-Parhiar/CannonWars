#include "ClientPCH.hpp"

std::unique_ptr<sf::RenderWindow> SFWindowManager::sInstance;

bool SFWindowManager::StaticInit()
{
	sInstance.reset(new sf::RenderWindow(sf::VideoMode(1200, 900), "Cannon Wars!"));
	return true;
}