#include "pch.h"
#include "CPlayer.h"
#include "CBmpMgr.h"
#include "CKeyMgr.h"
#include "CCollisionMgr.h"
#include "CScrollMgr.h"
#include "CObjMgr.h"
#include "CAbstractFactory.h"
#include "CBullet.h"
#include "CPlayerBullet.h"
#include "CEXBullet.h"
#include "CDataMgr.h"
#include "CLobberEX.h"
#include "CEffectMgr.h"
#include "CUIMgr.h"
#include "CDie.h"

CPlayer::CPlayer() : m_eCurState(STATE_END), m_ePreState(STATE_END), m_eCurDir(DIR_END), m_dwDashTime(GetTickCount()), m_fJumpSpeed(0.f), m_fJumpTime(0.f), m_bGravity(true), m_fJumpY(0.f), m_fY(0.f), m_bJump(false), m_dwShotTime(GetTickCount()), m_bAttack(false), m_iAttackCount(0), m_iEXCount(0.f), m_iMaxEXCount(5.f), m_tChargeTime(GetTickCount()), m_iMaxHp(8), m_bInvincibility(false), m_tInvincibilityTime(GetTickCount()), m_bOffset(true), m_iSelectWeapon(0), m_bParry(false), m_dwParry(GetTickCount()), m_bParryCount(false)
{
	m_pTargetLine = nullptr;
	ZeroMemory(m_PlayerSound, sizeof(m_PlayerSound));
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize()
{
	// 플레이어 사이즈 및 기본 정보
	m_tInfo = { 350.f, WINCY * 0.6f, 150.f, 90.f };

	m_tHitBoxInfo.fCX = m_tInfo.fCX * 0.35f;
	m_tHitBoxInfo.fCY = m_tInfo.fCY * 0.8f;

	m_tHitBoxInfo.fX = m_tInfo.fX;
	m_tHitBoxInfo.fY = m_tInfo.fY + m_tInfo.fCY * 0.1f;


	m_fSpeed = 7.5f;
	m_fDistance = 100.f;
	m_fJumpSpeed = 14.f;

	// 플레이어 이미지
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Player/PlayerLeft.bmp", L"PlayerLeft");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Player/PlayerRight.bmp", L"PlayerRight");

	// 플레이어 상태 초기화
	m_eCurState = IDLE;
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 7;
	m_tFrame.iMotion = 0;
	m_tFrame.dwTime = GetTickCount();
	m_tFrame.dwSpeed = 70;

	m_eCurDir = RIGHT;
	m_eObjType = PLAYER;
	m_eRender = RENDER_PLAYER;

	m_iHp = 5;

	m_bInvincibility = true;
	m_tInvincibilityTime = GetTickCount();

	m_PlayerSound[BASIC_FIRE] = m_pSoundMgr->Get_MapSound(L"BasicFire.mp3");
	m_PlayerSound[DAMAGED] = m_pSoundMgr->Get_MapSound(L"PlayerDamaged.wav");
	m_PlayerSound[SOUND_DASH] = m_pSoundMgr->Get_MapSound(L"PlayerDash.wav");
	m_PlayerSound[SOUND_JUMP] = m_pSoundMgr->Get_MapSound(L"PlayerJump.wav");
	m_PlayerSound[SOUND_EX] = m_pSoundMgr->Get_MapSound(L"PlayerEx.wav");
	m_PlayerSound[SOUND_DEAD] = m_pSoundMgr->Get_MapSound(L"PlayerDead.mp3");
	m_PlayerSound[SOUND_LAND] = m_pSoundMgr->Get_MapSound(L"PlayerLand.wav");
	m_PlayerSound[SOUND_PARRY] = m_pSoundMgr->Get_MapSound(L"PlayerParry.wav");
	m_PlayerSound[SOUND_TRAMP] = m_pSoundMgr->Get_MapSound(L"goofy-spring-sound.mp3");

	// 장착 중인 무기
	m_vecPlayerWeapon = CDataMgr::Get_Instance()->Get_CurEquipItem();

	if (m_vecPlayerWeapon[m_iSelectWeapon] == NOTHING)
		m_iSelectWeapon++;

	// 체력 증가 아이템 있는지
	vector<ITEM_TYPE> vecItem = CDataMgr::Get_Instance()->Get_PlayerItemInven();
	
	for (auto& vec : vecItem)
	{
		if (vec == INCRED_HP)
			m_iHp += 2;
	}
}

