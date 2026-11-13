//==========================================
// 
// ゲーム処理[game.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
//=============================
// インクルードファイル
//=============================
#include "object2D.h"
#include "game.h"
#include "manager.h"
#include "input.h"
#include "loadscripts.h"
#include "debugproc.h"
#include "player.h"
#include "stage.h"

//=============================
// 静的メンバ変数宣言
//=============================
CPlayer* CGame::m_pPlayer = NULL;	// プレイヤーへのポインタ
CStage* CGame::m_pStage = NULL;		// ステージへのポインタ
CMotionManager* CGame::m_pMotionManager = NULL;	// モーション管理へのポインタ

//=============================
// マクロ定義
//=============================
#define STAGE_SETTINGPATH	("data\\SCRIPTS\\stage_setting.txt")	// ステージ情報

//=============================
// コンストラクタ
//=============================
CGame::CGame(MODE mode) : CScene(mode)
{
	// 値のクリア
	m_state = STATE_NONE;
}

//=============================
// デストラクタ
//=============================
CGame::~CGame()
{
}

//=============================
// 初期化処理
//=============================
HRESULT CGame::Init(void)
{
	// ステート初期化
	m_state = STATE_START;

	// プレイヤー生成
	if (m_pPlayer != NULL)return E_FAIL;
	// 使用されていない場合
	m_pPlayer = CPlayer::Create(INIT_D3DVEC3, D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// ステージ生成
	if (m_pStage != NULL)return E_FAIL;
	m_pStage = CStage::Create(STAGE_SETTINGPATH);

	// モーション管理
	if (m_pMotionManager != NULL) return E_FAIL;
	m_pMotionManager = CMotionManager::Create();
	return S_OK;
}

//=============================
// 終了処理
//=============================
void CGame::Uninit(void)
{
	// 各ポインタのクリア
	m_pPlayer = NULL;	// プレイヤーへのポインタ
	m_pStage = NULL;	// ステージへのポインタ
	m_pMotionManager = NULL;	// モーション管理へのポインタ
}

//=============================
// 更新処理
//=============================
void CGame::Update(void)
{
	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	
	if (pKeyboard->GetTrigger(DIK_RETURN) == true && CManager::GetFadeState() == CFade::STATE_FADENONE)
	{ // 再読み込み
		CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_WHITE, CScene::MODE_GAME);
	}
}

//=============================
// 描画処理
//=============================
void CGame::Draw(void)
{
}