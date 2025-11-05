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
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(),SlotIndex);
	}
	
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);// 创建一个新的 SaveGame 对象实例（类型为指定的 LoadScreenSaveGameClass）
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);// 将通用 USaveGame 对象转换为我们自定义的 ULoadScreenSaveGame 类
	LoadScreenSaveGame->PlayerName = LoadSlot->PlayerName;// 将玩家名字从 UI 加载槽对象中复制到 SaveGame 对象中

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->GetLoadSlotName(),SlotIndex);// 调用 UE 内置函数，将 SaveGame 对象写入磁盘存档文件中
}
