#include "BPAsync_WaitInputKey.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UBPAsync_WaitKeyTask::UBPAsync_WaitKeyTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bOverrideConflictBindings(false)
{
}

UBPAsync_WaitKeyTask* UBPAsync_WaitKeyTask::CreateWaitInputKeyTask(UObject* InWorldContextObject
    , AActor* InTaskOwner
    , FKey InKey
    )
{
	// Factory create instance
	UBPAsync_WaitKeyTask* InputEvent = ::NewObject<UBPAsync_WaitKeyTask>(GetTransientPackage());
	{
		InputEvent->TaskOwner = InTaskOwner;
		InputEvent->ListenChords.Add(FInputChord(InKey));
	}
	return InputEvent;
}

UBPAsync_WaitKeyTask* UBPAsync_WaitKeyTask::CreateWaitInputActionTask(UObject* InWorldContextObject
    , AActor* InTaskOwner
	, FName InActionName
	)
{
	// Factory create instance
	UBPAsync_WaitKeyTask* InputEvent = ::NewObject<UBPAsync_WaitKeyTask>(GetTransientPackage());
	{
		InputEvent->TaskOwner = InTaskOwner;
		InputEvent->ListenAction = InActionName;
	}
	return InputEvent;
}

bool UBPAsync_WaitKeyTask::RegisterInputEvents(UInputComponent* InputComponent)
{
	static EInputEvent ListenInputEvents[] = { IE_Pressed , IE_Released };

	//
	// ## Bind key events.
	//
	TArray<FInputKeyBinding> KeyBindsToAdd;

	for (int32 BindIndex = 0; BindIndex < ListenChords.Num(); ++BindIndex) {
		FInputChord ListenChord = ListenChords[BindIndex];

		for (int32 ListenEventIndex = 0; ListenEventIndex < UE_ARRAY_COUNT(ListenInputEvents); ListenEventIndex++) {
			EInputEvent ListenInputEvent = ListenInputEvents[ListenEventIndex];

			FInputKeyBinding KB(ListenChord, ListenInputEvent);
			KB.bConsumeInput = true;
			KB.bExecuteWhenPaused = false;
			KB.KeyDelegate.GetDelegateWithKeyForManualSet()
				.BindUObject(this, &UBPAsync_WaitKeyTask::Internal_OnKeyEvent, FName(NAME_None), ListenInputEvent);
			const bool IsBound = KB.KeyDelegate.IsBound();
			ensure(IsBound);

			// Override conflict bindings.
			if (bOverrideConflictBindings) {
				for (int32 ExistingIndex = InputComponent->KeyBindings.Num() - 1; ExistingIndex >= 0; --ExistingIndex) {
					// Find conflict binding (same chord, same event).
					const FInputKeyBinding& ExistingBind = InputComponent->KeyBindings[ExistingIndex];
					if (ExistingBind.Chord == ListenChord
						&& ExistingBind.KeyEvent == ListenInputEvent
						) {
						// Remove it (can not restore!)
						InputComponent->KeyBindings.RemoveAt(ExistingIndex);
					}
				}
			}

			// To avoid binds in the same layer being removed by the parent override temporarily put them in this array and add later
			KeyBindsToAdd.Add(KB);
		}
	}

	for (int32 Index = 0; Index < KeyBindsToAdd.Num(); ++Index) {
		InputComponent->KeyBindings.Add(KeyBindsToAdd[Index]);
	}

	//
	// ## Bind action events.
	//
	if (ListenAction != NAME_Name) {
		TArray<FInputActionBinding> ActionBindsToAdd;

		for (int32 ListenEventIndex = 0; ListenEventIndex < UE_ARRAY_COUNT(ListenInputEvents); ListenEventIndex++) {
			EInputEvent ListenInputEvent = ListenInputEvents[ListenEventIndex];

			FInputActionBinding AB(ListenAction, ListenInputEvent);
			AB.bConsumeInput = true;
			AB.bExecuteWhenPaused = false;
			AB.ActionDelegate.GetDelegateWithKeyForManualSet()
				.BindUObject(this, &UBPAsync_WaitKeyTask::Internal_OnKeyEvent, ListenAction, ListenInputEvent);
			const bool IsBound = AB.ActionDelegate.IsBound();
			ensure(IsBound);
			//{
			//	FInputActionHandlerWithKeySignature IAHWKS = FInputActionHandlerWithKeySignature::CreateUObject(this, &UBPAsync_WaitKeyTask::Internal_OnKeyEvent, ListenAction, ListenInputEvent);

			//	UObject* Object = IAHWKS.GetUObject();
			//	const bool IsBound = IAHWKS.IsBound();
			//	FName BoundFuncName = IAHWKS.TryGetBoundFunctionName();
			//	IDelegateInstance* DelInst = IAHWKS.GetDelegateInstance();
			//	auto Handle = IAHWKS.GetHandle();

			//	ensure(IsBound && Object && DelInst && Handle.IsValid() && BoundFuncName != NAME_None);
			//}

			if (bOverrideConflictBindings) {
				for (int32 ExistingIndex = InputComponent->GetNumActionBindings() - 1; ExistingIndex >= 0; --ExistingIndex)
				{
					const FInputActionBinding& ExistingBind = InputComponent->GetActionBinding(ExistingIndex);
					if (ExistingBind.GetActionName() == AB.GetActionName()
						&& ExistingBind.KeyEvent == AB.KeyEvent
						) {
						InputComponent->RemoveActionBinding(ExistingIndex);
					}
				}
			}

			// To avoid binds in the same layer being removed by the parent override temporarily put them in this array and add later
			ActionBindsToAdd.Add(AB);
		}

		for (int32 Index = 0; Index < ActionBindsToAdd.Num(); ++Index) {
			InputComponent->AddActionBinding(ActionBindsToAdd[Index]);
		}
	}

	return true;
}

