#include "Cast_ProjectileAbility.h"

void UCast_ProjectileAbility::SpawnProjectile(const FVector& SpawnLocation, const FVector& SpawnDirection)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(ProjectileDamageEffect, 1);

	if (SpecHandle.IsValid())
	{
        const FVector ModifiedSpawnLocation = SpawnLocation + (SpawnDirection * ProjectileSpawnOffset);

		if (AProjectileBase* Projectile = GetWorld()->SpawnActorDeferred<AProjectileBase>(ProjectileClass, FTransform(SpawnDirection.Rotation(), ModifiedSpawnLocation)))
	    {
			Projectile->SetOwner(GetAvatarActorFromActorInfo());
			Projectile->InitialSpeed = 1500.f;
			Projectile->MaxSpeed = 1500.f;
			Projectile->GameplayEffect = ProjectileDamageEffect;
			Projectile->InitVelocity(SpawnDirection);
			Projectile->FinishSpawning(FTransform(SpawnDirection.Rotation(), ModifiedSpawnLocation));
	    }
	}
}

void UCast_ProjectileAbility::OnCastingSucceded_Implementation()
{
	Super::OnCastingSucceded_Implementation();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	FVector SpawnLocation = AvatarActor->GetActorLocation();
	FVector SpawnDirection = AvatarActor->GetActorForwardVector();
	SpawnProjectile(SpawnLocation, SpawnDirection);
}
