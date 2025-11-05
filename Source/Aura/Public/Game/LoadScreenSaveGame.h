// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SlotName = FString();//插槽名字

	UPROPERTY()
	int32 SlotIndex = 0;//插槽索引

	UPROPERTY()
	FString PlayerName = FString("Default Name");//玩家姓名
};
