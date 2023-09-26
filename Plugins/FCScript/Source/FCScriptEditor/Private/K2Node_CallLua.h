
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Textures/SlateIcon.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "K2Node_CallFunction.h"
#include "EdGraph/EdGraphPin.h"

#include "K2Node_CallLua.generated.h"

class FBlueprintActionDatabaseRegistrar;

UCLASS()
class FCSCRIPTEDITOR_API UK2Node_CallLua : public UK2Node_CallFunction, public IK2Node_AddPinInterface
{
	GENERATED_UCLASS_BODY()

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ Begin UK2Node Interface
	void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual FName GetCornerIcon() const override{return FName();}
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	// IK2Node_AddPinInterface interface
	virtual void AddInputPin() override;
	// End of IK2Node_AddPinInterface interface

	//~ Begin K2Node_ExecutionSequence Interface
	virtual bool CanRemovePin(const UEdGraphPin* Pin) const ;
	virtual bool CanAddPin() const override;
	/** Gets a unique pin name, the next in the sequence */
	FName GetUniquePinName();


	virtual void PostReconstructNode() override;	
	/**
	 * Removes the specified execution pin from an execution node
	 *
	 * @param	TargetPin	The pin to remove from the node
	 */
	void RemovePinFromExecutionNode(UEdGraphPin* TargetPin);

private:

	void UpdateVariadicNum();
	void SynchronizeArgumentPinType(UEdGraphPin* Pin);	
	// Returns the exec output pin name for a given 0-based index
	virtual FName GetPinNameGivenIndex(int32 Index) const;

	bool IsParamInputPin(const UEdGraphPin* TargetPin) const;
	int GetParamIndex(UEdGraphPin* TargetPin) const;
	FString ParamPinPrefixed() const;



	UPROPERTY()
	int32 VariadicNum;
};
