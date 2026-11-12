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

//=============================
// 静的メンバ変数宣言
//=============================
CPlayer* CGame::m_pPlayer = NULL;	// プレイヤーへのポインタ
//CWall* CGame::m_apWall[MAX_GAMEWALL] = {};	// 壁のポインタ
//CMachine* CGame::m_pMachine = NULL;	// マシンへのポインタ
//CTimer* CGame::m_pTimer = NULL;		// タイマーへのポインタ
//CPause* CGame::m_pPause = NULL;		// ポーズへのポインタ
//CGameUI* CGame::m_pGameUI = NULL;	// UIへのポインタ

//=============================
// マクロ定義
//=============================
#define GAME_TIME		(60)	// ゲーム時間
#define FIELD_WIDTH		(100.0f)	// フィールド幅
#define FIELD_DEPTH		(5000.0f)	// フィールド奥行き
#define FIELD_SPACE		(200.0f)		// フィールド間の隙間
#define FIELD_POS(x)	(D3DXVECTOR3(FIELD_SPACE * 2.0f * (float)(x - 1), 0.0f, 0.0f))	// フィールド位置
// ファイルパス
#define BLOCK_SETTINGPATH	("data\\SCRIPTS\\block_game.txt")	// ゲーム内でのブロック設置情報
#define ITEM_SETTINGPATH	("data\\SCRIPTS\\item_game.txt")	// ゲーム内でのアイテム設置情報
#define MESHSKIN_PATH		("data\\SCRIPTS\\meshskin_flat.txt")	// メッシュスキンの設置情報

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
	m_pPlayer = CPlayer::Create(INIT_D3DVEC3, D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));
	//m_pPlayer = CPlayer::Create(INIT_D3DVEC3, D3DXVECTOR3(0.0f, HALF_D3DXPI, 0.0f));

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CGame::Uninit(void)
{
	// 各ポインタのクリア
	m_pPlayer = NULL;	// プレイヤーへのポインタ
}

//=============================
// 更新処理
//=============================
void CGame::Update(void)
{
	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	
	if ((pKeyboard->GetTrigger(DIK_RETURN) == true || pJoypad->GetAnyButton() == true))
	{ // リザルトに遷移
		CManager::GetFade()->SetFade(CFade::TYPE_ALPHA, SCREEN_CENTER, COLOR_WHITE, CScene::MODE_GAME);
	}
}

//=============================
// 描画処理
//=============================
void CGame::Draw(void)
{
}