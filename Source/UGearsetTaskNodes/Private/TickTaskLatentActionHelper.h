#include "Engine/Engine.h"
#include "Engine/World.h"
#include "LatentActions.h"
#include "Engine/LatentActionManager.h"

/** helper for the TaskClass who needs tick listenners.
*	These TaskClasses must implement:
*	- bool DoTickTask(float DeltaSeconds)
*		return FALSE if the task has been finished.
*	- FString GetTickTaskDescription()
*		return some text for debugging.
*/
template<class TTaskClass>
class TTickTaskLatentActionHelper
	: public FPendingLatentAction
{
public:
	TWeakObjectPtr<TTaskClass> TaskPtr;

	TTickTaskLatentActionHelper(TTaskClass* InWaitTickTask)
		: TaskPtr(InWaitTickTask)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		TTaskClass* TickTask = TaskPtr.Get();
		if (!TickTask) {
			Response.DoneIf(true);
			return;
		}

		// Trigger tick task.
		const float DeltaSeconds = Response.ElapsedTime();
		const bool bStillTicking = TickTask->DoTickTask(DeltaSeconds);
		if (!bStillTicking) {
			Response.DoneIf(true);
			return;
		}
	}

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		// Show elapsed time.
		float TimeElapsed = -1;

		FString TickTaskDesc = "";
		TTaskClass* TickTask = TaskPtr.Get();
		if (TickTask) {
			TickTaskDesc = TickTask->GetTickTaskDescription();
		}

		if (TickTaskDesc == "") {
			TickTaskDesc = NSLOCTEXT("TickTaskHelper"
				, "DefaultTickTaskHelperTimeFmt"
				, "Here is a internal tick.")
				.ToString();
		}
		return TickTaskDesc;
	}
#endif

    static void StaticAddNewAction(AActor* InTaskOwner, TTaskClass* InTask, int32 InUUID)
    {
		// Register as a latent action
		if (UWorld* World = GEngine->GetWorldFromContextObject(InTaskOwner, EGetWorldErrorMode::LogAndReturnNull)) {
			FLatentActionManager& LatentManager = World->GetLatentActionManager();
			TTickTaskLatentActionHelper<TTaskClass>* NewAction = new TTickTaskLatentActionHelper<TTaskClass>(InTask);
			LatentManager.AddNewAction(InTaskOwner, InUUID, NewAction);
		}
    }

};

