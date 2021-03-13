#include "BPAsync_InputTouchTask.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TickTaskLatentActionHelper.h"

UBPAsync_InputTouchTask::UBPAsync_InputTouchTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bOverrideConflictBindings(false)
{
}

UBPAsync_InputTouchTask* UBPAsync_InputTouchTask::CreateInputTouchTask(UObject* InWorldContextObject
    , AActor* InTaskOwner
    )
{
	// Factory create instance
	UBPAsync_InputTouchTask* InputEvent = ::NewObject<UBPAsync_InputTouchTask>(GetTransientPackage());
	{
		InputEvent->TaskOwner = InTaskOwner;

		static int32 UUID = GetTypeHash(UBPAsync_InputTouchTask::StaticClass());
		TTickTaskLatentActionHelper<UBPAsync_InputTouchTask>::StaticAddNewAction(InTaskOwner
			, InputEvent
			, UUID
		);
	}
	return InputEvent;
}

bool UBPAsync_InputTouchTask::RegisterInputEvents(UInputComponent* InputComponent)
{
	static EInputEvent ListenInputEvents[] = { IE_Pressed , IE_Released, IE_Repeat };

	//
	// ## Bind key events.
	//
	TArray<FInputTouchBinding> TouchBindsToAdd;

	for (int32 ListenEventIndex = 0; ListenEventIndex < UE_ARRAY_COUNT(ListenInputEvents); ListenEventIndex++) {
		EInputEvent ListenInputEvent = ListenInputEvents[ListenEventIndex];

		FInputTouchBinding TB(ListenInputEvent);
		TB.bConsumeInput = true;
		TB.bExecuteWhenPaused = false;
		TB.TouchDelegate.GetDelegateForManualSet()
			.BindUObject(this, &UBPAsync_InputTouchTask::Internal_OnTouchEvent, ListenInputEvent);
		const bool IsBound = TB.TouchDelegate.IsBound();
		ensure(IsBound);

		// Override conflict bindings.
		if (bOverrideConflictBindings) {
			for (int32 ExistingIndex = InputComponent->TouchBindings.Num() - 1; ExistingIndex >= 0; --ExistingIndex) {
				// Find conflict binding (in fact, it means all TouchBindings registered in this InputComponent).
				const FInputTouchBinding& ExistingBind = InputComponent->TouchBindings[ExistingIndex];
				if (ExistingBind.KeyEvent == ListenInputEvent) {
					// Remove it (can not restore!)
					InputComponent->TouchBindings.RemoveAt(ExistingIndex);
				}
			}
		}

		// To avoid binds in the same layer being removed by the parent override temporarily put them in this array and add later
		TouchBindsToAdd.Add(TB);
	}

	for (int32 Index = 0; Index < TouchBindsToAdd.Num(); ++Index) {
		InputComponent->TouchBindings.Add(TouchBindsToAdd[Index]);
	}

	return true;
}

void UBPAsync_InputTouchTask::UnregisterInputEvents(UInputComponent* InputComponent)
{
	// Unregister bindings registed by this task.

	for (int32 ExistingIndex = InputComponent->TouchBindings.Num() - 1; ExistingIndex >= 0; --ExistingIndex) {
		const FInputTouchBinding& ExistingBind = InputComponent->TouchBindings[ExistingIndex];

		const bool IsBoundToThis = ExistingBind.TouchDelegate.IsBoundToObject(this);
		if (IsBoundToThis) {
			// Remove the binding bound to this task.
			InputComponent->TouchBindings.RemoveAt(ExistingIndex);
		}
	}

}

void UBPAsync_InputTouchTask::Internal_OnTouchEvent(ETouchIndex::Type FingerIndex, FVector Location, EInputEvent InInputEvent)
{
	FingerStatus[FingerIndex] = Location;

	if (!bFinished) {
		switch (InInputEvent)
		{
		case IE_Pressed:
			OnPressed.Broadcast(FingerIndex, Location, 0);
			break;
		case IE_Released:
			OnReleased.Broadcast(FingerIndex, Location, 0);
			break;
		case IE_Repeat:
			OnMove.Broadcast(FingerIndex, Location, 0);
			break;
		}

	}

}

bool UBPAsync_InputTouchTask::DoTickTask(float InDeltaSeconds)
{
	for (int32 FingerIndex = 0; FingerIndex < ETouchIndex::MAX_TOUCHES; FingerIndex++) {
		const FVector& TheFingerState = FingerStatus[FingerIndex];
		// Z > 0 means pressing.
		if (TheFingerState.Z > 0) {
			OnTouching.Broadcast((ETouchIndex::Type)FingerIndex, FingerStatus[FingerIndex], InDeltaSeconds);
		}
	}

	// Always tick
	return true;
}

FString UBPAsync_InputTouchTask::GetTickTaskDescription() const
{
	return "";
}
