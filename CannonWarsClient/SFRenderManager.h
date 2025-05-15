#pragma once
//I take care of rendering things!

class SFRenderManager
{
public:

	static void StaticInit();
	static std::unique_ptr<SFRenderManager>	sInstance;

	void Render();
	void RenderComponents();
	void DrawNamesOnCannon();

	//vert inefficient method of tracking scene graph...
	void AddComponent(SFSpriteComponent* inComponent);
	void RemoveComponent(SFSpriteComponent* inComponent);
	int	 GetComponentIndex(SFSpriteComponent* inComponent) const;

	sf::Vector2f FindCatCentre();
	void SetLocalShipSpawned(bool cond);
	bool   IsLocalShipSpawned() const { return m_localShipSpawned; }


private:

	SFRenderManager();

	void RenderUI();
	void RenderShadows();
	void UpdateView();
	void RenderTexturedWorld();

	uint8_t FindCatHealth();
	sf::Vector2f NumberofAliveCats();

	//this can't be only place that holds on to component- it has to live inside a GameObject in the world
	vector< SFSpriteComponent* > mComponents;
	sf::View view;
	sf::Sprite m_startScreen;
	sf::Sprite m_diedScreen;
	sf::Sprite m_winnerScreen;
	sf::Vector2f m_lastCatPos;
	bool m_localShipSpawned = false;
};
