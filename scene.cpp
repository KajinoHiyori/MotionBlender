//==========================================
// 
// 画面管理処理[mode.cpp]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "scene.h"
#include "game.h"

//=============================
// コンストラクタ
//=============================
CScene::CScene(MODE mode) : CObject(CObject::PRIORITY_SCENE)
{
	m_mode = mode;
}

//=============================
// デストラクタ
//=============================
CScene::~CScene()
{

}

//=============================
// シーン生成処理
//=============================
CScene* CScene::Create(MODE mode, bool bClear)
{
	CScene* pScene = NULL;	// シーンクラス型のポインタを用意

	switch (mode)
	{
	case MODE_GAME:	// ゲーム画面
		pScene = new CGame(mode);
		break;
	}

	// モードごとの初期化処理
	pScene->Init();
	return pScene;
}