//========================================================
// 
// MotionManager処理[leftmotion_manager.cpp]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "motion_manager.h"
#include "motion.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "manager.h"
#include "scene.h"
#include "game.h"

//=============================
// マクロ定義
//=============================

//=============================
// コンストラクタ
//=============================
CMotionManager::CMotionManager(CObject::PRIORITY priority) : CObject(priority)
{
	
}

//=============================
// デストラクタ
//=============================
CMotionManager::~CMotionManager()
{

}

//=============================
// MotionManager生成処理
//=============================
CMotionManager* CMotionManager::Create()
{
	CMotionManager* pMotionManager = NULL;	// オブジェクトへのポインタを用意
	if (pMotionManager == NULL)
	{ // NULLが設定できている場合
		pMotionManager = new CMotionManager;
		if (pMotionManager != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pMotionManager->Init();

			// 種類設定
			pMotionManager->SetType(TYPE_MOTION_MANAGER);

			return pMotionManager;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CMotionManager::Init()
{
	// 初期化
	m_state = STATE_NORMALUPDATE;
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CMotionManager::Uninit(void)
{
	
}

//=============================
// 更新処理
//=============================
void CMotionManager::Update(void)
{
	CDebugProc* pDebug =  CManager::GetDebugProc();
	switch (m_state)
	{
	case STATE_NORMALUPDATE:	// 通常更新
		pDebug->Print("//=================================\n");
		pDebug->Print("// 通常更新モード\n");
		pDebug->Print("//=================================\n");
		break;
	}
}

//=============================
// 描画処理
//=============================
void CMotionManager::Draw(void)
{
}