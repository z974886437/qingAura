// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties//效果属性
{
	GENERATED_BODY()

	FEffectProperties(){}//影响所有物

	FGameplayEffectContextHandle EffectContextHandle;//影响上下文手柄

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;//源能力系统组件

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;//源AvatarActor

	UPROPERTY()
	AController* SourceController = nullptr;//源控制器

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;//源角色

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;//目标能力系统组件

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;//目标AvatarActor

	UPROPERTY()
	AController* TargetController = nullptr;//目标控制器

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;//目标角色

	
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();//属性集

	//网络系统中用于注册变量复制（Replication）规则的一个函数声明,它的作用是告诉引擎：“哪些变量需要从服务器复制到客户端。”
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;//预属性更改
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;//游戏后效果执行

	/*
	 *  Primar Attributes 主要属性
	 */
	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Strength,Category = "Primar Attributes")
	FGameplayAttributeData	Strength;//力量
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Strength);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Intelligence,Category = "Primar Attributes")
	FGameplayAttributeData	Intelligence;//智力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Intelligence);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Resilience,Category = "Primar Attributes")
	FGameplayAttributeData	Resilience;//韧性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribut

	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Vigor,Category = "Primar Attributes")
	FGameplayAttributeData	Vigor;//活力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Vigor);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribut
	/*
	 *  Vital Attributes 重要属性
	 */
	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Health,Category = "Vital Attributes")
	FGameplayAttributeData Health;//健康
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Health);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_MaxHealth,Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;//最大健康
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHealth);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Mana,Category = "Vital Attributes")
	FGameplayAttributeData Mana;//法力值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,Replicated = OnRep_Mana,Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;//最大法力值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;//用于实现 属性的网络同步 + 通知响应

protected:
	void SetEffectProperties(const struct FGameplayEffectModCallbackData& Data,FEffectProperties& Props) const;//设置效果属性

};
