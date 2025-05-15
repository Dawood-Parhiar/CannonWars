#include "ClientPCH.hpp"

std::unique_ptr< SFRenderManager >	SFRenderManager::sInstance;

SFRenderManager::SFRenderManager()
{
	// Might need some view stuff in here or something.
	view.reset(sf::FloatRect(0, 0, 800, 600));
	SFWindowManager::sInstance->setView(view);
	m_startScreen.setTexture(*SFTextureManager::sInstance->GetTexture("start_screen"));
	m_diedScreen.setTexture(*SFTextureManager::sInstance->GetTexture("died_screen"));
	m_winnerScreen.setTexture(*SFTextureManager::sInstance->GetTexture("winner_screen"));
}

void SFRenderManager::RenderUI()
{
	sf::Font bebas = *FontManager::sInstance->GetFont("bebas");

	sf::Text RTT, InOut;

	sf::Vector2f basePos(view.getCenter().x - view.getSize().x / 2, view.getCenter().y - view.getSize().y / 2);

	RTT.setPosition(basePos.x + 20, basePos.y + 20);
	InOut.setPosition(basePos.x + 120, basePos.y + 20);

	RTT.setFont(bebas);
	InOut.setFont(bebas);

	RTT.setCharacterSize(24);
	InOut.setCharacterSize(24);
	
	RTT.setFillColor(sf::Color::Red);
	InOut.setFillColor(sf::Color::Red);
	
	// RTT
	float rttMS = NetworkManagerClient::sInstance->GetAvgRoundTripTime().GetValue() * 1000.f;
	string roundTripTime = StringUtils::Sprintf("RTT %d ms", (int)rttMS);
	RTT.setString(roundTripTime);

	// Bandwidth
	string bandwidth = StringUtils::Sprintf("In %d  Bps, Out %d Bps",
		static_cast< int >(NetworkManagerClient::sInstance->GetBytesReceivedPerSecond().GetValue()),
		static_cast< int >(NetworkManagerClient::sInstance->GetBytesSentPerSecond().GetValue()));

	InOut.setString(bandwidth);

	// Draw the text to screen.
	SFWindowManager::sInstance->draw(RTT);
	SFWindowManager::sInstance->draw(InOut);
}

void SFRenderManager::RenderShadows()
{
	/*sf::Vector2f player;
	if (FindCatCentre() == sf::Vector2f(-1, -1))
		player = m_lastCatPos;
	else
		player = FindCatCentre();
	auto cen = view.getCenter();
	auto size = view.getSize();

	sf::FloatRect bounds(cen.x - (size.x / 2), cen.y - (size.y / 2), size.x, size.y);
	
	// Optimization debug stuff.
	/*
	sf::FloatRect bounds(view.getCenter().x - (size.x / 2 / 2), view.getCenter().y - (size.y / 2 / 2), size.x / 2, size.y / 2);
	sf::RectangleShape r;
	r.setPosition(bounds.left, bounds.top);
	r.setSize(sf::Vector2f(bounds.width, bounds.height));
	r.setOutlineThickness(5);
	r.setFillColor(sf::Color::Transparent);
	r.setOutlineColor(sf::Color::Red);
	
	
	auto shadows = ShadowFactory::sInstance->getShadows(player, sf::Color::Black, bounds);
	for (auto s : shadows)
	{
		SFWindowManager::sInstance->draw(s);
	}
	//SFWindowManager::sInstance->draw(r);

	*/
}

void SFRenderManager::UpdateView()
{
	// Lower rate means more 'lag' on the camera following the player.
	float rate = .02f;
	if (FindCatCentre() != sf::Vector2f(-1, -1))
	{
		sf::Vector2f player = FindCatCentre();
		sf::Vector2f newCentre = view.getCenter() + ((player - view.getCenter()) * rate);
		view.setCenter(newCentre);
	}
	SFWindowManager::sInstance->setView(view);
}

void SFRenderManager::RenderTexturedWorld()
{
	for (auto spr : TexturedWorld::sInstance->getTexturedWorld())
	{
		SFWindowManager::sInstance->draw(spr);
	}
}

// Way of finding this clients cat, and then centre point. - Ronan
sf::Vector2f SFRenderManager::FindCatCentre()
{
	uint32_t catID = (uint32_t)'SHIP';
	for (auto obj : World::sInstance->GetGameObjects())
	{
		// Find a cat.
		if (obj->GetClassId() == catID)
		{
			Ship *ship = dynamic_cast<Ship*>(obj.get());
			auto id = ship->GetPlayerId();
			auto ourID = NetworkManagerClient::sInstance->GetPlayerId();
			if (id == ourID)
			{
				// If so grab the centre point.
				auto centre = ship->GetLocation();
				m_lastCatPos.x = centre.mX;
				m_lastCatPos.y = centre.mY;
				return sf::Vector2f(centre.mX, centre.mY);
			}
		}
	}
	return sf::Vector2f(-1, -1);
}

void SFRenderManager::SetLocalShipSpawned(bool spawned)
{
	m_localShipSpawned = spawned;
}

//using ronans code above to get the players details for the HUD
uint8_t SFRenderManager::FindCatHealth()
{
	uint32_t shipID = (uint32_t)'SHIP';
	for (auto obj : World::sInstance->GetGameObjects())
	{
		// Find a cat.
		if (obj->GetClassId() == shipID)
		{
			Ship *ship = dynamic_cast<Ship*>(obj.get());
			auto id = ship->GetPlayerId();
			auto ourID = NetworkManagerClient::sInstance->GetPlayerId();
			if (id == ourID)
			{
				return ship->GetHealth();
			}
		}
	}
	return 0;
}

