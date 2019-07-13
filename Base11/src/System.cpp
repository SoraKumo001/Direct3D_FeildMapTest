#include <windows.h>
#include "System.h"

using namespace AFL;

//マウス表示用
#define WM_MOUSESHOW (WM_APP+1)

#if defined(_OPENGL)
	#pragma comment(lib, "opengl32.lib")
#endif
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Proc
// 実行基本クラス
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-----------------------------------------------
// ---  動作  ---
// コンストラクタ
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
Proc::Proc()
{
	m_init = false;
	m_window = NULL;

	//マウス入力初期値
	m_mousePoint.x = 0;
	m_mousePoint.y = 0;
	m_mouseRDown = false;

	//キーボードと処理イベントの関連づけ
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
}

//-----------------------------------------------
// ---  動作  ---
// ウインドウ作成
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
bool Proc::init(Window* window)
{
	m_window = window;

	//ウインドウタイトルの設定
	window->setTitle(L"AFL WINDOW");
	//ウインドウの表示
	window->showWindow();
	window->setClientSize(800,600);
	window->setForeground();

	//ウインドウメッセージのフック
	window->addMessage(WM_PAINT,CLASSPROC(this,Proc,onPaint));
	window->addMessage(WM_CREATE,CLASSPROC(this,Proc,onCreate));
	window->addMessage(WM_CLOSE,CLASSPROC(this,Proc,onClose));
	window->addMessage(WM_SIZE,CLASSPROC(this,Proc,onSize));
	window->addMessage(WM_MOUSEWHEEL,CLASSPROC(this,Proc,onMouseWheel));


	m_threadTimer.setRender(CLASSPROC(this,Proc,onRender));	//レンダリング用
	m_threadTimer.setAction(CLASSPROC(this,Proc,onAction));	//動作制御用
	m_threadTimer.setIdle(CLASSPROC(this,Proc,onIdle));		//動作制御用
	m_threadTimer.setStart(CLASSPROC(this,Proc,onStart));	//開始設定用
	m_threadTimer.setEnd(CLASSPROC(this,Proc,onEnd));		//終了処理用


	m_threadTimer.setInterval(20);
	m_threadTimer.setWait(true);
	m_threadTimer.start(window->getWnd());					//スレッド動作開始
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
void Proc::onCreate(Message* m)
{

}

//-----------------------------------------------
// ---  動作  ---
// ウインドウクローズ
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onClose(Message* m)
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
void Proc::onPaint(Message* m)
{
	if(!m_init)
	{
		LPCWSTR text = L"起動準備中";
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(m->getWnd(), &ps); 
		TextOutW(hDC,0,0,text,lstrlenW(text));
		EndPaint(m->getWnd(),&ps);
	}
}

//-----------------------------------------------
// ---  動作  ---
// ウインドウサイズの変更
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onSize(Message* m)
{
	if (m_main)
	{
		m_critical.lock();
		m_main->size(m->getWnd(), m->getX(), m->getY());
		m_critical.unlock();
	}
}


//-----------------------------------------------
// ---  動作  ---
// レンダリング(レンダリング前に実行される)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onRender(HWND hWnd)
{
	m_critical.lock();
	m_main->render();
#if defined(_OPENGL)
	HDC hdc = m_window->getDC();
	SwapBuffers(hdc);
	m_window->releaseDC(hdc);
#endif
	m_critical.unlock();



}

//-----------------------------------------------
// ---  動作  ---
// 開始処理(スレッド開始時に一回実行される)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onStart(HWND hWnd)
{
#if defined(_OPENGL)
	m_device.init(hWnd);
	GLDevice::setTexturePath("Images");
	GLDevice::loadShaders(L"glsl/");
#else
	D3DDevice::setTexturePath(L"images\\");

#endif
	m_main = new Main();

	m_main->setInput(&m_input);
	m_main->size(hWnd, m_window->getClientWidth(), m_window->getClientHeight());
	m_main->init(hWnd);
}

//-----------------------------------------------
// ---  動作  ---
// 終了処理(スレッド終了時に一回実行)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onEnd(HWND hWnd)
{
	delete m_main;
}

//-----------------------------------------------
// ---  動作  ---
// メインループ(指定した時間分の回数を必ず実行)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onAction(HWND hWnd)
{
	m_critical.lock();
	//ウインドウサイズ変更時の処理
	if (m_window->getClientWidth() != m_windowSize.cx || m_window->getClientHeight() != m_windowSize.cy)
	{
		m_windowSize.cx = m_window->getClientWidth();
		m_windowSize.cy = m_window->getClientHeight();
		m_main->size(hWnd,m_windowSize.cx, m_windowSize.cy);
	}
	m_main->action();

	m_critical.unlock();


}

//-----------------------------------------------
// ---  動作  ---
// アイドルループ(原則時間分の回数、処理速度が足りない場合はスキップ)
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
void Proc::onIdle(HWND hWnd)
{
	if(GetAsyncKeyState(VK_F1)<0)
	{
		m_main->changeImage(0);
	}
	else if(GetAsyncKeyState(VK_F2)<0)
	{
		m_main->changeImage(1);
	}

	if(::GetForegroundWindow() == hWnd)
	{
		POINT p,p2;
		GetCursorPos(&p);
		p2 = p;
		ScreenToClient(hWnd,&p2);

		if(GetAsyncKeyState(VK_RBUTTON) & 1)
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
			FLOAT rotationX = (FLOAT)p.y - m_mousePoint2.y;
			m_main->rotationCameraX(rotationX);

			FLOAT rotationZ = (FLOAT)p.x - m_mousePoint2.x;
			m_main->rotationCameraZ(rotationZ);
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
}
void Proc::onMouseWheel(Message* pMessage)
{
	if (::GetForegroundWindow() == pMessage->getWnd())
	{
		INT wheel = pMessage->getLParamHi();
		int f = 1;
		if((SHORT)pMessage->getWParamHi() > 0)
			f = -1;
		m_main->setZoom(wheel*f);
	}

	
}
