//========================================================
// 
// Player処理[leftplayer.cpp]
// Author : Kajino Hiyori _Create2026/06/02
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "player.h"
#include "camera.h"
#include "input.h"
#include "color.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "model.h"
#include "scene.h"
#include "game.h"
#include "stage.h"
#include "motion_manager.h"

//=============================
// マクロ定義
//=============================
#define NUM_DIVMOTION		(7)		// 分割時のモデル数
#define BLEND_FRAME			(10)	// ブレンドモーションのフレーム数
#define SPEED				(1.0f)	// 移動速度
#define JUMP		(40.0f)	// ジャンプ
#define CHANGE_X	(40.0f)	// X方向切り替え
#define RADIUS		(45.0f)		// 半径
#define HEIGHT		(117.0f)	// 身長
#define MARGINE		(0.1f)		// 余白
// ファイルパス
#define SETTING_PATH		("data\\SCRIPTS\\player_setting.txt")	// プレイヤー設定情報
#define UPPERMOTION_PATH	("data\\SCRIPTS\\player_uppermotion.txt")	// 上半身モーション
#define UNDERMOTION_PATH	("data\\SCRIPTS\\player_undermotion.txt")	// 下半身モーション

//=============================
// コンストラクタ
//=============================
CPlayer::CPlayer(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_pos = INIT_D3DVEC3;		// モデルの位置
	m_posOld = INIT_D3DVEC3;	// モデルの1フレーム前の位置
	m_move = INIT_D3DVEC3;		// モデルの移動量
	m_rot = INIT_D3DVEC3;		// モデルの向き
	m_rotDest = INIT_D3DVEC3;	// モデルの目的の向き
	memset(&m_apModel[0], NULL, sizeof m_apModel);	// モデルのポインタ
	m_nNumModel = 0;			// モデル総数
	m_apMotion[DIV_MOTION_MAX] = {};			// モーション情報
	for (int nCntModel = 0; nCntModel < MODEL_MAX; nCntModel++)
	{
		memset(&m_aFileName[nCntModel][0], 0, sizeof m_aFileName[nCntModel]);
	}
	m_fSpeed = 0.0f;
	m_fJump = 0.0f;
	m_bJump = false;
}

//=============================
// デストラクタ
//=============================
CPlayer::~CPlayer()
{

}

//=============================
// Player生成処理
//=============================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = NULL;	// オブジェクトへのポインタを用意
	if (pPlayer == NULL)
	{ // NULLが設定できている場合
		pPlayer = new CPlayer;
		if (pPlayer != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pPlayer->Init(pos, rot);

			// 種類設定
			pPlayer->SetType(TYPE_PLAYER);

			return pPlayer;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 初期化
	m_pos = pos;
	m_posOld = pos;
	m_move = INIT_D3DVEC3;
	m_rot = rot;
	m_rotDest = rot;
	m_fSpeed = SPEED;
	m_fJump = JUMP;
	// モデル読み込み
	LoadScripts();

	if (m_apMotion[DIV_MOTION_UNDER] != NULL)
	{
		m_apMotion[DIV_MOTION_UNDER]->Set(MOTIONUNDER_WALK, false, 0);
	}
	if (m_apMotion[DIV_MOTION_UPPER] != NULL)
	{
		m_apMotion[DIV_MOTION_UPPER]->Set(MOTIONUPPER_NUTORAL, false, 0);
	}

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CPlayer::Uninit(void)
{
	// モデルの終了処理
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		SAFE_UNINIT(m_apModel[nCntModel]);
	}

	// モーションの破棄
	for (int nCntDiv = 0; nCntDiv < DIV_MOTION_MAX; nCntDiv++)
	{
		if (m_apMotion[nCntDiv] == NULL) continue;
		// モーションが用意されていた場合
		SAFE_DELETE_SINGULAR(m_apMotion[nCntDiv]);
		
	}
}

//=============================
// 更新処理
//=============================
void CPlayer::Update(void)
{
	// シーン情報
	CScene::MODE mode = CManager::GetMode();

	if (mode != CScene::MODE_GAME) return;

	// ゲーム処理
	UpdateGame();

	// デバック表示=================================================================================
	CDebugProc* pDebugProc = CManager::GetDebugProc();
	pDebugProc->Print("Playerの位置 : (%.2f, %.2f, %.2f)\n", m_pos.x, m_pos.y, m_pos.z);
	pDebugProc->Print("Player向き : (%.2f, %.2f, %.2f)\n", m_rot.x, m_rot.y, m_rot.z);
	pDebugProc->Print("Player移動量 : (%.2f, %.2f, %.2f)\n", m_move.x, m_move.y, m_move.z);
	pDebugProc->Print("Player速度 : %f\n", m_fSpeed);
}

//=============================
// 描画処理
//=============================
void CPlayer::Draw(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxGauge;	// ゲージ用マトリックス

	// ワールドマトリックスの初期化(デフォルトの値にする)
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ゲージ用のマトリックスを格納
	mtxGauge = m_mtxWorld;

	// モデルごとに描画
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (m_apModel[nCntModel] == NULL)continue;
		 // 使用されていた場合
		// モデルの描画
		m_apModel[nCntModel]->Draw();
		
	}
}

