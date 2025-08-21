#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"


USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit() const { return bIsCriticalHit; }// 判断是否是一次暴击命中
	bool IsBlockedHit() const { return bIsBlockedHit; }// 判断是否是一次被格挡的命中

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }// 设置当前命中是否为暴击
	void SetIsBlockedHit(bool bInIsBlockedHit ) { bIsBlockedHit = bInIsBlockedHit;}// 设置当前命中是否被格挡
	
	/* returns the actual struct used for serialization, subclasses must override this! 返回用于序列化的实际结构体，子类必须覆盖它！*/
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/* Custom serialization, subclasses must override this 自定义序列化，子类必须覆盖此*/
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

protected:

	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;

	
};
