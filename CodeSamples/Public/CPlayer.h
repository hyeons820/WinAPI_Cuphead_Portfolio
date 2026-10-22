#pragma once
#include "CObj.h"
#include "CLine.h"
#include "CSoundMgr.h"

class CPlayer : public CObj
{
public:
    enum PLAYER_STATE { IDLE, SHOT, EX, RUN, RUN_DIAGONAL, LOOK_UP, LOOK_UPSHOT, VILLAGE_RUN, DASH, JUMP, FALL, CROUCH_START, CROUCH_IDLE, CROUCH_SHOT, PARRY, HIT, DEAD, STATE_END };

    enum PLAYER_SOUND { BASIC_FIRE, DAMAGED, SOUND_DASH, SOUND_LAND, SOUND_JUMP, SOUND_PARRY, SOUND_EX, SOUND_DEAD, SOUND_TRAMP, PLAYER_SOUND_END };

public:
    CPlayer();
    virtual ~CPlayer();

public:
    bool Get_CurStateDead() { return m_eCurState == DEAD; }
    bool Get_ParryState() { return m_bParry; }
    const float Get_EXCount() const { return m_iEXCount; }

    void        Set_Offset(bool _bOffset) { m_bOffset = _bOffset; }

public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
    void OnCollision(const OBJ_ID eID) override;

private:
    void Key_Input();
    void Switch_State();
    void Switch_Motion(PLAYER_STATE eState);
    void Limit_Move();
    void Offest();

    // 현재 위치의 가장 가까이 붙어 있는 선 구하기
    void Get_TargetLine();

    // 플레이어 상태 관련 함수
    void Dash();
    void Jump();
    void Fall();
    void Attack();
    void AttackEX();
    void ChargeEX();

private:
    PLAYER_STATE m_eCurState;
    PLAYER_STATE m_ePreState;

    PLAYER_DIR   m_eCurDir;

    vector<ITEM_TYPE> m_vecPlayerWeapon;
    int m_iSelectWeapon;

    // 상태에 따른 값
    int         m_iMaxHp;
    bool        m_bOffset;

    bool        m_bInvincibility;
    DWORD       m_tInvincibilityTime;
    
    // 대쉬 관련
    DWORD   m_dwDashTime;

    // 점프, 낙하 관련
    float   m_fJumpTime;
    float   m_fJumpSpeed;
    float   m_fJumpY;
    float   m_fY;

    bool    m_bGravity;
    bool    m_bJump;

    CLine*  m_pTargetLine;

    // 공격 관련
    DWORD   m_dwShotTime;
    bool    m_bAttack;
    int     m_iAttackCount;

    // EX 횟수
    float   m_iEXCount;
    float   m_iMaxEXCount;
    DWORD   m_tChargeTime;

    // 패링 관련
    bool m_bParry;
    bool m_bParryCount;
    DWORD m_dwParry;

    // 사운드 관련
    FMOD_SOUND* m_PlayerSound[PLAYER_SOUND_END];
};

