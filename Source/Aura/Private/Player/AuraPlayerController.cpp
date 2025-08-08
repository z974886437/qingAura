// 青楼


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"//增强输入子系统
#include "EnhancedInputComponent.h"//增强输入组件
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"//交互/敌人接口

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;//启用网络复制功能:让这个 Actor 能够在服务端创建，并自动同步到客户端
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;//FHitResult 类型的变量，用于存储一次碰撞检测 光标命中
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);//用于进行鼠标位置下的光线检测
	if (!CursorHit.bBlockingHit) return;//没有命中任何阻挡物体，那么就直接退出当前函数

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	/*
	*Line trace from cursor. There are several scenarios: (光标的线条轨迹。有几种情况)
	* A.LastActor is null && ThisActor is null (LastActor为空 与 ThisActor为空)
	*   -Do nothing(不执行任何操作)
	* B.LastActor is null && ThisActor is volid (LastActor为空 与 ThisActor为有效)
	*   - Highlight ThisActor (高亮这名演员)
	* C.LastActor is volid && ThisActor is null (LastActor有效 与 ThisActor为空)
	*   - UnHighlight LastActor (取消高亮最后演员)
	* D.Both actors are volid,but LastActor != ThisActor(如果两个演员都有效，但是最后一个演员不等于这个演员)
	*   - UnHighlight LastActor,and Highlight ThisActor(取消高亮最后演员和高亮这个演员)
	* E.Both actors are volid,and are the same actor (如果两个演员都有效并且是同一个演员)
	*   -Do nothing(不执行任何操作)
	*/

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			//Case B(LastActor为空 与 ThisActor为有效)
			ThisActor->HighlightActor();
		}
		else
		{
			// Case A - both are null,do nothing(不执行任何操作)
		}
	}
	else // LastActor is volid
	{
		if (ThisActor == nullptr)
		{
			// Case C(LastActor有效 与 ThisActor为空)
			LastActor->UnHighlightActor();
		}
		else // both actor are volid 
		{
			if(LastActor != ThisActor)
			{
				//Case D(如果两个演员都有效，但是最后一个演员不等于这个演员)
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Case E - do nothing(不执行任何操作)
			}
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1,3.f,FColor::Red,*InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		// 获取当前 Pawn 的 AbilitySystemComponent（能力系统组件）
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(AuraContext);// 如果蓝图里没设定，会在这里直接崩溃，提示开发者修正

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());//增强输入系统
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext,0);//AuraContext 添加到当前玩家的输入系统中，并设定优先级为 0（最高优先级）
	}

	bShowMouseCursor = true;//让鼠标在游戏中可见
	DefaultMouseCursor = EMouseCursor::Default;//设置鼠标的样式，这里使用默认样式（箭头）

	//让你的游戏进入“游戏和UI混合输入模式”，并且不锁定鼠标到游戏窗口内
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);//就算鼠标捕获到窗口，也不要隐藏它
	SetInputMode(InputModeData);//设置输入模式为InputModeData

}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//它的作用是将 InputComponent 转换为 UEnhancedInputComponent 类型，CastChecked 是一种类型转换方法，它会在转换失败时触发断言，通常用于确保转换成功。
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	//用于将一个输入动作与特定的函数绑定，在玩家触发该输入时执行相应的函数
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	//从 FInputActionValue 中获取一个二维向量 (FVector2D)，并将其存储在 InputAxisVector 变量中
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//创建旋转
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

	//创建向前向量 即你的旋转向量
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);//前进后退
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);//左右移动
	}
}


