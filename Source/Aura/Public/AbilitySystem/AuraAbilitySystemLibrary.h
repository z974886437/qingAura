// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilityInfo;
class AAuraHUD;
class USpellMenuWidgetController;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
struct FWidgetControllerParams;

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams,AAuraHUD*& OutAuraHUD);//制作控件控制器参数

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);//获取覆盖小部件控制器

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);//获取属性菜单小部件控制器

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);//获取法术菜单小部件控制器

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass,float Level,UAbilitySystemComponent* ASC);//初始化默认属性

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* ASC,ECharacterClass CharacterClass);//赋予启动能力

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);//获取角色类信息

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);//获取技能信息

	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);//判断是否是一次格挡命中

	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);//判断是否是一次格挡命中

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bInIsBlockedHit);//设置格挡命中

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bInIsCriticalHit);//设置暴击命中

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject,TArray<AActor*>& OutOverlappingActors,const TArray<AActor*>& ActorsToIgnore,float Radius,const FVector& SphereOrigin);//在半径内获取现场玩家

	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor,AActor* SecondActor);//不是朋友

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);//施加伤害效果
	
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject,ECharacterClass CharacterClass,int32 CharacterLevel);//获得职业和等级的 XP 奖励
};
