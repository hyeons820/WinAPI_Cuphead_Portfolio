#include "pch.h"
#include "CStore.h"
#include "CBmpMgr.h"
#include "CObjMgr.h"
#include "CSoundMgr.h"
#include "CPig.h"
#include "CAbstractFactory.h"
#include "CUIMgr.h"
#include "CShopCoin.h"
#include "CItem.h"
#include "CDataMgr.h"
#include "CKeyMgr.h"
#include "CSceneMgr.h"
#include "CLobber.h"
#include "CNumber.h"
#include "Cx.h"
#include "CEffectMgr.h"

CStore::CStore() : m_iSelect(0), m_bUIActive(false), m_pPig(nullptr)
{

}

CStore::~CStore()
{
	Release();
}

void CStore::Initialize()
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/Store/Store.bmp", L"Store");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Images/Stage/Store/Item1.bmp", L"Item1");

	m_pPig = CAbstractFactory<CPig>::Create_Block(WINCX * 0.5f, WINCY * 0.42f);
	CObjMgr::Get_Instance()->Add_Object(BLOCK, m_pPig);

	m_vecItems = CDataMgr::Get_Instance()->Get_ShopItem();

	CUIMgr::Get_Instance()->Add_UI(SHOP_COIN, CAbstractFactory<CShopCoin>::Create_Obj());
	CUIMgr::Get_Instance()->Add_UI(SHOP_COIN, CAbstractFactory<CNumber>::Create_Obj());
	CUIMgr::Get_Instance()->Add_UI(SHOP_COIN, CAbstractFactory<Cx>::Create_Obj());

	CSoundMgr::Get_Instance()->PlayBGM(L"ShopBgm.mp3", g_fVolume);

	m_StoreSound[BUY] = CSoundMgr::Get_Instance()->Get_MapSound(L"Purchase.mp3");
	m_StoreSound[WELCOME] = CSoundMgr::Get_Instance()->Get_MapSound(L"store_pig_welcome.wav");
	m_StoreSound[GOOD_BYE] = CSoundMgr::Get_Instance()->Get_MapSound(L"store_pig_goodbye.wav");

	FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_StoreSound[WELCOME], nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_STORE));
}

void CStore::Update()
{
	if (m_vecItems[m_iSelect] != nullptr)
	{
		CUIMgr::Get_Instance()->Delete_UI(static_cast<CItem*>(m_vecItems[m_iSelect])->Get_UI());
		m_bUIActive = false;
	}

	Key_Input();

	CObjMgr::Get_Instance()->Update();

	for (auto& vec : m_vecItems)
	{
		if (vec == nullptr)
			continue;

		vec->Update();
	}

	CUIMgr::Get_Instance()->Update();
	CEffectMgr::Get_Instance()->Update();

}

void CStore::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();

	CUIMgr::Get_Instance()->Late_Update();

	CEffectMgr::Get_Instance()->Late_Update();

	if (m_vecItems[m_iSelect] == nullptr)
		return;
	m_vecItems[m_iSelect]->Late_Update();

	if (m_bUIActive == false)
	{
		static_cast<CItem*>(m_vecItems[m_iSelect])->Set_InfoUI(static_cast<CItem*>(m_vecItems[m_iSelect])->Get_ItemType());
		m_bUIActive = true;
	}
}

void CStore::Render(HDC hDC)
{
	HDC hShopDC = CBmpMgr::Get_Instance()->Find_Image(L"Store");

	BitBlt(hDC, 0, 0, WINCX, WINCY, hShopDC, 0, 0, SRCCOPY);

	CObjMgr::Get_Instance()->Render(hDC);

	for (auto& vec : m_vecItems)
	{
		if (vec == nullptr)
			continue;

		vec->Render(hDC);
	}

	CUIMgr::Get_Instance()->Render(hDC);
	CEffectMgr::Get_Instance()->Render(hDC);

	
	//POINT	ptMouse{};
	//
	//GetCursorPos(&ptMouse);
	//ScreenToClient(g_hWnd, &ptMouse);
	//
	//TCHAR szMouse[32] = L"";
	//swprintf_s(szMouse, L"\t : %f \ %f", (float)ptMouse.x, (float)ptMouse.y);
	//RECT rc = { 0, 300, 300, 600 };
	//DrawText(hDC, szMouse, -1, &rc, DT_CENTER | DT_SINGLELINE);
}

void CStore::Release()
{
	CDataMgr::Get_Instance()->Set_ShopItem(m_vecItems);

	for (auto& vec : m_vecItems)
		Safe_Delete<CObj*>(vec);

	m_vecItems.clear();

	m_pPig = nullptr;

	CSoundMgr::Get_Instance()->StopAll();
	CUIMgr::Destroy_Instance();
	CObjMgr::Destroy_Instance();
}

void CStore::Key_Input()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RIGHT))
	{
		if (m_vecItems[m_iSelect] != nullptr)
		{
			static_cast<CItem*>(m_vecItems[m_iSelect])->Reset_Anim();
			m_vecItems[m_iSelect]->Set_PosY(310.f);
		}
		
		++m_iSelect;
		
		if (m_iSelect >= m_vecItems.size())
			m_iSelect = m_vecItems.size() - 1;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_LEFT))
	{
		if (m_vecItems[m_iSelect] != nullptr)
		{
			static_cast<CItem*>(m_vecItems[m_iSelect])->Reset_Anim();
			m_vecItems[m_iSelect]->Set_PosY(310.f);
		}

		--m_iSelect;

		if (m_iSelect < 0)
			m_iSelect = 0;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN))
	{
		if (m_vecItems[m_iSelect] != nullptr)
		{
			int iPrice = static_cast<CItem*>(m_vecItems[m_iSelect])->Get_Price();
			int iCoin = CDataMgr::Get_Instance()->Get_Coin();

			if (iPrice <= iCoin)
			{
				CSoundMgr::Get_Instance()->StopSound(SOUND_EFFECT);

				FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_StoreSound[BUY], nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_EFFECT));

				CDataMgr::Get_Instance()->Set_Coin(-iPrice);

				CUIMgr::Get_Instance()->Delete_UI(static_cast<CItem*>(m_vecItems[m_iSelect])->Get_UI());
				m_bUIActive = false;
				CDataMgr::Get_Instance()->Set_PlayerItem(m_vecItems[m_iSelect]);

				vector<CObj*>::iterator iter = m_vecItems.begin();

				iter += m_iSelect;

				Safe_Delete<CObj*>(*iter);
				iter = m_vecItems.erase(iter);

				--m_iSelect;

				static_cast<CPig*>(m_pPig)->Set_State(CPig::BUY);

				
			}
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE))
	{
		CSoundMgr::Get_Instance()->StopSound(SOUND_STORE);
		FMOD_System_PlaySound(CSoundMgr::Get_Instance()->Get_System(), m_StoreSound[GOOD_BYE], nullptr, FALSE, CSoundMgr::Get_Instance()->Get_Channel(SOUND_STORE));
		
		static_cast<CPig*>(m_pPig)->Set_State(CPig::LEAVE);
	}

}