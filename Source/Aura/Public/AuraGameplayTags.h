// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  AruaGameplayTags
 *
 *  Singleton containing native Gameplay Tags  包含原生游戏标签的包含原生游戏标签的
 */

struct FAuraGameplayTags
{
public:

	static const FAuraGameplayTags& Get() {return GameplayTags;}
	static void InitializeNativeGameplayTags();//初始化本机游戏标签

protected:
	
private:
	static FAuraGameplayTags GameplayTags;//游戏标签

	
};
