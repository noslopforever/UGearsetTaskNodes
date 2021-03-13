#pragma once

#include "BPAsync_WaitInputEventBase.h"
#include "BPAsync_InputTouchTask.generated.h"

/** Async task to wait a touch input event.
*/
UCLASS()
class UBPAsync_InputTouchTask
    : public UBPAsync_WaitInputEventBase
{
    GENERATED_UCLASS_BODY()
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateDynMC, ETouchIndex::Type, InTouchIndex, FVector, InLocation, float, InDeltaSecondsWhenTouching);

    /** OnPressed event, called when the listenning key/action has been pressed.
     */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnPressed;

    /** OnReleased event, called when the listenning key/action has been pressed.
     */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnReleased;

    /** OnMove event, called when moving.
    */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnMove;

    /** OnTouching event, called at each tick when the key is touching.
    */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnTouching;

    /** If true, remove exist bindings with the same key/action name.
    */
    UPROPERTY()
    bool bOverrideConflictBindings;

    FVector FingerStatus[ETouchIndex::MAX_TOUCHES];

public:

    /** Create a WaitKeyTask.
     */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject", DefaultToSelf = "InTaskOwner"))
    static UBPAsync_InputTouchTask* CreateInputTouchTask(UObject* InWorldContextObject
        , AActor* InTaskOwner
        );

protected:

    // Begin UBPAsync_WaitInputEventBase interfaces
    virtual bool RegisterInputEvents(UInputComponent* InputComponent) override;
    virtual void UnregisterInputEvents(UInputComponent* InputComponent) override;
    // ~ End UBPAsync_WaitInputEventBase interfaces

public:
    // Begin TTickTaskLatentActionHelper functions
    bool DoTickTask(float InDeltaSeconds);
    FString GetTickTaskDescription() const;
    // ~ End TTickTaskLatentActionHelper functions

private:
    /** Listener binds to the InputComponent */
    void Internal_OnTouchEvent(ETouchIndex::Type FingerIndex, FVector Location, EInputEvent InInputEvent);
    
};
