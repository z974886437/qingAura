// 青楼


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
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
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;// 保存玩家出生点标签，用于下次加载时确定玩家生成位置

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

// 从当前正在使用的游戏实例中检索游戏内存档数据
ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());// 获取当前游戏实例（GameInstance），用于在关卡切换之间保存全局数据

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;// 从 GameInstance 中取出当前使用的存档槽名称（通常在加载界面选择时设置）
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex; // 从 GameInstance 中取出当前使用的存档槽索引（用于区分不同槽，例如 0、1、2）

	return GetSaveSlotData(InGameLoadSlotName,InGameLoadSlotIndex); // 调用内部函数 GetSaveSlotData，从对应的存档槽读取存档数据并返回
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());// 获取当前游戏实例（GameInstance），用于在关卡切换之间保存全局数据

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;// 从 GameInstance 中取出当前使用的存档槽名称（通常在加载界面选择时设置）
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex; // 从 GameInstance 中取出当前使用的存档槽索引（用于区分不同槽，例如 0、1、2）
	AuraGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;// 将玩家当前的出生点（PlayerStartTag）写回 GameInstance

	UGameplayStatics::SaveGameToSlot(SaveObject,InGameLoadSlotName,InGameLoadSlotIndex);// 使用引擎提供的静态函数将存档对象（SaveObject）保存到对应的槽中
}

// 保存当前世界（World）的所有可保存 Actor 状态
void AAuraGameModeBase::SaveWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();// 获取当前世界的地图名称（包含前缀）
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);// 移除 StreamingLevelsPrefix 前缀，得到实际地图名

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());// 获取游戏实例并转换为自定义的 AuraGI，用于访问当前存档槽
	check(AuraGI); // 确保 AuraGI 不为空，否则直接崩溃，避免空指针继续执行

	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName,AuraGI->LoadSlotIndex))// 获取当前存档槽中的 SaveGame 数据（如果不存在会创建）
	{
		if (!SaveGame->HasMap(WorldName))  // 检查当前地图是否已经有保存记录，如果没有则创建一个新的 FSavedMap
		{
			FSavedMap NewSavedMap;// 创建空的地图保存结构
			NewSavedMap.MapAssetName = WorldName; // 记录地图资源名（作为唯一 ID）
			SaveGame->SavedMaps.Add(NewSavedMap); // 将该新地图加入存档的 SavedMaps 数组
		}

		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName); // 获取该地图对应的保存数据（如果之前没有，则刚刚已添加）
		SavedMap.SavedActors.Empty(); // 清空该地图之前保存的 Actor 数据，准备重新写入

		for (FActorIterator It(World);It;++It) // 遍历世界中的所有 Actor（使用引擎提供的迭代器）
		{
			AActor* Actor = *It;// 当前遍历到的 Actor

			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue; // 跳过无效 Actor 或未实现 SaveInterface 的 Actor

			FSavedActor SavedActor; // 创建一个 FSavedActor 用于存储该 Actor 的保存数据
			SavedActor.ActorName = Actor->GetFName();// 保存 Actor 名字作为唯一标识
			SavedActor.Transform = Actor->GetTransform(); // 保存 Actor 当前 transform（位置、旋转、缩放）

			FMemoryWriter MemoryWriter(SavedActor.Bytes); // 用 MemoryWriter 写入到 SavedActor.Bytes（字节数组）

			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter,true); // 将对象序列化为字符串/名称形式，以便存档系统使用
			Archive.ArIsSaveGame = true; // 标记为存档操作，让对象只序列化带 SaveGame 标记的变量

			Actor->Serialize(Archive);// 调用 Actor 的 Serialize，把变量写入字节流

			SavedMap.SavedActors.AddUnique(SavedActor); // 将这个 Actor 的保存结果加入 SavedActors，避免重复项
		}

		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)  // 遍历所有已保存地图，为当前地图替换为最新版本的 SavedMap
		{
			// 找到目标地图后，用新保存的数据覆盖旧数据
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame,AuraGI->LoadSlotName,AuraGI->LoadSlotIndex); // 将整个存档写入硬盘（最终保存操作）
	}
}

