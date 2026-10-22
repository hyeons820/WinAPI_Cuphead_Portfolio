#pragma once
#include "CScene.h"
#include "CItem.h"

class CStore : public CScene
{
public:
	enum STORE_SOUND { WELCOME, BUY, GOOD_BYE, SOUND_END };

public:
	CStore();
	virtual ~CStore();

private:
	void Key_Input();

public:
	void Initialize() override;
	void Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;

private:
	vector<CObj*> m_vecItems;
	UI_NUM m_eUINum;
	CObj* m_pPig;

	int m_iSelect;
	bool m_bUIActive;

	// 사운드 관련
	FMOD_SOUND* m_StoreSound[SOUND_END];
};

