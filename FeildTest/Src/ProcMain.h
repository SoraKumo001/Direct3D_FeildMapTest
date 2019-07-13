#include <tchar.h>
#include "aflDirect3D.h"
#include "aflDirect3DField.h"
#include "aflDirect3DWorld.h"
#include "aflDirect3DUnitCustom.h"
#include "aflWindow.h"
#include "aflWinTool.h"
#include "aflSock.h"
#include "Unit.h"
#include "Share.h"
using namespace AFL;
using namespace AFL::WINDOWS;
using namespace AFL::DIRECT3D;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CameraTracking
// ’ÇÕ—pƒJƒƒ‰
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class  CameraTracking : public Camera3D
{
public:
	CameraTracking();
	void setCamera();
	void setCameraRange(INT wheel);
	void setMousePoint(POINT point);
	void moveRotationX(FLOAT value);
	void setUnit(Unit* unit);
	Unit* getUnit()const;
protected:
	Unit* m_unit;
	FLOAT m_cameraRotationX;
	FLOAT m_cameraRange;
	POINT m_mousePoint;
	FLOAT m_zoomFar;
	FLOAT m_zoomNear;
};
class UnitWindow : public DIRECT3D::UnitSprite
{
public:
	UnitWindow()
	{
		setPosW(10);
		createImage(100,100);
		getTexture()->clear(0x55ffffff);
	}
};
struct SDATA
{
	INT id;
	INT index;
	FLOAT x,y;
	DWORD stat;
};
class ProcMain
{
public:
	ProcMain();

	bool init(Window* Window);
	void onCreate(Message* pMessage);
	void onSize(Message* m);
	void onClose(Message* pMessage);
	void onPaint(Message* pMessage);
	void onMouseMove(Message* pMessage);
	void onMouseWheel(Message* pMessage);
	void onMouseShow(Message* pMessage);
	void onKeyDown(Message* pMessage);
	void onRender(HWND hWnd);
	void onAction(HWND hWnd);
	void onIdle(HWND hWnd);
	void onStart(HWND hWnd);

	void addScreenUnit(Unit* unit)
	{
		m_unit.add(unit);
		m_unitList.push_back(unit);
	}
	void delUnit(UnitChara* unit);
	UnitChara* createUnit(LPCSTR name);
	void changeImage(UnitChara* unit,INT index);

	void setNetwork(bool flag)
	{
		m_networkFlag = flag;
	}
	bool isNetwork()
	{
		return m_networkFlag;
	}
protected:
	bool m_changeScreen;
	Window* m_window;

	ThreadTimer m_threadTimer;
	DIRECT3D::World m_world;

	DIRECT3D::Unit m_tree;
	DIRECT3D::Unit* m_sky;
	DIRECT3D::Unit m_unit;
	DIRECT3D::UnitFPS m_unitFPS;
	DIRECT3D::UnitSprite m_unitText;
	DIRECT3D::UnitText m_unitDebug;
	DIRECT3D::FieldUnit m_unitField;
	Input m_input;
	CameraTracking m_camera;

	std::map<String,SP<Unit> > m_unitMap;
	std::list<SP<Unit> > m_unitList;
	std::list<Unit*> m_unitTree;

	SystemData m_systemData;

	bool m_networkFlag;
	bool m_mouseRDown;
	POINT m_mousePoint;
	POINT m_mousePoint2;

	SOCK::Sock m_sock;
	INT m_id;
	INT m_networkCount;

	String m_message;
	String m_treeName;

};

