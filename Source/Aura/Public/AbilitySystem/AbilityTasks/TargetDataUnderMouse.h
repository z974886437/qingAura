// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature,const FGameplayAbilityTargetDataHandle& , DataHandle);

/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,Category = "Ability|Tasks" , meta = (DisplayName = "TargetDataUnderMouse",HidePin = "OwningAbility",DefaultToSelf = "OwningAbility",BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);//创建鼠标下的目标数据

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;//有效数据

private:
	virtual void Activate() override;//激活
	void SendMouseCursorData();//发送鼠标光标数据
};
