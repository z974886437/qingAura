// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;//重写虚函数 PossessedBy()，它是 Unreal Engine（UE）中处理角色被控制器接管（Possess）时的关键函数
	virtual void OnRep_PlayerState() override;

	/* Players Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	/* end Player Interface */
	
	/* Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	
	/* end Combat Interface */

private:
	virtual void InitAbilityActorInfo() override;//初始化能力Actor信息
};
