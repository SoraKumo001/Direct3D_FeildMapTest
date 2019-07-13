#include <windows.h>
#include <time.h>
#include "aflDirect3DField.h"
#include "ProcMain.h"

//----------------------------------------------------
//メモリリークテスト用
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
//----------------------------------------------------
using namespace AFL;

//マウス表示用
#define WM_MOUSESHOW (WM_USER+1)
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CameraTracking
// 追跡用カメラ
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CameraTracking::CameraTracking()
{
	m_cameraRotationX = 20.0f;
	m_cameraRange = 1000.0f;
	m_zoomNear = 300.0f;
	m_zoomFar = 2000.0f;
	m_unit = NULL;
}
void CameraTracking::setCamera()
{
	if(!m_unit)
		return;
	Unit* unit = m_unit;
	float fRange = m_cameraRange;
	float fRX = getRotationX()*3.14f / 180;
	float fRZ = unit->getRotationZ()*3.14f / 180;
	FLOAT fX,fY,fZ;
	fX = -(float)sin(fRZ)*fRange * (float)cos(fRX);
	fY = (float)cos(fRZ)*fRange * (float)cos(fRX);
	fZ = (float)sin(fRX)*fRange;

	fX += -getX() + unit->getPosX();
	fY += -getY() + unit->getPosY();
	fZ += -getZ() + unit->getPosZ()+100;

	setRotationZ(getRotationZ()+(unit->getRotationZ()-getRotationZ())*0.3f);
	setRotationX(getRotationX()+(m_cameraRotationX-getRotationX())*0.3f);
	fX *= 0.3f;
	fY *= 0.3f;
	fZ *= 0.3f;

	setPosX(getX() + fX);
	setY(getY() + fY);
	setZ(getZ() + fZ);
}
void CameraTracking::setCameraRange(INT wheel)
{
	m_cameraRange += (FLOAT)wheel;
	if(m_cameraRange > m_zoomFar)
		m_cameraRange = m_zoomFar;
	else if(m_cameraRange < m_zoomNear)
		m_cameraRange = m_zoomNear;
}
void CameraTracking::setMousePoint(POINT point)
{
	m_mousePoint = point;
}
void CameraTracking::moveRotationX(FLOAT value)
{
	m_cameraRotationX += value;
	if(m_cameraRotationX > 100.0f)
	{
		m_cameraRotationX = 100.0f;
	}
	else if(m_cameraRotationX < -50.0f)
	{
		m_cameraRotationX = -50.0f;
	}
}
void CameraTracking::setUnit(Unit* unit)
{
	m_unit = unit;
}
Unit* CameraTracking::getUnit()const
{
	return m_unit;
}


ProcMain::ProcMain()
{
	//ウインドウ設定初期化
	m_window = NULL;
	m_changeScreen = false;

	//マウス入力初期値
	m_mousePoint.x = 0;
	m_mousePoint.y = 0;
	m_mouseRDown = false;

	//ユーザ入力設定
	m_input.setKey(UP,VK_UP);
	m_input.setKey(UP,'E');
	m_input.setKey(DOWN,VK_DOWN);
	m_input.setKey(DOWN,'C');
	m_input.setKey(LEFT,VK_LEFT);
	m_input.setKey(LEFT,'S');
	m_input.setKey(RIGHT,VK_RIGHT);
	m_input.setKey(RIGHT,'F');

	m_input.setKey(ATACK,'X');
	m_input.setKey(JUMP,VK_SPACE);
	m_input.setKey(JUMP,'Z');
	m_input.setKey(ZOOMUP,VK_PRIOR);
	m_input.setKey(ZOOMDOWN,VK_NEXT);

	//ネットワーク基本設定
	m_networkFlag = true;
	m_sock.create(10002,SOCK_DGRAM);
	m_sock.setSockOpt(SO_BROADCAST,true);
	m_id = GetTickCount();

	//影の有効化
	m_world.setShadow();

	//木
	m_treeName = "木";
}

