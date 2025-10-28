// 青楼

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;//获取描述
	virtual FString GetNextLevelDescription(int32 Level) override;//获取下一级描述
};
