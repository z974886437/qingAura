// 青楼


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false;//快照 = false

	RelevantAttributesToCapture.Add(IntDef);//本次执行计算中要捕获（监听/使用）哪些属性
	
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target 从源和目标手机标签.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//F聚合器 评估参数
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//从你设置的 RelevantAttributesToCapture 中获取指定属性的值
	float Int = 0.f;
	GetCapturedAttributeMagnitude(IntDef,Spec,EvaluationParameters,Int);
	Int = FMath::Max<float>(Int,0.f);//Int 最小值为0

	int32 PlayerLevel = 1;// 默认玩家等级为 1
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())// 如果技能效果的来源对象实现了 CombatInterface 接口
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());// 通过接口获取来源对象的玩家等级，并赋值给 PlayerLevel
	}
	
	return 50.f + 2.5f * Int + 15.f * PlayerLevel;
}