bool ProcMain::init(Window* aflWindow)
{
	m_window = aflWindow;

	//ウインドウメッセージのフック
	aflWindow->addMessage(WM_PAINT,CLASSPROC(this,ProcMain,onPaint));
	aflWindow->addMessage(WM_CREATE,CLASSPROC(this,ProcMain,onCreate));
	aflWindow->addMessage(WM_CLOSE,CLASSPROC(this,ProcMain,onClose));
	aflWindow->addMessage(WM_MOUSESHOW,CLASSPROC(this,ProcMain,onMouseShow));
	aflWindow->addMessage(WM_MOUSEWHEEL,CLASSPROC(this,ProcMain,onMouseWheel));
	//aflWindow->addMessage(WM_SYSCHAR,CLASSPROC(this,ProcMain,onKeyDown));
	aflWindow->addMessage(WM_KEYDOWN,CLASSPROC(this,ProcMain,onKeyDown));
	aflWindow->addMessage(WM_MOUSEMOVE,CLASSPROC(this,ProcMain,onMouseMove));
	aflWindow->addMessage(WM_SIZE,CLASSPROC(this,ProcMain,onSize));

	m_systemData.interval = 20;
	m_threadTimer.setInterval(m_systemData.interval );
	m_threadTimer.setWait(false);
	m_threadTimer.setRender(CLASSPROC(this,ProcMain,onRender));	//レンダリング用
	m_threadTimer.setAction(CLASSPROC(this,ProcMain,onAction));	//動作制御用
	m_threadTimer.setIdle(CLASSPROC(this,ProcMain,onIdle));		//動作制御用
	m_threadTimer.setStart(CLASSPROC(this,ProcMain,onStart));	//開始設定用
	m_threadTimer.start(aflWindow->getWnd());

	return true;
}
//-----------------------------------------------
// ---  動作  ---
// ウインドウ作成
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onCreate(Message* pMessage)
{
}
void ProcMain::onSize(Message* m)
{
	INT width = m->getX();
	INT height = m->getY();
	m_world.setRenderSize(width,height);
}
void ProcMain::onKeyDown(Message* m)
{
	//m_changeScreen = true;
	//m_screen.setScreenMode(pMessage->getWnd(),!m_screen.isScreenMode(),640,480);
	//DIRECT3D::Device::resetDevice();
}

//-----------------------------------------------
// ---  動作  ---
// ウインドウクローズ
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onClose(Message* pMessage)
{
	//スレッドに停止命令
	m_threadTimer.stop();
}

//-----------------------------------------------
// ---  動作  ---
// レンダリング
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onPaint(Message* pMessage)
{
	LPCSTR strMessage[] =
	{
		"デバイスの初期化を試みています。",
		"必要なバージョンのDirectXがインストールされていません。",
		"初期化に失敗しました。3D対応ドライバが無いか、サポートされていないカラーモードです。",
	};
	HWND hwnd = pMessage->getWnd();
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd,&ps);
	switch(DIRECT3D::Device::getStat())
	{
	case D3D_DEVICE_NOTINIT:
		::TextOutA(hdc,0,0,strMessage[0],(INT)strlen(strMessage[0]));
		break;
	case D3D_DEVICE_ERROR_DIRECTX:
		::TextOutA(hdc,0,0,strMessage[1],(INT)strlen(strMessage[1]));
		break;
	case D3D_DEVICE_ERROR_DEVIVE:
		::TextOutA(hdc,0,0,strMessage[2],(INT)strlen(strMessage[2]));
		break;
	case D3D_DEVICE_ACTIVE:
		if(m_message.length())
			::TextOutA(hdc,0,0,m_message,(INT)m_message.length());
		break;
	}
	EndPaint(hwnd,&ps);
}
void ProcMain::onMouseWheel(Message* pMessage)
{
	if (::GetForegroundWindow() == pMessage->getWnd())
	{
		INT wheel = pMessage->getLParamHi();
		int f = 1;
		if((SHORT)pMessage->getWParamHi() > 0)
			f = -1;
		m_camera.setCameraRange(wheel*f);
	}

}