//=============================
// ゲーム処理
//=============================
void CPlayer::UpdateGame(void)
{
	CGame* pGame = static_cast<CGame*>(CManager::GetScene());

	switch (pGame->GetMotionManagerState())
	{
	case CMotionManager::STATE_NORMALUPDATE:	// 通常処理
		UpdateNormalUpdate();
		break;
	}
}

//=============================
// STATE_NORMALUPDATE処理
//=============================
void CPlayer::UpdateNormalUpdate(void)
{
	CGame* pGame = static_cast<CGame*>(CManager::GetScene());
	
	for (int nCntMotion = 0; nCntMotion < DIV_MOTION_MAX; nCntMotion++)
	{ // モーションの更新
		m_apMotion[nCntMotion]->Update();
	}

	// 移動量管理
	SpeedManager();

	// ジャンプ量管理
	JumpManager();

	// 移動処理
	Move();

	// ジャンプ処理
	Jump();

	// 位置/角度更新処理
	Setter();

	// ステージとの判定
	CollisionStage(pGame->GetStage());

	CDebugProc* pDebug = CManager::GetDebugProc();
	pDebug->Print("移動[W/A/S/D]\n");
	pDebug->Print("ジャンプ[SPACE]\n");
	pDebug->Print("プレイヤーの移動量[← / →] %f\n", m_fSpeed);
	pDebug->Print("プレイヤーのジャンプ量[↑ / ↓] %f\n", m_fJump);
}

#define SPEED_CHANGER	(0.05f)	// 移動量変更
#define SPEED_MAX		(3.0f)	// 最高速度
#define SPEED_MIN		(0.01f)	// 最低速度
#define JUMP_CHANGER	(1.0f)	// ジャンプ量変更
#define JUMP_MAX		(50.0f)	// 最高加速度
#define JUMP_MIN		(10.0f)	// 最低加速度
//=============================
// 速度管理処理
//=============================
void CPlayer::SpeedManager(void)
{
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	if (pKeyboard->GetRepeat(DIK_RIGHTARROW) == true)
	{
		m_fSpeed += SPEED_CHANGER;
		if (m_fSpeed >= SPEED_MAX)
		{
			m_fSpeed = SPEED_MAX;
		}
	}
	else if (pKeyboard->GetRepeat(DIK_LEFTARROW) == true)
	{
		m_fSpeed -= SPEED_CHANGER;
		if (m_fSpeed <= SPEED_MIN)
		{
			m_fSpeed = SPEED_MIN;
		}
	}
}

