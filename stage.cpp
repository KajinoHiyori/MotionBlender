//========================================================
// 
// Stage処理[stage.cpp]
// Author : Kajino Hiyori _Create2026/11/12
//
//========================================================
//=============================
// インクルードファイル
//=============================
#include "main.h"
#include "stage.h"
#include "input.h"
#include "color.h"
#include "loadscripts.h"
#include "debugproc.h"
#include "renderer.h"
#include "manager.h"
#include "texture.h"
#include "objectWall.h"

//=============================
// マクロ定義
//=============================

//=============================
// コンストラクタ
//=============================
CStage::CStage(CObject::PRIORITY priority) : CObject(priority)
{
	// 値のクリア
	m_pField = NULL;
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		m_apWall[nCntWall] = NULL;
	}
}

//=============================
// デストラクタ
//=============================
CStage::~CStage()
{

}

//=============================
// Stage生成処理
//=============================
CStage* CStage::Create(const char* pFilePath)
{
	CStage* pStage = NULL;	// オブジェクトへのポインタを用意
	if (pStage == NULL)
	{ // NULLが設定できている場合
		pStage = new CStage;
		if (pStage != NULL)
		{ // インスタンス生成に成功した場合
			// 初期化処理
			pStage->Init(pFilePath);

			// 種類設定
			pStage->SetType(TYPE_STAGE);

			return pStage;
		}
	}
	// インスタンス生成に失敗
	return NULL;
}

//=============================
// 初期化処理
//=============================
HRESULT CStage::Init(const char* pFilePath)
{
	// モデル読み込み
	LoadScripts(pFilePath);

	return S_OK;
}

//=============================
// 終了処理
//=============================
void CStage::Uninit(void)
{
	// 値のクリア
	m_pField = NULL;
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		m_apWall[nCntWall] = NULL;
	}
}

//=============================
// 更新処理
//=============================
void CStage::Update(void)
{
}

//=============================
// 描画処理
//=============================
void CStage::Draw(void)
{
	
}

#define FIELDSET		("FIELDSET")		// FIELDSET
#define END_FIELDSET	("END_FIELDSET")	// END_FIELDSET
#define WIDTH			("WIDTH")			// WIDTH
#define DEPTH			("DEPTH")			// DEPTH
#define TEXTURE			("TEXTURE")			// TEXTURE
#define RANGE_W			("RANGE_W")			// RANGE_W
#define RANGE_D			("RANGE_D")			// RANGE_D

//=============================
// 情報読み取り処理
//=============================
HRESULT CStage::LoadScripts(const char* pFilePath)
{
	// フィールドを設定
	if (LoadField(pFilePath) == false) return E_FAIL;

	// 壁を設定
	if (LoadWall(pFilePath) == false) return E_FAIL;

	return S_OK;
}

