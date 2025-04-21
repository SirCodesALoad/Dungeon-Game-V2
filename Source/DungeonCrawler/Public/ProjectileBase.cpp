#include "ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create sphere collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionComponent;

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = ShouldBounce;

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);
	ParticleSystem->bAutoActivate = true;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/Particles/P_FireEffect")); 
	if (Particle.Succeeded())
	{
		ParticleSystem->SetTemplate(Particle.Object);
	}
	
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Start the self-destruction timer
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectileBase::DestroyProjectile, LifetimeBeforeDestruction, false);
}

void AProjectileBase::DestroyProjectile()
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile destroyed after timeout."));
	Destroy();
}



void AProjectileBase::InitVelocity(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && GetOwner() != OtherActor)
	{
		// Attempt to find an AbilitySystemComponent on the other actor
		UAbilitySystemComponent* AbilitySystemComponent = OtherActor->FindComponentByClass<UAbilitySystemComponent>();
		if (AbilitySystemComponent && GameplayEffect)
		{
			// Create and apply the gameplay effect
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1.f, EffectContext);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.DamageAmount")), ProjectileDamage);
				
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}

		// Destroy the projectile
		Destroy();
	}
}


