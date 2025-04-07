// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCrawlerCharacter.h"

#include <string>

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Public/ChangeHealthUIDisplayMessage.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADungeonCrawlerCharacter

ADungeonCrawlerCharacter::ADungeonCrawlerCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//StandardAttributes = CreateDefaultSubobject<UStandardAttributeSet>(TEXT("StandardAttributeSet"));
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ADungeonCrawlerCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	AbilitySystemComponent = NewObject<UAbilitySystemComponent>(this,TEXT("AbilitySystemComponent"));
	//StandardAttributes = NewObject<UStandardAttributeSet>(this, TEXT("StandardAttributeSet"));
}

void ADungeonCrawlerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet is: %s"), *GetNameSafe(StandardAttributes));

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (HasAuthority())
	{
		SetupInitialAbilitiesAndEffects();
	}
}

void ADungeonCrawlerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	TeamId = FGenericTeamId(Faction);
}

void ADungeonCrawlerCharacter::SetupInitialAbilitiesAndEffects()
{
	if(IsValid(AbilitySystemComponent) == false)
	{
		return;
	}

	if(IsValid(InitialAbilitySet))
	{
		if (IsValid(InitialAbilitySet->ClassAttributes))
		{
			StandardAttributes = NewObject<UStandardAttributeSet>(this, InitialAbilitySet->ClassAttributes);
			AbilitySystemComponent->AddAttributeSetSubobject(StandardAttributes);
			// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStandardAttributeSet::GetDamageAttribute())
			//                       .AddUObject(ClassAttributes, &UClassAttributeSet::DamageChangeDelegate);
		}
		else
		{
			StandardAttributes = NewObject<UStandardAttributeSet>(this, TEXT("StandardAttributeSet"));
			AbilitySystemComponent->AddAttributeSetSubobject(StandardAttributes);

		}
		InitiallyGrantedAbilitySpecHandles.Append(InitialAbilitySet->GrantAbilitiesToAbilitySystem(AbilitySystemComponent));
	}

	if(IsValid(InitialGameplayEffect))
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(InitialGameplayEffect->GetDefaultObject<UGameplayEffect>(), 0.f, AbilitySystemComponent->MakeEffectContext());
	}
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStandardAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthAttributeChanged);

}

void ADungeonCrawlerCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	FGameplayTag ChannelTag = FGameplayTag::RequestGameplayTag(TEXT("Lyra.StatChange.Health.Message")); // You will need to assign this value as appropriate for your code
	FChangeHealthUIDisplayMessage OutgoingMessage; // You will need to define the structure and fill out any members with data as appropriate for your code
	const float CurrHealthValue = OnAttributeChangeData.NewValue;
	const float MaxHealthValue = StandardAttributes->GetHealthMax();
	OutgoingMessage.HealthPercentage = OnAttributeChangeData.NewValue / MaxHealthValue;
	OutgoingMessage.HealthTextValue = FText::Format(FText::FromString(TEXT("{0}:{1}")), CurrHealthValue, MaxHealthValue);
	OutgoingMessage.ActorReporting = this;
	UE_LOG(LogTemp, Warning, TEXT("Health is: %f Percent"), CurrHealthValue);

	MessageSubsystem.BroadcastMessage(ChannelTag, OutgoingMessage);
	
	if(FMath::IsNearlyEqual(OnAttributeChangeData.NewValue, 0.f) && OnAttributeChangeData.OldValue > 0)
	{
		//FGameplayTag DeathTag = FGameplayTag::RequestGameplayTag(TEXT("Conditions.Dead"));
		//FGameplayEffectSpec GameplayEffect = 
		//this->AbilitySystemComponent->ApplyGameplayEffectToSelf();
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(DeathGameplayEffect,1, EffectContext);
		
		if (EffectSpec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
		}
		//this->AbilitySystemComponent->ApplyGameplayEffectToSelf()
		// Health has just reached zero.
		if (DeathMontage->IsValidLowLevelFast())
		{
			UAnimInstance *AnimInstance = this->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(DeathMontage, 1, EMontagePlayReturnType::MontageLength, 0, true);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADungeonCrawlerCharacter::AbilityInputBindingPressedHandler(EAbilityInput AbilityInput)
{
	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<uint32>(AbilityInput));
}

void ADungeonCrawlerCharacter::AbilityInputBindingReleasedHandler(EAbilityInput AbilityInput)
{
	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<uint32>(AbilityInput));
}

void ADungeonCrawlerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADungeonCrawlerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADungeonCrawlerCharacter::Look);

		for (const FAbilityInputToInputActionBinding& Binding : AbilityInputBindings.Bindings)
		{
			EnhancedInputComponent->BindAction(Binding.InputAction, ETriggerEvent::Triggered, this, &ThisClass::AbilityInputBindingPressedHandler, Binding.AbilityInput);
			EnhancedInputComponent->BindAction(Binding.InputAction, ETriggerEvent::Canceled, this, &ThisClass::AbilityInputBindingReleasedHandler, Binding.AbilityInput);
			EnhancedInputComponent->BindAction(Binding.InputAction, ETriggerEvent::Completed, this, &ThisClass::AbilityInputBindingReleasedHandler, Binding.AbilityInput);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADungeonCrawlerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADungeonCrawlerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}