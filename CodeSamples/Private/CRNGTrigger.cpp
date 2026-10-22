#include "pch.h"
#include "CRNGTrigger.h"
#include "CKeyMgr.h"
#include "CSceneMgr.h"
#include "CScrollMgr.h"
#include "CAbstractFactory.h"
#include "CUIMgr.h"
#include "CEffectMgr.h"

CRNGTrigger::CRNGTrigger() : m_bActive(false), m_bOnCollision(false), m_bEffect(false), m_dwTime(GetTickCount())
{
}

CRNGTrigger::~CRNGTrigger()
{
	Release();
}

void CRNGTrigger::Initialize()
{
	m_tHitBoxInfo = { 1480.f, 500.f, 120.f, 90.f };

	m_eObjType = TRIGGER;
	m_eRender = FRONTGROUND;
}

int CRNGTrigger::Update()
{
	__super::Update_Rect();

	return 0;
}

void CRNGTrigger::Late_Update()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN))
	{
		if (m_bActive == true)
		{
			CEffectMgr::Get_Instance()->Add_Effect({ WINCX * 0.5f, WINCY * 0.5f, 400.f, 300.f }, 9, 0, 50, WINCX, WINCY, L"Closing", true);
			m_dwTime = GetTickCount();
			m_bEffect = true;
			m_bActive = false;
		}
		else
		{
			CKeyMgr::Get_Instance()->Key_Reset();
		}
	}
	if (m_bEffect == true && m_dwTime + (50 * 9) < GetTickCount())
	{
		CSceneMgr::Get_Instance()->Set_SceneChange(CSceneMgr::SC_RNG);
	}
}

void CRNGTrigger::Render(HDC hDC)
{
	int	iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
	int	iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

	//Rectangle(hDC, m_tHitBoxRect.left + iScrollX, m_tHitBoxRect.top + iScrollY, m_tHitBoxRect.right + iScrollX, m_tHitBoxRect.bottom + iScrollY);
}

void CRNGTrigger::Release()
{
}

void CRNGTrigger::OnCollision(OBJ_ID eID)
{
	if (eID == WORLD_PLAYER)
	{
		m_bOnCollision = true;

		if (m_bActive == false)
		{
			CUIMgr::Get_Instance()->Add_UI(STAGE_PANEL, CAbstractFactory<CStagePanel>::Create_UI(2));
			m_bActive = true;
		}
	}
}

void CRNGTrigger::OnCollision_Exit(OBJ_ID eID)
{
	if (eID == WORLD_PLAYER)
	{
		m_bOnCollision = false;

		if (m_bActive == true && m_bOnCollision == false)
		{
			CUIMgr::Get_Instance()->Delete_UI(STAGE_PANEL);
			m_bActive = false;
		}
	}
}
