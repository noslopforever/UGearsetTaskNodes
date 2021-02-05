#include "BPAsync_WaitInputEventBase.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UBPAsync_WaitInputEventBase::UBPAsync_WaitInputEventBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bEventRegistered(false)
	, bFinished(false)
{
}

void UBPAsync_WaitInputEventBase::BeginDestroy()
{
	// The task is going to destroy but the owner is valid, unregister tasks.
	if (TaskOwner.IsValid()) {
		_UnregInputEvents();
	}

	Super::BeginDestroy();
}

void UBPAsync_WaitInputEventBase::Activate()
{
	_RegInputEvents();
}

void UBPAsync_WaitInputEventBase::SetEnabled(bool InEnable /*= true*/)
{
	if (InEnable) {
		_RegInputEvents();
	}
	else {
		_UnregInputEvents();
	}
}

void UBPAsync_WaitInputEventBase::DestroyTask()
{
	bFinished = true;
	_UnregInputEvents();

	SetReadyToDestroy();
}

void UBPAsync_WaitInputEventBase::_RegInputEvents()
{
	// Only registered once.
	if (bEventRegistered) {
		return;
	}

	// Exact InputComponent from TaskOwner.
	AActor* Owner = TaskOwner.Get();
	if (!Owner) {
		UE_LOG(LogScript, Error
			, TEXT("Reg WaitKeyEvent : The holder of WaitKeyTask is invalid.")
		);
		return;
	}
	if (!Owner->InputComponent) {
		UE_LOG(LogScript, Warning
			, TEXT("Reg WaitKeyEvent : Cannot find valid InputComponent. EnableInput first.")
		);
		return;
	}
	UInputComponent* InputComponent = Owner->InputComponent;

	// Notify derived classes.
	bEventRegistered = RegisterInputEvents(InputComponent);
}

void UBPAsync_WaitInputEventBase::_UnregInputEvents()
{
	// Exact InputComponent from TaskOwner.
	AActor* Owner = TaskOwner.Get();
	if (!Owner) {
		UE_LOG(LogScript, Error
			, TEXT("Unreg WaitKeyEvent : The holder of WaitKeyTask is invalid.")
		);
		return;
	}
	if (!Owner->InputComponent) {
		UE_LOG(LogScript, Warning
			, TEXT("Unreg WaitKeyEvent : Cannot find valid InputComponent. EnableInput first.")
		);
		return;
	}
	UInputComponent* InputComponent = Owner->InputComponent;

	// Notify derived classes.
	UnregisterInputEvents(InputComponent);
	bEventRegistered = false;
}

