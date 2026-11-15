//========================================================
// 
// MotionManager処理[motion_manager.h]
// Author : Kajino Hiyori _Create2026/11/04
//
//========================================================
#ifndef _MOTION_MANAGER_H_
#define	_MOTION_MANAGER_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"

//=============================
// 前方宣言
//=============================
class CMotion;	// モーションクラス
class CPlayer;	// プレイヤークラス

//=============================
// モーション管理クラス
//=============================
class CMotionManager : public CObject
{
public:
	// エディタの状態
	typedef enum
	{
		MODE_STANDERD = 0,	// 全てのモーションが稼働
		MODE_KEY,		// キーを更新
		STATE_MAX
	}MODE;

	CMotionManager(CObject::PRIORITY priority = CObject::PRIORITY_MOTIONMANAGER);
	~CMotionManager();
	static CMotionManager* Create();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	MODE GetMode(void) { return m_mode; }

private:
	void SetMode(void);
	MODE m_mode;	// 現在のエディタの状態
};

#endif