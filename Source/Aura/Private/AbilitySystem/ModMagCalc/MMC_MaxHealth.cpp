// 青楼


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;//快照 = false

	RelevantAttributesToCapture.Add(VigorDef);//本次执行计算中要捕获（监听/使用）哪些属性
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target 从源和目标手机标签
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//F聚合器 评估参数
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//从你设置的 RelevantAttributesToCapture 中获取指定属性的值.
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef,Spec,EvaluationParameters,Vigor);
	Vigor = FMath::Max<float>(Vigor,0.f);//Vigor 最小值为0

	int32 PlayerLevel = 1;// 默认玩家等级为 1
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())// 如果技能效果的来源对象实现了 CombatInterface 接口
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());// 通过接口获取来源对象的玩家等级，并赋值给 PlayerLevel
	}

	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
