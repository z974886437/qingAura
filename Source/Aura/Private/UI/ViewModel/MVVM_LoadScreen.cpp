// 青楼


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);// 创建第一个加载槽对象，并通过 LoadSlotViewModelClass 指定其类类型
	LoadSlots.Add(0,LoadSlot_0);// 将加载槽添加到 LoadSlots 字典中，索引 0 作为键
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlots.Add(1,LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlots.Add(2,LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);// 根据传入的索引查找并返回对应的加载槽视图模型
}
