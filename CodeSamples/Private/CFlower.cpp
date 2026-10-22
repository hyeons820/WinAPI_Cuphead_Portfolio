#include "pch.h"
#include "CFlower.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include <ctime>
#include "CObjMgr.h"
#include "CBoomerang.h"
#include "CAbstractFactory.h"
#include "CRain.h"
#include "CVine.h"
#include "CFlatformVine.h"
#include "CPetal.h"
#include "CEffectMgr.h"
#include "CDataMgr.h"

CFlower::CFlower() : m_eCurState(STATE_END), m_ePreState(STATE_END), m_tPatternCycle(GetTickCount()), m_tPattern1Time(GetTickCount()), m_bStopFrame(false), m_iBulletCount(0),
m_iPattern2Count(0), m_iBulletCount2(0), m_tPattern2Cycle(GetTickCount()), m_iPhase2(false), m_tPattern3Cycle(GetTickCount()), m_iPattern3Count(0), m_iPattern4Count(0), m_dwEffectTime(GetTickCount())
{
	ZeroMemory(m_FlowerSound, sizeof(m_FlowerSound));
}

CFlower::~CFlower()
{
}

void CFlower::Initialize()
{
	srand(unsigned int(GetTickCount()));

	m_eObjType = MONSTER;
	m_eRender = RENDER_MONSTER;

	m_tInfo.fCX = 300.f;
	m_tInfo.fCY = 350.f;

	m_tInfo.fX = 650;
	m_tInfo.fY = 350;


	m_tHitBoxInfo.fCX = m_tInfo.fCX * 0.5f;
	m_tHitBoxInfo.fCY = m_tInfo.fCY * 0.7f;

	m_tHitBoxInfo.fX = m_tInfo.fX + m_tHitBoxInfo.fCX * 0.2f;
	m_tHitBoxInfo.fY = m_tInfo.fY;

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/Stage2/BossIntro.bmp", L"BossIntro");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/Stage2/Boss.bmp", L"Boss");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/Stage2/Pattern1End.bmp", L"Pattern1End");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/Stage2/FiringPollen.bmp", L"FiringPollen");

	m_tFrame.iStart = 0.f;
	m_tFrame.iEnd = 20.f;
	m_tFrame.iMotion = 0.f;

	m_eCurState = INTRO;
	m_iHp = 120;
	
	m_FlowerSound[SOUND_INTRO] = m_pSoundMgr->Get_MapSound(L"flower_intro_yell.wav");
	m_FlowerSound[SOUND_PHASE2] = m_pSoundMgr->Get_MapSound(L"flower_man_phaseOneTwo_transition.wav");
	m_FlowerSound[RAINSTART] = m_pSoundMgr->Get_MapSound(L"RainStart.wav");
	m_FlowerSound[RAINLOOP] = m_pSoundMgr->Get_MapSound(L"RainLoop.wav");
	m_FlowerSound[VINE] = m_pSoundMgr->Get_MapSound(L"Vine.wav");
	m_FlowerSound[SOUND_DEAD] = m_pSoundMgr->Get_MapSound(L"Dead.wav");	

	m_WinSound = CSoundMgr::Get_Instance()->Get_MapSound(L"StageWin.mp3");
	m_ReadySound = CSoundMgr::Get_Instance()->Get_MapSound(L"StageStart.mp3");

	FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_FlowerSound[SOUND_INTRO], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_BOSS));
}

int CFlower::Update()
{
	CObj::Update_Rect();

	if (m_iPhase2 == true)
		return OBJ_NOEVENT;

	if (m_iHp <= 50)
	{
		m_iPhase2 = true;
	}

	if (m_iPhase2 == true)
	{
		m_bStopFrame = false;
		m_bEndMotion = false;
		m_eCurState = PHASE2_START;
	}

	return OBJ_NOEVENT;
}

void CFlower::Late_Update()
{
	Switch_State(m_eCurState);
	Switch_Motion();
    Move_Frame();
}

