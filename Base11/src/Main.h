#include "App.h"
#include "aflSock.h"
//#include "aflD3D11.h"
//#include "aflD3DUnit.h"
#include "afl3DWorld.h"
#include "afl3DField.h"
//#include "aflLeap.h"

#include "Camera.h"
#include "Unit.h"
#include "Share.h"
#include "Effect.h"


using namespace AFL;





struct SDATA
{
	INT id;
	INT index;
	FLOAT x, y;
	DWORD stat;
	FLOAT speed;
	FLOAT rotation;
	FLOAT life;
	CHAR name[21];
};

class Main
{
public:
	Main();
	~Main();
	void release();
	void init(LPVOID hWnd);

	void size(LPVOID hWnd,int width, int height);
	DWORD action(LPVOID data = NULL);
	DWORD render(LPVOID data = NULL);
	void render(Mesh* mesh);
	void setInput(AFL::Input* input);
	void setZoom(INT value);
	void rotationCameraX(FLOAT value);
	void rotationCameraZ(FLOAT value);
	void changeImage(INT index);
protected:
	UnitChara* createUnit(LPCSTR name);
	void changeImage(UnitChara* unit, INT index);
	void delUnit(UnitChara* unit);

	INT m_screenWidth;
	INT m_screenHeight;

	World m_world;
	UnitChara* m_chara;
	Screen m_screen;
	Unit m_unit;
	Unit* m_sky;
	UnitSprite m_sprite;
	UnitText m_text;
	UnitFPS m_fps;
	FieldUnit m_field;
	FieldWater m_water;

	CameraTracking m_camera;

	std::map<String, SP<Unit> > m_unitMap;
	std::list<SP<Unit> > m_unitList;

	Input* m_input;

	SystemData m_systemData;


	std::list<Unit*> m_unitTree;
	bool m_networkFlag;
	SOCK::Sock m_sock;
	INT m_id;
	INT m_networkCount;
	UnitCloud m_effect;
	UnitSpark m_spark;
};

