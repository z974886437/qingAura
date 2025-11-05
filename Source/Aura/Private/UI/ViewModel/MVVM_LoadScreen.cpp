// 青楼


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);// 创建第一个加载槽对象，并通过 LoadSlotViewModelClass 指定其类类型
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));// 设置该槽的名称为 "LoadSlot_0"，便于识别与保存
	LoadSlots.Add(0,LoadSlot_0);// 将加载槽添加到 LoadSlots 字典中，索引 0 作为键

	// 创建第二个加载槽对象（索引 1）
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlots.Add(1,LoadSlot_1);

	// 创建第三个加载槽对象（索引 2）
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlots.Add(2,LoadSlot_2);

	// 将总加载槽数量设置为当前 LoadSlots 的数量（通常为 3）
	SetNumLoadSlots(LoadSlots.Num());
}

// 根据索引获取对应的加载槽视图模型
UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);// 使用 FindChecked 查找，如果索引不存在会触发断言错误
}

// 当玩家在“新建存档”界面输入名字并确认按钮时调用
void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	// 获取当前正在运行的游戏模式对象（AAuraGameModeBase）
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	LoadSlots[Slot]->PlayerName = EnteredName;// 将玩家输入的名字赋值给对应存档槽

	AuraGameMode->SaveSlotData(LoadSlots[Slot],Slot);// 调用 GameMode 保存存档数据（传入该槽对象和槽索引）
	LoadSlots[Slot]->InitializeSlot();// 初始化该槽（例如更新 UI 状态、显示新存档信息等）
}

// 当点击“开始新游戏”按钮时调用
void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);// 通过广播事件让 UI 切换界面（例如从主菜单跳转到游戏中）
}

// 当选择某个已有存档槽时调用（当前为空，预留给后续逻辑）
void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	
}

// 设置加载槽数量（NumLoadSlots 是 MVVM 可绑定属性）
void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumLoadSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots,InNumLoadSlots);// UE_MVVM_SET_PROPERTY_VALUE 是 UE5 MVVM 提供的宏，用于安全更新绑定属性
}