void CFlower::Render(HDC hDC)
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

	//Rectangle(hDC, m_tHitBoxRect.left + iScrollX, m_tHitBoxRect.top, m_tHitBoxRect.right + iScrollX, m_tHitBoxRect.bottom);

	if (m_eCurState == INTRO)
	{
		GdiTransparentBlt(hDC,
			m_tRect.left + iScrollX,
			m_tRect.top,
			(int)m_tInfo.fCX,
			(int)m_tInfo.fCY,
			hMemDC,
			825 * m_tFrame.iStart,
			715 * m_tFrame.iMotion,
			825,
			715,
			RGB(3, 3, 3));
	}
	else if (m_eCurState == PATTERN1_END || m_eCurState == PHASE2_PATTERN2)
	{
		GdiTransparentBlt(hDC,
			m_tRect.left + iScrollX,
			m_tRect.top,
			(int)m_tInfo.fCX,
			(int)m_tInfo.fCY,
			hMemDC,
			600 * m_tFrame.iStart,
			700 * m_tFrame.iMotion,
			600,
			700,
			RGB(3, 3, 3));
	}
	else
	{
		GdiTransparentBlt(hDC,
			m_tRect.left + iScrollX - (m_tInfo.fCX * 1.05f - m_tInfo.fCX),
			m_tRect.top - (m_tInfo.fCX * 1.3f - m_tInfo.fCX),
			(int)m_tInfo.fCX * 1.3f,
			(int)m_tInfo.fCY * 1.3f,
			hMemDC,
			600 * m_tFrame.iStart,
			700 * m_tFrame.iMotion,
			600,
			700,
			RGB(3, 3, 3));
	}
}

void CFlower::Release()
{
}

void CFlower::OnCollision(OBJ_ID eID)
{
	if (m_eCurState == STATE_END)
		return;

	if (eID == BULLET)
	{
		if (m_iHp <= 1)
		{
			m_eCurState = DEAD;
			CDataMgr::Get_Instance()->Set_Kill();
			return;
		}
		--m_iHp;
	}
	else if (eID == EX_BULLET)
	{
		if (m_iHp <= 5)
		{
			m_eCurState = DEAD;
			CDataMgr::Get_Instance()->Set_Kill();
			return;
		}
		m_iHp -= 5;
	}
}

