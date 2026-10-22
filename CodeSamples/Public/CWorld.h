#pragma once
#include "CScene.h"
#include "CObj.h"

class CWorld : public CScene
{
public:
	CWorld();
	virtual ~CWorld();

public:
	void Initialize() override;
	void Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;

private:
	void Key_Input();

private:
	CObj* m_pInven;
	bool m_bTileRender;
	float m_iScrollX;
	float m_iScrollY;

	bool m_bEquip;
};

