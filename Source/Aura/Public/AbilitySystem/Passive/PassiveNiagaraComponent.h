// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"

class UAuraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPassiveNiagaraComponent();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;//被动法术标签

protected:
	virtual void BeginPlay() override;
	void OnPassiveActivate(const FGameplayTag& AbilityTag,bool bActivate);//被动激活
	void ActivateIfEquipped(UAuraAbilitySystemComponent* AuraASC);//如果装备则激活
	
};


