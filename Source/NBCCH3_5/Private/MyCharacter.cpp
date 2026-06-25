// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "EnhancedInputComponent.h"
#include "MyGameState.h"
#include "MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;  
	SpringArmComp->bUsePawnControlRotation = true;  

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
    
    //위젯컴포
    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(RootComponent);
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    
    //달리기 관련 기본값
    NormalSpeed = 600.0f;
    SprintSpeedMultiplier = 1.5f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    
    // 초기 체력
    MaxHealth = 100.0f;
    Health = MaxHealth;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
    
    UpdateOverheadHP();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅
        if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AMyCharacter::Move
                );
            }
            
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AMyCharacter::StartJump
                );
                
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &AMyCharacter::StopJump
                );
            }
            
            if (PlayerController->LookAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AMyCharacter::Look
                );
            }
            
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Triggered, 
                    this, 
                    &AMyCharacter::StartSprint
                );
                
                EnhancedInput->BindAction(
                    PlayerController->SprintAction, 
                    ETriggerEvent::Completed, 
                    this, 
                    &AMyCharacter::StopSprint
                );
            }    
        }
    }
}

void AMyCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return; //컨트롤러를 못찾으면 리턴
    
    const FVector2D MoveInput = value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

void AMyCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>())
    {
        Jump(); //기본제공함수
    }
}

void AMyCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
    {
        StopJumping(); //기본제공함수
    }
}

void AMyCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();

    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

void AMyCharacter::StartSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void AMyCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

int32 AMyCharacter::GetHealth() const
{
    return Health;
}

void AMyCharacter::AddHealth(float Amount)
{
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth); //최대 체력 넘지 않도록
    
    UpdateOverheadHP();
    UE_LOG(LogTemp, Log, TEXT("Health increased to: %f"), Health);
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); //부모제공 데미지처리 함수

    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth); //최소체력 넘지 않도록
    
    UpdateOverheadHP();
    UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);
    
    if (Health <= 0.0f) //체력이 다 닳으면 사망
    {
        OnDeath();
    }
    
    return ActualDamage;
}

// 사망 처리 함수
void AMyCharacter::OnDeath()
{
    AMyGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
    if (SpartaGameState)
    {
        SpartaGameState->OnGameOver();
    }
    UE_LOG(LogTemp, Error, TEXT("Character is Dead!"));
}

void AMyCharacter::UpdateOverheadHP()
{
    if (!OverheadWidget) return;
	
    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;
	
    if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
    }
}