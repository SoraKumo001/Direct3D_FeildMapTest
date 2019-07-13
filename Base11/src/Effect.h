#if !defined(_OPENGL) & !defined(__ANDROID__)
#include "aflD3DUnit.h"
#else
#include "aflOpenGLUnit.h"
#endif
using namespace AFL;

class UnitSpark : public UnitEffect
{
public:
	UnitSpark();
	void add(FLOAT x, FLOAT y, FLOAT z);
	void add2(FLOAT x, FLOAT y, FLOAT z);
protected:
};
class UnitCloud : public UnitEffect
{
public:
	UnitCloud();
	void add(FLOAT x, FLOAT y, FLOAT z);
protected:
};