// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	class UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag MontageTage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag SocketTage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.该类无需修改
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.向此类添加接口函数。这是将继承的类，以实现此接口。
public:
	virtual int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);//获取战斗插槽位置

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdataFacingTarget(const FVector& Target);//更新面向目标

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactMontage();//被击中反应蒙太奇

	virtual void Die() = 0;//死亡

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	bool IsDead() const;//死亡

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	AActor* GetAvatar() ;//获取 Avatar

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages();//获取攻击蒙太奇

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();//获取血液效果

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);//按标签获取标记蒙太奇

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	int32 GetMinionCount();//获取仆从数量
};
