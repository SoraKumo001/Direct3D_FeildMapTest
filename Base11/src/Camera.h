#if !defined(_OPENGL) & !defined(__ANDROID__)
#include "aflD3DUnit.h"
#else
#include "aflOpenGLUnit.h"
#endif
using namespace AFL;

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
	FLOAT m_zoomFar;
	FLOAT m_zoomNear;
};