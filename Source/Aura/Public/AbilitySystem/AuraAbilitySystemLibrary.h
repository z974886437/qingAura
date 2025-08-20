// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);//获取覆盖小部件控制器

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);//获取属性菜单小部件控制器

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass,float Level,UAbilitySystemComponent* ASC);//初始化默认属性

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
};
