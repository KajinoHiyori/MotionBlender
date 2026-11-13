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
		STATE_NORMALUPDATE = 0,	// 全てのモーションが稼働
		STATE_MAX
	}STATE;

	CMotionManager(CObject::PRIORITY priority = CObject::PRIORITY_MOTIONMANAGER);
	~CMotionManager();
	static CMotionManager* Create();
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	STATE GetState(void) { return m_state; }

private:
	STATE m_state;	// 現在のエディタの状態
};

#endif