// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& /* Asset Tags */);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/,const FGameplayTag& /*StatusTag*/,int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped,const FGameplayTag& /*AbilityTag*/,const FGameplayTag& /*Status*/,const FGameplayTag& /*Slot*/,const FGameplayTag& /*PrevSlot*/);//装备能力

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
	FAbilitiesGiven AbilitiesGivenDelegate;//能力赋予委托
	FAbilityStatusChanged AbilityStatusChanged;//技能状态已更改
	FAbilityEquipped AbilityEquipped;//装备能力

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);//添加角色能力。
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);//添加角色被动能力
	bool bStartupAbilitiesGiven = false;//给定的启动能力

	void AbilityInputTagHeld(const FGameplayTag& InputTag);//持续能力输入标签
	void AbilityInputTagReleased(const FGameplayTag& InputTag);//释放能力输入标签
	void ForEachAbility(const FForEachAbility& Delegate);//对于每个能力

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);//从规范中获取能力标签
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);//从规范中获取输入标签
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);//从规范中获取状态
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);//从技能标签获取状态
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);//从技能标签获取输入标签

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);//从能力标签获取规格
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);//升级属性

	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);//服务器升级属性
	
	void UpdateAbilityStatuses(int32 Level);//更新能力状态

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);//服务器花费法术点

	UFUNCTION(Server,Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Slot);//服务器装备能力

	void ClientEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Status, const FGameplayTag& Slot,const FGameplayTag& PreviousSlot);//客户端装备能力

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag,FString& OutDescription,FString& OutNextLevelDescription);//按能力标签获取描述

	void ClearSlot(FGameplayAbilitySpec* Spec);//清除插槽
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);//插槽的清除能力
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec,const FGameplayTag& Slot);//能力有槽位
protected:

	virtual void OnRep_ActivateAbilities() override;//在重复激活技能时
	
	UFUNCTION(Client,Reliable)// 用于标识该函数是一个客户端 RPC（远程过程调用）函数，且调用是可靠的。
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);//客户端应用效果

	UFUNCTION(Client,Reliable)// 用于标识该函数是一个客户端 RPC（远程过程调用）函数，且调用是可靠的。
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 AbilityLevel);//客户端更新能力状态
};
