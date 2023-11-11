#pragma once

#include "Components/TimelineComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Base.generated.h"

class UCameraComponent;
class UCurveFloat;
class UInputComponent;
class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class USoundCue;

UCLASS()
class CLICKER_API APawn_Base : public APawn
{
	GENERATED_BODY()

public:
	APawn_Base();

	virtual void Tick(float DeltaTime) override;

private:
	virtual void BeginPlay() override;
	FTimeline CurveTimeline;
	FTimerHandle EverySecond;

	//Components:
	UPROPERTY(EditDefaultsOnly) UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly) USoundCue* ClickSound;
	UPROPERTY(EditDefaultsOnly) UCurveFloat* CurveFloat;
	UPROPERTY(EditDefaultsOnly) USceneComponent* Root;
	UPROPERTY(EditDefaultsOnly) UStaticMeshComponent* ClickPowerButton;
	UPROPERTY(EditDefaultsOnly) UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly) UStaticMeshComponent* PassiveIncomeButton;
	UPROPERTY(EditDefaultsOnly) UTextRenderComponent* ClickPowerTxt;
	UPROPERTY(EditDefaultsOnly) UTextRenderComponent* PassiveIncomeTxt;
	UPROPERTY(EditDefaultsOnly) UTextRenderComponent* ScoreText;

	//Variables:
	UPROPERTY() bool ClickPowerAvailable;
	UPROPERTY() bool PassiveIncomeAvailable;
	UPROPERTY() int32 Score;
	UPROPERTY() int32 ClickPower;
	UPROPERTY() int32 ClickPowerAdds;
	UPROPERTY() int32 ClickPowerCost;
	UPROPERTY() int32 PassiveIncome;
	UPROPERTY() int32 PassiveIncomeAdds;
	UPROPERTY() int32 PassiveIncomeCost;

	//Functions:
	UFUNCTION() void AddScorePerSecond();
	UFUNCTION() void BuyClickPower(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION() void BuyScorePerSecond(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION() void ClickEvent(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);	
	UFUNCTION() void TimelineProgress(float Value) const;
	UFUNCTION() void UpdateShop();
};
