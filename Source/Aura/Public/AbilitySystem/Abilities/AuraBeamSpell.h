// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:

	virtual FString GetDescription(int32 Level) override;//获取描述
	virtual FString GetNextLevelDescription(int32 Level) override;//获取下一级描述
	
	UFUNCTION(BLueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);//存储鼠标数据信息

	UFUNCTION(BLueprintCallable)
	void StoreOwnerVariables();//存储变量

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);//跟踪第一个目标

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);//存储其他目标

	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);//主要目标死亡

	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);//其他目标死亡
protected:

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	FVector MouseHitLocation;//鼠标命中位置

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;//鼠标命中Actor

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;//所有者玩家控制器

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;//拥有角色

	UPROPERTY(EditDefaultsOnly,Category = "Beam")
	int32 MaxNumShockTargets = 5;//最大冲击目标数
};
