// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;//重写虚函数 PossessedBy()，它是 Unreal Engine（UE）中处理角色被控制器接管（Possess）时的关键函数
	virtual void OnRep_PlayerState() override;

	/* Combat Interface */
	virtual int32 GetPlayerLevel() override;
	
	/* end Combat Interface */

private:
	virtual void InitAbilityActorInfo() override;//初始化能力Actor信息
};
