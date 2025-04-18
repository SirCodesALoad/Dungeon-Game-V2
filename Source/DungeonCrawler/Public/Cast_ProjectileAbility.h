#pragma once
#include "CastTimeAbility.h"
#include "ProjectileBase.h"
#include "Cast_ProjectileAbility.generated.h"

UCLASS()
class UCast_ProjectileAbility : public UCastTimeAbility
{
	GENERATED_BODY()

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UGameplayEffect> ProjectileDamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float ProjectileSpawnOffset = 35.f;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SpawnProjectile(const FVector& SpawnLocation, const FVector& SpawnDirection);

	virtual void OnCastingSucceded_Implementation() override;
};