//=============================
// ジャンプ量管理処理
//=============================
void CPlayer::JumpManager(void)
{
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	if (pKeyboard->GetRepeat(DIK_UPARROW) == true)
	{
		m_fJump += JUMP_CHANGER;
		if (m_fJump >= JUMP_MAX)
		{
			m_fJump = JUMP_MAX;
		}
	}
	else if (pKeyboard->GetRepeat(DIK_DOWNARROW) == true)
	{
		m_fJump -= JUMP_CHANGER;
		if (m_fJump <= JUMP_MIN)
		{
			m_fJump = JUMP_MIN;
		}
	}
}

//=============================
// 移動処理
//=============================
void CPlayer::Move(void)
{
	// カメラ取得
	CCamera* pCamera = CManager::GetCamera();
	D3DXVECTOR3 rotCamera = pCamera->GetRotation();

	// スティックの角度を取得
	float fRotDiffKey = 0.0f;	// キー入力時の角度補正計算用
	float fRotDiffPad = 0.0f;	// ジョイパッド入力時の角度補正計算用

	// 移動管理
	D3DXVECTOR3 moveDirection = INIT_D3DVEC3;	// 移動方向の設定
	float fMoveDirection = 0.0f;	// 移動方向の管理

	float fAngle = 0.0f;	// 角度管理

	// 入力情報取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	if (pJoypad->GetLeftStickValue(&moveDirection.x, &moveDirection.z) == true)
	{ // ジョイパッド[LSTICK]の移動管理
		float fMove = sqrtf((float)(moveDirection.x * moveDirection.x + moveDirection.z * moveDirection.z)) * m_fSpeed;
		fAngle = atan2f(-(float)(moveDirection.x), (float)(moveDirection.z));
		fAngle = AngleNormalize(rotCamera.y - fAngle);

		m_move.x += sinf(fAngle) * fMove;
		m_move.z += cosf(fAngle) * fMove;
	}
	else if (pKeyboard->GetPress(DIK_A) == true)	// 右に移動
	{ // キーボード[IJKL]の移動管理
		moveDirection.x = -1.0f;
		if (pKeyboard->GetPress(DIK_W) == true)	// 奥に移動
		{
			fAngle = AngleNormalize(rotCamera.y - D3DX_PI / 4);
			m_move.x += sinf(fAngle) * m_fSpeed;
			m_move.z += cosf(fAngle) * m_fSpeed;
			moveDirection.z = 1.0f;
		}
		else if (pKeyboard->GetPress(DIK_S) == true)	// 手前に移動
		{
			fAngle = AngleNormalize(rotCamera.y + D3DX_PI / 4);
			m_move.x -= sinf(fAngle) * m_fSpeed;
			m_move.z -= cosf(fAngle) * m_fSpeed;
			moveDirection.z = -1.0f;
		}
		else
		{
			m_move.x -= cosf(rotCamera.y) * m_fSpeed;
			m_move.z += sinf(rotCamera.y) * m_fSpeed;
		}
	}
	else if (pKeyboard->GetPress(DIK_D) == true)	// 左に移動
	{
		moveDirection.x = 1.0f;
		if (pKeyboard->GetPress(DIK_W) == true)	// 奥に移動
		{
			fAngle = AngleNormalize(rotCamera.y - D3DX_PI / 4);
			m_move.x += cosf(fAngle) * m_fSpeed;
			m_move.z -= sinf(fAngle) * m_fSpeed;
			moveDirection.z = 1.0f;
		}
		else if (pKeyboard->GetPress(DIK_S) == true)	// 手前に移動
		{
			fAngle = AngleNormalize(rotCamera.y + D3DX_PI / 4);
			m_move.x += cosf(fAngle) * m_fSpeed;
			m_move.z -= sinf(fAngle) * m_fSpeed;
			moveDirection.z = -1.0f;
		}
		else
		{
			m_move.x += cosf(rotCamera.y) * m_fSpeed;
			m_move.z -= sinf(rotCamera.y) * m_fSpeed;
		}
	}
	else if (pKeyboard->GetPress(DIK_W) == true)	// 奥に移動
	{
		m_move.x += sinf(rotCamera.y) * m_fSpeed;
		m_move.z += cosf(rotCamera.y) * m_fSpeed;
		moveDirection.z = 1.0f;
	}
	else if (pKeyboard->GetPress(DIK_S) == true)	// 手前に移動
	{
		m_move.x -= sinf(rotCamera.y) * m_fSpeed;
		m_move.z -= cosf(rotCamera.y) * m_fSpeed;
		moveDirection.z = -1.0f;
	}

	// 移動方向の正規化
	D3DXVec3Normalize(&moveDirection, &moveDirection);

	// 移動状態の管理
	fMoveDirection = sqrtf(moveDirection.x * moveDirection.x + moveDirection.z * moveDirection.z);

	// 移動中の場合
	if (fMoveDirection != 0.0f)
	{ // 移動中
		m_rotDest.y = atan2f(-moveDirection.x, -moveDirection.z) + rotCamera.y;		// 目的地への向き
		m_rotDest.y = AngleNormalize(m_rotDest.y);
		// 目的向きに移動方向を合わせる
		moveDirection.x = sinf(m_rotDest.y) * -fMoveDirection;
		moveDirection.z = cosf(m_rotDest.y) * -fMoveDirection;
	}
}