void UBPAsync_WaitKeyTask::UnregisterInputEvents(UInputComponent* InputComponent)
{
	// Unregister bindings registed by this task.
	for (int32 BindIndex = 0; BindIndex < ListenChords.Num(); ++BindIndex) {
		FInputChord ListenChord = ListenChords[BindIndex];

		// Find bindings registed by this task.
		for (int32 ExistingIndex = InputComponent->KeyBindings.Num() - 1; ExistingIndex >= 0; --ExistingIndex) {
			const FInputKeyBinding& ExistingBind = InputComponent->KeyBindings[ExistingIndex];

			const bool IsBoundToThis = ExistingBind.KeyDelegate.IsBoundToObject(this);
			if (ExistingBind.Chord == ListenChord
				&& IsBoundToThis
				) {
				// Remove the binding bound to this task.
				InputComponent->KeyBindings.RemoveAt(ExistingIndex);
			}
		}
	}

	// ## Unreg ActionBindings registed by this class.
	if (ListenAction != NAME_Name) {
		for (int32 ExistingIndex = InputComponent->GetNumActionBindings() - 1; ExistingIndex >= 0; --ExistingIndex)
		{
			const FInputActionBinding& ExistingBind = InputComponent->GetActionBinding(ExistingIndex);

			const bool IsBoundToThis = ExistingBind.ActionDelegate.IsBoundToObject(this);
			if (ExistingBind.GetActionName() == ListenAction
				&& IsBoundToThis
				) {
				// Remove the binding bound to this task.
				InputComponent->RemoveActionBinding(ExistingIndex);
			}
		}
	}

}

void UBPAsync_WaitKeyTask::Internal_OnKeyEvent(FKey InKey, FName InActionName, EInputEvent InInputEvent)
{
	if (!bFinished) {
		switch (InInputEvent)
		{
		case IE_Pressed:
			OnPressed.Broadcast(InKey, InActionName);
			break;
		case IE_Released:
			OnReleased.Broadcast(InKey, InActionName);
			break;
		}

	}

}