//=============================
// フィールドを設定
//=============================
bool CStage::LoadField(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}
	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	float fWidth = 0.0f;			// 幅
	float fDepth = 0.0f;			// 奥行き
	char aTextureName[MAX_STRING] = {};		// テクスチャ名
	float fTextureRangeW = 0.0f;		// テクスチャ読み込み幅
	float fTextureRangeD = 0.0f;		// テクスチャ読み込み幅

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り
		if (strstr(&aStringCopy[0], FIELDSET) != NULL)
		{ // FIELDSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], END_FIELDSET) != NULL)
				{ // END_FIELDSETを読み込んだ
					bLoadEnd = true;
					break;
				}
				else if (strstr(&aStringCopy[0], WIDTH) != NULL)
				{ // WIDTHを読み込んだ
					fWidth = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				else if (strstr(&aStringCopy[0], DEPTH) != NULL)
				{ // DEPTHを読み込んだ
					fDepth = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				else if (strstr(&aStringCopy[0], TEXTURE) != NULL)
				{ // TEXTUREを読み込んだ
					pLoadScripts->LoadFileName(&aStringCopy[0]);
					memcpy(&aTextureName[0], &aStringCopy[0], sizeof aStringCopy);
				}
				else if (strstr(&aStringCopy[0], RANGE_W) != NULL)
				{ // RANGE_Wを読み込んだ
					fTextureRangeW = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				else if (strstr(&aStringCopy[0], RANGE_D) != NULL)
				{ // RANGE_Dを読み込んだ
					fTextureRangeD = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	// フィールド生成
	m_pField = CObject3D::Create(INIT_D3DVEC3, INIT_D3DVEC3, COLOR_WHITE, fWidth, fDepth, fTextureRangeW, fTextureRangeD);
	m_pField->SetTextureIdx(CManager::GetTexture()->Register(&aTextureName[0]));

	return true;
}

#define WALLSET		("WALLSET")		// WALLSET
#define END_WALLSET	("END_WALLSET")	// END_WALLSET
#define HEIGHT		("HEIGHT")		// HEIGHT
//=============================
// 壁を設定
//=============================
bool CStage::LoadWall(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");

	if (pFile == NULL)
	{ // 読み込みに失敗
		return false;
	}
	// 外部ファイル読み取り処理を生成
	CLoadScripts* pLoadScripts = new CLoadScripts();

	bool bLoadEnd = false;		// 読み込み終了フラグ
	char aString[MAX_STRING];		// ローカルで文字列を格納
	char aStringCopy[MAX_STRING];	// ローカルで文字列を格納[コピー]
	float fHeight = 0.0f;			// 高さ
	char aTextureName[MAX_STRING] = {};		// テクスチャ名
	float fTextureRangeW = 0.0f;		// テクスチャ読み込み幅
	float fTextureRangeD = 0.0f;		// テクスチャ読み込み幅

	do
	{ // 文字列のクリア
		memset(&aString[0], 0, sizeof aString);
		memset(&aStringCopy[0], 0, sizeof aStringCopy);

		// 読み込んだ行の文字列判定
		fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
		pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り
		if (strstr(&aStringCopy[0], WALLSET) != NULL)
		{ // WALLSETを読み込んだ
			while (1)
			{
				// 文字列のクリア
				memset(&aString[0], 0, sizeof aString);
				memset(&aStringCopy[0], 0, sizeof aStringCopy);

				// 読み込んだ行の文字列判定
				fgets(&aString[0], MAX_STRING, pFile);	// 1行読み込む
				pLoadScripts->Judgment(&aString[0], &aStringCopy[0]);	// 有効文字抜き取り

				if (strstr(&aStringCopy[0], END_WALLSET) != NULL)
				{ // END_WALLSETを読み込んだ
					bLoadEnd = true;
					break;
				}
				else if (strstr(&aStringCopy[0], HEIGHT) != NULL)
				{ // HEIGHTを読み込んだ
					fHeight = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				else if (strstr(&aStringCopy[0], TEXTURE) != NULL)
				{ // TEXTUREを読み込んだ
					pLoadScripts->LoadFileName(&aStringCopy[0]);
					memcpy(&aTextureName[0], &aStringCopy[0], sizeof aStringCopy);
				}
				else if (strstr(&aStringCopy[0], RANGE_W) != NULL)
				{ // RANGE_Wを読み込んだ
					fTextureRangeW = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
				else if (strstr(&aStringCopy[0], RANGE_D) != NULL)
				{ // RANGE_Dを読み込んだ
					fTextureRangeD = pLoadScripts->LoadFloat(&aStringCopy[0]);
				}
			}
		}
	} while (bLoadEnd == false);

	// ファイルを閉じる
	fclose(pFile);

	// 外部ファイル読み取り処理を破棄
	SAFE_DELETE_SINGULAR(pLoadScripts);

	// 壁を生成
	m_apWall[0] = CObjectWall::Create(D3DXVECTOR3(0.0f, 0.0f, m_pField->GetDepth()), INIT_D3DVEC3, COLOR_WHITE, m_pField->GetWidth(), fHeight, CObjectWall::CENTER_BUTTON);
	m_apWall[1] = CObjectWall::Create(D3DXVECTOR3(m_pField->GetWidth(), 0.0f, 0.0f), D3DXVECTOR3(0.0f, HALF_D3DXPI, 0.0f), COLOR_WHITE, m_pField->GetWidth(), fHeight, CObjectWall::CENTER_BUTTON);
	m_apWall[2] = CObjectWall::Create(D3DXVECTOR3(0.0f, 0.0f, -m_pField->GetDepth()), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), COLOR_WHITE, m_pField->GetWidth(), fHeight, CObjectWall::CENTER_BUTTON);
	m_apWall[3] = CObjectWall::Create(D3DXVECTOR3(-m_pField->GetWidth(), 0.0f, 0.0f), D3DXVECTOR3(0.0f, -HALF_D3DXPI, 0.0f), COLOR_WHITE, m_pField->GetWidth(), fHeight, CObjectWall::CENTER_BUTTON);
	
	// テクスチャインデックスを指定
	int nTextureIdx = CManager::GetTexture()->Register(&aTextureName[0]);

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		if (m_apWall[nCntWall] == NULL)continue;
		// テクスチャ読み込み位置を設定
		m_apWall[nCntWall]->SetTecU(0, fTextureRangeW, fTextureRangeD);
		m_apWall[nCntWall]->SetTextureIdx(nTextureIdx);
	}

	return true;
}