//=============================
// ジャンプ処理
//=============================
void CPlayer::Jump(void)
{
	if (CManager::GetInputKeyboard()->GetTrigger(DIK_SPACE) == true && m_bJump == false)
	{
		m_move.y = m_fJump;
		m_bJump = true;
	}
}

//=============================
// 位置/角度更新処理
//=============================
void CPlayer::Setter(void)
{
	// 重力
	m_move.y -= GRAVITY;

	// 移動量更新
	m_pos += m_move;

	// 移動量の更新
	m_move.x += (0.0f - m_move.x) * CORRECTION_MOVE;
	m_move.y += (0.0f - m_move.y) * CORRECTION_MOVE;
	m_move.z += (0.0f - m_move.z) * CORRECTION_MOVE;

	// Rplayerの方向を補正
	float fRotDiffKey = AngleNormalize(m_rotDest.y - m_rot.y);	// 差分を計算
	m_rot.y += (fRotDiffKey)*CORRECTION_ROT;
	m_rot.y = AngleNormalize(m_rot.y);
}

//=============================
// ステージとの判定処理
//=============================
void CPlayer::CollisionStage(CStage* pStage)
{
	// 高度判定
	if (pStage->GetFloor() >= m_pos.y)
	{
		m_bJump = false;
		m_move.y = 0.0f;
		m_pos.y = pStage->GetFloor();
	}

	// 移動範囲判定
	float fFieldWidth = pStage->GetWidth();
	float fFieldDepth = pStage->GetDepth();
	if (-fFieldWidth >= m_pos.x)
	{ // 左端
		m_pos.x = -fFieldWidth;
	}
	else if (fFieldWidth <= m_pos.x)
	{ // 右端
		m_pos.x = fFieldWidth;
	}
	if (-fFieldDepth >= m_pos.z)
	{ // 手前
		m_pos.z = -fFieldDepth;
	}
	else if (fFieldDepth <= m_pos.z)
	{ // 奥
		m_pos.z = fFieldDepth;
	}
}

//=============================
// 情報読み取り処理
//=============================
HRESULT CPlayer::LoadScripts(void)
{
	// キャラクター情報読み取り
	if (LoadCharactor() == false)
	{
		return E_FAIL;
	}

	return S_OK;
}

//=============================
// キャラクター情報読み取り処理
//=============================
bool CPlayer::LoadCharactor(void)
{
	// モデル名取得
	if (LoadModelFile() == false) return false;

	// 設定
	if (LoadModelSetting() == false) return false;

	// モーション設定
	if (LoadMotion() == false) return false;
		 
	return true;
}

