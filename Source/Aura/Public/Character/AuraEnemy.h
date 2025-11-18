// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"//用于访问你项目中定义的 接口类 UEnemyInterface，一般用于角色之间的交互，比如玩家攻击敌人、敌人响应伤害等
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;//拥有

	/*Highlight Interface(敌人界面）*/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/*end Highlight Interface(结束敌人界面）*/

	/* Combat Interface (战斗界面)*/
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	
	/* end Combat Interface (结束战斗界面)*/
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;//生命变化

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;//最大生命变化

	void HitReactTagChanged(const FGameplayTag Callback,int32 NewCount);//击中反应标签已更改

	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	bool bHitReacting = false;//B命中反应

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	float LifeSpan = 5.f;//寿命

	void SetLevel(int32 InLevel) { Level = InLevel;}

	UPROPERTY(BlueprintReadWrite,Category = "Combat")
	TObjectPtr<AActor> CombatTarget;//战斗标签
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;//初始化能力Actor信息
	virtual void InitializeDefaultAttributes() const override;////初始化默认属性
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;//眩晕标签已更改

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;//生命条

	UPROPERTY(EditAnywhere,Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;//行为树

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;//AI控制器

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();
};
