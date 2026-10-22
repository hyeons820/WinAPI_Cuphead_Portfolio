#include "pch.h"
#include "CRunAndGun.h"
#include "CBmpMgr.h"
#include "CLineMgr.h"
#include "CKeyMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CPlayer.h"
#include "CAbstractFactory.h"
#include "CEffectMgr.h"
#include "CUIMgr.h"
#include "CHp.h"
#include "CEXCard.h"
#include "CTileMgr.h"
#include "CCollisionMgr.h"
#include "CBalloon.h"
#include "CMonkey.h"
#include "CTutorialObj.h"
#include "CTrampoline.h"
#include "CMachine.h"
#include "CScaffolding.h"
#include "CCannon.h"
#include "CSceneMgr.h"

CRunAndGun::CRunAndGun() : m_pPlayer(nullptr), m_dwExitTime(GetTickCount()), m_bExit(false), m_dwTime(GetTickCount()), m_bEffect(false), m_dwReadyTime(GetTickCount()), m_bReady(true)
{
}

CRunAndGun::~CRunAndGun()
{
	Release();
}

void CRunAndGun::Initialize()
{
	CLineMgr::Get_Instance()->Load_Line(L"../Data/RNGLine.dat");
	CTileMgr::Get_Instance()->Load_Tile(L"../Data/RNGTile.dat");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/RnG/RnGBG2.bmp", L"RnGBG2");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/RnG/RnGFG.bmp", L"RnGFG");

	// 플레이어 생성
	CObjMgr::Get_Instance()->Add_Object(PLAYER, CAbstractFactory<CPlayer>::Create_Obj());
	static_cast<CPlayer*>(CObjMgr::Get_Instance()->Get_Player())->Set_Offset(true);

	CObjMgr::Get_Instance()->Add_Object(BLOCK, CAbstractFactory<CTutorialObj>::Create_TutorialBlock(5000, 470.f, 150.f, 200.f, 5000, 470.f, 120.f, 180.f, L"RNGObj1", 235.f, 307.f, BLOCK));

	CObjMgr::Get_Instance()->Add_Object(BLOCK, CAbstractFactory<CTutorialObj>::Create_TutorialBlock(5400, 470.f, 150.f, 200.f, 5400, 470.f, 120.f, 180.f, L"RNGObj1", 235.f, 307.f, BLOCK));
	CObjMgr::Get_Instance()->Add_Object(BLOCK, 
		CAbstractFactory<CTutorialObj>::Create_TutorialBlock(5800, 470.f, 150.f, 200.f, 5800, 470.f, 120.f, 180.f, L"RNGObj1", 235.f, 307.f, BLOCK));

	// 몬스터 생성
	for (int i = 0; i < 6; ++i)
	{
		int x = 400 * i;

		CObjMgr::Get_Instance()->Add_Object(MONSTER, CAbstractFactory<CBalloon>::Create_Block(2000 + x, WINCY * 0.15f));
	}

	CObjMgr::Get_Instance()->Add_Object(MONSTER, CAbstractFactory<CMonkey>::Create_Block(5200, 450));
	CObjMgr::Get_Instance()->Add_Object(MONSTER, CAbstractFactory<CMonkey>::Create_Block(5600, 450));

	CObjMgr::Get_Instance()->Add_Object(MONSTER, CAbstractFactory<CMachine>::Create_Block(6350, 330));

	CObjMgr::Get_Instance()->Add_Object(TRAMPOLINE, CAbstractFactory<CTrampoline>::Create_Block(1500, 440));

	CObjMgr::Get_Instance()->Add_Object(MONSTER, CAbstractFactory<CCannon>::Create_Scaffolding(8300, 250));
	CObjMgr::Get_Instance()->Add_Object(MONSTER, CAbstractFactory<CCannon>::Create_Scaffolding(9200, 250));

	int x = 300;
	int y = 70;

	for (int i = 0; i < 4; ++i)
	{
		if (i % 2 != 0)
		{
			CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CScaffolding>::Create_Scaffolding(7200 + i * x, 520));

			if (i == 3)
				continue;
			CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CScaffolding>::Create_Scaffolding(8400 + i * x, 520));
		}
		else
		{
			CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CScaffolding>::Create_Scaffolding(7200 + i * x + 30, 500 - y - 30));
			CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CScaffolding>::Create_Scaffolding(7200 + i * x - 30, 500 + y + 70));

			CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CScaffolding>::Create_Scaffolding(8400 + i * x + 30, 500 - y - 30));
			CObjMgr::Get_Instance()->Add_Object(TRIGGER, CAbstractFactory<CScaffolding>::Create_Scaffolding(8400 + i * x - 30, 500 + y + 70));
		}
	}

	CUIMgr::Get_Instance()->Add_UI(HP, CAbstractFactory<CHp>::Create_Obj());
	CUIMgr::Get_Instance()->Add_UI(EX_CARD, CAbstractFactory<CEXCard>::Create_Obj());

	m_BravoSound = CSoundMgr::Get_Instance()->Get_MapSound(L"RunNGunEnd.mp3");
	m_ReadySound = CSoundMgr::Get_Instance()->Get_MapSound(L"StageStart.mp3");

	CSoundMgr::Get_Instance()->PlayBGM(L"RNGBgm.mp3", g_fVolume);
}