int CPlayer::Update()
{
	if (m_bInvincibility == true)
	{
		if (m_tInvincibilityTime + 2000 < GetTickCount())
		{
			m_bInvincibility = false;
		}
	}

	if (m_tHitBoxInfo.fY >= WINCY + m_tHitBoxInfo.fCY && m_eCurState != DEAD)
	{
		m_eCurState = DEAD;
		CUIMgr::Get_Instance()->Add_UI(UDIE, CAbstractFactory<CDie>::Create_Obj());
	}

	CObj::Update_Rect();

	if (m_eCurState == CROUCH_START || m_eCurState == CROUCH_IDLE || m_eCurState == CROUCH_SHOT)
	{
		m_tHitBoxRect.top = m_tHitBoxRect.top + m_tHitBoxInfo.fCY * 0.5f;
	}

	Key_Input();

	Fall();
	Get_TargetLine();

	ChargeEX();

	return OBJ_NOEVENT;
}

void CPlayer::Late_Update()
{
	if (m_bOffset == true)
	{
		Offest();
	}
	
	Limit_Move();

	Switch_State();
	Move_Frame();
	Switch_Motion(m_eCurState);

	m_tInfo.fX = m_tHitBoxInfo.fX;
	m_tInfo.fY = m_tHitBoxInfo.fY - m_tInfo.fCY * 0.1f;	
}

void CPlayer::Render(HDC hDC)
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();

	HDC hMemDC = nullptr;

	if (m_eCurDir == LEFT || m_eCurDir == LEFT_UP || m_eCurDir == LEFT_LOOKUP)
	{
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"PlayerLeft");
	}
	else if (m_eCurDir == RIGHT || m_eCurDir == RIGHT_UP || m_eCurDir == RIGHT_LOOKUP)
	{
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"PlayerRight");
	}
	else if(hMemDC == nullptr)
		return;

	//Rectangle(hDC, m_tHitBoxRect.left + iScrollX, m_tHitBoxRect.top, m_tHitBoxRect.right + iScrollX, m_tHitBoxRect.bottom);

	GdiTransparentBlt(hDC,
		m_tRect.left + iScrollX,
		m_tRect.top,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		300 * m_tFrame.iStart,
		180 * m_tFrame.iMotion, 
		300,
		180,
		RGB(3, 3, 3));

	//POINT       ptMouse{};

	//GetCursorPos(&ptMouse);
	//ScreenToClient(g_hWnd, &ptMouse);
	//
	//TCHAR szMouse[32] = L"";
	//swprintf_s(szMouse, L"\t : %f \ %f", (float)ptMouse.x - iScrollX, (float)ptMouse.y);
	//RECT rc = { 0, 300, 300, 600 };
	//DrawText(hDC, szMouse, -1, &rc, DT_CENTER | DT_SINGLELINE);
	//
	//TCHAR szPlayer[32] = L"";
	//swprintf_s(szPlayer, L"\t : %f \ %f", m_tHitBoxInfo.fX, m_tHitBoxInfo.fY);
	//RECT rc2 = { 0, 0, 300, 300 };
	//DrawText(hDC, szPlayer, -1, &rc2, DT_CENTER | DT_SINGLELINE);
}

void CPlayer::Release()
{
	m_pSoundMgr = nullptr;
	ZeroMemory(m_PlayerSound, sizeof(m_PlayerSound));
}

