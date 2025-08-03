// 青楼


#include "UI/HUD/AuraHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

// 获取主 UI 控件控制器（OverlayWidgetController）
// 如果尚未创建，则创建、初始化并绑定依赖项
UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)// 如果 Overlay 控件控制器为空（尚未初始化）
	{
		// 使用指定的类类型创建 Overlay 控件控制器实例，设置其所有者为当前 HUD
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);// 传入构造参数（控制器、玩家状态、GAS组件、属性集等），为控制器配置数据来源
		OverlayWidgetController->BindCallbacksToDependencies();// 将 UI 控件绑定到属性变化（或其他系统事件）以实现 UI 的动态更新（如血条、蓝条自动刷新）
	}
	return OverlayWidgetController;// 如果已经存在（非空），直接返回已有的控件控制器实例（避免重复创建）
}

// 获取属性面板（Attribute Menu）控件控制器
// 若未初始化则创建、配置参数，并绑定依赖项
UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)// 如果属性面板控件控制器还未创建（第一次调用）
	{
		// 实例化一个新的属性面板控件控制器，指定所属为当前 HUD，类来自配置
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this,AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);// 设置控件控制器的参数，包括控制器、玩家状态、GAS 组件、属性集等
		AttributeMenuWidgetController->BindCallbacksToDependencies();// 将 GAS 属性等依赖项与控件绑定，确保属性变化时 UI 同步更新
	}
	return AttributeMenuWidgetController;// 返回已存在或刚创建好的控件控制器
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	//叠加小部件控制器类未初始化，请填写BP_AuraHUD
	checkf(OverlayWidgetClass,TEXT("Overlay Widget Class uninitialized,please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class uninitialized,please fill out BP_AuraHUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	//UI 控制器初始化流程
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	//把控制器（WidgetController）传递给 UI Widget（OverlayWidget），通常是为了让 UI 组件能够访问、绑定、响应控制器中的数据或事件
	OverlayWidget->SetWidgetController(WidgetController);
	//初始化 UI 的显示内容 —— 比如血条、蓝条、经验条，在 UI 创建后立即更新为当前角色的实际值
	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();//添加到视口
	
}

// void AAuraHUD::BeginPlay()
// {
// 	Super::BeginPlay();
// 	
// }
