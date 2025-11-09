// 青楼


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

// 保存指定加载槽（LoadSlot）的存档数据
void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	// 如果指定的存档槽已存在对应存档文件，则先删除旧文件，避免保存冲突
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(),SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(),SlotIndex);// 删除已存在的存档文件，以避免覆盖问题
	}
	
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);// 创建一个新的 SaveGame 对象实例（类型为指定的 LoadScreenSaveGameClass）
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);// 将通用 USaveGame 对象转换为我们自定义的 ULoadScreenSaveGame 类
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();// 将玩家名字从 UI 加载槽对象中复制到 SaveGame 对象中
	LoadScreenSaveGame->SaveSlotStatus = Taken;// 将存档槽的状态设置为 "已占用"（Taken）
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();// 将加载槽的地图名称保存到存档对象

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->GetLoadSlotName(),SlotIndex);// 调用 UE 内置函数，将 SaveGame 对象写入磁盘存档文件中
}

// 获取或创建指定存档槽位的数据对象
ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;// 定义一个指针，用于存储加载或创建的存档对象
	if (UGameplayStatics::DoesSaveGameExist(SlotName,SlotIndex))// 检查指定名称和索引的存档是否存在
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName,SlotIndex);// 如果存在，则从该槽位加载存档对象
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);// 如果不存在，则创建一个新的存档对象实例（使用预定义的存档类）
	}
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);// 将通用的 SaveGame 对象转换为具体类型 ULoadScreenSaveGame，以便访问自定义数据
	return LoadScreenSaveGame;// 返回加载或新建的存档对象
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	// 如果指定的存档槽已存在对应存档文件，则先删除旧文件，避免保存冲突
	if (UGameplayStatics::DoesSaveGameExist(SlotName,SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName,SlotIndex);// 删除已存在的存档文件，以避免覆盖问题
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->GetLoadSlotName();// 获取当前存档槽的名称
	const int32 SlotIndex = Slot->SlotIndex;// 获取当前存档槽的索引
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot,Maps.FindChecked(Slot->GetMapName())); // 根据存档槽的地图名称，从 Maps 字典中查找对应的地图对象，并加载该地图
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName,DefaultMap);// 将默认地图名称和对应的地图对象添加到 Maps 字典中
}