// 키 입력 시 플레이어 상태 변화
void CPlayer::Key_Input() 
{
	if (m_eCurState == DEAD)
		return;
	
	if (m_eCurState == DASH || m_eCurState == SHOT ||  m_eCurState == EX || m_eCurState == CROUCH_START)
	{
		if (m_bEndMotion == true)
			m_eCurState = IDLE;
	}
	else if (m_eCurState == CROUCH_SHOT)
	{
		if (m_bEndMotion == true)
		{
			m_eCurState = CROUCH_IDLE;
		}
	}
	else if (m_eCurState == LOOK_UPSHOT)
	{
		if (m_bEndMotion == true)
		{
			m_eCurState = LOOK_UP;
		}
	}
			
	if (m_eCurState == DASH)
		return;

	if (m_eCurState == SHOT)
		return;

	if (m_eCurState == EX)
		return;

	if (m_eCurState == CROUCH_SHOT)
		return;

	if (m_eCurState == LOOK_UPSHOT)
		return;

	if (m_eCurState == EX)
		return;

	if (m_eCurState == HIT)
		return;

	if (CKeyMgr::Get_Instance()->Key_Down('B'))
	{
		m_iSelectWeapon++;

		if (m_iSelectWeapon >= m_vecPlayerWeapon.size())
			m_iSelectWeapon = 0;

		if (m_vecPlayerWeapon[m_iSelectWeapon] == NOTHING)
		{
			for (int i = 0; i < m_vecPlayerWeapon.size(); ++i)
			{
				m_iSelectWeapon = i;

				if (m_vecPlayerWeapon[m_iSelectWeapon] != NOTHING)
					break;
			}
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Up(VK_UP))
	{
		if (m_eCurDir == LEFT_UP || m_eCurDir == LEFT || m_eCurDir == LEFT_LOOKUP)
		{
			m_eCurDir = LEFT;
		}
		else
		{
			m_eCurDir = RIGHT;
		}
	}

	if (m_eCurState == CROUCH_START && m_tFrame.iStart == m_tFrame.iEnd - 1 && CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		m_eCurState = CROUCH_IDLE;
		return;
	}
	else if (m_eCurState == CROUCH_IDLE && CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		m_eCurState = CROUCH_IDLE;

		if (CKeyMgr::Get_Instance()->Key_Down('X'))
		{
			m_bAttack = true;
			Attack();
			m_eCurState = CROUCH_SHOT;
		}

		if (CKeyMgr::Get_Instance()->Key_Down('V') && m_iEXCount >= 1)
		{
			m_eCurState = EX;
			return;
		}
		return;
	}
	else if (m_eCurState == CROUCH_START)
	{
		if (CKeyMgr::Get_Instance()->Key_Down('X'))
		{
			m_bAttack = true;
			Attack();
			m_eCurState = CROUCH_SHOT;
		}
		return;
	}

	if (m_eCurState == JUMP || m_eCurState == FALL || m_eCurState == PARRY)
	{
		if (CKeyMgr::Get_Instance()->Key_Down('Z'))
		{
			m_bParryCount = true;
			m_eCurState = PARRY;
			m_dwParry = GetTickCount();
			return;
		}

		if (CKeyMgr::Get_Instance()->Key_Down(VK_SHIFT))
		{
			m_bSound = false;
			m_eCurState = DASH;
			return;
		}
		if (CKeyMgr::Get_Instance()->Key_Down('X'))
		{
			if (m_dwShotTime + 10 < GetTickCount())
			{
				m_bAttack = true;
				Attack();
				m_dwShotTime = GetTickCount();
			}
			return;
		}
		
		if (CKeyMgr::Get_Instance()->Key_Down('V') && m_iEXCount >= 1)
		{
			m_eCurState = EX;
			return;
		}

		if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
		{
			m_eCurDir = LEFT;
			m_tHitBoxInfo.fX -= m_fSpeed;
		}
		else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
		{
			m_eCurDir = RIGHT;
			m_tHitBoxInfo.fX += m_fSpeed;
		}
		
		return;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_SHIFT))
	{
		m_bSound = false;
		m_eCurState = DASH;
		return;
	}
	if (CKeyMgr::Get_Instance()->Key_Down('Z'))
	{
		m_eCurState = JUMP;
		m_bJump = true;
		return;
	}
	if (CKeyMgr::Get_Instance()->Key_Down('V') && m_iEXCount >= 1)
	{
		m_eCurState = EX;
		return;
	}

	// TODO : 임시용
	if (CKeyMgr::Get_Instance()->Key_Down('T'))
	{
		m_eCurState = HIT;
		return;
	}
	if (CKeyMgr::Get_Instance()->Key_Down('Y'))
	{
		m_eCurState = DEAD;
		return;
	}

	// 기본 총 쏘기
	if (m_eCurState == IDLE && CKeyMgr::Get_Instance()->Key_Down('X'))
	{
		m_bAttack = true;
		Attack();
		m_eCurState = SHOT;
		return;
	}
	else if (CKeyMgr::Get_Instance()->Key_Down('V') && m_iEXCount >= 1)
	{
		m_eCurState = EX;
		return;
	}
	// 왼쪽 이동 중
	else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
	{
		m_eCurDir = LEFT;
		m_tHitBoxInfo.fX -= m_fSpeed;
		m_eCurState = RUN;

		// 왼 위 쪽 바라보며 가는 중
		if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
		{
			m_eCurDir = LEFT_UP;
			m_eCurState = RUN_DIAGONAL;

			// 왼 위 쪽 바라보며 총 쏘기
			if (CKeyMgr::Get_Instance()->Key_Down('X'))
			{
				if (m_dwShotTime + 10 < GetTickCount())
				{
					m_bAttack = true;
					Attack();
					m_dwShotTime = GetTickCount();
				}
				return;
			}
		}
		// 왼 쪽 가다가 앉기
		else if (CKeyMgr::Get_Instance()->Key_Down(VK_DOWN))
		{
			m_eCurState = CROUCH_START;
			return;
		}
		// 왼 쪽 가면서 총 쏘기
		if (CKeyMgr::Get_Instance()->Key_Down('X'))
		{
			if (m_dwShotTime + 10 < GetTickCount())
			{
				m_bAttack = true;
				Attack();
				m_dwShotTime = GetTickCount();
			}
			return;
		}
	}
	// 오른 쪽 이동
	else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{
		m_eCurDir = RIGHT;
		m_tHitBoxInfo.fX += m_fSpeed;
		m_eCurState = RUN;

		/// 오른 위 바라보며 가기
		if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
		{
			m_eCurDir = RIGHT_UP;
			m_eCurState = RUN_DIAGONAL;

			// 오른 위 바라보며 총 쏘기
			if (CKeyMgr::Get_Instance()->Key_Down('X'))
			{
				if (m_dwShotTime + 10 < GetTickCount())
				{
					m_bAttack = true;
					Attack();
					m_dwShotTime = GetTickCount();
				}
				return;
			}
		}
		// 오른 쪽 가다가 앉기
		else if (CKeyMgr::Get_Instance()->Key_Down(VK_DOWN))
		{
			m_eCurState = CROUCH_START;
			return;
		}
		// 오른 쪽 가면서 총 쏘기
		if (CKeyMgr::Get_Instance()->Key_Down('X'))
		{
			if (m_dwShotTime + 10 < GetTickCount())
			{
				m_bAttack = true;
				Attack();
				m_dwShotTime = GetTickCount();
			}
			return;
		}
	}
	// 앉기
	else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		m_eCurState = CROUCH_START;
		return;
	}
	// 올려다 보기
	else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
	{
		if (m_eCurDir == LEFT || m_eCurDir == LEFT_UP)
		{
			m_eCurDir = LEFT_LOOKUP;
			m_eCurState = LOOK_UP;
		}
		else if (m_eCurDir == RIGHT || m_eCurDir == RIGHT_UP)
		{
			m_eCurDir = RIGHT_LOOKUP;
			m_eCurState = LOOK_UP;
		}
		
		if (CKeyMgr::Get_Instance()->Key_Up(VK_LEFT))
		{
			m_eCurDir = LEFT_LOOKUP;
			m_eCurState = LOOK_UP;
			return;
		}

		if (m_eCurState == LOOK_UP && CKeyMgr::Get_Instance()->Key_Up(VK_RIGHT))
		{
			m_eCurDir = RIGHT_LOOKUP;
			m_eCurState = LOOK_UP;
			return;
		}

		if (CKeyMgr::Get_Instance()->Key_Down('X'))
		{
			m_bAttack = true;
			Attack();
			m_eCurState = LOOK_UPSHOT;
			return;
		}
	}
	else
	{
		m_eCurState = IDLE;

		if (m_eCurDir == RIGHT_LOOKUP)
			m_eCurDir = RIGHT;
		else if (m_eCurDir == LEFT_LOOKUP)
			m_eCurDir = LEFT;
		return;
	}
}

