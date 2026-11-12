//==========================================
// 
// ゲーム処理[game.h]
// Author : Kajino Hiyori _Create2026/06/29
// 
//==========================================
#ifndef _GAME_H_
#define _GAME_H_

//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "manager.h"

//=============================
// マクロ定義
//=============================
#define MAX_GAMEWALL	(128)	// 壁の最大数
#define MAX_FIELDLINE	(3)		// 移動レーン数

//=============================
// 前方宣言
//=============================
class CPlayer;		// プレイヤークラス

//=============================
// ゲームクラス
//=============================
class CGame : public CScene
{
public:
	// ゲームの状態
	typedef enum 
	{
		STATE_NONE = 0,	// 初期化
		STATE_START,	// 開始待ち
		STATE_GAMING,	// ゲーム中
		STATE_CLEAR,	// クリア
		STATE_LOSER,	// 失敗
		STATE_END,		// 遷移待機
		STATE_MAX
	}STATE;

	CGame(MODE mode);
	~CGame();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetState(STATE state) { m_state = state; }
	STATE GetState(void) { return m_state; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }

private:
	static CPlayer* m_pPlayer;		// プレイヤークラスへのポインタ
	STATE m_state;					// 状態管理
};
#endif