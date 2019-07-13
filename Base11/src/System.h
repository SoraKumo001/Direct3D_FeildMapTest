#pragma once

#include "App.h"
#include "aflSock.h"
#include "aflWindow.h"
#include "aflWinTool.h"
#include "afl3DWorld.h"
#include "afl3DField.h"


using namespace AFL;
using namespace AFL::WINDOWS;

#include "Unit.h"
#include "Share.h"
#include "Main.h"





//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Proc
// 実行基本クラス
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class Proc
{
public:
	Proc();

	bool init(Window* Window);
	void onCreate(Message* m);
	void onClose(Message* m);
	void onPaint(Message* m);
	void onMouseWheel(Message* m);
	void onMouseShow(Message* m);
	void onSize(Message* m);
	void onRender(HWND hWnd);
	void onAction(HWND hWnd);
	void onIdle(HWND hWnd);
	void onStart(HWND hWnd);
	void onEnd(HWND hWnd);


protected:
	SIZE m_windowSize;
	ThreadTimer m_threadTimer;
	Input m_input;
	Critical m_critical;
	bool m_mouseRDown;
	POINT m_mousePoint;
	POINT m_mousePoint2;
	Window* m_window;
	bool m_init;
	Main* m_main;
#if defined(_OPENGL)
	GLDevice m_device;
#endif
};