// 플레이어 애니메이션 제어
void CPlayer::Switch_State()
{
	if (m_ePreState == m_eCurState)
		return;

	m_tFrame.iStart = 0;
	m_tFrame.dwTime = GetTickCount();

	switch (m_eCurState)
	{
	case IDLE:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = true;
		break;

	case SHOT:
		m_tFrame.iEnd = 2;
		m_tFrame.iMotion = 1;
		m_tFrame.dwSpeed = 40;
		m_bEndMotion = false;
		break;

	case EX:
		m_tFrame.iEnd = 6;
		m_tFrame.iMotion = 3;
		m_tFrame.dwSpeed = 45;
		m_bEndMotion = false;
		break;

	case RUN:
		m_tFrame.iEnd = 15;
		m_tFrame.iMotion = 5;
		m_tFrame.dwSpeed = 20;
		m_bEndMotion = true;
		break;

	case RUN_DIAGONAL:
		m_tFrame.iEnd = 15;
		m_tFrame.iMotion = 6;
		m_tFrame.dwSpeed = 20;
		m_bEndMotion = true;
		break;

	case VILLAGE_RUN:
		m_tFrame.iEnd = 15;
		m_tFrame.iMotion = 4;
		m_tFrame.dwSpeed = 20;
		m_bEndMotion = true;
		break;

	case LOOK_UP:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = true;
		break;

	case LOOK_UPSHOT:
		m_tFrame.iEnd = 2;
		m_tFrame.iMotion = 2;
		m_tFrame.dwSpeed = 30;
		m_bEndMotion = false;
		break;

	case DASH:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 7;
		m_tFrame.dwSpeed = 40;
		m_bEndMotion = false;
		break;

	case JUMP:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 8;
		m_tFrame.dwSpeed = 30;
		m_bEndMotion = false;
		break;

	case FALL:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 8;
		m_tFrame.dwSpeed = 30;
		m_bEndMotion = false;
		break;

	case CROUCH_START:
		m_tFrame.iEnd = 6;
		m_tFrame.iMotion = 10;
		m_tFrame.dwSpeed = 15;
		m_bEndMotion = false;
		break;

	case CROUCH_IDLE:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 11;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;

	case CROUCH_SHOT:
		m_tFrame.iEnd = 2;
		m_tFrame.iMotion = 12;
		m_tFrame.dwSpeed = 40;
		m_bEndMotion = false;
		break;

	case PARRY:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 9;
		m_tFrame.dwSpeed = 30;
		m_bEndMotion = false;
		break;

	case HIT:
		m_tFrame.iEnd = 5;
		m_tFrame.iMotion = 13;
		m_tFrame.dwSpeed = 40;
		m_bEndMotion = false;
		break;

	case DEAD:
		m_tFrame.iEnd = 7;
		m_tFrame.iMotion = 14;
		m_tFrame.dwSpeed = 70;
		m_bEndMotion = false;
		break;
	}

	m_ePreState = m_eCurState;
}

