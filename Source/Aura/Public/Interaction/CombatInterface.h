// 青楼

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class UAnimMontage;
/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FVector GetCombatSocketLocation();//获取战斗插槽位置

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdataFacingTarget(const FVector& Target);//更新面向目标

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactMontage();//被击中反应蒙太奇

	virtual void Die() = 0;//死亡

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	bool IsDead() const;//死亡

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	AActor* GetAvatar() ;//获取 Avatar
};
