#include "GameMode_Base.h"
#include "PlayerController_Base.h"
#include "Pawn_Base.h"
//#include "UObject/ConstructorHelpers.h"

AGameMode_Base::AGameMode_Base()
{
		// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Pawn"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	DefaultPawnClass = APawn_Base::StaticClass();
	PlayerControllerClass = APlayerController_Base::StaticClass();
}
