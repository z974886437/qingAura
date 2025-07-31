// 青楼


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;//让角色的朝向跟随移动方向自动旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);//设置角色移动组件（Character Movement Component）的旋转速率
	GetCharacterMovement()->bConstrainToPlane = true;//限制角色移动在一个特定平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//让角色在游戏开始或生成时，自动对齐到你设置的运动平面

	bUseControllerRotationPitch = false;//用来控制角色是否跟随控制器的 Pitch（俯仰）旋转的
	bUseControllerRotationRoll = false;//控制角色是否跟随控制器的 Roll（翻滚）旋转
	bUseControllerRotationYaw = false;//控制角色是否跟随控制器的 Yaw（左右转头）旋转
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server(服务器的初始化能力参与者信息）
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the Client(客户端的启动能力参与者信息）
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo() 
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	//初始化 AuraPlayerState，让它知道谁是“实际拥有者”和“执行者”
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState,this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();//设置Auar角色能力系统组件
	AttributeSet = AuraPlayerState->GetAttributeSet();//设置属性集

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}
	InitializeDefaultAttributes();//初始化默认属性
}

