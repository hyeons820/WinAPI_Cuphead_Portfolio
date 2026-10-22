#pragma once
#include "Define.h"
#include "CItem.h"

typedef struct tagWorldPlayerData
{
	tagWorldPlayerData()
	{
		ZeroMemory(this, sizeof(tagWorldPlayerData));
	}

	tagWorldPlayerData(float _fX, float _fY, float _fScrollX, float _fScrollY) : fX(_fX), fY(_fY), fScrollX(_fScrollX), fScrollY(_fScrollY)
	{
	}

	// 월드맵의 플레이어 좌표
	float fX;
	float fY;
	
	float fScrollX;
	float fScrollY;

}WPDATA;

class CDataMgr
{
private:
	CDataMgr();
	CDataMgr(const CDataMgr& rhs) = delete;
	CDataMgr& operator=(CDataMgr& rObj) = delete;
	~CDataMgr();

public:
	void Set_WorldPlayerData(float _fX, float _fY) { m_tPlayerData.fX = _fX; m_tPlayerData.fY = _fY; }
	void Set_WorldCamera(float _fX, float _fY) { m_tPlayerData.fScrollX = _fX; m_tPlayerData.fScrollY = _fY; }
	void Set_Player();
	void Set_ShopItem(vector<CObj*>& vecItem);
	void Set_PlayerItem(CObj* pItem);
	void Set_CurPlayerItem(ITEM_TYPE eType, ITEM_TYPE eItem);
	void Set_Coin(int iCoin) { m_iCoin += iCoin; }
	void Set_TotalTime(DWORD dwTime) { m_dwTotalTime = dwTime; }
	void Set_Kill() { ++m_iKill; }
	void Set_Parry() { ++m_iParry; }

	const WPDATA* Get_WorldPlayerData() { return &m_tPlayerData; }
	const int	  Get_Coin() { return m_iCoin; }
	vector<CObj*> Get_ShopItem();
	vector<ITEM_TYPE> Get_PlayerItemInven();
	vector<ITEM_TYPE> Get_PlayerItem();
	vector<ITEM_TYPE> Get_CurEquipItem();
	DWORD Get_PlayTime() { return m_dwTotalTime; }
	int Get_Kill() { return m_iKill; }
	int Get_Parry() { return m_iParry; }


public:
	void Load_ShopItems();
	void Load_PlayerItems();

	void Release();

public:
	static CDataMgr* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CDataMgr;
		}
		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CDataMgr* m_pInstance;

	WPDATA m_tPlayerData;

	vector<CObj*> m_vecShopItems;
	vector<CObj*> m_vecPlayerItems;
	vector<ITEM_TYPE> m_vecCurPlayerItems;

	// 플레이어 코인
	int m_iCoin;

	DWORD m_dwTotalTime;
	int m_iKill;
	int m_iParry;
};

