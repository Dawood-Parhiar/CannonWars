#include "SocketWrapperPCH.hpp"

Player::Player() : m_health(10), m_ammo(3), m_name("Default"), m_position(10, 10, 10), m_rotation(0, 0, 0, 1), m_weapons{ 60, 70, 80, 90, 100 }
{
}

uint32_t Player::GetHealth() const
{
	return m_health;
}

uint32_t Player::GetAmmo() const
{
	return m_ammo;
}

void Player::Write(OutputMemoryStream& out_stream) const
{
	out_stream.Write(m_health);
	out_stream.Write(m_ammo);
	out_stream.Write(m_name, 128);
	out_stream.Write(m_position);
	out_stream.Write(m_rotation);
	out_stream.Write(m_weapons);
}

void Player::Read(InputMemoryStream& in_stream)
{
	in_stream.Read(m_health);
	in_stream.Read(m_ammo);
	in_stream.Read(m_name, 128);
	in_stream.Read(m_position);
	in_stream.Read(m_rotation);
	in_stream.Read(m_weapons);
}

void Player::ToString() const
{
	std::cout << m_name << " has health: " << m_health << ", Ammo: " << m_ammo << " Position: " << "(" << m_position.mX << ", " << m_position.mY << ", " << m_position.mZ << ")" << " Quaternion: " << "(" << m_rotation.mX << ", " << m_rotation.mY << ", " << m_rotation.mZ << ", " << m_rotation.mW << ")" << std::endl;
	for (const int m_weapon : m_weapons)
	{
		std::cout << m_weapon << std::endl;
	}
	return;
}
