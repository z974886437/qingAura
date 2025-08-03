// 青楼


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController//AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

// 获取用于控制主 UI 叠加层（Overlay）的 Widget 控制器（Controller）对象
UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	// 尝试从世界上下文对象获取索引为 0 的玩家控制器（一般是本地玩家）
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		// 将 HUD 强转为自定义的 AAuraHUD，确保我们有能力访问 Aura 专属的 UI 系统
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();// 获取玩家状态（PlayerState），通常保存属性和能力组件
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();// 从 PlayerState 获取 GAS 的核心组件 AbilitySystemComponent
			UAttributeSet* AS = PS->GetAttributeSet();// 获取属性集 AttributeSet，包含血量、法力等自定义属性
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);// 构造控件控制器所需的参数结构体（封装了 UI 所需要的数据来源）
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);// 返回主 UI 控制器实例（用于驱动血条、蓝条等 HUD 显示）
		}
	}
	return nullptr;// 如果以上任何一步失败，返回空指针
}
