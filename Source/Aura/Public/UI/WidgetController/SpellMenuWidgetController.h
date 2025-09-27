// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "SpellMenuWidgetController.generated.h"

struct FAuraGameplayTags;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool,bEquipButtonEnabled,FString,DescriptionString,FString,NextLevelDescriptionString);//拼写地球仪选择签名
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature,const FGameplayTag&, AbilityTye);//等待装备选择签名
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature,const FGameplayTag&, AbilityTag);//法术地球仪重新分配签名

struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};
/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;//广播初始值
	virtual void BindCallbacksToDependencies() override;//绑定回调到依赖项

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChanged;//法术点已更改

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;//Spell Globe 选定代表

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;//等待装备委托

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;//停止等待装备委托

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;//法术地球重新分配

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);//已选择法术地球仪

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();//花费点按钮按下

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();//地球取消选择

	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();//装备按钮按下

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag,const FGameplayTag& AbilityType);//按下 Spell Row Globe

	void OnAbilityEquipped(const FGameplayTag& AbilityTag,const FGameplayTag& Status,const FGameplayTag& Slot,const FGameplayTag& PreviousSlot);//装备能力

private:

	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus,int32 SpellPoints,bool& bShouldEnableSpellPointsButton,bool& bShouldEnableEquipButton);//应该启用按钮
	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None,FAuraGameplayTags::Get().Abilities_Status_Locked};//选择能力
	int32 CurrentSpellPoints = 0;//当前法术点
	bool bWaitingForEquipSelection = false;//等待装备选择
	FGameplayTag SelectedSlot;//选定的插槽
};