//-----------------------------------------------
// ---  動作  ---
// マウスカーソル設定
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onMouseMove(Message* m)
{
/*	INT x = m->getPosX();
	INT y = m->getPosY();
	POINT point = {x,y};
	m_world.getDrawPoint(m->getWnd(),&point);
	String dest;
	dest.printf("%03d,%03d",point.x,point.y);
	m_unitText.setText(dest);
*/
}

//-----------------------------------------------
// ---  動作  ---
// マウスカーソル設定
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onMouseShow(Message* pMessage)
{
	ShowCursor(pMessage->getWParam() != 0);
}

//-----------------------------------------------
// ---  動作  ---
// レンダリング(レンダリング前に実行される)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onRender(HWND hWnd)
{
	m_camera.setCamera();
	m_world.render(&m_unit);
	m_world.present(hWnd);
}


//-----------------------------------------------
// ---  動作  ---
// 開始処理(スレッド開始時に一回実行される)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::delUnit(UnitChara* unit)
{
	m_unit.del(unit);
}
UnitChara* ProcMain::createUnit(LPCSTR name)
{
	UnitChara* unitChara = new UnitChara;
	*(Unit*)unitChara = *m_unitMap[name].get();
	unitChara->createBoundingBox();
	m_unitList.push_back(unitChara);
	m_unit.add(unitChara);

	unitChara->setBounding();
	return unitChara;
}
void ProcMain::onStart(HWND hWnd)
{
	//デバイスの初期化
	if(!Device::init(hWnd,640,480))
	{
		m_threadTimer.stop();
		return;
	}
	VertexShader vs;
	//vs.open("fx9/field.fx","VS");
	//vs.save(L"fx9/field.hlsl");
	vs.open(L"fx9/field.hlsl");
	Device::addVShader("field",vs);
	vs.open("fx9/vs.hlsl");
	Device::addVShader("t0",vs);
	vs.open("fx9/vs4.hlsl");
	Device::addVShader("t4",vs);
	vs.open("fx9/vss.hlsl");
	Device::addVShader("s0",vs);
	vs.open("fx9/vss4.hlsl");
	Device::addVShader("s4",vs);


	m_message = "データの読み出し中です。";
	m_window->invalidate();
	Device::setTexturePath(L"images\\");

	//srand((DWORD)time(NULL));

	RECT rect;
	GetClientRect(hWnd,&rect);
	m_world.setRenderSize(m_window->getClientWidth(),m_window->getClientHeight());


	//出力先の初期化
	m_world.render();
	m_world.setDrawLimit(20000);

	//カメラ設定
	m_world.setCamera(&m_camera);
	m_camera.resetPoint();


	//フィールドマップ画像の読み出し
	m_unitField.openTexture("Images/map01.PNG");
	m_unitField.create(32,32);
	m_systemData.field = &m_unitField;
	m_unit.add(&m_unitField);

	//オブジェクト画像の読み出し
	Unit* unit;
	unit = new Unit;
	unit->setShadow(true);
	unit->openFile(L"mdo/ダルマ.mdo");
	unit->setPosW(-100);
	m_unitMap["ダルマ"] = unit;


	unit = new Unit;
	unit->setShadow(true);
	unit->openFile(L"mdo/ドン.mdo");
	m_unitMap["ドン"] = unit;

	unit = new Unit;
	unit->setShadow(true);
	unit->openFile(L"mdo/木3.mdo");
	m_unitMap["木"] = unit;

	unit = new Unit;
	unit->setShadow(true);
	unit->openFile(L"mdo/プリン.mdo");
	m_unitMap["プリン"] = unit;

	//空用オブジェクトの読み出し
	unit = new Unit;
	unit->setZBuffer(false);
	unit->setShadow(false);
	unit->setLight(false);
	unit->setScaleX(15);
	unit->setScaleY(15);
	unit->setScaleZ(15);
	unit->openFile(L"mdo/sky.mdo");
	//unit->setShader(false);
	unit->setPosW(-10);
	m_unitMap["空"] = unit;
	m_sky = createUnit("空");
	m_sky->setRotationY(90.0f);
	m_sky->setRotationX(90.0f);



	//メインキャラクター初期設定
	UnitChara* unitChara;
	unitChara = createUnit("ドン");
	unitChara->setPosX(4000.0f+(FLOAT)(rand()%1000));
	unitChara->setPosY(4000.0f+(FLOAT)(rand()%1000));
	unitChara->setAnimation("Walk");
	unitChara->setWeponFrame("Sw");
	//unitChara->setPosY(12400);
	//unitChara->setPosY(12400);
	unitChara->setPosZ(m_unitField.getPointHeight(unitChara->getPosX(),unitChara->getPosY()));
	unitChara->setInput(&m_input);
	m_input.setWnd(m_window->getWnd());
	m_systemData.unitChara.push_back(unitChara);
	m_camera.setUnit(unitChara);
	m_systemData.unitMain = unitChara;


	//FPS表示
	m_unitFPS.createImage(64,32);
	m_unit.add(&m_unitFPS);
	m_unit.add(&m_unitText);
	
	//操作メッセージ表示
	m_unitText.setPosY(16);
	m_unitText.createText("移動:カーソルキー ジャンプ:Z 攻撃:X",16,0xffffff,0);
	m_unitText.setPosW(10);
	
	m_unitDebug.createImage(256,16);
	m_unitDebug.setPosY(48);
	m_unit.add(&m_unitDebug);

	
	INT fieldDataWidth = m_unitField.getDataWidth();
	INT fieldDataHeight = m_unitField.getDataHeight();
	FLOAT fieldPartsWidth = m_unitField.getPartsWidth();
	FLOAT fieldPartsHeight = m_unitField.getPartsHeight();

	//木の配置
	srand(0);
	INT i;
	for(i=1;i<25;i++)
	{
		unitChara = createUnit(m_treeName);
		m_unitTree.push_back(unitChara);
		unitChara->addAnimationTime(rand());
		unitChara->setLimit(true);
		unitChara->setStatus(UnitChara::statStatic);
		unitChara->setPosX(((float)(rand()%fieldDataWidth)+0.5f)*fieldPartsWidth);
		unitChara->setPosY(((float)(rand()%fieldDataHeight)+0.5f)*fieldPartsHeight);
		m_unitField.setFieldPointFlag(unitChara->getPosX(),unitChara->getPosY(),1);
		unitChara->setPosZ(m_unitField.getPointHeight(unitChara->getPosX(),unitChara->getPosY())-10.0f);
	}

	InvalidateRect(hWnd,NULL,true);
	UpdateWindow(hWnd);
	m_world.onStart(&m_systemData);

	m_world.setUnit(&m_unit);

	//起動メッセージのクリア
	m_message.clear();

}

