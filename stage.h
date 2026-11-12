//========================================================
// 
// Stage処理[stage.h]
// Author : Kajino Hiyori _Create2026/11/12
//
//========================================================
#ifndef _STAGE_H_
#define	_STAGE_H_
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "object.h"
#include "object3D.h"

//=============================
// マクロ定義
//=============================
#define MAX_WALL	(4)	// 壁の最大数

//=============================
// 前方宣言
//=============================
class CObject3D;	// 地面
class CObjectWall;	// 壁

//=============================
// プレイヤークラス
//=============================
class CStage : public CObject
{
public:
	CStage(CObject::PRIORITY priority = CObject::PRIORITY_MESH);
	~CStage();
	static CStage* Create(const char* pFilePath);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const char* pFilePath);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) {}
	float GetFloor(void) { return m_pField->GetPosition().y; }
	float GetWidth(void) { return m_pField->GetWidth(); }
	float GetDepth(void) { return m_pField->GetDepth();}

private:
	HRESULT LoadScripts(const char* pFilePath);
	bool LoadField(const char* pFilePath);
	bool LoadWall(const char* pFilePath);
	CObject3D* m_pField;
	CObjectWall* m_apWall[MAX_WALL];
};

#endif