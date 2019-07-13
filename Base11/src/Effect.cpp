#ifndef __ANDROID__
#include <windows.h>
#else
#include <android/log.h>
#endif

#include "Effect.h"

using namespace AFL;

UnitSpark::UnitSpark()
{
	openImage("image/ef04.tif");
}
void UnitSpark::add(FLOAT x, FLOAT y, FLOAT z)
{
	INT count = 7;
	INT color = 0x50050520;
	INT margin = 10;
	INT alive = 4;
	FLOAT size = 300.0f;

	for (INT i = 0; i<count; i++)
		UnitEffect::add(x + rand() % margin - margin*0.5f, y + rand() % margin - margin*0.5f, z - rand() % margin - margin*0.5f, size - rand() % 150, color, alive, (FLOAT)(rand() % 360));

}
void UnitSpark::add2(FLOAT x, FLOAT y, FLOAT z)
{
	INT count = 10;
	INT color = 0x50250505;
	INT margin = 20;
	INT alive = 10;
	FLOAT size = 600.0f;

	for (INT i = 0; i<count; i++)
		UnitEffect::add(x + rand() % margin - margin*0.5f, y + rand() % margin - margin*0.5f, z - rand() % margin - margin*0.5f, size + rand() % 150-150*0.5f, color, alive, (FLOAT)(rand() % 360));

}
UnitCloud::UnitCloud()
{
	openImage("image/crowd.png");
}
void UnitCloud::add(FLOAT x, FLOAT y, FLOAT z)
{
	INT count = 3;
	INT color = 0x15020202;
	INT margin = 1;
	INT alive = 20;
	FLOAT size = 16.0f;

	for (INT i = 0; i<count; i++)
		UnitEffect::add(x + rand() % margin, y + rand() % margin, z - rand() % margin, size - rand() % 150, color, alive, (FLOAT)(rand() % 360));

}