//=============================
// モデル名取得
//=============================
bool CPlayer::LoadModelFile(void)
{
	FILE* pFile = fopen(SETTING_PATH, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nCntModel = 0;	// モデルの読み込み数を格納

	// モデルファイル名取得
	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], NUM_MODEL) != NULL)
		{ // NUM_MODELを読み込んだ
			m_nNumModel = pLoadScripts->LoadInt(&aStringCopy[0]);
			if (m_nNumModel != MODEL_MAX)
			{
				// ファイルを閉じる
				fclose(pFile);	
				// 外部ファイル読み取り処理を破棄
				SAFE_DELETE_SINGULAR(pLoadScripts); 
				return false;	// スクリプトと内部指定値が異なる場合早期リターン
			}
		}

		if (strstr(&aStringCopy[0], MODEL_FILENAME) != NULL)
		{ // MODEL_FILENAMEを読み込んだ
			pLoadScripts->LoadFileName(&aStringCopy[0]);
			memcpy(&m_aFileName[nCntModel][0], &aStringCopy[0], sizeof aStringCopy);
			nCntModel++;
			if (nCntModel == m_nNumModel)
			{
				bLoadEnd = true;
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	return true;
}

//=============================
// モデル設置情報
//=============================
bool CPlayer::LoadModelSetting(void)
{
	FILE* pFile = fopen(SETTING_PATH, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nIdx = 0;
	int aParentIdx[MODEL_MAX] = {};	// 親を指定

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

		if (strstr(&aStringCopy[0], CHARACTERSET) != NULL)
		{ // CHARACTERSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], NUM_PARTS) != NULL)
				{ // NUM_PARTSを読み込んだ
					int nNumParts = pLoadScripts->LoadInt(&aStringCopy[0]);
					if (m_nNumModel != nNumParts)
					{ // モデル数とパーツ数が合わない場合早期リターン
						// ファイルを閉じる
						fclose(pFile);
						// 外部ファイル読み取り処理を破棄
						SAFE_DELETE_SINGULAR(pLoadScripts);
						return false;
					}
				}

				if (strstr(&aStringCopy[0], PARTSSET) != NULL)
				{ // PARTSSETを読み込んだ
					int nParent = -1;
					D3DXVECTOR3 pos = INIT_D3DVEC3;
					D3DXVECTOR3 rot = INIT_D3DVEC3;
					
					while (1)
					{
						// 文字列のクリア
						memset(&aString[0], 0, sizeof aString);
						memset(&aStringCopy[0], 0, sizeof aStringCopy);

						// 読み込んだ行の文字列判定
						fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
						pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

						if (strstr(&aStringCopy[0], INDEX) != NULL)
						{ // INDEXを読み込んだ
							nIdx = pLoadScripts->LoadInt(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], PARENT) != NULL)
						{ // PARENTを読み込んだ
							aParentIdx[nIdx] = pLoadScripts->LoadInt(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], POS) != NULL)
						{ // POSを読み込んだ
							pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], ROT) != NULL)
						{ // ROTを読み込んだ
							rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
						}
						if (strstr(&aStringCopy[0], END_PARTSSET) != NULL)
						{ // END_PARTSSETを読み込んだ
							m_apModel[nIdx] = CModel::Create(pos, rot, &m_aFileName[nIdx][0]);
							break;
						}
					}
				}
				
				if (strstr(&aStringCopy[0], END_CHARACTERSET) != NULL)
				{ // END_CHARACTERSETを読み込んだ
					bLoadEnd = true;
					break;
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	// モデルの親を指定
	for (int nCntModel = 0; nCntModel < m_nNumModel; nCntModel++)
	{
		if (aParentIdx[nCntModel] == -1)
		{
			m_apModel[nCntModel]->SetParent(NULL);
			continue;
		}
		m_apModel[nCntModel]->SetParent(m_apModel[aParentIdx[nCntModel]]);
	}

	return true;
}

//=============================
// モーション設置情報
//=============================
bool CPlayer::LoadMotion(void)
{
	// 下半身モーションを読み込み
	LoadMotion(NUM_DIVMOTION, MODEL_00LOWERBACK, UNDERMOTION_PATH, DIV_MOTION_UNDER, MOTIONUNDER_MAX);

	// 上半身モーションを読み込み
	LoadMotion(NUM_DIVMOTION, MODEL_07BODY, UPPERMOTION_PATH, DIV_MOTION_UPPER, MOTIONUPPER_MAX);

	return true;
}

