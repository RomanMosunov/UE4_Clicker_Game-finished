#include "Pawn_Base.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

APawn_Base::APawn_Base()
{
	//Event Tick Enable
	PrimaryActorTick.bCanEverTick = true;

	//Spawn Components
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ClickPowerTxt = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ClickPowerAddsTxt"));
	ClickPowerButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClickPowerBtn"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	PassiveIncomeTxt = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PassiveIncomeAddsTxt"));
	PassiveIncomeButton = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PassiveIncomeButton"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText"));

	//Find Objects
	static ConstructorHelpers::FObjectFinder<UCurveFloat> C_AnimTrack(TEXT("/Game/Materials/C_AnimTrack"));
	static ConstructorHelpers::FObjectFinder<USoundCue> Click_cue(TEXT("/Game/Sounds/Click_cue"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MI_Base(TEXT("/Game/Materials/MI_Base"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Cube(TEXT("/Game/Materials/SM_Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Sphere(TEXT("/Game/Materials/SM_Sphere"));

	//Check Components
	check(C_AnimTrack.Object);
	check(Camera);
	check(Click_cue.Object);
	check(ClickPowerTxt);
	check(ClickPowerButton);
	check(Mesh);
	check(MI_Base.Object);
	check(PassiveIncomeTxt);
	check(PassiveIncomeButton);
	check(Root);
	check(ScoreText);
	check(SM_Cube.Object);
	check(SM_Sphere.Object);

	ClickSound = Click_cue.Object;
	CurveFloat = C_AnimTrack.Object;

	SetRootComponent(Root);

	Camera->SetupAttachment(Root);
	Camera->SetRelativeTransform(FTransform(FRotator::ZeroRotator, FVector::OneVector , FVector::OneVector));

	Mesh->SetupAttachment(Root);
	Mesh->SetRelativeTransform(FTransform(FRotator::ZeroRotator, FVector(600.0f, -300.0f, -100.0f), FVector::OneVector));
	Mesh->SetStaticMesh(SM_Sphere.Object);
	Mesh->SetMaterial(0, MI_Base.Object);
	Mesh->OnClicked.AddDynamic(this, &ThisClass::ClickEvent);
	//Mesh->OnClicked.AddDynamic(this, &APawn_Base::ClickEvent);

	ScoreText->SetupAttachment(Root);
	ScoreText->SetRelativeTransform(FTransform(FRotator(0.0f, 180.0f, 0.0f), FVector(600.0f, -300.0f, 100.0f), FVector::OneVector));
	ScoreText->SetWorldSize(100);
	ScoreText->SetText(FText::FromString(TEXT("Click to start")));
	//ScoreText->SetText(TEXT("Click to start"));
	
	ClickPowerButton->SetupAttachment(Root);
	ClickPowerButton->SetStaticMesh(SM_Cube.Object);
	ClickPowerButton->SetRelativeTransform(FTransform(FRotator(0.0, 180.0, 0.0), FVector(600.0f, 0.0f, 200.0f), FVector::OneVector));
	ClickPowerButton->OnClicked.AddDynamic(this, &ThisClass::BuyClickPower);
	ClickPowerButton->SetMaterial(0, MI_Base.Object);
	
	ClickPowerTxt->SetupAttachment(ClickPowerButton);
	ClickPowerTxt->SetRelativeTransform(FTransform(FRotator::ZeroRotator, FVector(0.0f, -150.0f, -50.0f), FVector::OneVector));
	ClickPowerTxt->SetWorldSize(50);

	PassiveIncomeButton->SetupAttachment(Root);
	PassiveIncomeButton->SetStaticMesh(SM_Cube.Object);
	PassiveIncomeButton->SetRelativeTransform(FTransform(FRotator(0.0, 180.0, 0.0), FVector(600.0f, 0.0f, 0.0f), FVector::OneVector));
	PassiveIncomeButton->OnClicked.AddDynamic(this, &ThisClass::BuyScorePerSecond);
	PassiveIncomeButton->SetMaterial(0, MI_Base.Object);

	PassiveIncomeTxt->SetupAttachment(PassiveIncomeButton);
	PassiveIncomeTxt->SetRelativeTransform(FTransform(FRotator::ZeroRotator, FVector(0.0f, -150.0f, -50.0f), FVector::OneVector));
	PassiveIncomeTxt->SetWorldSize(50);

	//Set Variables
	Score = 0;
	ClickPower = 1;
	ClickPowerCost = 4;
	ClickPowerAdds = 1;
	PassiveIncome = 0;
	PassiveIncomeCost = 10;
	PassiveIncomeAdds = 1;
}																									  

void APawn_Base::BeginPlay()
{
	Super::BeginPlay();

	//Bind Timeline Progress
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	if (!EverySecond.IsValid()) 
	{
		GetWorldTimerManager().SetTimer(EverySecond, this, &ThisClass::AddScorePerSecond, 1.0, true);
	}
	APawn_Base::UpdateShop();
}

void APawn_Base::UpdateShop()
{
	FString Active = FString::Printf(TEXT("Click Power: %d \n Next Level: %d"), ClickPower, ClickPowerCost);
	FString Passive = FString::Printf(TEXT("Passive Income: %d \n Next Level: %d"), PassiveIncome, PassiveIncomeCost);

	ClickPowerTxt->SetText(FText::FromString(Active));
	PassiveIncomeTxt->SetText(FText::FromString(Passive));
	ScoreText->SetText(FText::AsNumber(Score));

	//Set Material color
	if (Score >= ClickPowerCost)
	{
		ClickPowerAvailable = true;
		ClickPowerButton->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(0.0, 1.0, 0.0));
	}
	else
	{
		ClickPowerAvailable = false;
		ClickPowerButton->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(1.0, 0.0, 0.0));
	}

	if (Score >= PassiveIncomeCost)
	{
		PassiveIncomeAvailable = true;
		PassiveIncomeButton->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(0.0, 1.0, 0.0));
	}
	else
	{
		PassiveIncomeAvailable = false;
		PassiveIncomeButton->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(1.0, 0.0, 0.0));
	}
	UE_LOG(LogTemp, Display, TEXT("UpdatedScore = %d"), Score);
}

void APawn_Base::AddScorePerSecond()
{
	if (PassiveIncome > 0)
	{
		Score += PassiveIncome;
		APawn_Base::UpdateShop();
	}
}

void APawn_Base::BuyClickPower(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (ClickPowerAvailable)
	{
		Score -= ClickPowerCost;
		ClickPower += ClickPowerAdds;
		ClickPowerCost *= 2;
		APawn_Base::UpdateShop();
	}
}

void APawn_Base::BuyScorePerSecond(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (PassiveIncomeAvailable)
	{
		Score -= PassiveIncomeCost;
		PassiveIncome += 1;
		PassiveIncomeCost *= 2;
		APawn_Base::UpdateShop();
	}
}

//Update Mesh By OnClick
void APawn_Base::TimelineProgress(const float Value) const
{
	const FVector StartScale = FVector::OneVector;
	const FVector EndScale = FVector(0.6, 0.6, 0.6);
	const FVector NewScale = FMath::Lerp(StartScale, EndScale, Value);
	Mesh->SetWorldScale3D(NewScale);

	const FVector StartColor = FVector(1.0, 0.0, 0.8);
	const FVector EndColor = FVector(1.0, 0.0, 0.0);
	const FVector NewColor = FMath::Lerp(StartColor, EndColor, Value);
	Mesh->SetVectorParameterValueOnMaterials(TEXT("Color"), NewColor);
}

//Add Score
void APawn_Base::ClickEvent(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	Score += ClickPower;
	ScoreText->SetText(FText::AsNumber(Score));
	CurveTimeline.PlayFromStart();
	UGameplayStatics::PlaySound2D(this, ClickSound);
	APawn_Base::UpdateShop();
}

void APawn_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurveTimeline.TickTimeline(DeltaTime);
}
