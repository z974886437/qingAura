// 青楼


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);//GEngine->资产管理器
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();//初始化本机游戏标签

	//This is required to use target Data!这是使用目标数据所必需的！
	UAbilitySystemGlobals::Get().InitGlobalData();// 初始化 Gameplay Ability System 的全局数据（只需调用一次，全局生效）
}


