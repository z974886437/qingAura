// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FName PlayerStartTag = FName();//播放器开始标签

	UPROPERTY()
	FString LoadSlotName = FString();//加载槽名称

	UPROPERTY()
	int32 LoadSlotIndex = 0;//装载槽索引
};