// Returns the alive cats in the X, and the total numbers of cats in the Y.
sf::Vector2f SFRenderManager::NumberofAliveCats()
{
	int numberOfCats = 0;
	int aliveCats = 0;
	uint32_t catID = (uint32_t)'SHIP';
	for (auto obj : World::sInstance->GetGameObjects())
	{
		// Find a cat.
		if (obj->GetClassId() == catID)
		{
			Ship *ship = dynamic_cast<Ship*>(obj.get());
			numberOfCats++;
			if (ship->GetHealth() > 0)
				aliveCats++;
		}
	}
	return sf::Vector2f(aliveCats, numberOfCats);
}

void SFRenderManager::StaticInit()
{
	sInstance.reset(new SFRenderManager());
}


void SFRenderManager::AddComponent(SFSpriteComponent* inComponent)
{
	mComponents.push_back(inComponent);
}

void SFRenderManager::RemoveComponent(SFSpriteComponent* inComponent)
{
	int index = GetComponentIndex(inComponent);

	if (index != -1)
	{
		int lastIndex = mComponents.size() - 1;
		if (index != lastIndex)
		{
			mComponents[index] = mComponents[lastIndex];
		}
		mComponents.pop_back();
	}
}

int SFRenderManager::GetComponentIndex(SFSpriteComponent* inComponent) const
{
	for (int i = 0, c = mComponents.size(); i < c; ++i)
	{
		if (mComponents[i] == inComponent)
		{
			return i;
		}
	}

	return -1;
}


//this part that renders the world is really a camera-
//in a more detailed engine, we'd have a list of cameras, and then render manager would
//render the cameras in order
void SFRenderManager::RenderComponents()
{
	//Get the logical viewport so we can pass this to the SpriteComponents when it's draw time

	for (SFSpriteComponent* c : mComponents)
	{
		SFHealthSpriteComponent* ptr = dynamic_cast<SFHealthSpriteComponent*>(c);
		if (ptr)
			SFWindowManager::sInstance->draw(ptr->GetSprite());
		else
			SFWindowManager::sInstance->draw(c->GetSprite());
	}
}

void SFRenderManager::DrawNamesOnCannon()
{
	if (mComponents.size() > 0)
	{
		UpdateView();

		RenderTexturedWorld();
		RenderComponents();

		// --- draw each ship's name above its cannon ---
		auto& window = *SFWindowManager::sInstance;
		auto font = *FontManager::sInstance->GetFont("bebas");

		for (auto& go : World::sInstance->GetGameObjects())
		{
			if (go->GetClassId() == uint32_t('SHIP'))
			{
				Ship* ship = static_cast<Ship*>(go.get());
				// determine the name
				std::string name;
				auto playerId = ship->GetPlayerId();
				if (playerId == NetworkManagerClient::sInstance->GetPlayerId())
				{
					name = ConnectionDetails::sInstance->GetClientName();
				}
				else
				{
					auto entry = ScoreBoardManager::sInstance->GetEntry(playerId);
					name = entry ? entry->GetPlayerName() : "???";
				}

				// build the text
				sf::Text txt(name, font, 18);
				txt.setFillColor(sf::Color::White);
				// center horizontally
				auto b = txt.getLocalBounds();
				txt.setOrigin(b.width / 2, b.height);
				// position just above the cannon
				auto loc = ship->GetLocation();
				txt.setPosition(loc.mX, loc.mY - ship->GetCollisionRadius() - 5.f);

				window.draw(txt);
			}
		}
	}
}

void SFRenderManager::Render()
{

	// Clear the back buffer
	SFWindowManager::sInstance->clear(sf::Color::Black);

	// The game has started.
	if (!SFRenderManager::sInstance->IsLocalShipSpawned())
	{
		SFWindowManager::sInstance->draw(m_startScreen);
	}

		// Update the view position.
		UpdateView();

		SFRenderManager::sInstance->RenderTexturedWorld();

		SFRenderManager::sInstance->RenderComponents();


		DrawNamesOnCannon();

		// Draw UI elements.
		SFRenderManager::sInstance->RenderUI();

		// Player is dead.
		if (m_localShipSpawned && FindCatCentre() == sf::Vector2f(-1, -1))
		{
			// Print some you are dead screen
			sf::Vector2f died(view.getCenter().x - view.getSize().x / 2, view.getCenter().y - view.getSize().y / 2);
			m_diedScreen.setPosition(died);
			SFWindowManager::sInstance->draw(m_diedScreen);
		}
		else
		{
			// We are the last man standing.
			sf::Vector2f cats = NumberofAliveCats();

			
			if (cats.x == 1.f && FindCatHealth() > 0 && 
				ScoreBoardManager::sInstance->GetEntry(NetworkManagerClient::sInstance->GetPlayerId())->GetScore() > 0)
			{
				// Draw some you are the winner screen.
				sf::Vector2f winner(view.getCenter().x - view.getSize().x / 2, view.getCenter().y - view.getSize().y / 2);
				m_winnerScreen.setPosition(winner);
				SFWindowManager::sInstance->draw(m_winnerScreen);
			}
		}

	
	

	// Present our back buffer to our front buffer
	SFWindowManager::sInstance->display();
}
