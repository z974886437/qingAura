// 青楼


#include "UI/ViewModel/MVVM_LoadSlot.h"

// 初始化存档槽
void UMVVM_LoadSlot::InitializeSlot()
{
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();  // 获取存档槽的状态值，并将其作为索引传递给 WidgetSwitcher
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex); // 触发广播，更新界面上对应的 WidgetSwitcher 的索引
}

// 设置玩家名字
void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName,InPlayerName);// 使用 MVVM 模式的宏，设置 PlayerName 属性的值
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName,InMapName);// 使用 MVVM 模式的宏，设置 MapName 属性的值
}

void UMVVM_LoadSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel,InLevel);// 使用 MVVM 模式的宏，设置
}

// 设置存档槽名字
void UMVVM_LoadSlot::SetLoadSlotName(FString InLoadSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InLoadSlotName);// 使用 MVVM 模式的宏，设置 LoadSlotName 属性的值
}
