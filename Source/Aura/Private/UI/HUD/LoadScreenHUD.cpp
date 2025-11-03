// 青楼


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this,LoadScreenViewModelClass);// 创建一个新的 LoadScreenViewModel 对象，并指定该对象的类类型
	LoadScreenViewModel->InitializeLoadSlots();// 初始化加载槽位，可能是用于设置加载的不同状态或内容

	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(),LoadScreenWidgetClass);// 创建一个 LoadScreenWidget 小部件，并指定该小部件的类类型
	LoadScreenWidget->AddToViewport();// 将小部件添加到视口中显示
	LoadScreenWidget->BlueprintInitializeWidget();// 调用小部件的初始化函数进行蓝图层面的初始化
	
}
