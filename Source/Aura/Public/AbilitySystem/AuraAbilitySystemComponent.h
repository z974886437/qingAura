// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class ULoadScreenSaveGame;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* Asset Tags */);//效果资产标签
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);//赋予的能力
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&);//代表每种能力
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/,const FGameplayTag& /*StatusTag*/,int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped,const FGameplayTag& /*AbilityTag*/,const FGameplayTag& /*Status*/,const FGameplayTag& /*Slot*/,const FGameplayTag& /*PrevSlot*/);//装备能力
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility,const FGameplayTag& /*AbilityTag*/);//停用被动能力
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect,const FGameplayTag& /*AbilityTag*/,bool /*bActivate*/);//激活被动效果


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
	FDeactivatePassiveAbility DeactivatePassiveAbility;//停用被动能力
	FActivatePassiveEffect ActivatePassiveEffect;//激活被动效果

	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);//从保存数据中添加角色能力
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);//添加角色能力。
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);//添加角色被动能力
	bool bStartupAbilitiesGiven = false;//给定的启动能力

	void AbilityInputTagPressed(const FGameplayTag& InputTag);//技能输入标签按下
	void AbilityInputTagHeld(const FGameplayTag& InputTag);//持续能力输入标签
	void AbilityInputTagReleased(const FGameplayTag& InputTag);//释放能力输入标签
	void ForEachAbility(const FForEachAbility& Delegate);//对于每个能力

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);//从规范中获取能力标签
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);//从规范中获取输入标签
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);//从规范中获取状态
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);//从技能标签获取状态
	FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag);//从能力标签中获取槽位
	bool SlotIsEmpty(const FGameplayTag& Slot);//插槽为空
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec,const FGameplayTag& Slot);//能力有槽位
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);//能力有任意槽位
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);//获取带有插槽的规格
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;//是被动能力
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec,const FGameplayTag& Slot);//为能力分配槽位

	UFUNCTION(NetMulticast,Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag,bool bActivate);//组播激活被动效应

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);//从能力标签获取规格
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);//升级属性

	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);//服务器升级属性
	
	void UpdateAbilityStatuses(int32 Level);//更新能力状态

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);//服务器花费法术点

	UFUNCTION(Server,Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Slot);//服务器装备能力

	UFUNCTION(Client,Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Status, const FGameplayTag& Slot,const FGameplayTag& PreviousSlot);//客户端装备能力

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag,FString& OutDescription,FString& OutNextLevelDescription);//按能力标签获取描述

	static void ClearSlot(FGameplayAbilitySpec* Spec);//清除插槽
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);//插槽的清除能力
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec,const FGameplayTag& Slot);//能力有槽位
protected:

	virtual void OnRep_ActivateAbilities() override;//在重复激活技能时
	
	UFUNCTION(Client,Reliable)// 用于标识该函数是一个客户端 RPC（远程过程调用）函数，且调用是可靠的。
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle);//客户端应用效果

	UFUNCTION(Client,Reliable)// 用于标识该函数是一个客户端 RPC（远程过程调用）函数，且调用是可靠的。
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 AbilityLevel);//客户端更新能力状态
};
