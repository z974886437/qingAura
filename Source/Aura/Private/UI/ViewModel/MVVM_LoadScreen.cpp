// 青楼


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);// 创建第一个加载槽对象，并通过 LoadSlotViewModelClass 指定其类类型
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));// 设置该槽的名称为 "LoadSlot_0"，便于识别与保存
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0,LoadSlot_0);// 将加载槽添加到 LoadSlots 字典中，索引 0 作为键

	// 创建第二个加载槽对象（索引 1）
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1,LoadSlot_1);

	// 创建第三个加载槽对象（索引 2）
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex = 2;
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
	if (!IsValid(AuraGameMode))
	{
		GEngine->AddOnScreenDebugMessage(1,15.f,FColor::Magenta,FString("Please switch to Single Player"));
		return;
	}

	LoadSlots[Slot]->SetMapName(AuraGameMode->DefaultMapName);// 将新存档的地图名设为 GameMode 中定义的默认地图名
	LoadSlots[Slot]->SetPlayerName(EnteredName);// 将玩家输入的名字赋值给对应存档槽
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SlotStatus = Taken;// 将指定存档槽的状态设置为 "已占用"（Taken）
	LoadSlots[Slot]->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;// 将 GameMode 中的默认出生点标签赋值给该存档槽（用于后续加载定位玩家出生点）

	AuraGameMode->SaveSlotData(LoadSlots[Slot],Slot);// 调用 GameMode 保存存档数据（传入该槽对象和槽索引）
	LoadSlots[Slot]->InitializeSlot();// 初始化该槽（例如更新 UI 状态、显示新存档信息等）

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());// 获取当前游戏实例（GameInstance），用于在关卡切换之间保存全局数据
	AuraGameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();// 将当前选定的存档槽名记录到 GameInstance 中，方便后续加载
	AuraGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;// 同时保存槽的索引编号，用于识别是哪一个存档槽
	AuraGameInstance->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;// 记录玩家出生点标签（例如用于选择出生位置）
}

// 当点击“开始新游戏”按钮时调用
void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);// 通过广播事件让 UI 切换界面（例如从主菜单跳转到游戏中）
}

// 当选择某个已有存档槽时调用（当前为空，预留给后续逻辑）
void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TTuple<int32,UMVVM_LoadSlot*> LoadSlot : LoadSlots)// 遍历加载槽（LoadSlots）中的每一项，Key 是槽的索引，Value 是加载槽对象
	{
		if (LoadSlot.Key == Slot)// 如果当前槽的索引与玩家选择的槽索引相同，则禁用该槽的选择按钮
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);// 否则，启用其他槽的选择按钮
		}
	}
	SelectedSlot = LoadSlots[Slot]; // 设置当前选中的槽
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot)) // 检查 SelectedSlot 是否有效，确保当前有选中的槽
	{
		AAuraGameModeBase::DeleteSlot(SelectedSlot->GetLoadSlotName(),SelectedSlot->SlotIndex);// 调用游戏模式的 DeleteSlot 函数，传入当前选中槽的名称和索引，执行删除操作
		SelectedSlot->SlotStatus = Vacant;// 将当前槽的状态设置为 Vacant，表示该槽已空
		SelectedSlot->InitializeSlot();// 重新初始化槽，恢复到初始状态
		SelectedSlot->EnableSelectSlotButton.Broadcast(true); // 启用该槽的选择按钮
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	// 获取当前正在运行的 GameMode，并转换为自定义的 AAuraGameModeBase 类型
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());// 获取当前游戏实例，用于跨关卡保存玩家的全局数据（例如出生点、存档名等）
	
	// 将选中的存档槽的出生点标签同步到 GameInstance 中
	// 这样在切换关卡后，GameMode 可以读取这个标签并让玩家在正确的出生点生成
	AuraGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();// 将当前选中的存档名称保存到 GameInstance 中
	AuraGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;// 保存当前选中的存档槽索引（一般用于区分不同的存档位）
	
	if (IsValid(SelectedSlot))// 如果当前有选中的存档槽，且该槽有效
	{
		AuraGameMode->TravelToMap(SelectedSlot); // 调用游戏模式中的 TravelToMap 函数，传入选中的存档槽，执行场景切换或加载操作
	}
}

void UMVVM_LoadScreen::LoadData()
{
	// 获取当前游戏模式的引用，并转换为 AAuraGameModeBase 类型
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AuraGameMode)) return;
	
	for (const TTuple<int32,UMVVM_LoadSlot*> LoadSlot : LoadSlots)// 遍历加载槽（LoadSlots）中的每一项，Key 是槽的索引，Value 是加载槽对象
	{
		// 获取当前加载槽的保存数据，通过游戏模式中的 GetSaveSlotData 函数获取
		ULoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(),LoadSlot.Key);

		const FString PlayerName = SaveObject->PlayerName;// 从保存对象中提取玩家名字
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;// 获取保存槽的状态（例如：空闲、已加载、已完成等）
		
		LoadSlot.Value->SlotStatus = SaveSlotStatus;// 设置当前加载槽的状态
		LoadSlot.Value->SetPlayerName(PlayerName);// 设置加载槽显示的玩家名字
		LoadSlot.Value->InitializeSlot();// 初始化加载槽（根据保存的数据来配置）
		
		LoadSlot.Value->SetMapName(SaveObject->MapName);// 设置加载槽的地图名称
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;// 同步出生点标签，用于后续进入游戏时确定玩家生成位置
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}

// 设置加载槽数量（NumLoadSlots 是 MVVM 可绑定属性）
void UMVVM_LoadScreen::SetNumLoadSlots(int32 InNumLoadSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumLoadSlots,InNumLoadSlots);// UE_MVVM_SET_PROPERTY_VALUE 是 UE5 MVVM 提供的宏，用于安全更新绑定属性
}
