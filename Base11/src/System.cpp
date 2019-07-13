#include <windows.h>
#include "System.h"

using namespace AFL;

//�}�E�X�\���p
#define WM_MOUSESHOW (WM_APP+1)

#if defined(_OPENGL)
	#pragma comment(lib, "opengl32.lib")
#endif
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Proc
// ���s��{�N���X
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-----------------------------------------------
// ---  ����  ---
// �R���X�g���N�^
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
Proc::Proc()
{
	m_init = false;
	m_window = NULL;

	//�}�E�X���͏����l
	m_mousePoint.x = 0;
	m_mousePoint.y = 0;
	m_mouseRDown = false;

	//�L�[�{�[�h�Ə����C�x���g�̊֘A�Â�
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
// ---  ����  ---
// �E�C���h�E�쐬
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
bool Proc::init(Window* window)
{
	m_window = window;

	//�E�C���h�E�^�C�g���̐ݒ�
	window->setTitle(L"AFL WINDOW");
	//�E�C���h�E�̕\��
	window->showWindow();
	window->setClientSize(800,600);
	window->setForeground();

	//�E�C���h�E���b�Z�[�W�̃t�b�N
	window->addMessage(WM_PAINT,CLASSPROC(this,Proc,onPaint));
	window->addMessage(WM_CREATE,CLASSPROC(this,Proc,onCreate));
	window->addMessage(WM_CLOSE,CLASSPROC(this,Proc,onClose));
	window->addMessage(WM_SIZE,CLASSPROC(this,Proc,onSize));
	window->addMessage(WM_MOUSEWHEEL,CLASSPROC(this,Proc,onMouseWheel));


	m_threadTimer.setRender(CLASSPROC(this,Proc,onRender));	//�����_�����O�p
	m_threadTimer.setAction(CLASSPROC(this,Proc,onAction));	//���쐧��p
	m_threadTimer.setIdle(CLASSPROC(this,Proc,onIdle));		//���쐧��p
	m_threadTimer.setStart(CLASSPROC(this,Proc,onStart));	//�J�n�ݒ�p
	m_threadTimer.setEnd(CLASSPROC(this,Proc,onEnd));		//�I�������p


	m_threadTimer.setInterval(20);
	m_threadTimer.setWait(true);
	m_threadTimer.start(window->getWnd());					//�X���b�h����J�n
	return true;
}

//-----------------------------------------------
// ---  ����  ---
// �E�C���h�E�쐬
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
void Proc::onCreate(Message* m)
{

}

//-----------------------------------------------
// ---  ����  ---
// �E�C���h�E�N���[�Y
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
void Proc::onClose(Message* m)
{
	//�X���b�h�ɒ�~����
	m_threadTimer.stop();
}

//-----------------------------------------------
// ---  ����  ---
// �����_�����O
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
void Proc::onPaint(Message* m)
{
	if(!m_init)
	{
		LPCWSTR text = L"�N��������";
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(m->getWnd(), &ps); 
		TextOutW(hDC,0,0,text,lstrlenW(text));
		EndPaint(m->getWnd(),&ps);
	}
}

//-----------------------------------------------
// ---  ����  ---
// �E�C���h�E�T�C�Y�̕ύX
// ---  ����  ---
// ����
// --- �߂�l ---
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
// ---  ����  ---
// �����_�����O(�����_�����O�O�Ɏ��s�����)
// ---  ����  ---
// ����
// --- �߂�l ---
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
// ---  ����  ---
// �J�n����(�X���b�h�J�n���Ɉ����s�����)
// ---  ����  ---
// ����
// --- �߂�l ---
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
// ---  ����  ---
// �I������(�X���b�h�I�����Ɉ����s)
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
void Proc::onEnd(HWND hWnd)
{
	delete m_main;
}

//-----------------------------------------------
// ---  ����  ---
// ���C�����[�v(�w�肵�����ԕ��̉񐔂�K�����s)
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
void Proc::onAction(HWND hWnd)
{
	m_critical.lock();
	//�E�C���h�E�T�C�Y�ύX���̏���
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
// ---  ����  ---
// �A�C�h�����[�v(�������ԕ��̉񐔁A�������x������Ȃ��ꍇ�̓X�L�b�v)
// ---  ����  ---
// ����
// --- �߂�l ---
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
