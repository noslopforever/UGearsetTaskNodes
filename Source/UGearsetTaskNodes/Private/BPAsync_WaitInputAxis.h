#pragma once

#include "BPAsync_WaitInputEventBase.h"
#include "BPAsync_WaitInputAxis.generated.h"

/** Async action for waiting a input-axis.
*/
UCLASS()
class UBPAsync_WaitAxisTask
    : public UBPAsync_WaitInputEventBase
{
    GENERATED_UCLASS_BODY()
public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateDynMC, float, InAxisValue);

    /** On event triggered */
    UPROPERTY(BlueprintAssignable)
    FDelegateDynMC OnEventTriggered;

    /** Axis to listen
    */
    UPROPERTY()
    FName ListenAxisName;

    /** If true, remove exist bindings with the same axis name.
    */
    UPROPERTY()
    bool bOverrideConflictBindings;

public:

    /** Create a WaitAxis task.
     */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject", DefaultToSelf = "InTaskOwner"))
    static UBPAsync_WaitAxisTask* CreateWaitInputAxisTask(UObject* InWorldContextObject, AActor* InTaskOwner, FName InAxisName);

protected:

    // Begin UBPAsync_WaitInputEventBase interfaces
    virtual bool RegisterInputEvents(UInputComponent* InputComponent) override;
    virtual void UnregisterInputEvents(UInputComponent* InputComponent) override;
    // ~ End UBPAsync_WaitInputEventBase interfaces

private:

    UFUNCTION()
    void Internal_OnAxisEvent(float InAxisValue);

};