//-----------------------------------------------
// ---  動作  ---
// メインループ(指定した時間分の回数を必ず実行)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::onAction(HWND hWnd)
{
	DWORD dwTime = m_threadTimer.getInterval()*5;

	auto it = m_unitTree.begin();
	for(;it!=m_unitTree.end();++it)
	{
		(*it)->addAnimationTime(dwTime);
	}

	//m_sky->setRotationZ(m_sky->getRotationZ()+dwTime/4000.0f);

	if(m_networkFlag)
	{
		m_networkCount++;
		if(m_networkCount*m_threadTimer.getInterval() >= 100)
		{
			m_networkCount = 0;

			//座標データの送信
			UnitChara* unit = m_systemData.unitMain;
			SDATA s = {m_id,unit->getImageIndex(),unit->getPosX(),unit->getPosY(),unit->getStatus()};
			m_sock.sendTo(&s,sizeof(s),10002);

			//受信
			CHAR buff[512];
			while(m_sock.recvFrom(buff,512)==sizeof(SDATA))
			{
				SDATA s;
				s = *(SDATA*)buff;
				if(s.id != m_id)
				{
					std::map<INT,UnitRemote>::iterator it = m_systemData.unitRemote.find(s.id);
					if(it == m_systemData.unitRemote.end())
					{
						UnitChara* unitChara;
						if(s.index == 0)
						{
							unitChara = createUnit("ドン");
							unitChara->setWeponFrame("Sw");
						}
						else
						{
							unitChara = createUnit("ダルマ");
							unitChara->setWeponFrame("HEAD");
						}
						unitChara->setImageIndex(s.index);
						unitChara->setLimit(true);

						unitChara->setAnimation("Walk");
						m_systemData.unitChara.push_back(unitChara);

						UnitRemote unitRemote;
						unitRemote.unitChara = unitChara;
						m_systemData.unitRemote[s.id] = unitRemote;
						it = m_systemData.unitRemote.find(s.id);
						unitChara->setPosX(s.x);
						unitChara->setPosY(s.y);
					}

					UnitRemote* unitRemote = &it->second;
					unitRemote->lastTime = GetTickCount();
					if(unitRemote->unitChara->getStatus() != s.stat)
					{
						if(s.stat == UnitChara::statJump)
							unitRemote->unitChara->setCommand(UnitChara::moveJUMP);
						else if(s.stat == UnitChara::statAtack)
							unitRemote->unitChara->setCommand(UnitChara::atackNormal);
					}
					changeImage(unitRemote->unitChara,s.index);

					unitRemote->x = s.x;
					unitRemote->y = s.y;	

				}
			}
		}

		DWORD nowTime = GetTickCount();
		//ネットワークキャラクターの位置補正
		std::list<std::map<INT,UnitRemote>::iterator> deleteList;

		std::map<INT,UnitRemote>::iterator it;
		for(it=m_systemData.unitRemote.begin();it!=m_systemData.unitRemote.end();it++)
		{
			UnitRemote& unitRemote = it->second;
			if(nowTime > unitRemote.lastTime + 1000)
			{
				deleteList.push_back(it);
				continue;
			}
			UnitChara& unitChara = *unitRemote.unitChara;
			FLOAT rx = unitRemote.x - unitChara.getPosX();
			FLOAT ry = unitRemote.y - unitChara.getPosY();
			FLOAT l = rx*rx + ry*ry;

			if(l > 2000*2000)
			{
				unitChara.setPosX(unitRemote.x);
				unitChara.setPosY(unitRemote.y);
			}
			else if(l > 100*100)
			{
				l = sqrt(l);
				float s = (float)acos(ry/l);
				float r = s *  180.0f / 3.141592f;
				if(rx < 0)
					r = 360 - r;
				unitChara.setRotationZ(-r+180);
				unitChara.setCommand(UnitChara::moveFRONT);

			}
		}


		std::list<std::map<INT,UnitRemote>::iterator>::iterator itDel;
		for(itDel = deleteList.begin();itDel != deleteList.end();++itDel)
		{
			delUnit((*itDel)->second.unitChara);
			m_systemData.unitRemote.erase(*itDel);

		}

		std::list<UnitChara*>::iterator it1,it2;
		for(it1=m_systemData.unitChara.begin();it1!=m_systemData.unitChara.end();it1++)
		{
			(*it1)->setBounding();
		}
		for(it1=m_systemData.unitChara.begin();it1!=m_systemData.unitChara.end();it1++)
		{
			UnitChara* unit1 = *it1;
			for(it2=it1,it2++;it2!=m_systemData.unitChara.end();it2++)
			{
				UnitChara* unit2 = *it2;
				if(unit1->isHit(unit2))
				{
					FLOAT s1 = unit1->getMoveSpeed();
					bool attack = false;
					if(unit1->getStatus() == UnitChara::statAtack && unit1->isHitWepon(unit2))
						attack = true;

					FLOAT s = (s1 + unit2->getMoveSpeed())*0.5f;
					FLOAT x = unit1->getPosX() - unit2->getPosX();
					FLOAT y = unit1->getPosY() - unit2->getPosY();
					FLOAT r = x*x+y*y;
					if(r > 0.1f)
					{
						FLOAT l = 1.0f/sqrtf(r);
						x *= l * s;
						y *= l * s;
						unit1->setContactVector(x,y);
						if(attack)
							unit2->setContactVector(-x*10.0f,-y*10.0f);
					}
				}
			}
		}
	}
	
	m_world.onAction((LPVOID)&m_systemData);
}
//-----------------------------------------------
// ---  動作  ---
// アイドルループ(原則時間分の回数、処理速度が足りない場合はスキップ)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void ProcMain::changeImage(UnitChara* unit,INT index)
{
	if(unit->getImageIndex() != index)
	{
		FLOAT x = unit->getPosX();
		FLOAT y = unit->getPosY();
		FLOAT z = unit->getPosZ();
		FLOAT rz = unit->getRotationZ();
		if(index == 0)
		{
			unit->copyImage(m_unitMap["ドン"].get());
			unit->setWeponFrame("Sw");
		}
		else
		{
			unit->copyImage(m_unitMap["ダルマ"].get());
			unit->setWeponFrame("HEAD");
		}
		unit->setPos(x,y,z);
		unit->setRotationZ(rz);
		unit->createBoundingBox();
		unit->setImageIndex(index);
		unit->setAnimation("Walk");
	}
}
void ProcMain::onIdle(HWND hWnd)
{
	if(GetAsyncKeyState(VK_F1)<0)
	{
		changeImage(m_systemData.unitMain,1);
	}
	else if(GetAsyncKeyState(VK_F2)<0)
	{
		changeImage(m_systemData.unitMain,0);
	}
	if(m_changeScreen)
	{
		DIRECT3D::Device::setFullScreen(!DIRECT3D::Device::isFullScreen());
		m_changeScreen = false;
	}
	if (::GetForegroundWindow() == hWnd)
	{
		POINT p,p2;
		GetCursorPos(&p);
		p2 = p;
		ScreenToClient(hWnd,&p2);
		m_camera.setMousePoint(p2);
	
		if(GetAsyncKeyState(VK_RBUTTON) &1)
		{
			m_mousePoint = p;
			m_mousePoint2 = p;

			CURSORINFO ci;
			ci.cbSize = sizeof(CURSORINFO);
			GetCursorInfo(&ci);
			if(ci.flags & CURSOR_SHOWING)
				PostMessage(hWnd,WM_MOUSESHOW,false,0);
			m_mouseRDown = true;
		}
		else if(GetAsyncKeyState(VK_RBUTTON))
		{
			m_camera.moveRotationX((FLOAT)p.y-m_mousePoint2.y);
			Unit* unit = m_camera.getUnit();
			unit->setRotationZ(unit->getRotationZ()+p.x-m_mousePoint2.x);
			m_mousePoint2 = p;
		}
		else
		{
			if(m_mouseRDown)
			{
				SetCursorPos(m_mousePoint.x,m_mousePoint.y);
				PostMessage(hWnd,WM_MOUSESHOW,true,0);
				m_mouseRDown = false;
			}
		}
	}
	m_world.onIdel(NULL);
	CHAR buff[1024];
	sprintf(buff,"%6.0f,%6.0f",m_systemData.unitMain->getPosX(),m_systemData.unitMain->getPosY());
	m_unitDebug.setText(buff);

}
