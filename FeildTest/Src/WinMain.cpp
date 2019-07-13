#include <windows.h>

//#include "../LibSrc/aflWinTool.h"
#include "ProcMain.h"


#ifdef _MSC_VER
	#ifdef _DEBUG	//���������[�N�e�X�g
		#include <crtdbg.h>
		#define malloc(a) _malloc_dbg(a,_NORMAL_BLOCK,__FILE__,__LINE__)
		inline void*  operator new(size_t size, LPCSTR strFileName, INT iLine)
			{return _malloc_dbg(size, _NORMAL_BLOCK, strFileName, iLine);}
		inline void operator delete(void *pVoid, LPCSTR strFileName, INT iLine)
			{_free_dbg(pVoid, _NORMAL_BLOCK);}
		#define NEW new(__FILE__, __LINE__)
		#define CHECK_MEMORY_LEAK _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
	#else
		#define CHECK_MEMORY_LEAK
	#endif //_DEBUG
#else
		#define CHECK_MEMORY_LEAK
#endif 

using namespace AFL;


const static char g_strWindowTitle[] = "AFL WINDOW";

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CHECK_MEMORY_LEAK

	WINDOWS::PathName path(GetCommandLineA());
	path.changePath();

	WINDOWS::Window mainWindow;	//���C���E�C���h�E
	ProcMain procMain;		//�v���V�[�W���[���C��

	//�E�C���h�E�^�C�g���̐ݒ�
	mainWindow.setTitle(g_strWindowTitle);
	//�E�C���h�E�̕\��
	mainWindow.showWindow();
	mainWindow.setClientSize(800,600);
	mainWindow.setForeground();

	procMain.init(&mainWindow);

	SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
	//�E�C���h�E��������܂ő҂�
	mainWindow.message();

	return 0;
}
