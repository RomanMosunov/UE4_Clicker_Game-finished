#include "PlayerController_Base.h"

// Called when the game starts or when spawned
void APlayerController_Base::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}