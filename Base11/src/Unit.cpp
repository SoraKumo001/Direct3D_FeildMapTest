#ifndef __ANDROID__
	#include <windows.h>
#endif
#include "Unit.h"
#include "afl3DWorld.h"
#include "afl3DField.h"
#include "Share.h"


#ifdef _MSC_VER
	#ifdef _DEBUG	//メモリリークテスト
		#include <crtdbg.h>
		#define malloc(a) _malloc_dbg(a,_NORMAL_BLOCK,__FILE__,__LINE__)
		inline void*  operator new(size_t size, LPCSTR strFileName, INT iLine)
			{return _malloc_dbg(size, _NORMAL_BLOCK, strFileName, iLine);}
		inline void operator delete(void *pVoid, LPCSTR strFileName, INT iLine)
			{_free_dbg(pVoid, _NORMAL_BLOCK);}
		#define NEW new(__FILE__, __LINE__)
		#define CHECK_MEMORY_LEAK _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
	#else
		#define NEW new
		#define CHECK_MEMORY_LEAK
	#endif //_DEBUG
#else
		#define CHECK_MEMORY_LEAK
#endif

using namespace AFL;

UnitChara::UnitChara()
{
	setLimit(true);
	m_input = NULL;
	m_command = 0;
	m_frameWepon = NULL;
	m_frameBody = NULL;
	m_moveVector = 0.0f;

	m_contactVector = 0.0f;


	m_speed = 1.0f;
	m_stat = 0;
	m_weight = 0.0f;
	m_weightAtack = 0.0f;

	m_imageIndex = 0;

}
UnitChara::~UnitChara()
{
}

bool UnitChara::onRender(LPVOID data,FLOAT& x,FLOAT& y,FLOAT& z)
{
	return true;
}
void UnitChara::setCommand(COMMAND command)
{
	m_command |= command;
}
void UnitChara::setName(LPCSTR name)
{
	if (!m_unitLabel.get())
	{
		m_unitLabel = new UnitText();
		m_unitLabel->setPointStat(0);
		m_unitLabel->setFontSize(32);
		m_unitLabel->setPosZ(130.0f);
		m_unitLabel->setVShaderName("TEXTURE3D");
		m_unitLabel->setView2D(false);
		m_unitLabel->setPosW(1.0f);
		add(m_unitLabel.get());
	}

	m_unitLabel->setText(name);
	m_name = name;
}
void UnitChara::setLife(FLOAT value)
{
	m_life = value;
	if (!m_unitLife.get())
	{
		m_unitLife = new UnitLife();
		add(m_unitLife.get());
	}
	m_unitLife->setValue(value);
}
FLOAT UnitChara::getLife()
{
	return m_life;
}

