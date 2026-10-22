#include "pch.h"
#include "CCoin.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CDataMgr.h"
#include "CEffectMgr.h"

CCoin::CCoin()
{
}

CCoin::~CCoin()
{
	Release();
}

void CCoin::Initialize()
{
	m_tHitBoxInfo.fCX = 100.f;
	m_tHitBoxInfo.fCY = 150.f;

	m_eObjType = COIN;
	m_eRender = RENDER_COIN;

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/CoinCopy.bmp", L"Coin");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/CoinDead.bmp", L"CoinDead");


	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 15;
	m_tFrame.dwSpeed = 80;
	m_tFrame.dwTime = GetTickCount();

	m_CoinSound = m_pSoundMgr->Get_MapSound(L"Coin.mp3");
}

int CCoin::Update()
{
	if (m_bDead == true)
	{
		m_pSoundMgr->StopSound(SOUND_EFFECT);
		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_CoinSound, nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_EFFECT));

		CEffectMgr::Get_Instance()->Add_Effect({ m_tHitBoxInfo.fX, m_tHitBoxInfo.fY, 150.f, 150.f }, 16, 0, 30, 150, 150, L"CoinDead");

		return OBJ_DEAD;
	}

	return OBJ_NOEVENT;
}

void CCoin::Late_Update()
{
	__super::Update_Rect();

	Move_Frame();
}

void CCoin::Render(HDC hDC)
{
	int	iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();

	HDC hCoinDC = CBmpMgr::Get_Instance()->Find_Image(L"Coin");

	GdiTransparentBlt(hDC,
		m_tHitBoxRect.left + iScrollX,
		m_tHitBoxRect.top,
		(int)m_tHitBoxInfo.fCX,
		(int)m_tHitBoxInfo.fCY,
		hCoinDC,
		(int)m_tHitBoxInfo.fCX * m_tFrame.iStart,
		0,
		(int)m_tHitBoxInfo.fCX,
		(int)m_tHitBoxInfo.fCY,
		RGB(3, 3, 3));
}

void CCoin::Release()
{
}

void CCoin::OnCollision(OBJ_ID eID)
{
	if (eID == PLAYER)
	{
		// TODO : ÄÚÀÎ ¸Ô´Â ÀÌÆåÆ®
		CDataMgr::Get_Instance()->Set_Coin(1);
		Set_Dead();
	}
}
