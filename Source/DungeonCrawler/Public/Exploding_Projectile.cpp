#include "Exploding_Projectile.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "DungeonCrawler/DungeonCrawlerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AExploding_Projectile::AExploding_Projectile()
{
	// Create sphere collision component
	ExplosionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionSphereComponent"));
	ExplosionSphere->InitSphereRadius(ExplosionRadius);
	ExplosionSphere->SetCollisionProfileName(TEXT("ExplodingProjectile"));
	ExplosionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	ExplosionSphere->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignore everything by default
	ExplosionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Overlap with characters
	ExplosionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // Overlap with dynamic objects
	ExplosionSphere->SetGenerateOverlapEvents(true);

	ExplosionParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplodingParticleSystem"));
	ExplosionParticleSystem->SetupAttachment(RootComponent);
	ExplosionParticleSystem->bAutoActivate = true;
	ExplosionParticleSystem->SetVisibility(false);

}


void AExploding_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                  FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosionSphere->InitSphereRadius(ExplosionRadius);
	Explode(OtherActor);
}

void AExploding_Projectile::Explode(AActor* OtherActor = nullptr)
{
	DrawDebugSphere(
GetWorld(),
GetActorLocation(),
ExplosionRadius,
12, // Sphere segments
FColor::Red,
false, // Persistent lines
5.0f // Duration
);
	TArray<AActor*> AllActors;
	// Find all actors of type ADungeonCrawlerCharacter
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADungeonCrawlerCharacter::StaticClass(), AllActors);

	ExplosionParticleSystem->SetVisibility(true);
	ParticleSystem->SetVisibility(false);
	
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AExploding_Projectile::DestroyAfterExplosion, 1.5f, false);

	for (AActor* Target : AllActors)
	{
		ADungeonCrawlerCharacter* TargetCharacter = Cast<ADungeonCrawlerCharacter>(Target);
		if (TargetCharacter)
		{
			// Apply damage using Gameplay Effect
			float Distance = FVector::Dist(TargetCharacter->GetActorLocation(), GetActorLocation());
			if (Distance <= ExplosionRadius)
			{
				UAbilitySystemComponent* TargetASC = TargetCharacter->FindComponentByClass<UAbilitySystemComponent>();
				if (TargetASC && TargetCharacter != OtherActor)
				{
					FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
					EffectContext.AddSourceObject(this);

					FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(ExplosionGameplayEffect, 1.f, EffectContext);
					if (SpecHandle.IsValid())
					{
						SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.DamageAmount")), ExplosionDamage);
				
						TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}
}

void AExploding_Projectile::DestroyProjectile()
{
	Explode();
}

void AExploding_Projectile::DestroyAfterExplosion()
{
}