// 플레이어 상태 별 기능
void CPlayer::Switch_Motion(PLAYER_STATE eState)
{
	switch (m_eCurState)
	{
	case IDLE:
		m_fJumpSpeed = 14.f;
		m_bParry = false;
		m_bSound = false;
		break;

	case SHOT:
		break;

	case EX:
		if (m_tFrame.iStart == 4)
		{
			m_bAttack = true;
			++m_iAttackCount;
		}

		m_bGravity = false;
		m_bJump = false;

		AttackEX();

		if (m_bEndMotion == true)
		{
			m_bGravity = true;
			m_fJumpTime = 0.f;
			m_eCurState = FALL;
			m_iAttackCount = 0;
		}
		break;

	case RUN:
		break;

	case VILLAGE_RUN:
		break;

	case LOOK_UP:
		break;

	case LOOK_UPSHOT:
		break;

	case DASH:
		m_bGravity = false;
		m_bInvincibility = true;
		Dash();
		if (m_tFrame.iStart == m_tFrame.iEnd - 1)
			m_bInvincibility = false;
		break;

	case JUMP:
		m_bGravity = false;
		Jump();

		if (m_fJumpY < 0)
		{
			m_eCurState = FALL;
			m_bJump = false;
			m_fJumpTime = 0.f;
			m_bGravity = true;
		}

		break;

	case FALL:		
		break;

	case CROUCH_START:
		break;

	case CROUCH_IDLE:
		break;

	case CROUCH_SHOT:
		break;

	case PARRY:	
		if (m_fJumpY < 0 && m_bJump == true)
		{
			m_bJump = false;
			m_bGravity = true;
			m_fJumpTime = 0.f;
		}
		else if (m_bJump == true)
		{
			m_fJumpY = m_fJumpSpeed * m_fJumpTime - (9.8f * m_fJumpTime * m_fJumpTime) * 0.5f;

			m_tHitBoxInfo.fY -= m_fJumpY;

			m_fJumpTime += 0.1f;
		}
		else if (m_bGravity == false)
		{
			Jump();

			if (m_fJumpY < 0)
			{
				m_bJump = false;
				m_fJumpTime = 0.f;
				m_bGravity = true;
			}
		}
	
		if (m_dwParry + 100 > GetTickCount())
			m_bParry = true;
		else
			m_bParry = false;

		break;

	case HIT:
		if (m_iHp > 0 && m_bInvincibility == false)
		{
			--m_iHp;
		}

		m_bGravity = false;
		m_bJump = false;
		m_bInvincibility = true;

		if (m_bEndMotion == true)
		{
			m_bGravity = true;
			m_fJumpTime = 0.f;
			m_eCurState = FALL;
		}
		break;

	case DEAD:		
		if (m_iHp > 0 && m_bInvincibility == false)
		{
			--m_iHp;
		}
		m_bGravity = false;
		m_bJump = false;
		m_bInvincibility = true;

		if (m_bEndMotion == true)
			m_tFrame.iStart = m_tFrame.iEnd + 1;
		break;
	}
}

