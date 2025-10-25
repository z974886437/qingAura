#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;//源能力系统组件

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;//目标能力系统组件

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;//Debuff更新

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;//Debuff伤害

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;//Debuff期间

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;//Debuff频率

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;//死亡冲量级

	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;//死亡冲动

	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;//击退力大小

	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;//击退力几率

	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;//击退力

	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;//是径向损伤

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;//径向损伤内半径

	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;//径向损伤外半径

	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;//径向损伤起源
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
	FVector GetDeathImpulse() const { return DeathImpulse; }//获取死亡冲击
	FVector GetKnockbackForce() const { return KnockbackForce; }//获得击退力
	bool IsRadialDamage() const { return bIsRadialDamage; }//是径向损伤
	float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius; };//获取径向损伤内半径
	float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius; }//得到径向损伤外半径
	FVector GetRadialDamageOrigin() const { return RadialDamageOrigin; }//获取径向损伤原点

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }// 设置当前命中是否为暴击
	void SetIsBlockedHit(bool bInIsBlockedHit ) { bIsBlockedHit = bInIsBlockedHit;}// 设置当前命中是否被格挡
	void SetIsSuccessfulDebuff(bool bInIsDebuff) { bIsSuccessfulDebuff = bInIsDebuff;}//设置成功减益
	void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage;}//设置减益伤害
	void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }//设置减益持续时间
	void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }//设置减益频率
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; }//设置死亡冲击
	void SetKnockbackForce(const FVector& InForce) { KnockbackForce = InForce; }//设置击退力
	void SetIsRadialDamage(bool bInIsRadialDamage) { bIsRadialDamage = bInIsRadialDamage; }//设定为径向损伤
	void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius) { RadialDamageInnerRadius = InRadialDamageInnerRadius;}//设置径向损伤内半径
	void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius) { RadialDamageOuterRadius = InRadialDamageOuterRadius;}//设置径向损伤外半径
	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) { RadialDamageOrigin = InRadialDamageOrigin; }//设置径向损伤原点
	
	
	
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

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;//死亡冲击

	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;//击退力

	UPROPERTY()
	bool bIsRadialDamage = false;//是径向损伤

	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;//径向损伤内半径

	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;//径向损伤外半径

	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;//径向损伤起源
};

// 特化 TStructOpsTypeTraits 结构体模板，针对 FAuraGameplayEffectContext 类型
template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum// 枚举定义了该结构体的一些特殊行为
	{
		WithNetSerializer = true,// 告诉引擎：这个结构体自己实现了 NetSerialize
		WithCopy = true// 告诉引擎：这个结构体支持复制（可拷贝）
	};
};
