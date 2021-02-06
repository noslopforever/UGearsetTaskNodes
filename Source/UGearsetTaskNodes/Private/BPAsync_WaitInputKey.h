#pragma once

#include "BPAsync_WaitInputEventBase.h"
#include "BPAsync_WaitInputKey.generated.h"

/** Async task to wait a key input event.
*/
UCLASS()
class UBPAsync_WaitKeyTask
    : public UBPAsync_WaitInputEventBase
{
    GENERATED_UCLASS_BODY()
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDelegateDynMC, struct FKey, InKey, FName, InActionName);

    /** OnPressed event, called when the listenning key/action has been pressed.
     */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnPressed;

    /** OnRelesaed event, called when the listenning key/action has been pressed.
     */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnReleased;

    /** Keys to listen
    */
    UPROPERTY()
    TArray<FInputChord> ListenChords;

    /** Actions to listen
    */
    UPROPERTY()
    FName ListenAction;

    /** If true, remove exist bindings with the same key/action name.
    */
    UPROPERTY()
    bool bOverrideConflictBindings;

public:

    /** Create a WaitKeyTask.
     */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject", DefaultToSelf = "InTaskOwner"))
    static UBPAsync_WaitKeyTask* CreateWaitInputKeyTask(UObject* InWorldContextObject
        , AActor* InTaskOwner
        , FKey InKey
        );

    /** Create a WaitActionTask.
     */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject", DefaultToSelf = "InTaskOwner"))
    static UBPAsync_WaitKeyTask* CreateWaitInputActionTask(UObject* InWorldContextObject
        , AActor* InTaskOwner
        , FName InActionName
        );

protected:

    // Begin UBPAsync_WaitInputEventBase interfaces
    virtual bool RegisterInputEvents(UInputComponent* InputComponent) override;
    virtual void UnregisterInputEvents(UInputComponent* InputComponent) override;
    // ~ End UBPAsync_WaitInputEventBase interfaces

private:

    void Internal_OnKeyEvent(FKey InKey, FName InActionName, EInputEvent InInputEvent);

};