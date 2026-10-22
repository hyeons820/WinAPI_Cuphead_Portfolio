#include "pch.h"
#include "CWorld.h"
#include "CBmpMgr.h"
#include "CObjMgr.h"
#include "CAbstractFactory.h"
#include "CWorldPlayer.h"
#include "CScrollMgr.h"
#include "CWorldTree.h"
#include "CWorldTree2.h"
#include "CCollisionMgr.h"
#include "CTileMgr.h"
#include "CKeyMgr.h"
#include "CWorldRightTree.h"
#include "CWorldRightTree2.h"
#include "CDataMgr.h"
#include "CBossTrigger.h"
#include "CUIMgr.h"
#include "CSoundMgr.h"
#include "CStoreTrigger.h"
#include "CInven.h"
#include "CEquipItem.h"
#include "CRNGTrigger.h"
#include "CEffectMgr.h"
#include "CQuest.h"
#include "CTextBox.h"

CWorld::CWorld() : m_bTileRender(false), m_iScrollX(0.f), m_iScrollY(0.f), m_bEquip(false)
{
	
}

CWorld::~CWorld()
{
	Release();
}

void CWorld::Initialize()
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/World/Background.bmp", L"Background");
	CObjMgr::Get_Instance()->Add_Object(WORLD_PLAYER, CAbstractFactory<CWorldPlayer>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(BLOCK, CAbstractFactory<CWorldTree>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(BLOCK, CAbstractFactory<CWorldTree2>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(BLOCK, CAbstractFactory<CWorldRightTree>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(BLOCK, CAbstractFactory<CWorldRightTree2>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CBossTrigger>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CStoreTrigger>::Create_Obj());
	CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CRNGTrigger>::Create_Obj());

	m_iScrollX = CDataMgr::Get_Instance()->Get_WorldPlayerData()->fScrollX;
	m_iScrollY = CDataMgr::Get_Instance()->Get_WorldPlayerData()->fScrollY;

	CScrollMgr::Get_Instance()->Set_Scroll(m_iScrollX, m_iScrollY);

	CTileMgr::Get_Instance()->Load_Tile(L"../Data/Tile.dat");

	CSoundMgr::Get_Instance()->PlayBGM(L"WorldBgm.mp3", g_fVolume);
}

void CWorld::Update()
{
	CObjMgr::Get_Instance()->Update();
	CTileMgr::Get_Instance()->Update();
	CUIMgr::Get_Instance()->Update();
	CEffectMgr::Get_Instance()->Update();

	Key_Input();
}

void CWorld::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();

	CTileMgr::Get_Instance()->Late_Update();

	CUIMgr::Get_Instance()->Late_Update();

	CEffectMgr::Get_Instance()->Late_Update();


	CCollisionMgr::Collision_Block(CObjMgr::Get_Instance()->Get_List(WORLD_PLAYER), CObjMgr::Get_Instance()->Get_List(BLOCK));
	CCollisionMgr::Collision_Block(CObjMgr::Get_Instance()->Get_List(WORLD_PLAYER), CTileMgr::Get_Instance()->Get_TileList());

	CCollisionMgr::Collision_Circle(CObjMgr::Get_Instance()->Get_List(WORLD_PLAYER), CObjMgr::Get_Instance()->Get_List(TRIGGER));
	CCollisionMgr::Collision_Exit(CObjMgr::Get_Instance()->Get_List(WORLD_PLAYER), CObjMgr::Get_Instance()->Get_List(TRIGGER));
}

void CWorld::Render(HDC hDC)
{
	m_iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	m_iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	HDC hGroundDC = CBmpMgr::Get_Instance()->Find_Image(L"Background");

	BitBlt(hDC, (int)m_iScrollX, (int)m_iScrollY, WORLDCX, WORLDCY, hGroundDC, 0, 0, SRCCOPY);
	
	CObjMgr::Get_Instance()->Render(hDC);

	if (m_bTileRender == true)
	{
		CTileMgr::Get_Instance()->Render(hDC);
	}

	CUIMgr::Get_Instance()->Render(hDC);

	CEffectMgr::Get_Instance()->Render(hDC);

}

void CWorld::Release()
{
	// 삭제하기 전에 저장
	CDataMgr::Get_Instance()->Set_WorldPlayerData(CObjMgr::Get_Instance()->Get_WorldPlayer()->Get_HitBoxInfo()->fX, CObjMgr::Get_Instance()->Get_WorldPlayer()->Get_HitBoxInfo()->fY);
	m_pInven = nullptr;
	CSoundMgr::Get_Instance()->StopAll();
	CDataMgr::Get_Instance()->Set_WorldCamera(m_iScrollX, m_iScrollY);
	CUIMgr::Destroy_Instance();
	CScrollMgr::Destroy_Instance();
	CTileMgr::Destroy_Instance();
	CObjMgr::Destroy_Instance();
}

void CWorld::Key_Input()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
	{
		POINT pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		pt.x -= (int)CScrollMgr::Get_Instance()->Get_ScrollX();
		pt.y -= (int)CScrollMgr::Get_Instance()->Get_ScrollY();

		CTileMgr::Get_Instance()->Picking(pt);
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON))
	{
		CTileMgr::Get_Instance()->Delete_Tile();
	}

	//if (CKeyMgr::Get_Instance()->Key_Down('S'))
	//	CTileMgr::Get_Instance()->Save_Tile();

	//if (CKeyMgr::Get_Instance()->Key_Down('L'))
	//	CTileMgr::Get_Instance()->Load_Tile();

	if (CKeyMgr::Get_Instance()->Key_Down('B'))
		m_bTileRender = !m_bTileRender;

	if (CKeyMgr::Get_Instance()->Key_Down('I') && m_bEquip == false)
	{
		m_pInven = CAbstractFactory<CEquipItem>::Create_Obj();
		CUIMgr::Get_Instance()->Add_UI(EQUIP_ITEM, m_pInven);
		m_bEquip = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE))
	{
		CUIMgr::Get_Instance()->Delete_UI(WEAPON_LOGO);
		CUIMgr::Get_Instance()->Delete_UI(EQUIP_ITEM);
		m_pInven = nullptr;
		m_bEquip = false;
	}
	else
	{
		CKeyMgr::Get_Instance()->Key_Reset();
	}
}