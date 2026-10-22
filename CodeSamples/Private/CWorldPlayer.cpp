#include "pch.h"
#include "CWorldPlayer.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CDataMgr.h"

CWorldPlayer::CWorldPlayer() : m_ePreState(STATE_END), m_eCurState(STATE_END)
{
}

CWorldPlayer::~CWorldPlayer()
{
    Release();
}

void CWorldPlayer::Initialize()
{
    m_tInfo.fCX = 79.f;
    m_tInfo.fCY = 87.f;
   
    m_tHitBoxInfo = { CDataMgr::Get_Instance()->Get_WorldPlayerData()->fX, CDataMgr::Get_Instance()->Get_WorldPlayerData()->fY, TILECX, TILECY};

    m_fSpeed = 5.f;

    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Player/PlayerWorld.bmp", L"PlayerWorld");

    m_tFrame.iMotion = 4;

    m_eObjType = WORLD_PLAYER;

    m_eRender = RENDER_PLAYER;
}

int CWorldPlayer::Update()
{
    m_tInfo.fX = m_tHitBoxInfo.fX;
    m_tInfo.fY = m_tHitBoxInfo.fY;

    __super::Update_Rect();

    Key_Input();

    return OBJ_NOEVENT;
}

void CWorldPlayer::Late_Update()
{
    Motion_Change();

    Move_Frame();
}

void CWorldPlayer::Render(HDC hDC)
{
    int iScrollX = (int)CScrollMgr::Get_Instance()->Get_ScrollX();
    int iScrollY = (int)CScrollMgr::Get_Instance()->Get_ScrollY();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"PlayerWorld");

    GdiTransparentBlt(hDC,
        m_tRect.left + iScrollX,
        m_tRect.top + iScrollY,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        79 * m_tFrame.iStart,
        87 * m_tFrame.iMotion,
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        RGB(99, 92, 99));

    //Rectangle(hDC, m_tHitBoxRect.left + iScrollX, m_tHitBoxRect.top + iScrollY, m_tHitBoxRect.right + iScrollX, m_tHitBoxRect.bottom + iScrollY);

  //POINT       ptMouse{};
  //
  //GetCursorPos(&ptMouse);
  //ScreenToClient(g_hWnd, &ptMouse);
  //
  //TCHAR szMouse[32] = L"";
  //swprintf_s(szMouse, L"\t : %f \ %f", (float)ptMouse.x, (float)ptMouse.y);
  //
  //RECT rc = { 0, 300, 300, 600 };
  //DrawText(hDC, szMouse, -1, &rc, DT_CENTER | DT_SINGLELINE);
  //
  //TCHAR szPlayer[32] = L"";
  //swprintf_s(szPlayer, L"\t : %f \ %f", m_tHitBoxInfo.fX, m_tHitBoxInfo.fY);
  //RECT rc2 = { 0, 0, 300, 300 };
  //DrawText(hDC, szPlayer, -1, &rc2, DT_CENTER | DT_SINGLELINE);
}

void CWorldPlayer::Release()
{
}

void CWorldPlayer::OnCollision(OBJ_ID eID)
{
}

void CWorldPlayer::Key_Input()
{
    float x = (m_tHitBoxInfo.fX / sqrtf(m_tHitBoxInfo.fX * m_tHitBoxInfo.fX + m_tHitBoxInfo.fY * m_tHitBoxInfo.fY));
    float y = (m_tHitBoxInfo.fY / sqrtf(m_tHitBoxInfo.fX * m_tHitBoxInfo.fX + m_tHitBoxInfo.fY * m_tHitBoxInfo.fY));
    
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            m_tHitBoxInfo.fX -= m_fSpeed * x;
            m_tHitBoxInfo.fY -= m_fSpeed * y;

            m_eCurState = WALK;
            m_tFrame.iMotion = 5;

            Offset(m_fSpeed * x, m_fSpeed * y);
            
            return;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            m_tHitBoxInfo.fX -= m_fSpeed * x;
            m_tHitBoxInfo.fY += m_fSpeed * y;

            m_eCurState = WALK;
            m_tFrame.iMotion = 7;

            Offset(m_fSpeed * x, m_fSpeed * y);

            return;
        }

        m_tHitBoxInfo.fX -= m_fSpeed;
        m_eCurState = WALK;
        m_tFrame.iMotion = 6;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            m_tHitBoxInfo.fX += m_fSpeed * x;
            m_tHitBoxInfo.fY -= m_fSpeed * y;

            m_eCurState = WALK;
            m_tFrame.iMotion = 1;

            Offset(m_fSpeed * x, m_fSpeed * y);
            
            return;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            m_tHitBoxInfo.fX += m_fSpeed * x;
            m_tHitBoxInfo.fY += m_fSpeed * y;

            m_eCurState = WALK;
            m_tFrame.iMotion = 3;

            Offset(m_fSpeed * x, m_fSpeed * y);
            
            return;
        }

        m_tHitBoxInfo.fX += m_fSpeed;
        m_eCurState = WALK;
        m_tFrame.iMotion = 2;
    }
    else  if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        m_tHitBoxInfo.fY -= m_fSpeed;
        m_eCurState = WALK;
        m_tFrame.iMotion = 0;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        m_tHitBoxInfo.fY += m_fSpeed;
        m_eCurState = WALK;
        m_tFrame.iMotion = 4;
    }
    else
    {
        m_eCurState = IDLE;
    }

    Offset(m_fSpeed, m_fSpeed);
}

void CWorldPlayer::Offset(float _fXSpeed, float _fYSpeed)
{
    float   iOffSetMinX = WINCX * 0.5f - 50.f;
    float   iOffSetMaxX = WINCX * 0.5f + 50.f;

    float   iOffSetMinY = WINCY * 0.5f - 50.f;
    float   iOffSetMaxY = WINCY * 0.5f + 50.f;

	float	iScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float   iScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    if (iOffSetMinX > m_tHitBoxInfo.fX + iScrollX)
    {
        CScrollMgr::Get_Instance()->Set_ScrollX(_fXSpeed);
    }

    if (iOffSetMaxX <= m_tHitBoxInfo.fX + iScrollX)
    {
        CScrollMgr::Get_Instance()->Set_ScrollX(-_fXSpeed);
    }

    if (iOffSetMinY > m_tHitBoxInfo.fY + iScrollY)
    {
        CScrollMgr::Get_Instance()->Set_ScrollY(_fYSpeed);
    }

    if (iOffSetMaxY <= m_tHitBoxInfo.fY + iScrollY)
    {
        CScrollMgr::Get_Instance()->Set_ScrollY(-_fYSpeed);
    }
}

void CWorldPlayer::Motion_Change()
{
    if (m_ePreState != m_eCurState)
    {

        m_tFrame.iStart = 0;
        m_tFrame.dwTime = GetTickCount();

        switch (m_eCurState)
        {
        case WALK:
            m_tFrame.iEnd = 11;
            m_tFrame.dwSpeed = 40;
            break;

        case WIN:
            m_tFrame.iEnd = 11;
            m_tFrame.iMotion = 8;
            m_tFrame.dwSpeed = 40;
            break;

        case IDLE:
            m_tFrame.iEnd = 0;
            break;
        }

        m_ePreState = m_eCurState;
    }
}