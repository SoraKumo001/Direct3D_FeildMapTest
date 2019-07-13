#pragma once

enum CTLKEYS
{
	UP,DOWN,LEFT,RIGHT,ATACK,JUMP,ZOOMUP,ZOOMDOWN
};
struct UnitRemote
{
	FLOAT x,y;
	FLOAT speed;
	FLOAT life;
	UnitChara* unitChara;
	DWORD lastTime;
};
struct SystemData
{
	AFL::FieldUnit* field;
	DWORD interval;
	std::list<UnitChara*> unitChara;
	UnitChara* unitMain;
	std::map<INT,UnitRemote> unitRemote;
	LPVOID world;
};