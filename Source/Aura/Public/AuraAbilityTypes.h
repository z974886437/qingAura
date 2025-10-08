#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;//源能力系统组件

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;//目标能力系统组件

	UPROPERTY()
	float BaseDamage = 0.f;

	UPROPERTY()
	float AbilityLevel = 1.f;

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float DebuffChance = 0.f;//Debuff更新

	UPROPERTY()
	float DebuffDamage = 0.f;//Debuff伤害

	UPROPERTY()
	float DebuffDuration = 0.f;//Debuff期间

	UPROPERTY()
	float DebuffFrequency = 0.f;//Debuff频率

	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;//死亡冲量级
};

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit() const { return bIsCriticalHit; }// 判断是否是一次暴击命中
	bool IsBlockedHit() const { return bIsBlockedHit; }// 判断是否是一次被格挡的命中
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; } //是成功的减益
	float GetDebuffDamage() const { return DebuffDamage; }//获得减益伤害
	float GetDebuffDuration() const { return DebuffDuration; }//获得减益持续时间
	float GetDebuffFrequency() const { return DebuffFrequency; }//获得减益频率
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType;}//获取伤害类型

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }// 设置当前命中是否为暴击
	void SetIsBlockedHit(bool bInIsBlockedHit ) { bIsBlockedHit = bInIsBlockedHit;}// 设置当前命中是否被格挡
	void SetIsSuccessfulDebuff(bool bInIsDebuff) { bIsSuccessfulDebuff = bInIsDebuff;}//设置成功减益
	void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage;}//设置减益伤害
	void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }//设置减益持续时间
	void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }//设置减益频率
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	
	
	/* returns the actual struct used for serialization, subclasses must override this! 返回用于序列化的实际结构体，子类必须覆盖它！*/
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/** 创建此上下文的副本，用于复制以供以后修改 */
	virtual FGameplayEffectContext* Duplicate() const
	{
		FGameplayEffectContext* NewContext = new FGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// 对命中结果进行深层复制
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/* Custom serialization, subclasses must override this 自定义序列化，子类必须覆盖此*/
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

protected:

	UPROPERTY()
	bool bIsBlockedHit = false;//被阻挡命中
	
	UPROPERTY()
	bool bIsCriticalHit = false;//是暴击

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;//是成功的减益

	UPROPERTY()
	float DebuffDamage = 0.f;//减益伤害

	UPROPERTY()
	float DebuffDuration = 0.f;//减益持续时间

	UPROPERTY()
	float DebuffFrequency = 0.f;//减益频率

	TSharedPtr<FGameplayTag> DamageType;//伤害类型
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,// 告诉引擎：这个结构体自己实现了 NetSerialize
		WithCopy = true// 告诉引擎：这个结构体支持复制（可拷贝）
	};
};
