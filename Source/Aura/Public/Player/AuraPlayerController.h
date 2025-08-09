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
class UAuraAbilitySystemComponent;
class USplineComponent;

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
	FHitResult CursorHit;//FHitResult 类型的变量，用于存储一次碰撞检测 光标命中

	void AbilityInputTagPressed(FGameplayTag InputTag);//能力输入标签按下
	void AbilityInputTagReleased(FGameplayTag InputTag);//能力输入标签释放
	void AbilityInputTagHeld(FGameplayTag InputTag);//能力输入标签持续按住

	UPROPERTY(EditDefaultsOnly,CateGory = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;//输入配置

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	
	FVector CachedDestination = FVector::ZeroVector;//缓存目的地
	float FollowTime = 0.f;//跟随时间
	float ShortPressThreshold = 0.5f;//短按阀值
	bool bAutoRunning = false;//自动奔跑
	bool bTargeting = false;//目标

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;//自动运行接受半径

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;//样条线

	void AutoRun();//自动奔跑
};
