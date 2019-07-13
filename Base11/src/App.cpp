#include <windows.h>
#include <clocale>
#include "System.h"

//----------------------------------------------------
//メモリリークテスト用
#if _MSC_VER && !defined(_WIN32_WCE)
#ifdef _DEBUG	//メモリリークテスト
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
// ---  動作  ---
// プログラム開始関数
// ---  引数  ---
// --
// --- 戻り値 ---
// --
//-----------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nCmdShow)
{
	CHECK_MEMORY_LEAK			//メモリーリーク検出用
	setlocale(LC_ALL, "");

	//カレントディレクトリを実行ファイルと同じ位置に設定
	WINDOWS::PathName path(GetCommandLineA());
	path.changePath();

	Window mainWindow;			//メインウインドウ
	Proc* proc = NEW Proc;		//プロシージャーメイン
	
	proc->init(&mainWindow);	//プロシージャクラスの初期化
	mainWindow.message();		//ウインドウが閉じられるまで待つ
	delete proc;				//プロシージャの解放
	return 0;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// AppData
// アプリケーションデータ管理用
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//共通データ用グローバルインスタンス
AppData App;

//-----------------------------------------------
// ---  動作  ---
// コンストラクタ
// ---  引数  ---
// 無し
// --- 戻り値 ---
// 
//-----------------------------------------------
AppData::AppData()
{
}

