#pragma once
#include "ProjectileBase.h"
#include "Exploding_Projectile.generated.h"

UCLASS()
class AExploding_Projectile : public AProjectileBase 
{
	GENERATED_BODY()
	
public:
	AExploding_Projectile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> ExplosionGameplayEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Properties")
	float ExplosionDamage = 15.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* ExplosionSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Properties")
	float ExplosionRadius = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	class UParticleSystemComponent* ExplosionParticleSystem;
	
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	virtual void Explode(AActor* OtherActor);

	virtual void DestroyProjectile() override;
	void DestroyAfterExplosion();
};
