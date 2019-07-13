#ifndef __ANDROID__
#include <windows.h>
#else
#include <android/log.h>
#endif

#include "Main.h"

//----------------------------------------------------
//メモリリークテスト用
#if _MSC_VER && !defined(_WIN32_WCE) && _DEBUG
//メモリリークテスト
#include <crtdbg.h>
#define malloc(a) _malloc_dbg(a,_NORMAL_BLOCK,__FILE__,__LINE__)
inline void*  operator new(size_t size, LPCSTR strFileName, INT iLine)
{ return _malloc_dbg(size, _NORMAL_BLOCK, strFileName, iLine); }
inline void operator delete(void *pVoid, LPCSTR strFileName, INT iLine)
{
	_free_dbg(pVoid, _NORMAL_BLOCK);
}
#define NEW new(__FILE__, __LINE__)
#define CHECK_MEMORY_LEAK _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
#else
#define NEW new
#define CHECK_MEMORY_LEAK
#endif


Main::Main()
{
	//ネットワーク基本設定
	m_networkFlag = true;
	if (m_networkFlag) {
		m_sock.create(10002, SOCK_DGRAM);
		m_sock.setSockOpt(SO_BROADCAST, true);
	}
	m_id = timeGetTime();

}
Main::~Main()
{
}
UnitChara* Main::createUnit(LPCSTR name)
{
	UnitChara* unitChara = new UnitChara;
	*(Unit*)unitChara = *m_unitMap[name].get();
	m_unitList.push_back(unitChara);
	m_unit.add(unitChara);
	return unitChara;
}
void Main::delUnit(UnitChara* unit)
{
	m_unit.del(unit);
}
void Main::changeImage(UnitChara* unit, INT index)
{
	//キャラクターモデルの切り替え
	if (unit->getImageIndex() != index)
	{
		FLOAT x = unit->getPosX();
		FLOAT y = unit->getPosY();
		FLOAT z = unit->getPosZ();
		FLOAT rz = unit->getRotationZ();
		if (index == 0)
		{
			unit->copyImage(m_unitMap["ドン"].get());
		}
		else
		{
			unit->copyImage(m_unitMap["ダルマ"].get());
		}
		unit->setWeponFrame("_Wepon");
		unit->setBodyFrame("_Body");
		unit->setPos(x, y, z);
		unit->setRotationZ(rz);
		unit->setImageIndex(index);
		unit->setAnimation("Base");
	}
}
//-----------------------------------------------
// ---  動作  ---
// ウインドウ作成
// ---  引数  ---
// 無し
// --- 戻り値 ---
//
//-----------------------------------------------
void Main::init(LPVOID hWnd)
{
	m_systemData.interval = 30;

	//画像読み込みパスの設定
	//D3DDevice::setTexturePath(L"images\\");

	//ワールド設定
	m_world.setCamera(&m_camera);

	m_screen.createScreen((HWND)hWnd,m_screenWidth, m_screenHeight);
	m_world.setScreen(&m_screen);
	m_world.getCamera()->resetPoint();
	m_world.setDrawLimit(10000);

	m_systemData.field = &m_field;

	m_world.addUnit(&m_unit);
	//エフェクト用ユニット
	m_unit.add(&m_effect);
	m_unit.add(&m_spark);

	//通常イメージの読み出し
	LPCSTR files[] = { "mdo/Don.mdo", "mdo/Daruma.mdo","mdo/tree.mdo","mdo/rock.mdo" };
	LPCSTR names[] = { "ドン", "ダルマ", "木", "岩" };
	Unit* unit;
	for (INT i = 0; i < 4; i++)
	{
		unit = new Unit;
		unit->setShadow(true);
		unit->openFile(files[i]);
		m_unitMap[names[i]] = unit;
	}

	//水面データ設定
	m_water.openTexture("map01.png");
	m_water.create(32, 32, 0, 0);
	m_water.clear(16);
	m_water.setOutIndex(16);
	m_water.setPosW(0.1f);
	m_unit.add(&m_water);
	//フィールドデータ設定
	m_field.openTexture("map01.png", "p5.jpg");
	m_field.setOutHeight(-512.0f);
	m_field.setOutIndex(2);
	m_field.create(32, 32, 100, 20);
	m_field.setPosW(-1.0f);
	m_unit.add(&m_field);

	//空用オブジェクトの読み出し
	unit = new UnitChara;
	unit->setZBuffer(false);
	unit->setShadow(false);
	unit->setLight(false);
	unit->setScaleX(10);
	unit->setScaleY(10);
	unit->setScaleZ(10);
	unit->openFile("mdo/sky.mdo");
	unit->setRotationY(90.0f);
	unit->setRotationX(90.0f);
	unit->setPosW(-10);
	m_unitMap["空"] = unit;
	m_sky = createUnit("空");

	//FPS表示用
	m_fps.setPosW(10.0f);
	m_unit.add(&m_fps);

	INT fieldDataWidth = m_field.getDataWidth();
	INT fieldDataHeight = m_field.getDataHeight();
	FLOAT fieldPartsWidth = m_field.getPartsWidth();
	FLOAT fieldPartsHeight = m_field.getPartsHeight();

	UnitChara* unitChara;
	//木の配置
	INT i;
	for (i = 1; i<10; i++)
	{
		INT index = rand() % 2;
		unitChara = createUnit(index ? "木" : "岩");
		unitChara->setLimit(true);
		unitChara->setStatus(UnitChara::statStatic);
		FLOAT x = (FLOAT)(rand() % fieldDataWidth);
		FLOAT y = (FLOAT)(rand() % fieldDataHeight);
		if (index == 0)
		{
			FLOAT scaleX = (FLOAT)(rand() % 6 + 1);
			FLOAT scaleY = (FLOAT)(rand() % 6 + 1);
			FLOAT scaleZ = (FLOAT)(rand() % 6 + 1);
			unitChara->setPosX((x + scaleX*0.5f)*fieldPartsWidth);
			unitChara->setPosY((y + scaleY*0.5f)*fieldPartsHeight);
			unitChara->setScaleX(scaleX);
			unitChara->setScaleY(scaleY);
			unitChara->setScaleZ(scaleZ);

			INT i;
			INT j;
			for (j = 0; j<scaleY; j++)
			{
				for (i = 0; i<scaleX; i++)
				{
					m_field.setFieldPointFlag((x + i + 0.5f)*fieldPartsWidth, (y + j + 0.5f)*fieldPartsHeight, 1);
				}
			}
		}
		else
		{
			unitChara->setPosX((x + 0.5f)*fieldPartsWidth);
			unitChara->setPosY((y + 0.5f)*fieldPartsHeight);
			m_field.setFieldPointFlag(unitChara->getPosX(), unitChara->getPosY(), 1);
			unitChara->setRotationZ((FLOAT)(rand() % 360));
		}
		unitChara->setPosZ(m_field.getPointHeight(unitChara->getPosX(), unitChara->getPosY()) - 20.0f);
		
	}

	//初期キャラクターのパラメータ設定
	unitChara = createUnit("ドン");
/*	File file;
	if (file.open("name.txt"))
	{
		CHAR name[100];
		if(file.gets(name, 99))
			unitChara->setName(name);
	}*/
	unitChara->setPosX(4000.0f + (FLOAT)(rand() % 1000));
	unitChara->setPosY(4000.0f + (FLOAT)(rand() % 1000));
	unitChara->setInput(m_input);
	unitChara->setLife(0.0f);
	unitChara->setAnimation("Walk");
	unitChara->setWeponFrame("_Wepon");
	unitChara->setBodyFrame("_Body");
	unitChara->setPosZ(m_field.getPointHeight(unitChara->getPosX(), unitChara->getPosY()));
	unitChara->setSpeed(1.0f);
	unitChara->setLife(0.0f);
	m_chara = unitChara;
	m_camera.setUnit(unitChara);

	m_systemData.unitChara.push_back(unitChara);
	m_systemData.unitMain = unitChara;
	m_systemData.world = &m_world;

	//追加キャラ
	unitChara = createUnit("ドン");
	unitChara->setName("敵キャラクター");
	unitChara->setLife(1.0f);
	unitChara->setPosX(4000.0f + (FLOAT)(rand() % 1000));
	unitChara->setPosY(4000.0f + (FLOAT)(rand() % 1000));
	unitChara->setAnimation("Walk");
	unitChara->setPosZ(m_field.getPointHeight(unitChara->getPosX(), unitChara->getPosY()));
	unitChara->setWeponFrame("_Wepon");
	unitChara->setBodyFrame("_Body");
	m_systemData.unitChara.push_back(unitChara);
	
}
void Main::size(LPVOID hWnd,int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	m_screen.createScreen((HWND)hWnd,width, height );

	m_camera.setAngle(width, height);
	m_camera.resetPoint();
}

