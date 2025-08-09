// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& /* Asset Tags */);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	//能力演员信息集
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;//效果资产标签

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);//添加角色能力

	void AbilityInputTagHeld(const FGameplayTag& InputTag);//持续能力输入标签
	void AbilityInputTagReleased(const FGameplayTag& InputTag);//释放能力输入标签

protected:

	UFUNCTION(Client,Reliable)// 用于标识该函数是一个客户端 RPC（远程过程调用）函数，且调用是可靠的。
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);//客户端应用效果
};
