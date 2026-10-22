#pragma once
#include "CObj.h"
class CRNGTrigger :
    public CObj
{
public:
	CRNGTrigger();
	virtual ~CRNGTrigger();

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
	void OnCollision(OBJ_ID eID) override;
	void OnCollision_Exit(OBJ_ID eID) override;

private:
	bool m_bOnCollision;
	bool m_bActive;
	bool m_bEffect;

	DWORD m_dwTime;
};

