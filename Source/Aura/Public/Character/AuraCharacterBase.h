// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;//接口函数的重写声明，用于告诉 GAS 如何获取角色或 Actor 上的 AbilitySystemComponent
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}//用于在外部获取角色或 Actor 上挂载的属性集（AttributeSet），即 GAS 系统中的属性数据容器。


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;//武器

	//UAbilitySystemComponent 是来自 Gameplay Ability System（GAS） 的一个核心类，用于处理能力（Ability）、属性（Attribute）、效果（Effect）等。
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//设置成U属性，获得一个指向属性集的指针

	//GAS 框架中的基类，表示一组可以被能力（Ability）或效果（Effect）修改的属性，比如生命值、法力值、攻击力等。你通常会从它派生出自己的属性类。
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();//初始化能力Actor信息

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attirbutes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;//默认主要属性

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attirbutes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;//默认次要属性



	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Level) const;//应用效果到自身
	void InitializeDefaultAttributes() const;//初始化默认属性
	

};
