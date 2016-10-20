#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "UISleepWnd.h"
#include "../level.h"
#include "../actor.h"
#include "../ActorCondition.h"
#include "../hudmanager.h"
#include "../inventory.h"
#include "UIInventoryUtilities.h"
#include "../WeaponMagazined.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"
#include "../grenadelauncher.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"

CUICellItem* CUIInventoryWnd::CurrentItem()
{
	return m_pCurrentCellItem;
}

PIItem CUIInventoryWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?static_cast<PIItem>(m_pCurrentCellItem->m_pData) : NULL;
}


void CUIInventoryWnd::SetItemSelected (CUICellItem* itm)
{
	auto curr=CurrentItem();
	if (curr!=nullptr  && curr->m_selected)
		curr->m_selected=false;
	if (itm!=nullptr && !itm->m_selected)	
		itm->m_selected=true;
}

void CUIInventoryWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm) return;
	SetItemSelected(itm);
	m_pCurrentCellItem				= itm;
	UIItemInfo.InitItem			(CurrentIItem());
	
	m_pUIBagList->GetCellContainer()->clear_select_suitables();
	m_pUIBeltList->GetCellContainer()->clear_select_suitables();
	m_pUIKnifeList->GetCellContainer()->clear_select_suitables();
	m_pUIPistolList->GetCellContainer()->clear_select_suitables();
	m_pUIAutomaticList->GetCellContainer()->clear_select_suitables();
	m_pUIOutfitList->GetCellContainer()->clear_select_suitables();

	m_pUIBagList->select_suitables_by_item(CurrentIItem());
	m_pUIBeltList->select_suitables_by_item(CurrentIItem());
	m_pUIKnifeList->select_suitables_by_item(CurrentIItem());
	m_pUIPistolList->select_suitables_by_item(CurrentIItem());
	m_pUIAutomaticList->select_suitables_by_item(CurrentIItem());
	m_pUIOutfitList->select_suitables_by_item(CurrentIItem());
}

void CUIInventoryWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPropertiesBox &&	msg==PROPERTY_CLICKED)
	{
		ProcessPropertiesBoxClicked	();
	}else 
	if (UIExitButton == pWnd && BUTTON_CLICKED == msg)
	{
		GetHolder()->StartStopMenu			(this,true);
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIInventoryWnd::InitInventory_delayed()
{
	m_b_need_reinit = true;
}

void CUIInventoryWnd::InitInventory() 
{
	CInventoryOwner *pInvOwner	= smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!pInvOwner)				return;

	m_pInv						= &pInvOwner->inventory();

	UIPropertiesBox.Hide		();
	ClearAllLists				();
	m_pMouseCapturer			= nullptr;
	SetCurrentItem				(nullptr);

	//Slots
	PIItem _itm							= m_pInv->m_slots[KNIFE_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIKnifeList->SetItem		(itm);
	}

	_itm							= m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIPistolList->SetItem		(itm);
	}


	_itm								= m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIAutomaticList->SetItem		(itm);
	}

	PIItem _outfit						= m_pInv->m_slots[OUTFIT_SLOT].m_pIItem;
	CUICellItem* outfit					= (_outfit)?create_cell_item(_outfit):NULL;
	m_pUIOutfitList->SetItem			(outfit);

	TIItemContainer::iterator it, it_e;
	for(it=m_pInv->m_belt.begin(),it_e=m_pInv->m_belt.end(); it!=it_e; ++it) 
	{
		CUICellItem* itm			= create_cell_item(*it);
		m_pUIBeltList->SetItem		(itm);
	}
	
	ruck_list		= m_pInv->m_ruck;
	std::sort		(ruck_list.begin(),ruck_list.end(),InventoryUtilities::GreaterRoomInRuck);

	int i=1;
	for(it=ruck_list.begin(),it_e=ruck_list.end(); it!=it_e; ++it,++i) 
	{
		CUICellItem* itm			= create_cell_item(*it);
		m_pUIBagList->SetItem		(itm);
	}
	//fake
	_itm								= m_pInv->m_slots[GRENADE_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIBagList->SetItem			(itm);
	}

	InventoryUtilities::UpdateWeight					(UIBagWnd, true);

	m_b_need_reinit					= false;
}  

