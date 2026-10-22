#pragma once
#include "CObj.h"
class CCoin : public CObj
{
public:
    CCoin();
    virtual ~CCoin();

public:
    void Initialize() override;
    int Update() override;
    void Late_Update() override;
    void Render(HDC hDC) override;
    void Release() override;
    void OnCollision(OBJ_ID eID) override;

private:
    FMOD_SOUND* m_CoinSound;
};

