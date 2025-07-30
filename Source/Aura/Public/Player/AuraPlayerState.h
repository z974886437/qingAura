// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;//接口函数的重写声明，用于告诉 GAS 如何获取角色或 Actor 上的 AbilitySystemComponent
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}//用于在外部获取角色或 Actor 上挂载的属性集（AttributeSet），即 GAS 系统中的属性数据容器。

protected:
	//UAbilitySystemComponent 是来自 Gameplay Ability System（GAS） 的一个核心类，用于处理能力（Ability）、属性（Attribute）、效果（Effect）等。
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;//设置成U属性，获得一个指向属性集的指针

	//GAS 框架中的基类，表示一组可以被能力（Ability）或效果（Effect）修改的属性，比如生命值、法力值、攻击力等。你通常会从它派生出自己的属性类。
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
};
