#pragma once
#if !defined(_OPENGL) & !defined(__ANDROID__)
	#include "aflD3DUnit.h"
#else
	#include "aflOpenGLUnit.h"
#endif
#include "aflInput.h"

using namespace AFL;

class UnitLife : public UnitVector
{
public:
	UnitLife()
	{
		m_frame = 2.0f;
		m_size.x = 128.0f;
		m_size.y = 16.0f;
		setView2D(false);
		setZBuffer(false);

		m_vsName = "VECTOR2";
		setPosW(1.0f);
	}
	void setValue(FLOAT value)
	{
		AFL::VectorObject vo;
		NVector2 v = m_size;
		v.x *= 0.5f;
		v.y *= 0.5f;
		vo.drawBox(-v.x, -v.y, m_size.x, m_size.y, 0x88ffffff);
		FLOAT s = (m_size.x - m_frame*2.0f)*value;
		vo.drawBox(-v.x + m_frame, -v.y + m_frame,s , m_size.y - m_frame*2.0f, 0x88ff0000);
		create(&vo);
	}
protected:
	NVector2 m_size;
	FLOAT m_frame;
};
class UnitChara : public Unit
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
		statCrash=16,
		statDied=32
	};
	//virtual void onContact(Unit* unitObject);
	//virtual void onContactAtack(Unit* unitObject);

	UnitChara();
	virtual ~UnitChara();

	void setSpeed(FLOAT speed){m_speed = speed;}
	void setCommand(COMMAND command);
	DWORD getCommand(){ return m_command; }
	void setBounding();
	bool isHit(UnitChara* unitChara);
	bool isHitWepon(UnitChara* unitChara);
	void setWeponFrame(LPCSTR frameName);
	void setBodyFrame(LPCSTR frameName);

	int getStatus(){return m_stat;}
	void setStatus(INT stat,INT param=0){m_stat = stat;m_statParam=param;}
	void setInput(AFL::Input* input){m_input = input;}

	FLOAT getMoveSpeed();
	void setContactVector(FLOAT x, FLOAT y);
	INT getImageIndex() const;
	void setImageIndex(INT index);
	FLOAT getMoveSpeed2();
	FLOAT getSpeed()const;
	virtual void onAction(LPVOID value);
	void setName(LPCSTR name);
	void setLife(FLOAT value);
	FLOAT getLife();
	LPCSTR getName()
	{
		return m_name;
	}
protected:
	virtual bool onRender(LPVOID world,FLOAT& x,FLOAT& y,FLOAT& z);
	virtual void onStart(LPVOID world,LPVOID value);

	NVector* getBoundingBody(){return m_vectBoundingBody;}
	NVector m_contactVector;
	NVector m_moveVector;
private:
	AFL::String m_name;
	DWORD m_command;
	FLOAT m_speed;
	NVector m_vectBoundingBody[2];
	NVector m_vectBoundingWepon[2];
	FLOAT m_weight,m_weightAtack;
	int m_stat;
	INT m_statParam;
	AFL::Input* m_input;
	INT m_imageIndex;
	Frame* m_frameWepon;
	Frame* m_frameBody;

	FLOAT m_life;
	AFL::SP<UnitLife> m_unitLife;
	SP<UnitText> m_unitLabel;
};
