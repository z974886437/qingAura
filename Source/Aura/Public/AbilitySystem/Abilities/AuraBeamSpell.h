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
	UFUNCTION(BLueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);//存储鼠标数据信息

	UFUNCTION(BLueprintCallable)
	void StoreOwnerVariables();//存储变量
protected:

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	FVector MouseHitLocation;//鼠标命中位置

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;//鼠标命中Actor

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;//所有者玩家控制器

	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;//拥有角色
};