void CPlayer::Limit_Move()
{
	int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();

	if (m_tHitBoxInfo.fX - iScrollX <= MINCX)
		m_tHitBoxInfo.fX = MINCX;

	if (m_tHitBoxInfo.fX >= MAXCX - 50 - iScrollX)
		m_tHitBoxInfo.fX = MAXCX - 50 - iScrollX;
}

void CPlayer::Offest()
{
	int     iOffSetMinX = 300;
	int     iOffSetMaxX = 500;

	int		iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();

	if (iOffSetMinX > m_tInfo.fX + iScrollX)
	{
		CScrollMgr::Get_Instance()->Set_ScrollX(m_fSpeed);
	}

	if (iOffSetMaxX < m_tInfo.fX + iScrollX)
	{
		CScrollMgr::Get_Instance()->Set_ScrollX(-m_fSpeed);
	}
}

void CPlayer::Get_TargetLine()
{
	m_fY = 0.f;
	m_fY = m_tHitBoxInfo.fY;

	bool bLineCollision = CCollisionMgr::Collision_Line(this, &m_fY, m_tHitBoxInfo.fX, (m_tHitBoxInfo.fCY * 0.5f));

	if (m_bJump || bLineCollision == false)
	{
		return;
	}
	else
	{
		if ((bLineCollision) && (m_fY < m_tInfo.fY - m_tHitBoxInfo.fCY * 0.5f))
		{
			m_fJumpTime = 0.f;
			m_tHitBoxInfo.fY = m_fY - m_tHitBoxInfo.fCY * 0.5f;

			if (m_eCurState == FALL || m_eCurState == PARRY)
			{
				m_eCurState = IDLE;
			}

			
		}
		else
		{
			m_fJumpTime = 0.f;
			m_tHitBoxInfo.fY = m_fY - m_tHitBoxInfo.fCY * 0.5f;

			if (m_eCurState == FALL || m_eCurState == PARRY)
			{
				m_eCurState = IDLE;

				m_pSoundMgr->StopSound(SOUND_PLAYER);

				FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_LAND], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

				CEffectMgr::Get_Instance()->Add_Effect({ m_tInfo.fX, (float)m_tRect.bottom - 20.f, 280.f, 160.f }, 11, 0, 20, 120, 70, L"LandDust");
			}
		}
	}
}