DWORD Main::render(LPVOID data)
{
	m_sky->setPos(m_camera.getX(), m_camera.getY(), m_camera.getZ());
	m_camera.setCamera();

	m_world.clear(0.5f, 0.5f, 1.0f, 0.0f);
	m_world.draw();
	m_world.present();
	return 0;
}

DWORD Main::action(LPVOID data)
{
	m_sky->setRotationZ(m_sky->getRotationZ() + 0.03f);
	DWORD dwTime = m_systemData.interval * 5;

	auto it = m_unitTree.begin();
	for (; it != m_unitTree.end(); ++it)
	{
		(*it)->addAnimationTime(dwTime);
	}

	if (m_networkFlag)
	{
		m_networkCount++;
		if (m_networkCount*m_systemData.interval >= 100 || m_systemData.unitMain->getLife() <= 0.0f)
		{
			m_networkCount = 0;

			//座標データの送信
			UnitChara* unit = m_systemData.unitMain;
			SDATA s = { m_id, unit->getImageIndex(), unit->getPosX(), unit->getPosY(), (DWORD)unit->getStatus(),
			unit->getCommand() & UnitChara::moveBACK ? -unit->getSpeed() : unit->getSpeed(), unit->getRotationZ(),unit->getLife() };
			strcpy(s.name, unit->getName());
			m_sock.sendTo(&s, sizeof(s), 10002);

			//受信
			CHAR buff[512];
			while (m_sock.recvFrom(buff, 512) == sizeof(SDATA))
			{
				SDATA s;
				s = *(SDATA*)buff;
				if (s.id != m_id)
				{
					std::map<INT, UnitRemote>::iterator it = m_systemData.unitRemote.find(s.id);
					if (it == m_systemData.unitRemote.end())
					{
						UnitChara* unitChara;
						if (s.index == 0)
						{
							unitChara = createUnit("ドン");
						}
						else
						{
							unitChara = createUnit("ダルマ");
						}
						unitChara->setWeponFrame("_Wepon");
						unitChara->setBodyFrame("_Body");
						unitChara->setImageIndex(s.index);
						unitChara->setSpeed(s.speed);
						unitChara->setLimit(true);

						unitChara->setAnimation("Walk");
						m_systemData.unitChara.push_back(unitChara);

						UnitRemote unitRemote;
						unitRemote.unitChara = unitChara;
						m_systemData.unitRemote[s.id] = unitRemote;
						it = m_systemData.unitRemote.find(s.id);
						unitChara->setPosX(s.x);
						unitChara->setPosY(s.y);
						unitChara->setRotationZ(s.rotation);
						unitChara->setName(s.name);
					}

					UnitRemote* unitRemote = &it->second;
					unitRemote->lastTime = timeGetTime();
					if (unitRemote->unitChara->getStatus() != s.stat)
					{
						if (s.stat == UnitChara::statJump)
							unitRemote->unitChara->setCommand(UnitChara::moveJUMP);
						else if (s.stat == UnitChara::statAtack)
							unitRemote->unitChara->setCommand(UnitChara::atackNormal);
					}
					changeImage(unitRemote->unitChara, s.index);

					unitRemote->x = s.x;
					unitRemote->y = s.y;
					unitRemote->speed = s.speed;
					unitRemote->life = s.life;
				}
			}
		}

		DWORD nowTime = timeGetTime();
		//ネットワークキャラクターの位置補正
		std::list<std::map<INT, UnitRemote>::iterator> deleteList;

		std::map<INT, UnitRemote>::iterator it;
		for (it = m_systemData.unitRemote.begin(); it != m_systemData.unitRemote.end(); it++)
		{
			UnitRemote& unitRemote = it->second;
			if (nowTime > unitRemote.lastTime + 1000)
			{
				deleteList.push_back(it);
				continue;
			}
			UnitChara& unitChara = *unitRemote.unitChara;
			FLOAT rx = unitRemote.x - unitChara.getPosX();
			FLOAT ry = unitRemote.y - unitChara.getPosY();
			FLOAT l = rx*rx + ry*ry;
			unitChara.setLife(unitRemote.life);
			if (unitRemote.life <= 0.0f)
			{
				m_spark.add2(unitChara.getPosX(), unitChara.getPosY(), unitChara.getPosZ());
				unitChara.setLife(1.0f);
				unitChara.setPosZ(5000.0f);
			}
			if (l > 2000 * 2000)
			{
				unitChara.setPosX(unitRemote.x);
				unitChara.setPosY(unitRemote.y);
			}
			else if (l > 100 * 100)
			{
				l = sqrt(l);
				float s = (float)acos(ry / l);
				float r = s *  180.0f / 3.141592f;
				if (rx < 0)
					r = 360 - r;
				if (unitRemote.speed > 0)
				{
					unitChara.setRotationZ(-r + 180);
					unitChara.setCommand(UnitChara::moveFRONT);
				}
				else
				{
					unitChara.setRotationZ(-r);
					unitChara.setCommand(UnitChara::moveBACK);
				}
			}
		}

		//未使用キャラの削除
		std::list<std::map<INT, UnitRemote>::iterator>::iterator itDel;
		for (itDel = deleteList.begin(); itDel != deleteList.end(); ++itDel)
		{
			delUnit((*itDel)->second.unitChara);
			m_systemData.unitRemote.erase(*itDel);

		}

		std::list<UnitChara*>::iterator it1, it2;
		for (it1 = m_systemData.unitChara.begin(); it1 != m_systemData.unitChara.end(); it1++)
		{
			(*it1)->setBounding();
			if ((*it1)->getMoveSpeed2() > 6.0f)
				m_effect.add((*it1)->getPosX(), (*it1)->getPosY(), (*it1)->getPosZ());

			if ((*it1)->getLife() < 0.0f)
			{
				m_spark.add2((*it1)->getPosX(), (*it1)->getPosY(), (*it1)->getPosZ());
				(*it1)->setPosZ(5000.0f);
				(*it1)->setLife(1.0f);
			}
		}
		UnitChara* unit = m_systemData.unitMain;
		if (unit->getLife() <= 0.0f)
		{
			m_spark.add2(unit->getPosX(), unit->getPosY(), unit->getPosZ());
			unit->setPosZ(5000.0f);
			unit->setLife(1.0f);
		}

		for (it1 = m_systemData.unitChara.begin(); it1 != m_systemData.unitChara.end(); it1++)
		{
			UnitChara* unit1 = *it1;
			for (it2 = m_systemData.unitChara.begin(); it2 != m_systemData.unitChara.end(); it2++)
			{
				if (it1 == it2)
					continue;
				UnitChara* unit2 = *it2;

				if (unit1->getStatus() == UnitChara::statAtack && unit1->isHitWepon(unit2))
				{
					//武器との接触処理
					FLOAT s1 = unit1->getMoveSpeed();

					FLOAT s = 1.0f;
					FLOAT x = unit1->getPosX() - unit2->getPosX();
					FLOAT y = unit1->getPosY() - unit2->getPosY();
					FLOAT r = x*x + y*y;
					if (r > 0.1f)
					{
						FLOAT l = 1.0f / sqrtf(r);
						x *= l * s;
						y *= l * s;
						unit2->setStatus(UnitChara::statCrash);
						unit2->setContactVector(-x*5.0f, -y*5.0f);
						unit2->setLife(unit2->getLife()-0.05f);
						m_spark.add(unit2->getPosX(), unit2->getPosY(), unit2->getPosZ() + 50.0f);
					}
				}
				else if (unit1->isHit(unit2))
				{
					//通常接触処理
					FLOAT s1 = unit1->getMoveSpeed();

					FLOAT s = (s1 + unit2->getMoveSpeed())*0.5f;
					FLOAT x = unit1->getPosX() - unit2->getPosX();
					FLOAT y = unit1->getPosY() - unit2->getPosY();
					FLOAT r = x*x + y*y;
					if (r > 0.1f)
					{
						FLOAT l = 1.0f / sqrtf(r);
						x *= l * s;
						y *= l * s;
						unit1->setContactVector(x, y);
					}
				}

			}
		}
	}
	m_world.onAction((LPVOID)&m_systemData);
	return true;
}

void Main::setInput(AFL::Input* input)
{
	m_input = input;
}
void Main::setZoom(INT value)
{
	m_camera.setCameraRange(value);
}
void Main::rotationCameraX(FLOAT value)
{
	m_camera.moveRotationX(value);
}
void Main::rotationCameraZ(FLOAT value)
{
	Unit* unit = m_camera.getUnit();
	unit->setRotationZ(unit->getRotationZ() + value);
}
void Main::changeImage(INT index)
{
	changeImage(m_systemData.unitMain, index);
}
