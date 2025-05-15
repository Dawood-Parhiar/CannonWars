#pragma once
#include "MemoryBitStream.hpp"

class InputState
{
public:

	InputState() :
		mDesiredRightAmount(0),
		mDesiredLeftAmount(0),
		mDesiredForwardAmount(0),
		mDesiredBackAmount(0),
		mIsShooting(false),
		mCannonLeft(false),
		mCannonRight(false)
	{}

	float GetDesiredHorizontalDelta()	const { return mDesiredRightAmount - mDesiredLeftAmount; }
	float GetDesiredVerticalDelta()		const { return mDesiredForwardAmount - mDesiredBackAmount; }
	bool  IsShooting()					const { return mIsShooting; }

	bool Write(OutputMemoryBitStream& inOutputStream) const;
	bool Read(InputMemoryBitStream& inInputStream);

	bool GetCannonLeft() const { return mCannonLeft; }
	bool GetCannonRight() const { return mCannonRight; }

private:
	friend class InputManager;

	float	mDesiredRightAmount, mDesiredLeftAmount;
	float	mDesiredForwardAmount, mDesiredBackAmount;
	bool	mIsShooting;
	bool mCannonLeft;
	bool mCannonRight;

};