void CPlayer::Dash()
{
	if (m_tFrame.iStart >= 1)
	{
		if (m_bSound == false)
		{
			CEffectMgr::Get_Instance()->Add_Effect({ m_tInfo.fX, m_tInfo.fY - 20.f, 91.75f, 100.f }, 19, 0, 20, 100, 100, L"DashDust");

			m_pSoundMgr->StopSound(SOUND_PLAYER);
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_DASH], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

			m_bSound = true;
		}
		if (m_eCurDir == LEFT || m_eCurDir == LEFT_UP)
		{
			if (m_dwDashTime + (m_tFrame.dwSpeed * m_tFrame.iEnd) > GetTickCount())
				m_tHitBoxInfo.fX -= m_fSpeed;
		}
		else if (m_eCurDir == RIGHT || m_eCurDir == RIGHT_UP)
		{
			if (m_dwDashTime + (m_tFrame.dwSpeed * m_tFrame.iEnd) > GetTickCount())
				m_tHitBoxInfo.fX += m_fSpeed;
		}
	}
	if (m_tFrame.iStart == m_tFrame.iEnd)
	{
		m_bGravity = true;
		m_bJump = false;
		m_fJumpTime = 0.f;
		m_eCurState = FALL;
		m_bSound = false;
	}
	m_dwDashTime = GetTickCount();
}

void CPlayer::Jump()
{
	if (m_bSound == false)
	{
		m_pSoundMgr->StopSound(SOUND_PLAYER);
		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_JUMP], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

		m_bSound = true;
	}

	m_fJumpY = m_fJumpSpeed * m_fJumpTime - (9.8f * m_fJumpTime * m_fJumpTime) * 0.5f;

	m_tHitBoxInfo.fY -= m_fJumpY;

	m_fJumpTime += 0.1f;
}

void CPlayer::Fall()
{
	if (m_bGravity == false)
		return;

	m_tHitBoxInfo.fY += m_fJumpTime * 9.8f;
	m_fJumpTime += 0.1f;
}

void CPlayer::Attack()
{
	m_pSoundMgr->StopSound(SOUND_WEAPON);
	FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[BASIC_FIRE], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_WEAPON));

	if (m_bAttack == true)
	{
		switch (m_vecPlayerWeapon[m_iSelectWeapon])
		{
		case BASIC_BULLET:
			CObjMgr::Get_Instance()->Add_Object(BULLET, CAbstractFactory<CPlayerBullet>::Create_Bullet(m_eCurDir, m_tHitBoxInfo.fX, m_tHitBoxRect.bottom - ((m_tHitBoxRect.bottom - m_tHitBoxRect.top) * 0.5f)));
			break;

		case LOBBER_BULLET:
			CObjMgr::Get_Instance()->Add_Object(BULLET, CAbstractFactory<CPlayerLobberBullet>::Create_LobberBullet(m_eCurDir, m_tHitBoxInfo.fX, m_tHitBoxRect.bottom - ((m_tHitBoxRect.bottom - m_tHitBoxRect.top) * 0.5f)));
			break;
		}
		
		m_bAttack = false;
	}
}

