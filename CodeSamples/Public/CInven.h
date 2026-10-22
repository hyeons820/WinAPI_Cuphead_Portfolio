#pragma once
#include "CUI.h"
class CInven : public CUI
{
public:
    CInven();
    virtual ~CInven();

public:
    void Set_ItemType(ITEM_TYPE eType) { m_eItemType = eType; }

public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
    void OnCollision(OBJ_ID eID) override {};

    void Set_Item();
    void Key_Input();

private:
    vector<ITEM_TYPE> m_vecPlayeritems;
    vector<CObj*> m_vecWeaponLogo;
    vector<POINT> m_vecPoint;

    ITEM_TYPE m_eItemType;

    DWORD m_dwTime;

    CObj* m_pSelect;

    int m_iSelect;
};

