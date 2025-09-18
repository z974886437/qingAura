// 青楼


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");// 创建一个弹簧臂组件，用于挂载摄像机并控制其跟随和旋转
	CameraBoom->SetupAttachment(GetRootComponent());// 将弹簧臂挂载到角色根组件上
	CameraBoom->SetUsingAbsoluteRotation(true);// 使用绝对旋转而非继承自父组件的旋转
	CameraBoom->bDoCollisionTest = false;// 禁用碰撞检测，避免弹簧臂被墙壁等阻挡摄像机

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("ToDownCameraComponent");// 创建一个摄像机组件，作为顶视摄像机
	TopDownCameraComponent->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);// 将摄像机挂载到弹簧臂末端
	TopDownCameraComponent->bUsePawnControlRotation = false;// 摄像机不使用角色控制器旋转（顶视角通常不需要角色旋转影响）
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");// 创建一个 Niagara 粒子组件，用于播放升级特效
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());// 将粒子组件挂载到角色根组件
	LevelUpNiagaraComponent->bAutoActivate = false;// 默认不自动激活，需要手动触发特效
	
	
	GetCharacterMovement()->bOrientRotationToMovement = true;//让角色的朝向跟随移动方向自动旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);//设置角色移动组件（Character Movement Component）的旋转速率
	GetCharacterMovement()->bConstrainToPlane = true;//限制角色移动在一个特定平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//让角色在游戏开始或生成时，自动对齐到你设置的运动平面

	bUseControllerRotationPitch = false;//用来控制角色是否跟随控制器的 Pitch（俯仰）旋转的
	bUseControllerRotationRoll = false;//控制角色是否跟随控制器的 Roll（翻滚）旋转
	bUseControllerRotationYaw = false;//控制角色是否跟随控制器的 Yaw（左右转头）旋转

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server(服务器的初始化能力参与者信息）
	InitAbilityActorInfo();
	AddCharacterAbilities();//添加角色能力
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the Client(客户端的启动能力参与者信息）
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))// 检查粒子组件是否有效，避免空指针或已被销毁
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation(); // 获取摄像机当前位置
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();// 获取粒子系统当前位置
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();// 计算粒子系统指向摄像机的旋转（让粒子面向摄像机）
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);// 设置粒子系统的世界旋转，使其朝向摄像机
		LevelUpNiagaraComponent->Activate(true); // 激活粒子系统，播放升级特效
	}
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
	
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo() 
{
	// Init ability actor info for the Server(记住服务器的初始化能力参与者信息）
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	//初始化 AuraPlayerState，让它知道谁是“实际拥有者”和“执行者”
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState,this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();// 调用 AuraAbilitySystemComponent 的扩展方法，做额外初始化（比如绑定事件）
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();//设置Auar角色能力系统组件
	AttributeSet = AuraPlayerState->GetAttributeSet();//设置属性集

	// 如果角色有控制器，并且控制器有 HUD，则初始化 UI Overlay，把必要数据传给 HUD
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}
	InitializeDefaultAttributes();//初始化默认属性
}



