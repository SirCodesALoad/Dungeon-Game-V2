#pragma once
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();
	
	virtual void BeginPlay() override;

	// Sphere collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	class UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Properties")
	float InitialSpeed = 1500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Properties")
	float MaxSpeed = 1500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Properties")
	bool ShouldBounce = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Properties")
	float ProjectileDamage = 15.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Properties")
	float LifetimeBeforeDestruction = 5.f;

	FTimerHandle DestroyTimerHandle;


	virtual void DestroyProjectile();
	// Function to initialize the projectile's velocity
	void InitVelocity(const FVector& ShootDirection);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};

