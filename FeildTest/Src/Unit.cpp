#include <windows.h>
#include "Unit.h"
#include "aflDirect3DWorld.h"
#include "aflDirect3DField.h"
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
using namespace AFL::DIRECT3D;

UnitChara::UnitChara()
{
	//視野外消去設定
	setLimit(true);
	//入力デバイスの設定
	m_input = NULL;
	//初期コマンドのクリア
	m_command = 0;
	m_frameWepon = NULL;

	//加速度のクリア
	m_moveVector.x = 0.0f;
	m_moveVector.y = 0.0f;
	m_moveVector.z = 0.0f;
	//接触反動力のクリア
	m_contactVector.x = 0.0f;
	m_contactVector.y = 0.0f;
	m_contactVector.z = 0.0f;

	m_jump = 35.0f;			//ジャンプ力
	m_speed = 3.0f;			//加速能力
	m_stat = 0;				//状態
	m_imageIndex = 0;			//初期モデルデータ番号
}

void UnitChara::input()
{
	//入力に対するコマンド設定
	if(m_input->isDown(ATACK))
	{
		setCommand(atackNormal);
	}
	if(m_input->isDown(UP))
	{
		setCommand(moveFRONT);
	}
	else if(m_input->isDown(DOWN))
	{
		setCommand(moveBACK);
	}
	if(m_input->isDown(LEFT))
	{
		setCommand(moveLEFT);
	}
	else if(m_input->isDown(RIGHT))
	{
		setCommand(moveRIGHT);
	}
	if(m_input->isDown(JUMP))
	{
		setCommand(moveJUMP);
	}
}


UnitChara::~UnitChara()
{
}

bool UnitChara::onRender(AFL::DIRECT3D::World* world,FLOAT& x,FLOAT& y,FLOAT& z)
{
	Camera3D* camera = (Camera3D*)world->getCamera();
	NMatrix* matrix = &camera->getView();

	FLOAT fx = x - camera->getX();
	FLOAT fy = y - camera->getY();
	FLOAT fz = z - camera->getZ();
	fx *= fx;
	fy *= fy;
	fz *= fz;
	z -= (fx + fy + fz) / 100000;


	return true;
}
void UnitChara::setCommand(COMMAND command)
{
	m_command |= command;
}
void UnitChara::onStart(AFL::DIRECT3D::World* world,LPVOID value)
{
	setAnimation("Walk",0);
}
void UnitChara::onAction(AFL::DIRECT3D::World* world,LPVOID value)
{
	if(	m_stat & statStatic)
		return;
	SystemData* systemData = (SystemData*)value;
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
		//地面補正
		setPosZ(fHeight);
		//地上減速処理
		m_moveVector.x *= 0.92f;
		m_moveVector.y *= 0.92f;
		m_moveVector.z = 0.0f;
	}
	else
	{
		//空中減速処理
		m_moveVector.x *= 0.99f;
		m_moveVector.y *= 0.99f;
		m_moveVector.z -= 2.0f;
	}
	//通常減速処理
	m_moveVector.x *= 0.95f;
	m_moveVector.y *= 0.95f;

	float moveVect = m_moveVector.x * m_moveVector.x +
		 m_moveVector.y *  m_moveVector.y +  
		 m_moveVector.z *  m_moveVector.z;
	if(moveVect > 50 * 50)
	{
		moveVect = 50 / sqrtf(moveVect);
		m_moveVector.x *= moveVect;
		m_moveVector.y *= moveVect;
		m_moveVector.z *= moveVect;
	}
	/*
	//加速リミット処理
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
		*/
	//接触反動処理
	m_moveVector.x += m_contactVector.x;
	m_moveVector.y += m_contactVector.y;
	m_moveVector.z += m_contactVector.z;
	m_contactVector.x = 0.0f;
	m_contactVector.y = 0.0f;
	m_contactVector.z = 0.0f;



	//位置最終調整
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
	if(fX>unitField->getFieldWidth())
		fX = unitField->getFieldWidth();
	if(fY>unitField->getFieldHeight())
		fY = unitField->getFieldHeight();

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

	if(strcmp(animeName,"Walk") == 0)
	{
		float speedNow = sqrt(m_moveVector.x*m_moveVector.x+m_moveVector.y*m_moveVector.y);
		addAnimationTime((DWORD)(20*speedNow));
	}
	else
	{
		if(strcmp(animeName,"Attack") == 0)
		{
			if(!isAnimation())
			{
				m_stat &= ~statAtack;
				setAnimation("Base",1800,false);
			}
		}
		addAnimationTime(systemData->interval*5);
	}

	//入力処理
	if(m_input)
	{
		input();
	}

	//コマンド処理
	float r = 0.0f;
	m_stat &= ~statMove;
	if(!(m_stat & statJump || m_stat & statAtack))
	{
		if(!isAnimation())
		{
			setAnimation("Walk",500);
		}

		if(m_command & moveFRONT)
		{
			m_stat |= statMove;
		}
		else if(m_command & moveBACK)
		{
			r = 180.0f;
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
			m_moveVector.z += m_jump;
			setAnimation("Base",2000,false);
		}
	}

	//方向調整
	FLOAT speed = m_speed;

	if(m_stat & statMove)
	{
		r = (getRotationZ() + r)*3.14f / 180.0f;
		m_moveVector.x += speed * (float)sin(r); 
		m_moveVector.y -= speed * (float)cos(r);

		//m_weight += 0.1f;
	}

	m_command = 0;
}
void UnitChara::setBounding()
{
	getBoundingBox(m_bounding);
	getBoundingBox(m_frameWepon,m_vectBoundingWepon);

}
bool UnitChara::isHit(UnitChara* unitChara)
{
	NVector* vector = unitChara->getBoundingVector();
	if(m_bounding[0].x <= vector[1].x && 
		m_bounding[0].y <= vector[1].y && 
		m_bounding[0].z <= vector[1].z && 
		vector[0].x <= m_bounding[1].x && 
		vector[0].y <= m_bounding[1].y && 
		vector[0].z <= m_bounding[1].z)
			return true;
	return false;
}
bool UnitChara::isHitWepon(UnitChara* unitChara)
{
	NVector* vector = unitChara->getBoundingVector();
	if(m_vectBoundingWepon[0].x <= vector[1].x && 
		m_vectBoundingWepon[0].y <= vector[1].y && 
		m_vectBoundingWepon[0].z <= vector[1].z && 
		vector[0].x <= m_vectBoundingWepon[1].x && 
		vector[0].y <= m_vectBoundingWepon[1].y && 
		vector[0].z <= m_vectBoundingWepon[1].z)
			return true;
	return false;
}