#pragma once
class Player
{
public:
	Player();
	uint32_t GetHealth() const;
	uint32_t GetAmmo() const;
	void Write(OutputMemoryStream& out_stream) const;
	void Read(InputMemoryStream& in_stream);
	void ToString() const;

private:
	uint32_t m_health;
	uint32_t m_ammo;
	char m_name[128];
	Vector3 m_position;
	Quaternion m_rotation;
	vector<int> m_weapons;
};