//=============================
// モーション設置情報
//=============================
bool CPlayer::LoadMotion(int nNumModel, int nModelStart, const char* pFileName, DIV_MOTION div, int nNumMotion)
{
	FILE* pFile = fopen(pFileName, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}

	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	m_apMotion[div] = new CMotion();

	// モーションにモデルの情報を設定
	m_apMotion[div]->SetModel(&m_apModel[nModelStart], nNumModel);

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	int nIdx = 0;
	int aParentIdx[MODEL_MAX] = {};	// 親を指定
	CMotion::INFO motionInfo[MOTIONTYPE_MAX] = {};	// モーション情報
	int nCntMotion = 0;
	int nCntKey = 0;
	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り
		if (strstr(&aStringCopy[0], MOTIONSET) != NULL)
		{ // MOTIONSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], END_MOTIONSET) != NULL)
				{ // END_MOTIONSETを読み込んだ
					m_apMotion[div]->SetInfo(motionInfo[nCntMotion]);
					nCntMotion++;
					if (nCntMotion == nNumMotion)
					{
						bLoadEnd = true;
					}
					break;
				}
				else if (strstr(&aStringCopy[0], LOOP) != NULL)
				{ // LOOPを読み込んだ
					int nLoop = pLoadScripts->LoadInt(&aStringCopy[0]);
					switch (nLoop)
					{
					case 0:	// ループなし
						motionInfo[nCntMotion].bLoop = false;
						break;

					case 1:	// ループあり
						motionInfo[nCntMotion].bLoop = true;
						break;
					}
				}
				else if (strstr(&aStringCopy[0], NUM_KEY) != NULL)
				{ // NUM_KEYを読み込んだ
					motionInfo[nCntMotion].nNumKey = pLoadScripts->LoadInt(&aStringCopy[0]);
					nCntKey = 0;
				}
				else if (strstr(&aStringCopy[0], KEYSET) != NULL)
				{ // KEYSETを読み込んだ
					int nCntModel = 0;
					while (1)
					{
						// 文字列のクリア
						memset(&aString[0], 0, sizeof aString);
						memset(&aStringCopy[0], 0, sizeof aStringCopy);

						// 読み込んだ行の文字列判定
						fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
						pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

						if (strstr(&aStringCopy[0], END_KEYSET) != NULL)
						{ // END_KEYSETを読み込んだ
							nCntKey++;	// キー数の加算
							break;
						}
						else if (strstr(&aStringCopy[0], FRAME) != NULL)
						{ // FRAMEを読み込んだ
							motionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = pLoadScripts->LoadInt(&aStringCopy[0]);
						}
						else if (strstr(&aStringCopy[0], KEY) != NULL)
						{ // KEYを読み込んだ
							while (1)
							{
								// 文字列のクリア
								memset(&aString[0], 0, sizeof aString);
								memset(&aStringCopy[0], 0, sizeof aStringCopy);

								// 読み込んだ行の文字列判定
								fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
								pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

								if (strstr(&aStringCopy[0], END_KEY) != NULL)
								{ // END_KEYを読み込んだ
									nCntModel++;
									break;
								}
								else if (strstr(&aStringCopy[0], POS) != NULL)
								{ // POSを読み込んだ
									D3DXVECTOR3 pos = pLoadScripts->LoadVector3(&aStringCopy[0]);
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fPosX = pos.x;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fPosY = pos.y;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fPosZ = pos.z;
								}
								else if (strstr(&aStringCopy[0], ROT) != NULL)
								{ // ROTを読み込んだ
									D3DXVECTOR3 rot = pLoadScripts->LoadVector3(&aStringCopy[0]);
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fRotX = rot.x;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fRotY = rot.y;
									motionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntModel].fRotZ = rot.z;
								}
							}

						}
					}
				}

			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	return true;
}