// 从存档中加载当前世界（World）的所有可保存 Actor 状态
void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();// 获取当前世界的地图名称（包含前缀）
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);// 移除 StreamingLevelsPrefix 前缀，得到实际地图名

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());// 获取游戏实例并转换为自定义的 AuraGI，用于访问当前存档槽
	check(AuraGI); // 确保 AuraGI 不为空，否则直接崩溃，避免空指针继续执行

	// 判断当前存档槽是否存在有效存档文件
	if (UGameplayStatics::DoesSaveGameExist(AuraGI->LoadSlotName,AuraGI->LoadSlotIndex))
	{
		// 尝试从磁盘加载存档文件，并转换为自定义的 ULoadScreenSaveGame 类型
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGI->LoadSlotName,AuraGI->LoadSlotIndex));

		// 如果加载失败，打印日志并退出函数
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura,Error,TEXT("Failed to load slot"));
			return;
		}
		
		for (FActorIterator It(World);It;++It) // 遍历世界中的所有 Actor（使用引擎提供的迭代器）
		{
			AActor* Actor = *It;// 当前遍历到的 Actor

			if (!Actor->Implements<USaveInterface>()) continue;// 只加载实现了 SaveInterface 的 Actor（不具备保存功能的 Actor 被忽略）

			for (FSavedActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors) // 遍历对应地图下的所有已保存的 Actor 数据
			{
				if (SavedActor.ActorName == Actor->GetFName())  // 判断当前 Actor 是否与存档中的某个 Actor 名称匹配（通过名字唯一识别）
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor)) // 询问 Actor 是否允许加载 Transform（位置/旋转/缩放）
					{
						Actor->SetActorTransform(SavedActor.Transform); // 根据存档中的 Transform 恢复 Actor 的位置与旋转
					}

					FMemoryReader MemoryReader(SavedActor.Bytes); // 创建 MemoryReader，从 SavedActor.Bytes 读取序列化数据

					FObjectAndNameAsStringProxyArchive Archive(MemoryReader,true); // 创建反序列化代理，支持名称与字符串序列化
					Archive.ArIsSaveGame = true; // 标记为 SaveGame 反序列化，确保只处理带 SaveGame 标记的变量
					Actor->Serialize(Archive);  // 将存档中的数据写回 Actor 的成员变量

					ISaveInterface::Execute_LoadActor(Actor);  // 调用蓝图或 C++ 实现的 LoadActor 钩子，用于做恢复后的额外逻辑
					
				}
			}
		}
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->GetLoadSlotName();// 获取当前存档槽的名称
	const int32 SlotIndex = Slot->SlotIndex;// 获取当前存档槽的索引
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot,Maps.FindChecked(Slot->GetMapName())); // 根据存档槽的地图名称，从 Maps 字典中查找对应的地图对象，并加载该地图
}

// 选择玩家出生点的实现函数（当玩家进入游戏时调用）
AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());// 获取当前游戏实例（GameInstance），用于在关卡切换之间保存全局数据
	
	TArray<AActor*> Actors;// 定义一个 Actor 数组，用于存放所有 APlayerStart 类的对象

	// 获取当前关卡中所有 APlayerStart 类型的 Actor，并放入数组 Actors 中
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),Actors);
	
	if (Actors.Num() > 0)// 检查是否存在至少一个玩家出生点
	{
		AActor* SelectedActor = Actors[0];// 默认选中第一个出生点，防止没有匹配标签时返回空指针
		
		for (AActor* Actor : Actors)// 遍历所有找到的出生点
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))// 尝试将 Actor 转换为 APlayerStart 类型
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)// 判断这个出生点的标签是否等于 "TheTag"
				{
					SelectedActor = PlayerStart;// 如果匹配，则选择这个出生点
					break;// 找到目标出生点后直接跳出循环
				}
			}
		}
		return SelectedActor;// 返回选定的出生点
	}
	return nullptr;// 若没有任何出生点，返回空指针（不建议出现）
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName,DefaultMap);// 将默认地图名称和对应的地图对象添加到 Maps 字典中
}
