#pragma once
#include "CMonster.h"
class CFlower : public CMonster
{
public:
	enum FLOWER_STATE { INTRO, IDLE, PATTERN1_START, PATTERN1_END, PATTERN2_START, PATTERN2, PATTERN2_END, PHASE2_START, PHASE2_IDLE, 
		PHASE2_PATTERN1, PHASE2_PATTERN2, DEAD, STATE_END };

	enum FLOWER_SOUND { SOUND_INTRO, SOUND_PHASE2, RAINSTART, RAINLOOP, VINE, SOUND_DEAD, SOUND_END};

public:
	CFlower();
	virtual ~CFlower();

public:
	bool Get_CurStateDead() { return m_eCurState == DEAD; }

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
	void OnCollision(OBJ_ID eID) override;

	void Move_Frame() override;

private:
	void Switch_State(FLOWER_STATE eState);
	void Switch_Motion();

	void Create_Bullet();
	void Create_Rain();

	void Create_PlatformVine();
	void Create_Petal();

private:
	FLOWER_STATE m_ePreState;
	FLOWER_STATE m_eCurState;

	bool m_iPhase2;

	// 패턴 1 : 총알 생성
	DWORD m_tPatternCycle;
	DWORD m_tPattern1Time;

	int m_iBulletCount;

	// 패턴 2 : 총알 비
	DWORD m_tPattern2Cycle;

	int m_iPattern2Count;
	int m_iBulletCount2;

	// 패턴 3 : 2페이즈 뿌리 올리기
	DWORD m_tPattern3Cycle;
	int m_iPattern3Count;

	int m_iPreVine;

	// 패턴 4 : 2페이즈 입에서 총알 발사
	int m_iPattern4Count;

	bool m_bStopFrame;

	// 사운드 관련
	FMOD_SOUND* m_FlowerSound[SOUND_END];

	FMOD_SOUND* m_WinSound;
	FMOD_SOUND* m_ReadySound;

	// 죽었을 때 이펙트
	DWORD m_dwEffectTime;
};