void CPlayer::AttackEX()
{
	if (m_bAttack == true && m_iAttackCount == 1 && m_iEXCount >= 1)
	{
		m_pSoundMgr->StopSound(SOUND_EXWEAPON);
		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_EX], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_EXWEAPON));

		switch (m_vecPlayerWeapon[m_iSelectWeapon])
		{
		case BASIC_BULLET:
			CObjMgr::Get_Instance()->Add_Object(EX_BULLET, CAbstractFactory<CEXBullet>::Create_EXBullet(m_eCurDir, m_tHitBoxInfo.fX, m_tHitBoxInfo.fY));
			m_bAttack = false;
			--m_iEXCount;
			break;

		case LOBBER_BULLET:
			CObjMgr::Get_Instance()->Add_Object(EX_BULLET, CAbstractFactory<CLobberEX>::Create_LobberBullet(m_eCurDir, m_tHitBoxInfo.fX, m_tHitBoxInfo.fY));
			m_bAttack = false;
			--m_iEXCount;
			break;
		}
	}
}

void CPlayer::ChargeEX()
{
	if (m_iEXCount >= m_iMaxEXCount)
		return;

	if (m_tChargeTime + 500 < GetTickCount())
	{
		m_iEXCount += 0.1f;
		m_tChargeTime = GetTickCount();
	}
}

void CPlayer::OnCollision(OBJ_ID eID)
{
	if (eID == MONSTER || eID == MONSTER_BULLET)
	{
		if (m_bInvincibility == true)
			return;

		if (m_iHp <= 1)
		{
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_DEAD], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));
			CUIMgr::Get_Instance()->Add_UI(UDIE, CAbstractFactory<CDie>::Create_Obj());
			m_eCurState = DEAD;
			return;
		}

		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[DAMAGED], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

		m_eCurState = HIT;
		m_tInvincibilityTime = GetTickCount();
	}

	if (eID == PARRY_BULLET)
	{
		if (m_bParry)
		{
			CDataMgr::Get_Instance()->Set_Parry();
			m_pSoundMgr->StopSound(SOUND_PLAYER);
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_PARRY], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

			CEffectMgr::Get_Instance()->Add_Effect({ m_tHitBoxInfo.fX, m_tHitBoxInfo.fY, 200.f, 200.f }, 5, 0, 50, 200, 200, L"Parry");
			m_bGravity = false;
			m_bJump = true;
			m_fJumpTime = 0.1f;
			m_fJumpSpeed = 14.f;
			Jump();
			return;
		}

		if (m_bInvincibility == true)
			return;

		if (m_iHp <= 1)
		{
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_DEAD], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));
			CUIMgr::Get_Instance()->Add_UI(UDIE, CAbstractFactory<CDie>::Create_Obj());
			m_eCurState = DEAD;
			return;
		}

		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[DAMAGED], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

		m_eCurState = HIT;
		m_tInvincibilityTime = GetTickCount();
	}

	if (eID == PARRY_OBJ)
	{
		if (!m_bParryCount)
			return;

		if (m_bParry)
		{
			m_bParryCount = false;
			m_pSoundMgr->StopSound(SOUND_PLAYER);
			FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_PARRY], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));

			CEffectMgr::Get_Instance()->Add_Effect({ m_tHitBoxInfo.fX, m_tHitBoxInfo.fY, 200.f, 200.f }, 5, 0, 50, 200, 200, L"Parry");
			m_bGravity = false;
			m_bJump = true;
			m_fJumpTime = 0.1f;
			m_fJumpSpeed = 14.f;
			Jump();
			return;
		}
	}

	if (eID == TRAMPOLINE && m_eCurState == FALL || eID == TRAMPOLINE && m_eCurState == PARRY)
	{
		m_pSoundMgr->StopSound(SOUND_PLAYER);
		FMOD_System_PlaySound(m_pSoundMgr->Get_Instance()->Get_System(), m_PlayerSound[SOUND_TRAMP], nullptr, FALSE, m_pSoundMgr->Get_Instance()->Get_Channel(SOUND_PLAYER));
		m_bJump == true;
		m_eCurState = JUMP;
		m_fJumpTime = 0.f;
		m_fJumpSpeed = 17.f;
	}
}
