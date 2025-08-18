// 青楼

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,//元素师
	Warrior,//战士
	Ranger//游侠
};

// 声明一个可在蓝图中使用的结构体，用来保存角色职业的默认信息
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	// 在编辑器中只读（不可运行时修改），分类到 "Class Defaults"
	// 用来存储一个 GameplayEffect 类（Blueprint 或 C++ 类），用来设置角色的初始主要属性
	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;//主要属性
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly,Category = "Character Class Defaults")
	TMap<ECharacterClass,FCharacterClassDefaultInfo> CharacterClassInformation;//字符类信息

	UPROPERTY(EditDefaultsOnly,Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;//次要属性

	UPROPERTY(EditDefaultsOnly,Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;//重要属性

	UPROPERTY(EditDefaultsOnly,Category = "Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;//普通能力

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
