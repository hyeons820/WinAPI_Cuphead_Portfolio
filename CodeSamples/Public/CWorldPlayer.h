#pragma once
#include "CObj.h"


class CWorldPlayer : public CObj
{
public:
	enum WORLD_PLAYER_STATE { WALK, WIN, STATE_END, IDLE };

public:
	CWorldPlayer();
	virtual ~CWorldPlayer();

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
	void OnCollision(OBJ_ID eID) override;

private:
	void Key_Input();
	void Offset(float _fXSpeed, float _fYSpeed);
	void Motion_Change();

private:
	WORLD_PLAYER_STATE m_ePreState;
	WORLD_PLAYER_STATE m_eCurState;
};

