// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"//用于访问你项目中定义的 接口类 UEnemyInterface，一般用于角色之间的交互，比如玩家攻击敌人、敌人响应伤害等
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	/*Enemy Interface(敌人界面）*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/*end Enemy Interface(结束敌人界面）*/

	/* Combat Interface (战斗界面)*/
	virtual int32 GetPlayerLevel() override;
	
	/* end Combat Interface (结束战斗界面)*/
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;//初始化能力Actor信息

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	int32 Level = 1;
};
