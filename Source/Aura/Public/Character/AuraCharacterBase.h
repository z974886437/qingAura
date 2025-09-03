// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"


class UNiagaraSystem;
class UGameplayAbility;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UAnimMontage;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;//接口函数的重写声明，用于告诉 GAS 如何获取角色或 Actor 上的 AbilitySystemComponent
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}//用于在外部获取角色或 Actor 上挂载的属性集（AttributeSet），即 GAS 系统中的属性数据容器。

	/* Combat Interface */
    virtual UAnimMontage* GetHitReactMontage_Implementation() override;//Get Hit React 蒙太奇实现
    virtual void Die() override;//死
    virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;//获取战斗插槽位置
    virtual bool IsDead_Implementation() const override;//死亡
    virtual AActor* GetAvatar_Implementation()  override;//获取Avatar
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;//获取攻击蒙太奇
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;//获取血液效果
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;//按标签获取标记蒙太奇
    /* end Combat Interface*/

	UFUNCTION(NetMulticast,Reliable)//服务器调用，所有客户端都执行，并且保证消息送达。
	virtual void MulticastHandleDeath();//多播句柄死亡

	UPROPERTY(EditAnywhere,Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;//攻击蒙太奇
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;//武器

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName WeaponTipSocketName;//武器尖端插座名称

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere,Category = "Combat")
	FName TailSocketName;//尾部插座名称

	bool bDead = false;//b死亡
	
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

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attirbutes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;//默认重要要属性

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Level) const;//应用效果到自身
	virtual void InitializeDefaultAttributes() const;//初始化默认属性

	void AddCharacterAbilities();//添加角色能力

	/* Dissolve Effects 溶解效果*/

	void Dissolve();//溶解

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);//启动溶解时间轴

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);//启动武器溶解时间轴

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;//溶解材质实例

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;//武器溶解材质实例

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	UNiagaraSystem* BloodEffect;//血液效应

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	USoundBase* DeathSound;//死亡声音
private:

	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;//启动能力

	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;//命中反应montage
};
