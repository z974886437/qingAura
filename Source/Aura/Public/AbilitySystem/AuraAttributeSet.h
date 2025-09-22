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

//声明一个返回值为 FGameplayAttribute 的无参委托类型（Delegate）
//DECLARE_DELEGATE_RetVal(FGameplayAttribute,FAttributeSignature);

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

// 定义一个类型别名 FAttributeFuncPtr，代表一个函数指针类型：
// 这个函数指针指向的是一个“无参数，返回 FGameplayAttribute 的静态函数”
//typedef is specific to the FGameplayAttribute() signature,but TStaticFunPtr is generic to any signature signature choser
// typedef 特定于游戏属性签名，但静态函数指针对于任何选择的签名都是通用的
//typedef TBaseStaticDelegateInstance<FGameplayAttribute(),FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T,FDefaultDelegateUserPolicy>::FFuncPtr;

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
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;//游戏后效果执行
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;//属性更改后
	
	// 创建一个映射表 TagsToAttributes：
	// 键（Key）：FGameplayTag，代表一个属性标签（如 Attributes.Primary.Strength）
	// 值（Value）：函数指针（FFuncPtr），指向一个返回 FGameplayAttribute、无参数的静态函数
	//
	// 这个结构是为了手动实现类似委托的功能 —— 通过标签映射到静态属性函数（如 GetStrengthAttribute）
	// 与使用 FAttributeSignature 类似，但这是更底层的做法
	//1.TMap<FGameplayTag,TBaseStaticDelegateInstance<FGameplayAttribute(),FDefaultDelegateUserPolicy>::FFuncPtr> TagsToAttributes;
	//2. TMap<FGameplayTag,FGameplayAttribute(*)()> TagsToAttributes;
	TMap<FGameplayTag,TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

	//TStaticFuncPtr<float(int32,float,int32)> RandomFunctionPointer;//随机功能指针
	//static float RandomFunction(int32 I,float F,int32 I2) {return 0.f;} //随机函数

	/*
	 *  Primar Attributes 主要属性
	 */
	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Strength,Category = "Primary Attributes")
	FGameplayAttributeData	Strength;//力量
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Strength);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用
	
	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Intelligence,Category = "Primary Attributes")
	FGameplayAttributeData	Intelligence;//智力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Intelligence);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Resilience,Category = "Primary Attributes")
	FGameplayAttributeData	Resilience;//韧性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribut

	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Vigor,Category = "Primary Attributes")
	FGameplayAttributeData	Vigor;//活力
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Vigor);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribut

	/*
	 * Secondary Attributes 次要属性
	 */

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Armor,Category = "Secondary Attributes")
	FGameplayAttributeData	Armor;//盔甲
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Armor);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ArmorPenetration,Category = "Secondary Attributes")
	FGameplayAttributeData	ArmorPenetration;//穿甲
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArmorPenetration);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_BlockChance,Category = "Secondary Attributes")
	FGameplayAttributeData	BlockChance;//格挡概率
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,BlockChance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitChance,Category = "Secondary Attributes")
	FGameplayAttributeData	CriticalHitChance;//暴击概率
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitChance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitDamage,Category = "Secondary Attributes")
	FGameplayAttributeData	CriticalHitDamage;//暴击伤害
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitResistance,Category = "Secondary Attributes")
	FGameplayAttributeData	CriticalHitResistance;//暴击抗性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitResistance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_HealthRegeneration,Category = "Secondary Attributes")
	FGameplayAttributeData	HealthRegeneration;//生命恢复
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,HealthRegeneration);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ManaRegeneration,Category = "Secondary Attributes")
	FGameplayAttributeData	ManaRegeneration;//法力恢复
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ManaRegeneration);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxHealth,Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;//最大健康
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHealth);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMana,Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;//最大法力值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	/*
	 *  Resistance Attributes
	 */

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_FireResistance,Category = "Resistance Attributes")
	FGameplayAttributeData FireResistance;//火抗性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,FireResistance);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_LightningResistance,Category = "Resistance Attributes")
	FGameplayAttributeData LightningResistance;//闪电抗性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,LightningResistance);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ArcaneResistance,Category = "Resistance Attributes")
	FGameplayAttributeData ArcaneResistance;//奥术抗性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArcaneResistance);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_PhysicalResistance,Category = "Resistance Attributes")
	FGameplayAttributeData PhysicalResistance;//物理抗性
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,PhysicalResistance);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用


	
	/*
	 *  Vital Attributes 重要属性
	 */
	//蓝图可读取但不可修改，启用网络同步，并在值同步变化时自动调用 OnRep_Strength() 函数（客户端）
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Health,Category = "Vital Attributes")
	FGameplayAttributeData Health;//健康
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Health);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Mana,Category = "Vital Attributes")
	FGameplayAttributeData Mana;//法力值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);//Gameplay Ability System（GAS） 提供的一个快捷宏，用来一键生成多个访问器函数，简化 Attribute 的声明和使用

	/*
	 *  Meta Attribute 元属性
	 */

	UPROPERTY(BlueprintReadOnly,Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;//传入伤害
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,IncomingDamage);

	UPROPERTY(BlueprintReadOnly,Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;//传入伤害
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,IncomingXP);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;//用于实现力量 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;//用于实现 智力属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;//用于实现 韧性属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;//用于实现 活力属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;//用于实现 盔甲属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;//用于实现 护甲穿透属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;//用于实现 格挡概率属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;//用于实现 暴击几率属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;//用于实现 暴击伤害属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;//用于实现 暴击抗性属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;//用于实现 生命恢复属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;//用于实现 法力恢复属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;//用于实现 属性的网络同步 + 通知响应

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;//用于实现 属性的网络同步 + 通知响应
	

private:
	void SetEffectProperties(const  FGameplayEffectModCallbackData& Data,FEffectProperties& Props) const;//设置效果属性
	void ShowFloatingText(const FEffectProperties& Props,float Damage,bool bBlockedHit,bool bCriticalHit) const;//显示浮动文本
	void SendXPEvent(const FEffectProperties& Props);//发送 XP 事件
	bool bTopOffHealth = false;//补充健康
	bool bTopOffMana = false;//补充法力
	
};
