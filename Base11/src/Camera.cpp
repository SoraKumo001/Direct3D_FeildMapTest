#ifndef __ANDROID__
#include <windows.h>
#else
#include <android/log.h>
#endif

#if !defined(_OPENGL) & !defined(__ANDROID__)
#include "aflD3DUnit.h"
#else
#include "aflOpenGLUnit.h"
#endif

#include "Camera.h"

using namespace AFL;

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
	if (!m_unit)
		return;
	Unit* unit = m_unit;
	float fRange = m_cameraRange;
	float fRX = getRotationX()*3.14f / 180;
	float fRZ = unit->getRotationZ()*3.14f / 180;
	FLOAT fX, fY, fZ;
	fX = -(float)sin(fRZ)*fRange * (float)cos(fRX);
	fY = (float)cos(fRZ)*fRange * (float)cos(fRX);
	fZ = (float)sin(fRX)*fRange;

	fX += -getX() + unit->getPosX();
	fY += -getY() + unit->getPosY();
	fZ += -getZ() + unit->getPosZ() + 100;

	setRotationZ(getRotationZ() + (unit->getRotationZ() - getRotationZ())*0.3f);
	setRotationX(getRotationX() + (m_cameraRotationX - getRotationX())*0.3f);
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
	if (m_cameraRange > m_zoomFar)
		m_cameraRange = m_zoomFar;
	else if (m_cameraRange < m_zoomNear)
		m_cameraRange = m_zoomNear;
}

void CameraTracking::moveRotationX(FLOAT value)
{
	m_cameraRotationX += value;
	if (m_cameraRotationX > 100.0f)
	{
		m_cameraRotationX = 100.0f;
	}
	else if (m_cameraRotationX < -50.0f)
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