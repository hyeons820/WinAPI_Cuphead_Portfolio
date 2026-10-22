#include "pch.h"
#include "CEquipItem.h"
#include "CBmpMgr.h"
#include "CDataMgr.h"
#include "CAbstractFactory.h"
#include "CWeaponLogo.h"
#include "CUIMgr.h"
#include "CKeyMgr.h"
#include "CInven.h"

CEquipItem::CEquipItem() : m_pSelect(nullptr), m_iSelect(0), m_dwTime(GetTickCount())
{
	ZeroMemory(&m_tPoint, sizeof(m_tPoint));
}

CEquipItem::~CEquipItem()
{
	Release();
}

void CEquipItem::Initialize()
{
	m_tInfo = { (WINCX * 0.5f), (WINCY * 0.5f), 350.f, 400.f };

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/UI/EquipPanel.bmp", L"EquipPanel");

	m_tPoint[0] = { (long)(WINCX * 0.4f),  (long)(WINCY * 0.68f) };
	m_tPoint[1] = { (long)(WINCX * 0.5f),  (long)(WINCY * 0.68f) };

	__super::Update_Rect();

	Set_CurItem();
}

int CEquipItem::Update()
{
	if (m_pSelect != nullptr)
		m_pSelect->Set_Pos(m_tPoint[m_iSelect].x, m_tPoint[m_iSelect].y);


	return 0;
}

void CEquipItem::Late_Update()
{
	Key_Input();
}

void CEquipItem::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"EquipPanel");

	// 사진,
	GdiTransparentBlt(hDC,
		m_tRect.left,
		m_tRect.top,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		0,
		0,
		588,
		667,
		RGB(3, 3, 3));
}

void CEquipItem::Release()
{
	m_vecWeaponLogo.clear();
	m_vecPlayeritems.clear();
}

void CEquipItem::Key_Input()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RIGHT))
	{
		++m_iSelect;

		if (m_iSelect >= m_vecWeaponLogo.size())
			m_iSelect = m_vecWeaponLogo.size() - 1;
	}
	else
	{
		CKeyMgr::Get_Instance()->Key_Reset();
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_LEFT))
	{
		--m_iSelect;

		if (m_iSelect < 0)
			m_iSelect = 0;
	}
	else
	{
		CKeyMgr::Get_Instance()->Key_Reset();
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN) && m_dwTime + 300 < GetTickCount())
	{ 
		m_dwTime = GetTickCount();

		CUIMgr::Get_Instance()->Delete_UI(WEAPON_LOGO);

		// 인벤 열기, 선택한 아이템 정보 넘겨주기
		m_vecWeaponLogo.clear();

		CObj* pInven = CAbstractFactory<CInven>::Create_Obj();
		CUIMgr::Get_Instance()->Add_UI(INVEN, pInven);

		if (m_iSelect > m_vecPlayeritems.size() - 1)
			static_cast<CInven*>(pInven)->Set_ItemType(NOTHING);
		else
			static_cast<CInven*>(pInven)->Set_ItemType(m_vecPlayeritems[m_iSelect]);

		m_pSelect = nullptr;

		CKeyMgr::Get_Instance()->Key_Reset();

		CUIMgr::Get_Instance()->Delete_UI(EQUIP_ITEM);
	}

}

void CEquipItem::Set_CurItem()
{
	m_vecPlayeritems = CDataMgr::Get_Instance()->Get_CurEquipItem();

	m_vecWeaponLogo.reserve(m_vecPlayeritems.size());

	for (int i = 0; i < m_vecPlayeritems.size(); ++i)
	{
		switch (m_vecPlayeritems[i])
		{
		case BASIC_BULLET:
			m_vecWeaponLogo.push_back(CAbstractFactory<CWeaponLogo>::Create_Obj());
			m_vecWeaponLogo.back()->Set_AnimStart(0);
			break;

		case LOBBER_BULLET:
			m_vecWeaponLogo.push_back(CAbstractFactory<CWeaponLogo>::Create_Obj());
			m_vecWeaponLogo.back()->Set_AnimStart(2);
			break;

		default:
			m_vecWeaponLogo.push_back(CAbstractFactory<CWeaponLogo>::Create_Obj());
			m_vecWeaponLogo.back()->Set_AnimStart(4);
			break;
		}

		m_vecWeaponLogo.back()->Set_Pos(m_tPoint[i].x, m_tPoint[i].y);
		m_vecWeaponLogo.back()->Set_FrameKey(L"WeaponLogo");
		CUIMgr::Get_Instance()->Add_UI(WEAPON_LOGO, m_vecWeaponLogo.back());
	}

	if (m_vecWeaponLogo.size() == 1)
	{
		m_vecWeaponLogo.push_back(CAbstractFactory<CWeaponLogo>::Create_Obj());
		m_vecWeaponLogo.back()->Set_AnimStart(4);
		m_vecWeaponLogo.back()->Set_Pos(m_tPoint[1].x, m_tPoint[1].y);
		m_vecWeaponLogo.back()->Set_FrameKey(L"WeaponLogo");
		CUIMgr::Get_Instance()->Add_UI(WEAPON_LOGO, m_vecWeaponLogo.back());
	}

	m_pSelect = CAbstractFactory<CWeaponLogo>::Create_Obj();
	m_pSelect->Set_FrameKey(L"Select");
	m_pSelect->Set_Pos(m_tPoint[m_iSelect].x, m_tPoint[m_iSelect].y);
	CUIMgr::Get_Instance()->Add_UI(WEAPON_LOGO, m_pSelect);
}