void CUIInventoryWnd::DropCurrentItem(bool b_all)
{

	CActor *pActor			= smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)				return;

	if(!b_all && CurrentIItem() && !CurrentIItem()->IsQuestItem())
	{
		SendEvent_Item_Drop		(CurrentIItem());
		SetCurrentItem			(NULL);
		InventoryUtilities::UpdateWeight			(UIBagWnd, true);
		return;
	}

	if(b_all && CurrentIItem() && !CurrentIItem()->IsQuestItem())
	{
		u32 cnt = CurrentItem()->ChildsCount();

		for(u32 i=0; i<cnt; ++i){
			CUICellItem*	itm				= CurrentItem()->PopChild();
			PIItem			iitm			= (PIItem)itm->m_pData;
			SendEvent_Item_Drop				(iitm);
		}

		SendEvent_Item_Drop					(CurrentIItem());
		SetCurrentItem						(NULL);
		InventoryUtilities::UpdateWeight	(UIBagWnd, true);
		return;
	}
}

//------------------------------------------

bool CUIInventoryWnd::ToSlot(CUICellItem* itm, bool force_place)
{
	CUIDragDropListEx*	old_owner			= itm->OwnerList();
	PIItem	iitem							= (PIItem)itm->m_pData;
	u32 _slot								= iitem->GetSlot();

	if(GetInventory()->CanPutInSlot(iitem)){
		CUIDragDropListEx* new_owner		= GetSlotList(_slot);
		
		if(_slot==GRENADE_SLOT && !new_owner )return true; //fake, sorry (((

		bool result							= GetInventory()->Slot(iitem);
		VERIFY								(result);

		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );

		new_owner->SetItem					(i);
	
		SendEvent_Item2Slot					(iitem);

		SendEvent_ActivateSlot				(iitem);
		InventoryUtilities::UpdateWeight					(UIBagWnd, true);
		return								true;
	}else
	{ // in case slot is busy
		if(!force_place ||  _slot==NO_ACTIVE_SLOT || GetInventory()->m_slots[_slot].m_bPersistent) return false;

		PIItem	_iitem						= GetInventory()->m_slots[_slot].m_pIItem;
		CUIDragDropListEx* slot_list		= GetSlotList(_slot);
		VERIFY								(slot_list->ItemsCount()==1);

		CUICellItem* slot_cell				= slot_list->GetItemIdx(0);
		VERIFY								(slot_cell && ((PIItem)slot_cell->m_pData)==_iitem);

		bool result							= ToBag(slot_cell, false);
		VERIFY								(result);
		InventoryUtilities::UpdateWeight					(UIBagWnd, true);
		return ToSlot						(itm, false);
	}
}

bool CUIInventoryWnd::ToBag(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(GetInventory()->CanPutInRuck(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pUIBagList);
		}else
				new_owner					= m_pUIBagList;


		bool result							= GetInventory()->Ruck(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		SendEvent_Item2Ruck					(iitem);
		InventoryUtilities::UpdateWeight					(UIBagWnd, true);
		return true;
	}
	return false;
}

bool CUIInventoryWnd::ToBelt(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(GetInventory()->CanPutInBelt(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pUIBeltList);
		}else
				new_owner					= m_pUIBeltList;

		bool result							= GetInventory()->Belt(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
	//.	UIBeltList.RearrangeItems();
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		SendEvent_Item2Belt					(iitem);
		return								true;
	}
	return									false;
}

void CUIInventoryWnd::AddItemToBag(PIItem pItem)
{
	CUICellItem* itm						= create_cell_item(pItem);
	m_pUIBagList->SetItem					(itm);
}

bool CUIInventoryWnd::OnItemStartDrag(CUICellItem* itm)
{
	return false; //default behaviour
}

bool CUIInventoryWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem		(itm);
	return				false;
}

