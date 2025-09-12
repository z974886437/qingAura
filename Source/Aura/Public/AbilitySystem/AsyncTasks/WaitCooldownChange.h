// 青楼

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "WaitCooldownChange.generated.h"

struct FGameplayEffectSpec;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * 
 */
UCLASS()
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;//冷却开始

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;//冷却结束

	UFUNCTION(BlueprintCallable,meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTag);//等待冷却时间变化

	UFUNCTION(BlueprintCallable)
	void EndTask();//结束任务

protected:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag;//冷却标签

	void CooldownTagChanged(const FGameplayTag InCooldownTag,int32 NewCount);//冷却时间标签已更改
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,const FGameplayEffectSpec& SpecApplied,FActiveGameplayEffectHandle ActiveEffectHandle);//添加主动效果
};
