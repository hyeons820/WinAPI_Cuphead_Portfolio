#pragma once
#include "CUI.h"
class CEquipItem : public CUI
{
public:
    CEquipItem();
    virtual ~CEquipItem();


public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
    void OnCollision(OBJ_ID eID) override {};

private:
    void Key_Input();
    void Set_CurItem();

private:
    vector<ITEM_TYPE> m_vecPlayeritems;
    vector<CObj*> m_vecWeaponLogo;
    POINT m_tPoint[2];
    CObj* m_pSelect;

    DWORD m_dwTime;

    int m_iSelect;
};