void CRunAndGun::Update()
{
	if (m_bReady == true && m_dwReadyTime + (50 * 10) < GetTickCount())
	{
		CSoundMgr::Get_Instance()->StopSound(SOUND_SCENE);
		FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_ReadySound, nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_SCENE));
		CEffectMgr::Get_Instance()->Add_Effect({ WINCX * 0.5f, WINCY * 0.5f, 512.f, 288.f }, 22, 0, 30, WINCX, WINCY, L"Ready", true);
		m_bReady = false;
	}

	CObjMgr::Get_Instance()->Update();
	CEffectMgr::Get_Instance()->Update();
	CUIMgr::Get_Instance()->Update();
	CTileMgr::Get_Instance()->Update();
	//CLineMgr::Get_Instance()->Update();
	//Key_Input();

	CScrollMgr::Get_Instance()->Scroll_Lock(RNGCX, RNGCY);

	m_pPlayer = CObjMgr::Get_Instance()->Get_Player();

	if (m_pPlayer->Get_Info()->fX >= 10000.f)
	{
		if (m_bExit == false)
		{
			CSoundMgr::Get_Instance()->StopSound(SOUND_SCENE);
			FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_BravoSound, nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_SCENE));

			CEffectMgr::Get_Instance()->Add_Effect({ WINCX * 0.5f, WINCY * 0.5f, 1282.6f, 724.f }, 2, 8, 50, WINCX, WINCY, L"Bravo", true);
			m_dwExitTime = GetTickCount();
			m_bExit = true;
		}
	}
	else if (static_cast<CPlayer*>(m_pPlayer)->Get_CurStateDead())
	{
		if (m_bExit == false)
		{
			m_dwExitTime = GetTickCount();
			m_bExit = true;
		}
	}
	if (m_bExit && m_dwExitTime + 3000 < GetTickCount() && !m_bEffect)
	{
		CEffectMgr::Get_Instance()->Add_Effect({ WINCX * 0.5f, WINCY * 0.5f, 400.f, 300.f }, 9, 0, 50, WINCX, WINCY, L"Closing", true);
		m_dwTime = GetTickCount();
		m_bEffect = true;
	}
	if (m_bEffect == true && m_dwTime + (50 * 9) < GetTickCount())
	{
		CSceneMgr::Get_Instance()->Set_SceneChange(CSceneMgr::SC_WORLD);
	}
}