bool CUIInventoryWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	if (!old_owner || !new_owner)
		return false;

	EListType t_new		= GetType(new_owner);
	EListType t_old		= GetType(old_owner);
	if (t_new == iwBag && t_old == iwBag)
	{
		CUICellItem* focusedCellItem=new_owner->GetCellContainer()->GetFocuseditem();
		if (!focusedCellItem)
			return false;

		PIItem draggedItem=static_cast<PIItem>(itm->m_pData);
		PIItem focusedItem=static_cast<PIItem>(focusedCellItem->m_pData);
		if (!draggedItem || !focusedItem)
			return false;
		//����� ��� ������� ������� �� ������
		CWeapon*			weapon				= smart_cast<CWeapon*>			(focusedItem);
		if (weapon)
		{
			CScope*				pScope				= smart_cast<CScope*>			(draggedItem);
			CSilencer*			pSilencer			= smart_cast<CSilencer*>		(draggedItem);
			CGrenadeLauncher*	pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(draggedItem);
			CWeaponAmmo*		pAmmo				= smart_cast<CWeaponAmmo*>		(draggedItem);
			m_pUIBagList->m_i_scroll_pos=m_pUIBagList->ScrollPos();
			if (pScope || pSilencer || pGrenadeLauncher)
			{
				SetCurrentItem(itm);
				AttachAddon(weapon);
			} 
			else if (pAmmo!=nullptr && weapon->CanLoadAmmo(pAmmo))
				weapon->LoadAmmo(pAmmo);
		}
		return true;
	}
	else if (t_new == t_old)
		return true;

	switch(t_new){
		case iwSlot:{
			if(GetSlotList(CurrentIItem()->GetSlot())==new_owner)
				ToSlot	(itm, true);
			CUICellItem* focusedCellItem=new_owner->GetCellContainer()->GetFocuseditem();
			if (!focusedCellItem)
				break;
			PIItem draggedItem=static_cast<PIItem>(itm->m_pData);
			PIItem focusedItem=static_cast<PIItem>(focusedCellItem->m_pData);
			if (!draggedItem || !focusedItem)
				break;
			CWeapon*			weapon				= smart_cast<CWeapon*>			(focusedItem);
			if (weapon)
			{
				CWeaponAmmo*		pAmmo				= smart_cast<CWeaponAmmo*>		(draggedItem);
				if (!pAmmo)
					break;
				if (pAmmo!=nullptr && weapon->CanLoadAmmo(pAmmo))
					weapon->LoadAmmo(pAmmo);
			}
		}break;
		case iwBag:{
			ToBag	(itm, true);
		}break;
		case iwBelt:{
			ToBelt	(itm, true);
		}break;
	default: break;
	};

	DropItem				(CurrentIItem(), new_owner);

	return true;
}

bool CUIInventoryWnd::OnItemDbClick(CUICellItem* itm)
{
	if(TryUseItem((PIItem)itm->m_pData))		
		return true;

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	EListType t_old						= GetType(old_owner);
	switch(t_old){
		case iwSlot:{
			ToBag	(itm, false);
		}break;

		case iwBag:{
			if(!ToSlot(itm, false)){
				if( !ToBelt(itm, false) )
					ToSlot	(itm, true);
			}
			CWeaponAmmo* ammo=static_cast<CWeaponAmmo*>(itm->m_pData);
			if (!ammo)
				break;
			auto pistol=m_pInv->m_slots[PISTOL_SLOT].m_pIItem;
			auto rifle=m_pInv->m_slots[RIFLE_SLOT].m_pIItem;
			if (pistol != NULL && pistol->CanLoadAmmo(ammo))
			{
				CWeaponMagazined* weapon=smart_cast<CWeaponMagazined*>(pistol);
				weapon->LoadAmmo(ammo);
				break;
			}
			if (rifle != NULL && rifle->CanLoadAmmo(ammo))
			{
				CWeaponMagazined* weapon=smart_cast<CWeaponMagazined*>(rifle);
				weapon->LoadAmmo(ammo);
				break;
			}
		}break;

		case iwBelt:{
			ToBag	(itm, false);
		}break;
	};

	return true;
}

bool CUIInventoryWnd::OnItemFocusReceive(CUICellItem* itm)
{
	if (itm)
		itm->m_focused=true;
	return						false;
}

bool CUIInventoryWnd::OnItemFocusLost(CUICellItem* itm)
{
	if (itm)
		itm->m_focused=false;
	return						false;
}


bool CUIInventoryWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	ActivatePropertiesBox		();
	return						false;
}

CUIDragDropListEx* CUIInventoryWnd::GetSlotList(u32 slot_idx)
{
	if(slot_idx == NO_ACTIVE_SLOT || GetInventory()->m_slots[slot_idx].m_bPersistent)	return NULL;
	switch (slot_idx)
	{
		case KNIFE_SLOT:
			return m_pUIKnifeList;
			break;

		case PISTOL_SLOT:
			return m_pUIPistolList;
			break;

		case RIFLE_SLOT:
			return m_pUIAutomaticList;
			break;

		case OUTFIT_SLOT:
			return m_pUIOutfitList;
			break;

	};
	return NULL;
}



void CUIInventoryWnd::ClearAllLists()
{
	m_pUIBagList->ClearAll					(true);
	m_pUIBeltList->ClearAll					(true);
	m_pUIOutfitList->ClearAll				(true);
	m_pUIKnifeList->ClearAll				(true);
	m_pUIPistolList->ClearAll				(true);
	m_pUIAutomaticList->ClearAll			(true);
}