void CFlower::Switch_State(FLOWER_STATE eState)
{
	switch (m_eCurState)
	{
	case INTRO:
		m_pFrameKey = L"BossIntro";

		if (m_bEndMotion == true)
		{
			m_tPattern1Time = GetTickCount();

			CSoundMgr::Get_Instance()->StopSound(SOUND_SCENE);
			FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_ReadySound, nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_SCENE));
			CEffectMgr::Get_Instance()->Add_Effect({ WINCX * 0.5f, WINCY * 0.5f, 512.f, 288.f }, 22, 0, 30, WINCX, WINCY, L"Ready", true);

			m_eCurState = IDLE;
		}
		break;

	case IDLE:
		m_pSoundMgr->StopSound(SOUND_BOSS);

		if (m_tPatternCycle + 2500 < GetTickCount())
		{
			int iPattern = rand() % 2;;

			iPattern++;

			switch (iPattern)
			{
			case 1:
				m_eCurState = PATTERN1_START;
				m_tPattern1Time = GetTickCount();
				break;
			
			case 2:
				m_eCurState = PATTERN2_START;
				break;
			
			default:
				m_eCurState = IDLE;
				m_tPatternCycle = GetTickCount();
				break;
			}
		}
		break;

	case PATTERN1_START:
		if (m_bEndMotion == true)
		{
			Create_Bullet();
			m_bStopFrame = true;
		}
		
		if (m_tPattern1Time + 1500 < GetTickCount() && m_bEndMotion == true)
		{
			m_eCurState = PATTERN1_END;
			CObj::Update_Rect();
			
			m_bStopFrame = false;
		}
		break;

	case PATTERN1_END:

		if (m_bEndMotion == true)
		{
			m_iBulletCount = 0;
			m_tPatternCycle = GetTickCount();
			m_eCurState = IDLE;
		}
		break;

	case PATTERN2_START:
		if (m_bEndMotion == true)
		{
			m_eCurState = PATTERN2;

			m_pSoundMgr->StopSound(SOUND_BOSS);
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_FlowerSound[RAINSTART], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_BOSS));
		}
		break;

	case PATTERN2:
		if (m_bEndMotion == true)
		{
			Create_Rain();
			m_bEndMotion = false;

			m_pSoundMgr->StopSound(SOUND_BOSS);
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_FlowerSound[RAINLOOP], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_BOSS));
		}

		if (m_iBulletCount2 >= 5)
		{
			m_eCurState = PATTERN2_END;
			m_iBulletCount2 = 0;
		}

		break;

	case PATTERN2_END:
		if (m_bEndMotion == true)
		{
			m_tPatternCycle = GetTickCount();
			m_eCurState = IDLE;
		}
		break;

	case PHASE2_START:
		if (m_bEndMotion == true)
		{
			m_pSoundMgr->StopSound(SOUND_BOSS);
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_FlowerSound[VINE], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_BOSS));

			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_FlowerSound[SOUND_PHASE2], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_BOSS));

			CObjMgr::Get_Instance()->Add_Object(MONSTER_BULLET, CAbstractFactory<CVine>::Create_Obj());
			m_tPatternCycle = GetTickCount();
			m_eCurState = PHASE2_IDLE;
		}
		break;

	case PHASE2_IDLE:
		if (m_tPatternCycle + 2500 < GetTickCount())
		{
			m_pSoundMgr->StopSound(SOUND_BOSS);

			int iPattern = rand() % 2;

			iPattern++;

			switch (iPattern)
			{
			case 1:
				m_eCurState = PHASE2_PATTERN1;
				break;

			case 2:
				m_eCurState = PHASE2_PATTERN2;
				break;

			default:
				m_eCurState = PHASE2_IDLE;
				m_tPatternCycle = GetTickCount();
				break;
			}
		}
		break;

	case PHASE2_PATTERN1:
		Create_PlatformVine();

		if (m_iPattern3Count >= 3)
		{
			m_eCurState = PHASE2_IDLE;
			m_iPattern3Count = 0;
			m_tPatternCycle = GetTickCount();
		}

		break;

	case PHASE2_PATTERN2:
		Create_Petal();
		if (m_bEndMotion == true)
		{
			m_eCurState = PHASE2_IDLE;
			m_iPattern4Count = 0;
			m_tPatternCycle = GetTickCount();
		}
		break;

	case DEAD:
		m_pSoundMgr->StopSound(SOUND_BOSS);
		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_FlowerSound[SOUND_DEAD], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_BOSS));
		
		CSoundMgr::Get_Instance()->StopSound(SOUND_SCENE);
		FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_WinSound, nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_SCENE));
		
		CEffectMgr::Get_Instance()->Add_Effect({ WINCX * 0.5f, WINCY * 0.5f, 512.f, 288.f }, 26, 0, 50, WINCX, WINCY, L"KnockOut", true);

		m_eCurState = STATE_END;
		break;

	case STATE_END:
		if (m_dwEffectTime + 300 < GetTickCount())
		{
			float fX = m_tHitBoxRect.left + (rand() % (m_tHitBoxRect.right - m_tHitBoxRect.left));
			float fY = m_tHitBoxRect.top + (rand() % (m_tHitBoxRect.bottom - m_tHitBoxRect.top));

			CEffectMgr::Get_Instance()->Add_Effect({ fX, fY, 680, 726 }, 9, 0, 40, 350, 350, L"Damaged");
			m_dwEffectTime = GetTickCount();
		}
		break;
	}
}