void CRunAndGun::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
	CEffectMgr::Get_Instance()->Late_Update();
	CUIMgr::Get_Instance()->Late_Update();
	//CLineMgr::Get_Instance()->Late_Update();
	CTileMgr::Get_Instance()->Late_Update();

	CCollisionMgr::Collision_Block(CObjMgr::Get_Instance()->Get_List(PLAYER), CTileMgr::Get_Instance()->Get_TileList());

	CCollisionMgr::Collision_Block(CObjMgr::Get_Instance()->Get_List(PLAYER), CObjMgr::Get_Instance()->Get_List(BLOCK));
	CCollisionMgr::Collision_Rect(CObjMgr::Get_Instance()->Get_List(PLAYER), CObjMgr::Get_Instance()->Get_List(MONSTER));
	CCollisionMgr::Collision_Rect(CObjMgr::Get_Instance()->Get_List(PLAYER), CObjMgr::Get_Instance()->Get_List(MONSTER_BULLET));
	CCollisionMgr::Collision_Circle(CObjMgr::Get_Instance()->Get_List(PLAYER), CObjMgr::Get_Instance()->Get_List(TRAMPOLINE));
	CCollisionMgr::Collision_Rect(CObjMgr::Get_Instance()->Get_List(MONSTER), CObjMgr::Get_Instance()->Get_List(BULLET));
	CCollisionMgr::Collision_Circle(CObjMgr::Get_Instance()->Get_List(MONSTER), CObjMgr::Get_Instance()->Get_List(EX_BULLET));
	CCollisionMgr::Collision_Circle(CObjMgr::Get_Instance()->Get_List(MONSTER), CObjMgr::Get_Instance()->Get_List(BLOCK));
	CCollisionMgr::Collision_Circle(CObjMgr::Get_Instance()->Get_List(MONSTER_BULLET), CObjMgr::Get_Instance()->Get_List(TRIGGER));
}

void CRunAndGun::Render(HDC hDC)
{
	int	iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();

	HDC hBGroundDC = CBmpMgr::Get_Instance()->Find_Image(L"RnGBG2");
	HDC hFGroundDC = CBmpMgr::Get_Instance()->Find_Image(L"RnGFG");

	BitBlt(hDC, iScrollX, 0, RNGCX, RNGCY, hBGroundDC, 0, 700, SRCCOPY);

	CObjMgr::Get_Instance()->Render(hDC);

	//CLineMgr::Get_Instance()->Render(hDC);

	for (int i = 0; i < RNGCX + WINCX; i += WINCX)
	{
		GdiTransparentBlt(hDC, iScrollX + i, 0, WINCX, WINCY, hFGroundDC, i, 700, WINCX, WINCY, RGB(3, 3, 3));
	}
	GdiTransparentBlt(hDC, iScrollX + (RNGCX - WINCX), 0, WINCX, WINCY, hFGroundDC, RNGCX - WINCX, 700, WINCX, WINCY, RGB(3, 3, 3));
	//CTileMgr::Get_Instance()->Render(hDC);
	CEffectMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);
}

void CRunAndGun::Release()
{
	m_pPlayer = nullptr;

	CSoundMgr::Get_Instance()->StopAll();
	CLineMgr::Destroy_Instance();
	CTileMgr::Destroy_Instance();
	CEffectMgr::Destroy_Instance();
	CScrollMgr::Destroy_Instance();
	CUIMgr::Destroy_Instance();
	CObjMgr::Destroy_Instance();
}

void CRunAndGun::Key_Input()
{
	//if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
	//	CScrollMgr::Get_Instance()->Set_ScrollX(15.f);
	//
	//if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	//	CScrollMgr::Get_Instance()->Set_ScrollX(-15.f);

	//if (CKeyMgr::Get_Instance()->Key_Down('L'))
		//CLineMgr::Get_Instance()->Load_Line(L"../Data/RNGLine.dat");

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

	if (CKeyMgr::Get_Instance()->Key_Down('S'))
		CTileMgr::Get_Instance()->Save_Tile();

	if (CKeyMgr::Get_Instance()->Key_Down('L'))
		CTileMgr::Get_Instance()->Load_Tile(L"../Data/RNGLine.dat");
}