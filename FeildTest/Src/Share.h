#pragma once

enum CTLKEYS
{
	UP,DOWN,LEFT,RIGHT,ATACK,JUMP,ZOOMUP,ZOOMDOWN
};
struct UnitRemote
{
	FLOAT x,y;
	UnitChara* unitChara;
	DWORD lastTime;
};
struct SystemData
{
	AFL::DIRECT3D::FieldUnit* field;
	DWORD interval;
	std::list<UnitChara*> unitChara;
	UnitChara* unitMain;
	std::map<INT,UnitRemote> unitRemote;
};