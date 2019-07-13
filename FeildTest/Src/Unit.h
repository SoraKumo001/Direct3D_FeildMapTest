#include "aflDirect3DUnit.h"
#include "aflInput.h"

#pragma once

class World;
class UnitChara : public AFL::DIRECT3D::Unit
{
public:
	enum COMMAND
	{
		moveFRONT=1,
		moveBACK=2,
		moveRIGHT=4,
		moveLEFT=8,
		moveJUMP=16,
		atackNormal=32
	};
	enum STAT
	{
		statStatic=1,
		statMove=2,
		statAtack=4,
		statJump=8,
		statDied=16
	};

	UnitChara();
	virtual ~UnitChara();

	void setSpeed(FLOAT speed){m_speed = speed;}
	void setCommand(COMMAND command);
	void setBounding();
	bool isHit(UnitChara* unitChara);
	bool isHitWepon(UnitChara* unitChara);

	int getStatus(){return m_stat;}
	void setStatus(int stat){m_stat = stat;}
	void setInput(AFL::Input* input){m_input = input;}

	FLOAT getMoveSpeed()
	{
		return sqrtf(m_moveVector.x*m_moveVector.x+m_moveVector.y*m_moveVector.y);
	}
	void setContactVector(FLOAT x,FLOAT y)
	{
		m_contactVector.x = x;
		m_contactVector.y = y;
	}
	void setImageIndex(INT index)
	{
		m_imageIndex = index;
	}
	INT getImageIndex() const
	{
		return m_imageIndex;
	}
	void setWeponFrame(LPCSTR name)
	{
		m_frameWepon = getFrame(name);
	}
protected:
	virtual bool onRender(AFL::DIRECT3D::World* world,FLOAT& x,FLOAT& y,FLOAT& z);
	virtual void onAction(AFL::DIRECT3D::World* world,LPVOID value);
	virtual void onStart(AFL::DIRECT3D::World* world,LPVOID value);

	NVector* getBoundingVector(){return m_bounding;}
	NVector m_contactVector;
	NVector m_moveVector;

	void input();
private:
	INT m_imageIndex;
	DWORD m_command;
	FLOAT m_speed;
	FLOAT m_jump;
	NVector m_bounding[2];
	NVector m_vectBoundingWepon[2];
	int m_stat;
	AFL::Input* m_input;
	AFL::DIRECT3D::Frame* m_frameWepon;
};
