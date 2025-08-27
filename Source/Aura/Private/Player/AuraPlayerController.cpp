// 青楼


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"//增强输入子系统
#include "EnhancedInputComponent.h"//增强输入组件
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"//交互/敌人接口
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;//启用网络复制功能:让这个 Actor 能够在服务端创建，并自动同步到客户端

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount,ACharacter* TargetCharacter,bool bBlockedHit,bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		// 创建一个新的 UDamageTextComponent 实例，属于当前对象（this），类来自 DamageTextComponentClass
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter,DamageTextComponentClass);
		DamageText->RegisterComponent();// 注册组件到引擎，使其能够被渲染和更新
		// 把这个伤害文本组件挂到目标角色的根组件上（相对位置不变）
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);// 再立刻从角色身上解绑（但不会销毁），让它独立存在（常用于飘字UI）
		DamageText->SetDamageText(DamageAmount,bBlockedHit,bCriticalHit);  // 设置伤害数值（比如 "99999"）传给组件内部去显示
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		//功能：找到离 Pawn 最近的样条曲线（Spline）上的点。ControlledPawn->GetActorLocation()获取 Pawn 当前世界坐标。ESplineCoordinateSpace::World让返回值也使用世界坐标系。
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		//到目的地的巨离
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	//FHitResult CursorHit;//FHitResult 类型的变量，用于存储一次碰撞检测 光标命中
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);//用于进行鼠标位置下的光线检测
	if (!CursorHit.bBlockingHit) return;//没有命中任何阻挡物体，那么就直接退出当前函数

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
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
	// if (LastActor == nullptr)
	// {
	// 	if (ThisActor != nullptr)
	// 	{
	// 		//Case B(LastActor为空 与 ThisActor为有效)
	// 		ThisActor->HighlightActor();
	// 	}
	// 	else
	// 	{
	// 		// Case A - both are null,do nothing(不执行任何操作)
	// 	}
	// }
	// else // LastActor is volid
	// {
	// 	if (ThisActor == nullptr)
	// 	{
	// 		// Case C(LastActor有效 与 ThisActor为空)
	// 		LastActor->UnHighlightActor();
	// 	}
	// 	else // both actor are volid 
	// 	{
	// 		if(LastActor != ThisActor)
	// 		{
	// 			//Case D(如果两个演员都有效，但是最后一个演员不等于这个演员)
	// 			LastActor->UnHighlightActor();
	// 			ThisActor->HighlightActor();
	// 		}
	// 		else
	// 		{
	// 			// Case E - do nothing(不执行任何操作)
	// 		}
	// 	}
	// }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1,3.f,FColor::Red,*InputTag.ToString());
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))//检查当前输入是否是鼠标左键（LMB），只精确匹配，不会匹配父级 Tag。
	{
		bTargeting = ThisActor  ? true : false;// 如果鼠标下有目标 Actor，就进入锁定目标模式；否则关闭锁定
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))// 如果当前输入标签不是 "鼠标左键"（InputTag_LMB）
	{
		if (GetASC()) // 如果有能力系统组件，就转发这个输入（表示按住这个标签对应的技能）
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;// 不是左键输入，直接返回，不再处理后面的移动逻辑
	}

	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag); // 锁定目标模式下，左键会持续触发技能（而不是走点击地面移动）
	
	if (!bTargeting && !bShiftKeyDown)// 如果当前没有锁定目标（bTargeting=false）并且 Shift 键没有按下
	{
		const APawn* ControlledPawn = GetPawn();// 获取当前被控制的 Pawn（角色）
		if (FollowTime <= ShortPressThreshold && ControlledPawn)// 1. 判断是否是短按（FollowTime <= ShortPressThreshold）并且角色存在
		{
			// 2. 调用导航系统同步计算从角色当前位置到目标位置的路径（UNavigationPath 对象）	
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this,
				ControlledPawn->GetActorLocation(),// 起点：角色位置
				CachedDestination)) // 终点：鼠标点击位置
			{
				Spline->ClearSplinePoints();  // 3. 清空之前的样条曲线点
				for (const FVector& PointLoc : NavPath->PathPoints)// 4. 把路径点逐个加入到样条曲线上
				{
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(),PointLoc,8.f,8,FColor::Green,false,5.f); // 5. 用绿色小球在场景中画出路径点，方便调试
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];// 6. 缓存导航路径终点（最后一个点）为目标位置
					bAutoRunning = true; // 7. 开启自动寻路标志位，让角色开始沿路径移动
				}
			}
		}
		// 7. 重置跟随时间，关闭锁定目标模式
		FollowTime = 0.f;
		bTargeting = false;
	}
}
  
void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))// 如果当前输入标签不是 "鼠标左键"（InputTag_LMB）
	{
		if (GetASC()) // 如果有能力系统组件，就转发这个输入（表示按住这个标签对应的技能）
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;// 不是左键输入，直接返回，不再处理后面的移动逻辑
	}

	if (bTargeting || bShiftKeyDown)// 如果是鼠标左键（LMB），判断当前是否在锁定目标模式（bTargeting）
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag); // 锁定目标模式下，左键会持续触发技能（而不是走点击地面移动）
		}
	}
	else// 没有锁定目标
	{
		FollowTime += GetWorld()->GetDeltaSeconds(); // 记录鼠标按住左键的时间（可用来判断点击 vs 长按）
		
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;// 缓存目的地，用于移动
		}

		if (APawn* ControlledPawn = GetPawn())// 获取当前控制的 Pawn（玩家角色）
		{
			// 计算从角色位置到点击位置的方向向量（并单位化）
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection); // 添加移动输入，让角色向点击位置走
		}
	}
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

	// 绑定移动输入：当 MoveAction 被触发（Triggered）时调用 Move()
	// Triggered 表示输入值发生变化（如按下或摇杆移动时）
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	// 绑定 Shift 键按下事件：Started 表示刚按下时触发
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Started,this,&AAuraPlayerController::ShiftPressed);
	// 绑定 Shift 键松开事件：Completed 表示按键释放时触发
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed,this,&AAuraPlayerController::ShiftReleased);
	// 绑定技能输入：从 InputConfig 中批量绑定技能标签 → 对应按下、释放、长按事件的处理函数
	AuraInputComponent->BindAbilityActions(InputConfig, // 输入配置（技能标签与按键映射表）
		this, // 拥有者（PlayerController）
		&ThisClass::AbilityInputTagPressed, // 技能按下回调
		&ThisClass::AbilityInputTagReleased,// 技能释放回调
		&ThisClass::AbilityInputTagHeld // 技能长按回调
		);
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


