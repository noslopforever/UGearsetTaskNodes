#include "BPAsync_WaitInputAxis.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UBPAsync_WaitAxisTask::UBPAsync_WaitAxisTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bOverrideConflictBindings(false)
{
}

UBPAsync_WaitAxisTask* UBPAsync_WaitAxisTask::CreateWaitInputAxisTask(UObject* InWorldContextObject, AActor* InTaskOwner, FName InAxisName)
{
	// Factory create instance
	UBPAsync_WaitAxisTask* InputEvent = ::NewObject<UBPAsync_WaitAxisTask>(GetTransientPackage());
	{
		InputEvent->TaskOwner = InTaskOwner;
		InputEvent->ListenAxisName = InAxisName;
	}
	return InputEvent;
}

bool UBPAsync_WaitAxisTask::RegisterInputEvents(UInputComponent* InputComponent)
{
	// Bind axis events.
	FInputAxisBinding AB(ListenAxisName);
	AB.bConsumeInput = true;
	AB.bExecuteWhenPaused = false;
	AB.AxisDelegate.BindDelegate(this, GET_FUNCTION_NAME_CHECKED(UBPAsync_WaitAxisTask, Internal_OnAxisEvent));

	if (bOverrideConflictBindings) {
		for (int32 ExistingIndex = InputComponent->AxisBindings.Num() - 1; ExistingIndex >= 0; --ExistingIndex)
		{
			const FInputAxisBinding& ExistingBind = InputComponent->AxisBindings[ExistingIndex];
			if (ExistingBind.AxisName == AB.AxisName
				) {
				InputComponent->AxisBindings.RemoveAt(ExistingIndex);
			}
		}
	}
	InputComponent->AxisBindings.Add(AB);

	return true;
}

void UBPAsync_WaitAxisTask::UnregisterInputEvents(UInputComponent* InputComponent)
{
	// Unregister bindings registed by this task.
	for (int32 ExistingIndex = InputComponent->AxisBindings.Num() - 1; ExistingIndex >= 0; --ExistingIndex)
	{
		const FInputAxisBinding& ExistingBind = InputComponent->AxisBindings[ExistingIndex];
		const bool IsBoundToThis = ExistingBind.AxisDelegate.IsBoundToObject(this);
		if (ExistingBind.AxisName == ListenAxisName
			&& IsBoundToThis
			) {
			InputComponent->AxisBindings.RemoveAt(ExistingIndex);
		}
	}
}

void UBPAsync_WaitAxisTask::Internal_OnAxisEvent(float InAxisValue)
{
	if (!bFinished) {
		OnEventTriggered.Broadcast(InAxisValue);
	}
}

