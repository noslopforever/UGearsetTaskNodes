#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "InputCoreTypes.h"
#include "Components/InputComponent.h"
#include "BPAsync_WaitInputEventBase.generated.h"

/** Async action for waiting a input event.
*/
UCLASS(Abstract, meta=(ExposedAsyncProxy = InputTask))
class UBPAsync_WaitInputEventBase
    : public UBlueprintAsyncActionBase
{
    GENERATED_UCLASS_BODY()
public:

    /** Owner of the task
    */
    UPROPERTY()
    TWeakObjectPtr<AActor> TaskOwner;

    /** Is event registered.
    */
    UPROPERTY(BlueprintReadOnly)
    bool bEventRegistered;

    /** Finished
    */
    UPROPERTY(BlueprintReadOnly)
    bool bFinished;

public:
    // Begin Object interfaces
    virtual void BeginDestroy() override;
    // ~ End Object interfaces
    // Begin UBlueprintAsyncActionBase interface
    virtual void Activate() override;
    // ~ End UBlueprintAsyncActionBase interface

    /** Enable/Disable the input event listener, but hold the task so it can be accessed in future.
    */
    UFUNCTION(BlueprintCallable)
    void SetEnabled(bool InEnable = true);

    /** Called in Blueprint to end and destroy the task.
    */
    UFUNCTION(BlueprintCallable)
    void DestroyTask();

protected:

    /** Bind key events.
    */
    virtual bool RegisterInputEvents(UInputComponent* InputComponent)
        PURE_VIRTUAL(UBPAsync_WaitInputEventBase::RegisterInputEvents, return false; )

    /** Unbind key events.
    */
    virtual void UnregisterInputEvents(UInputComponent* InputComponent)
        PURE_VIRTUAL(UBPAsync_WaitInputEventBase::UnregisterInputEvents, )

private:
    /** Bind key events. */
    void _RegInputEvents();
    /** Unbind key events. */
    void _UnregInputEvents();

};

FORCEINLINE bool IsBoundByUObject(const FInputActionBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.ActionDelegate.IsBoundToObject(InObject);
}

FORCEINLINE bool IsBoundByUObject(const FInputKeyBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.KeyDelegate.IsBoundToObject(InObject);
}

FORCEINLINE bool IsBoundByUObject(const FInputTouchBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.TouchDelegate.IsBoundToObject(InObject);
}

FORCEINLINE bool IsBoundByUObject(const FInputAxisBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.AxisDelegate.IsBoundToObject(InObject);
}

FORCEINLINE bool IsBoundByUObject(const FInputAxisKeyBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.AxisDelegate.IsBoundToObject(InObject);
}

FORCEINLINE bool IsBoundByUObject(const FInputVectorAxisBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.AxisDelegate.IsBoundToObject(InObject);
}
FORCEINLINE bool IsBoundByUObject(const FInputGestureBinding& ExistingBind, UObject* InObject)
{
    return ExistingBind.GestureDelegate.IsBoundToObject(InObject);
}



