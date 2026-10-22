#pragma once
#include "CScene.h"
#include "CObj.h"

class CRunAndGun : public CScene
{
public:
	CRunAndGun();
	virtual ~CRunAndGun();

public:
	void Initialize() override;
	void Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;

private:
	void Key_Input();

private:
	CObj* m_pPlayer;

	DWORD m_dwExitTime;
	DWORD m_dwTime;
	DWORD m_dwReadyTime;
	
	bool m_bExit;
	bool m_bEffect;
	bool m_bReady;

	FMOD_SOUND* m_BravoSound;
	FMOD_SOUND* m_ReadySound;
};

