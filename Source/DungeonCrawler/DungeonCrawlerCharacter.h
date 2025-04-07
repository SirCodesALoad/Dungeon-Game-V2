// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Public/EAbilityInput.h"
#include "Public/UAbilitySet.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Public/UStandardAttributeSet.h"
#include "DungeonCrawlerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

USTRUCT()
struct FAbilityInputToInputActionBinding
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputAction;

	UPROPERTY(EditDefaultsOnly)
	EAbilityInput AbilityInput;
};

USTRUCT()
struct FAbilityInputBindings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<FAbilityInputToInputActionBinding> Bindings;
};

UENUM()
enum EFaction : int {
	Heroes = 0,
	Badguys = 1,
	NonCombatants = 255
};

UCLASS(config=Game)
class ADungeonCrawlerCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	TSubclassOf<UGameplayEffect> DeathGameplayEffect = ConstructorHelpers::FClassFinder<UGameplayEffect>(TEXT("/Game/Gameplay/GameplayEffects/GE_Death")).Class;


	// UFUNCTION(BlueprintCallable, Category="Input")
	// void OnAbilityOne(const FInputActionValue& InputValue);
	// UFUNCTION(BlueprintCallable, Category="Input")
	// void OnAbilityTwo(const FInputActionValue& InputValue);

public:
	UPROPERTY(BlueprintReadWrite, Category = "Attributes")
	UStandardAttributeSet* StandardAttributes { nullptr };
	UPROPERTY(VisibleAnywhere, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent { nullptr };
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> InitialGameplayEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UAbilitySet* InitialAbilitySet;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FAbilityInputBindings AbilityInputBindings;
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InitiallyGrantedAbilitySpecHandles;
	UPROPERTY(BlueprintReadWrite, Category = "Abilities")
	TArray<UTexture*> AbilityIcons { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
	UAnimMontage* DeathMontage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("/Game/Characters/Animations/SwordAndShieldReTarget/Tank_Death")).Object;
	
	ADungeonCrawlerCharacter();
	virtual void PreInitializeComponents() override;
	virtual UStandardAttributeSet* GetAttributeSet() const { return StandardAttributes;};

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent;};
	/** Returns current (final) value of an attribute */
	float GetNumericAttribute(const FGameplayAttribute &Attribute) const;

	/** Sets the base value of an attribute. Existing active modifiers are NOT cleared and will act upon the new base value. */
	void SetNumericAttributeBase(const FGameplayAttribute &Attribute, float NewBaseValue);
	void AbilityInputBindingPressedHandler(EAbilityInput AbilityInput);
	void AbilityInputBindingReleasedHandler(EAbilityInput AbilityInput);

protected:
	void SetupInitialAbilitiesAndEffects();
	void OnHealthAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	TEnumAsByte<EFaction> Faction = EFaction::NonCombatants;
	FGenericTeamId TeamId;
	

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	virtual void PossessedBy(AController* NewController) override;
};

