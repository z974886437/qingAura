// 青楼

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UAuraUserWidget;
struct FWidgetControllerParams;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);//获取覆盖小部件控制器
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);//获取属性菜单小部件控制器

	void InitOverlay(APlayerController* PC, APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS);//初始化覆盖
	
protected:
	//virtual void BeginPlay() override;
private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;//覆盖小部件
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;//覆盖小部件

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;//覆盖小部件控制器

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;//覆盖小部件控制器类

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;//属性菜单小部件控制器

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;//属性菜单小部件控制器类
};
