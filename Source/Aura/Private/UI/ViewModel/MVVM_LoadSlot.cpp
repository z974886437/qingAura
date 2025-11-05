// 青楼


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	// TODO: Check slot status based on laded data 根据加载的数据检查插槽状态
	SetWidgetSwitcherIndex.Broadcast(1);
}