void CFlower::Switch_Motion()
{
	if (m_ePreState == m_eCurState)
		return;

	m_tFrame.iStart = 0;
	m_tFrame.dwTime = GetTickCount();

	switch (m_eCurState)
	{
	case INTRO:
		m_tFrame.iEnd = 19;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case IDLE:
		m_pFrameKey = L"Boss";

		m_tFrame.iEnd = 11;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = true;
		break;

	case PATTERN1_START:
		m_tFrame.iEnd = 10;
		m_tFrame.iMotion = 1;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case PATTERN1_END:
		m_pFrameKey = L"Pattern1End";

		m_tFrame.iEnd = 5;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 100;
		m_bEndMotion = false;
		break;

	case PATTERN2_START:
		m_tFrame.iEnd = 5;
		m_tFrame.iMotion = 3;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case PATTERN2:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 4;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case PATTERN2_END:
		m_pFrameKey = L"Boss";

		m_tFrame.iEnd = 5;
		m_tFrame.iMotion = 5;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case PHASE2_START:
		m_pFrameKey = L"Boss";

		m_tFrame.iEnd = 8;
		m_tFrame.iMotion = 6;
		m_tFrame.dwSpeed = 90;
		m_bEndMotion = false;
		break;

	case PHASE2_IDLE:
		m_pFrameKey = L"Boss";

		m_tFrame.iEnd = 9;
		m_tFrame.iMotion = 7;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = true;
		break;

	case PHASE2_PATTERN2:
		m_pFrameKey = L"FiringPollen";

		m_tFrame.iEnd = 21;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 40;
		m_bEndMotion = false;
		break;

	case DEAD:
		m_pFrameKey = L"Boss";

		m_tFrame.iEnd = 5;
		m_tFrame.iMotion = 8;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case STATE_END:
		m_pFrameKey = L"Boss";

		m_tFrame.iEnd = 5;
		m_tFrame.iMotion = 8;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;
	}

	m_ePreState = m_eCurState;
}

void CFlower::Create_Bullet()
{
	if (m_iBulletCount == 0)
	{
		CObjMgr::Get_Instance()->Add_Object(MONSTER_BULLET, CAbstractFactory<CBoomerang>::Create_MonsterBullet(m_tHitBoxRect.left, m_tHitBoxInfo.fY));
		++m_iBulletCount;
	}
}

void CFlower::Create_Rain()
{
	int x = rand() % (WINCX - 400);
	
	x += 100;

	if (m_tPattern2Cycle + 700 < GetTickCount())
	{
		CObjMgr::Get_Instance()->Add_Object(MONSTER_BULLET, CAbstractFactory<CRain>::Create_MonsterBullet(x, 0.f));
		m_iBulletCount2++;
		m_tPattern2Cycle = GetTickCount();
	}
}

void CFlower::Create_PlatformVine()
{
	if (m_iPattern3Count <= 2)
	{
		if (m_tPattern3Cycle + 3000 < GetTickCount())
		{
			int x = rand() % 3;
			int iX(0);

			if (m_iPreVine == x)
				return;

			switch (x)
			{
			case 0:
				iX = PLATFORM1X;
				break;
	
			case 1:
				iX = PLATFORM2X;
				break;

			case 2:
				iX = PLATFORM3X;
				break;
			}
			m_iPreVine = x;

			CObjMgr::Get_Instance()->Add_Object(MONSTER_BULLET, CAbstractFactory<CFlatformVine>::Create_MonsterBullet(iX, m_tInfo.fY + m_tInfo.fCY * 0.1f));
			++m_iPattern3Count;
			m_tPattern3Cycle = GetTickCount();
		}
	}
}

void CFlower::Create_Petal()
{
	if (m_iPattern4Count == 0 && m_tFrame.iStart == 13)
	{
		CObjMgr::Get_Instance()->Add_Object(MONSTER_BULLET, CAbstractFactory<CPetal>::Create_MonsterBullet(m_tHitBoxInfo.fX - m_tHitBoxInfo.fCX * 0.8f, m_tHitBoxInfo.fY));
		++m_iPattern4Count;
	}
}

void CFlower::Move_Frame()
{
	if (m_bStopFrame == true)
		return;

	if (m_tFrame.dwTime + m_tFrame.dwSpeed < GetTickCount())
	{
		m_tFrame.dwTime = GetTickCount();

		++m_tFrame.iStart;

		if (m_tFrame.iStart == m_tFrame.iEnd)
		{
			m_bEndMotion = true;
		}

		if (m_tFrame.iStart > m_tFrame.iEnd)
		{
			m_tFrame.iStart = 0;
		}
	}
}