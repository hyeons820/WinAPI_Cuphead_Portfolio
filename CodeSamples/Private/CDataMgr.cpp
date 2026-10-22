#include "pch.h"
#include "CDataMgr.h"
#include "CAbstractFactory.h"
#include "CLobberBullet.h"
#include "CIncredHp.h"

CDataMgr* CDataMgr::m_pInstance = nullptr;

CDataMgr::CDataMgr() : m_iCoin(0), m_dwTotalTime(GetTickCount()), m_iKill(0), m_iParry(0)
{
	m_tPlayerData = { (float)WORLD_PLAYER_SPAWNX, (float)WORLD_PLAYER_SPAWNY, 0.f, 0.f };
}

CDataMgr::~CDataMgr()
{
	Release();
}

void CDataMgr::Set_Player()
{

}

// 상점이 가지고 있는 아이템 조정
void CDataMgr::Set_ShopItem(vector<CObj*>& vecItem)
{
	for (auto& vec : m_vecShopItems)
	{
		Safe_Delete<CObj*>(vec);
	}
	m_vecShopItems.clear();

	for (auto& vec : vecItem)
	{
		if (vec)
			m_vecShopItems.push_back(vec->Clone());
		else
			m_vecShopItems.push_back(nullptr);
	}
}

// 플레이어가 가지고 있는 아이템 조정
void CDataMgr::Set_PlayerItem(CObj* pItem)
{
	CObj* pNewItem = pItem->Clone();
	m_vecPlayerItems.push_back(pNewItem);
}

// 플레이어가 현재 들고 있는 아이템 조정
void CDataMgr::Set_CurPlayerItem(ITEM_TYPE eType, ITEM_TYPE eItem)
{
	if (eType == eItem)
		return;

	auto iter1 = find_if(m_vecCurPlayerItems.begin(), m_vecCurPlayerItems.end(), [&](auto& iter)->bool
		{
			if (iter == eItem)
				return true;

			return false;
		});

	if (iter1 != m_vecCurPlayerItems.end())
	{
		*iter1 = ITEM_END;
	}

	// 교체
	auto iter = find_if(m_vecCurPlayerItems.begin(), m_vecCurPlayerItems.end(), [&](auto& iter)->bool
		{
			if (iter == eType)
				return true;

			return false;
		});

	if (iter == m_vecCurPlayerItems.end())
		return;

	if (iter1 != m_vecCurPlayerItems.end())
	{
		*iter1 = NOTHING;
	}
	
	ITEM_TYPE PreType = *iter;

	iter = m_vecCurPlayerItems.erase(iter);


	switch (eItem)
	{
	case BASIC_BULLET:
		iter = m_vecCurPlayerItems.insert(iter, BASIC_BULLET);
		break;

	case LOBBER_BULLET:
		iter = m_vecCurPlayerItems.insert(iter, LOBBER_BULLET);
		break;

	case NOTHING:
		iter = m_vecCurPlayerItems.insert(iter, NOTHING);
		break;

	default:
		return;
	}

	auto iter2 = find_if(m_vecCurPlayerItems.begin(), m_vecCurPlayerItems.end(), [&](auto& iter)->bool
		{
			if (iter != NOTHING)
				return true;

			return false;
		});

	if (iter2 == m_vecCurPlayerItems.end())
	{
		*m_vecCurPlayerItems.begin() = PreType;
	}
}

// 플레이어가 현재 들고 있는 아이템 타입 반환
vector<ITEM_TYPE> CDataMgr::Get_CurEquipItem()
{
	vector<ITEM_TYPE> vecCurEquip;

	for (int i = 0; i < m_vecCurPlayerItems.size(); ++i)
	{
		vecCurEquip.push_back(m_vecCurPlayerItems[i]);
	}

	return vecCurEquip;
}

// 업데이트 된 상점 아이템 반환
vector<CObj*> CDataMgr::Get_ShopItem()
{
	vector<CObj*> copyvec;

	for (auto& vec : m_vecShopItems)
	{
		if (vec)
			copyvec.push_back(vec->Clone());
		else
			copyvec.push_back(nullptr);
	}

	return copyvec;
}

// 플레이어가 가지고 있는 아이템 타입 반환
vector<ITEM_TYPE> CDataMgr::Get_PlayerItemInven()
{
	vector<ITEM_TYPE> vecPlayer;

	vecPlayer.push_back(BASIC_BULLET);
	
	for (auto& vec : m_vecPlayerItems)
	{
		if (vec == nullptr)
			continue;

		vecPlayer.push_back(static_cast<CItem*>(vec)->Get_ItemType());
	}

	vecPlayer.push_back(NOTHING);
	
	return vecPlayer;
}

vector<ITEM_TYPE> CDataMgr::Get_PlayerItem()
{
	vector<ITEM_TYPE> vecPlayer;

	vecPlayer.push_back(BASIC_BULLET);

	for (auto& vec : m_vecPlayerItems)
	{
		if (vec == nullptr)
			continue;

		if (static_cast<CItem*>(vec)->Get_ItemType() == INCRED_HP)
			continue;

		vecPlayer.push_back(static_cast<CItem*>(vec)->Get_ItemType());
	}

	vecPlayer.push_back(NOTHING);

	return vecPlayer;
}

// 제일 처음 상점 업데이트
void CDataMgr::Load_ShopItems()
{
	m_vecShopItems.push_back(nullptr);
	m_vecShopItems.push_back(CAbstractFactory<CLobberBullet>::Create_Item(100.f, 310.f, 160.f, 200.f, L"Item1"));

	m_vecShopItems.push_back(CAbstractFactory<CIncredHp>::Create_Item(220.f, 310.f, 132.5f, 200.f, L"Item2"));
}

// 제일 처음 플레이어 아이템 업데이트
void CDataMgr::Load_PlayerItems()
{
	m_vecPlayerItems.push_back(nullptr);
	m_vecCurPlayerItems.push_back(BASIC_BULLET);
	m_vecCurPlayerItems.push_back(NOTHING);
}

void CDataMgr::Release()
{
	for_each(m_vecShopItems.begin(), m_vecShopItems.end(), Safe_Delete<CObj*>);
	m_vecShopItems.clear();

	for_each(m_vecPlayerItems.begin(), m_vecPlayerItems.end(), Safe_Delete<CObj*>);
	m_vecPlayerItems.clear();
	
}