void UnitChara::onStart(LPVOID data, LPVOID value)
{
	setAnimation("Base", 0, true);
}
void UnitChara::onAction(LPVOID value)
{
	//m_unitLife.setPos(getPosX(), getPosY(), getPosZ());
	SystemData* systemData = (SystemData*)value;
	World* world = (World*)systemData->world;
	if(	m_stat & statStatic)
		return;
	FieldUnit* unitField = systemData->field;

	FLOAT fHeight = unitField->getPointHeight(getPosX(),getPosY());
	if(m_moveVector.z <= 0 && getPosZ() <= fHeight)
	{
		if(getPosZ() != fHeight)
		{
			m_moveVector.z = 0.0f;
		}
		if(m_stat & statJump)
		{
			setAnimation("Walk",100);
			m_stat &= ~statJump;
		}
	
		setPosZ(fHeight);
		m_moveVector.x *= 0.92f;
		m_moveVector.y *= 0.92f;
		m_moveVector.z = 0.0f;
	}
	else
	{
		m_moveVector.x *= 0.99f;
		m_moveVector.y *= 0.99f;
		m_moveVector.z -= 2.0f;
	}

	//減速処理
	//m_moveVector.x *= 0.95f;
	//m_moveVector.y *= 0.95f;

	if(m_moveVector.x < -50.0f)
		m_moveVector.x = -50.0f;
	else if(m_moveVector.x > 50.0f)
		m_moveVector.x = 50.0f;

	if(m_moveVector.y < -50.0f)
		m_moveVector.y = -50.0f;
	else if(m_moveVector.y > 50.0f)
		m_moveVector.y = 50.0f;

	if(m_moveVector.z < -50.0f)
		m_moveVector.z = -50.0f;
	else if(m_moveVector.z > 50.0f)
		m_moveVector.z = 50.0f;


	m_moveVector.x += m_contactVector.x;
	m_moveVector.y += m_contactVector.y;
	m_moveVector.z += m_contactVector.z;
	m_contactVector.x = 0.0f;
	m_contactVector.y = 0.0f;
	m_contactVector.z = 0.0f;




	FLOAT fX = m_moveVector.x;
	FLOAT fY = m_moveVector.y;
	FLOAT fZ = m_moveVector.z;

	FLOAT fHeight2 = unitField->getPointHeight(getPosX()+fX,getPosY()+fY);
	FLOAT fHeight3 = fHeight - fHeight2;
	if(fHeight2 > getPosZ())
	{
		if(fHeight3 < -30.0f)
		{
			fX = 0.0f;
			fY = 0.0f;
		}
		else
		{
			fX *= (30.0f + fHeight3)/30.0f;
			fY *= (30.0f + fHeight3)/30.0f;
		}
	}
	fX += getPosX();
	fY += getPosY();
	fZ += getPosZ();

	if(fX < 1.0f)
		fX = 1.0f;
	if(fY < 1.0f)
		fY = 1.0f;
	if(fX>unitField->getFieldWidth()-256.0f)
		fX = unitField->getFieldWidth()-256.0f;
	if(fY>unitField->getFieldHeight()-256.0f)
		fY = unitField->getFieldHeight()-256.0f;

	if(unitField->getFieldPointFlag(fX,fY))
	{
		unitField->setFieldSide(&fX,&fY,getPosX(),getPosY());
	}

	if(unitField->getFieldPointFlag(fX,fY))
	{
		unitField->setFieldSide(&fX,&fY,getPosX(),getPosY());
	}

	setPosX(fX);
	setPosY(fY);
	setPosZ(fZ);
	fHeight = unitField->getPointHeight(getPosX(),getPosY());
	if(fZ < fHeight)
		setPosZ(fHeight);

	LPCSTR animeName = getAnimationName();

	if(m_stat & statCrash)
	{
		//クラッシュ解除
		if(m_moveVector < 1.0f)
		{
			m_stat = 0;
		}
		else if(strcmp(animeName,"Crash") != 0)
			setAnimation("Crash",100,false);
	}


	if(strcmp(animeName,"Walk") == 0)
	{
		float speedNow = sqrt(m_moveVector.x*m_moveVector.x+m_moveVector.y*m_moveVector.y);
		addAnimationTime((DWORD)(20*speedNow));
	}
	else if(strcmp(animeName,"Base") == 0)
	{
		addAnimationTime(systemData->interval);
	}
	else
	{
		if(strcmp(animeName,"Attack") == 0)
		{
			if(!isAnimation())
			{
				m_stat &= ~statAtack;
				setAnimation("Base",1800,true);
			}
		}
		addAnimationTime(systemData->interval*5);
	}
	//入力イベントからキャラクタに命令を与える
	if(m_input)
	{
		if(m_input->isDown(ATACK))
		{
			setCommand(UnitChara::atackNormal);
		}
		if(m_input->isDown(UP))
		{
			setCommand(UnitChara::moveFRONT);
		}
		else if(m_input->isDown(DOWN))
		{
			setCommand(UnitChara::moveBACK);
		}
		if(m_input->isDown(LEFT))
		{
			setCommand(UnitChara::moveLEFT);
		}
		else if(m_input->isDown(RIGHT))
		{
			setCommand(UnitChara::moveRIGHT);
		}
		if(m_input->isDown(JUMP))
		{
			setCommand(UnitChara::moveJUMP);
		}
	}

	//状態に応じてモーションの再設定
	float r = 0.0f;
	m_stat &= ~statMove;
	if(!(m_stat & statJump || m_stat & statAtack || m_stat & statCrash))
	{
		if(!isAnimation())
		{
			setAnimation("Walk",500);
		}

		if(m_command & moveFRONT)
		{
			if(strcmp(animeName,"Walk") != 0)
				setAnimation("Walk",500);
			m_stat |= statMove;
		}
		else if(m_command & moveBACK)
		{
			r = 180.0f;
			if(strcmp(animeName,"Walk") != 0)
				setAnimation("Walk",500);
			m_stat |= statMove;
		}
		if(m_command & moveLEFT)
		{
			setRotationZ(getRotationZ()-3);
		}
		else if(m_command & moveRIGHT)
		{
			setRotationZ(getRotationZ()+3);
		}
		if(m_command & atackNormal)
		{
			m_stat |= statAtack;
			setAnimation("Attack",1000,false);
		}
		else if(m_command & moveJUMP)
		{
			m_stat |= statJump;
			m_moveVector.z += 35.0f;
			setAnimation("Base",2000,true);
		}
		if(!m_command)
		{
			if(strcmp(animeName,"Base")!=0)
				setAnimation("Base",2000,true);
		}
	}

	//移動状態に応じて加速設定
	FLOAT speed = m_speed;
	if(m_stat & statMove)
	{
		r = (getRotationZ() + r)*3.14f / 180.0f;
		m_moveVector.x += speed * (float)sin(r); 
		m_moveVector.y -= speed * (float)cos(r);

		m_weight += 0.1f;
	}
	
	m_command = 0;
}
void UnitChara::setBounding()
{
	if(m_frameBody)
	{
		getBoundingBox(m_frameBody,m_vectBoundingBody);
	}
	if(m_frameWepon)
	{
		getBoundingBox(m_frameWepon,m_vectBoundingWepon);
	}

}
bool UnitChara::isHit(UnitChara* unitChara)
{
	NVector* vector = unitChara->getBoundingBody();
	if(m_vectBoundingBody[0].x <= vector[1].x && 
		m_vectBoundingBody[0].y <= vector[1].y && 
		m_vectBoundingBody[0].z <= vector[1].z && 
		vector[0].x <= m_vectBoundingBody[1].x && 
		vector[0].y <= m_vectBoundingBody[1].y && 
		vector[0].z <= m_vectBoundingBody[1].z)
			return true;
	return false;
}
bool UnitChara::isHitWepon(UnitChara* unitChara)
{
	NVector* vector = unitChara->getBoundingBody();
	if(m_vectBoundingWepon[0].x <= vector[1].x && 
		m_vectBoundingWepon[0].y <= vector[1].y && 
		m_vectBoundingWepon[0].z <= vector[1].z && 
		vector[0].x <= m_vectBoundingWepon[1].x && 
		vector[0].y <= m_vectBoundingWepon[1].y && 
		vector[0].z <= m_vectBoundingWepon[1].z)
			return true;
	return false;
}
void UnitChara::setWeponFrame(LPCSTR name)
{
	//m_frame->setParent();
	m_frameWepon = getFrame(name);
}
void UnitChara::setBodyFrame(LPCSTR name)
{
	m_frame->setParent();
	m_frameBody = getFrame(name);
}

FLOAT UnitChara::getMoveSpeed()
{
	return sqrtf(m_moveVector.x*m_moveVector.x + m_moveVector.y*m_moveVector.y);
}
void UnitChara::setContactVector(FLOAT x, FLOAT y)
{
	m_contactVector.x = x;
	m_contactVector.y = y;
}
INT UnitChara::getImageIndex() const
{
	return m_imageIndex;
}
void UnitChara::setImageIndex(INT index)
{
	m_imageIndex = index;
}
FLOAT UnitChara::getMoveSpeed2()
{
	return sqrtf(m_moveVector.x*m_moveVector.x + m_moveVector.y*m_moveVector.y + m_moveVector.z * m_moveVector.z);
}
FLOAT UnitChara::getSpeed()const
{
	return m_speed;
}
