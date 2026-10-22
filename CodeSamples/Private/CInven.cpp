#include "pch.h"
#include "CInven.h"
#include "CBmpMgr.h"
#include "CDataMgr.h"
#include "CWeaponLogo.h"
#include "CAbstractFactory.h"
#include "CUIMgr.h"
#include "CKeyMgr.h"
#include "CEquipItem.h"

CInven::CInven() : m_eItemType(ITEM_END), m_iSelect(0), m_dwTime(GetTickCount())
{
}

CInven::~CInven()
{
	Release();
}

void CInven::Initialize()
{
	m_tInfo = { (WINCX * 0.5f), (WINCY * 0.5f), 350.f, 400.f };

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/UI/InvenPanel.bmp", L"InvenPanel");

	Set_Item();
	
	__super::Update_Rect();
}

int CInven::Update()
{
	if (m_pSelect != nullptr)
		m_pSelect->Set_Pos(m_vecPoint[m_iSelect].x, m_vecPoint[m_iSelect].y);

	return 0;
}

void CInven::Late_Update()
{
	Key_Input();
}

void CInven::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"InvenPanel");

	// 사진,
	GdiTransparentBlt(hDC,
		m_tRect.left,
		m_tRect.top,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		0,
		0,
		592,
		666,
		RGB(3, 3, 3));
}

void CInven::Release()
{
	m_pSelect = nullptr;
}

void CInven::Set_Item()
{
	m_vecPlayeritems = CDataMgr::Get_Instance()->Get_PlayerItem();

	m_vecPoint.resize(m_vecPlayeritems.size());

	for (int i = 0; i < m_vecPoint.size(); ++i)
	{
		m_vecPoint[i].x = (m_tInfo.fX - m_tInfo.fCX * 0.5f) + (70.f * (i + 1));
		m_vecPoint[i].y = (m_tInfo.fY - m_tInfo.fCX * 0.5f) + (70.f);
	}

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
			continue;
		}

		m_vecWeaponLogo.back()->Set_Pos(m_vecPoint[i].x, m_vecPoint[i].y);
		m_vecWeaponLogo.back()->Set_FrameKey(L"WeaponLogo");
		CUIMgr::Get_Instance()->Add_UI(WEAPON_LOGO, m_vecWeaponLogo.back());
	}

	// 장착 해제 로고
	m_vecWeaponLogo.push_back(CAbstractFactory<CWeaponLogo>::Create_Obj());
	m_vecWeaponLogo.back()->Set_AnimStart(4);
	m_vecWeaponLogo.back()->Set_Pos(m_vecPoint[m_vecPoint.size() - 1].x, m_vecPoint[m_vecPoint.size() - 1].y);
	m_vecWeaponLogo.back()->Set_FrameKey(L"WeaponLogo");
	CUIMgr::Get_Instance()->Add_UI(WEAPON_LOGO, m_vecWeaponLogo.back());

	// 선택 로고
	m_pSelect = CAbstractFactory<CWeaponLogo>::Create_Obj();
	m_pSelect->Set_FrameKey(L"Select");
	m_pSelect->Set_Pos(m_vecPoint[m_iSelect].x, m_vecPoint[m_iSelect].y);
	CUIMgr::Get_Instance()->Add_UI(WEAPON_LOGO, m_pSelect);
}

void CInven::Key_Input()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RIGHT) && m_dwTime + 300 < GetTickCount())
	{
		m_dwTime = GetTickCount();

		++m_iSelect;

		if (m_iSelect >= m_vecWeaponLogo.size())
			m_iSelect = m_vecWeaponLogo.size() - 1;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_LEFT) && m_dwTime + 300 < GetTickCount())
	{
		m_dwTime = GetTickCount();

		--m_iSelect;

		if (m_iSelect < 0)
			m_iSelect = 0;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN) && m_dwTime + 300 < GetTickCount())
	{

		m_dwTime = GetTickCount();

		CDataMgr::Get_Instance()->Set_CurPlayerItem(m_eItemType, m_vecPlayeritems[m_iSelect]);
		
		CKeyMgr::Get_Instance()->Key_Reset();
		
		CUIMgr::Get_Instance()->Delete_UI(WEAPON_LOGO);
		CUIMgr::Get_Instance()->Delete_UI(INVEN);
		
		CUIMgr::Get_Instance()->Add_UI(EQUIP_ITEM, CAbstractFactory<CEquipItem>::Create_Obj());
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE) && m_dwTime + 300 < GetTickCount())
	{
		CKeyMgr::Get_Instance()->Key_Reset();

		CUIMgr::Get_Instance()->Delete_UI(WEAPON_LOGO);
		CUIMgr::Get_Instance()->Delete_UI(INVEN);

		CUIMgr::Get_Instance()->Add_UI(EQUIP_ITEM, CAbstractFactory<CEquipItem>::Create_Obj());
	}
}
