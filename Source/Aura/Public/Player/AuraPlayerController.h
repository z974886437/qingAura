// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraInputConfig;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;//玩家点击
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;//设置输入组件
	
private:
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;//增强输入系统

	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> MoveAction;//移动动作

	void Move(const struct FInputActionValue& InputActionValue);//定义F输入动作值

	void CursorTrace();//空光标轨迹

	IEnemyInterface* LastActor;//最后Actor
	IEnemyInterface* ThisActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);//能力输入标签按下
	void AbilityInputTagReleased(FGameplayTag InputTag);//能力输入标签释放
	void AbilityInputTagHeld(FGameplayTag InputTag);//能力输入标签持续按住

	UPROPERTY(EditDefaultsOnly,CateGory = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;//输入配置
};
