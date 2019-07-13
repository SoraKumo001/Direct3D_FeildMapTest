#include <windows.h>
#include <clocale>
#include "System.h"

//----------------------------------------------------
//���������[�N�e�X�g�p
#if _MSC_VER && !defined(_WIN32_WCE)
#ifdef _DEBUG	//���������[�N�e�X�g
#include <crtdbg.h>
#define malloc(a) _malloc_dbg(a,_NORMAL_BLOCK,__FILE__,__LINE__)
inline void* operator new(size_t size, LPCSTR strFileName, INT iLine)
{
	return _malloc_dbg(size, _NORMAL_BLOCK, strFileName, iLine);
}
inline void operator delete(void* pVoid, LPCSTR strFileName, INT iLine)
{
	_free_dbg(pVoid, _NORMAL_BLOCK);
}
#define NEW new(__FILE__, __LINE__)
#define CHECK_MEMORY_LEAK _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
#else
#define NEW new
#define CHECK_MEMORY_LEAK
#endif //_DEBUG
#else
#define CHECK_MEMORY_LEAK
#endif
//----------------------------------------------------


using namespace AFL;
using namespace AFL::WINDOWS;


//-----------------------------------------------
// ---  ����  ---
// �v���O�����J�n�֐�
// ---  ����  ---
// --
// --- �߂�l ---
// --
//-----------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nCmdShow)
{
	CHECK_MEMORY_LEAK			//�������[���[�N���o�p
	setlocale(LC_ALL, "");

	//�J�����g�f�B���N�g�������s�t�@�C���Ɠ����ʒu�ɐݒ�
	WINDOWS::PathName path(GetCommandLineA());
	path.changePath();

	Window mainWindow;			//���C���E�C���h�E
	Proc* proc = NEW Proc;		//�v���V�[�W���[���C��
	
	proc->init(&mainWindow);	//�v���V�[�W���N���X�̏�����
	mainWindow.message();		//�E�C���h�E��������܂ő҂�
	delete proc;				//�v���V�[�W���̉��
	return 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// AppData
// �A�v���P�[�V�����f�[�^�Ǘ��p
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//���ʃf�[�^�p�O���[�o���C���X�^���X
AppData App;

//-----------------------------------------------
// ---  ����  ---
// �R���X�g���N�^
// ---  ����  ---
// ����
// --- �߂�l ---
// 
//-----------------------------------------------
AppData::AppData()
{
}

