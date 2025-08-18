// 青楼


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"

UExecCalc_Damage::UExecCalc_Damage()
{
	
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();// 从 ExecutionParams 获取技能的来源（施法者）的 AbilitySystemComponent
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();// 从 ExecutionParams 获取技能的目标（被击中者）的 AbilitySystemComponent

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;// 从来源 ASC 拿到施法者的 AvatarActor（通常是角色 Pawn/Character）
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;// 从目标 ASC 拿到目标的 AvatarActor

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();// 获取当前正在执行的 GameplayEffectSpec（包含技能等级、标签、SetByCaller 参数等）
	
}
