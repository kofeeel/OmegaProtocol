#include "AbilitySystem/Abilities/Skills/SequenceSkillGameplayAbility.h"

#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "OmochaGameplayTags.h"
#include "VectorTypes.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/AbilityTasks/WaitForMouseClickTask.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

USequenceSkillGameplayAbility::USequenceSkillGameplayAbility()
{
	// Default settings
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Initialize default values
	CurrentExecutionStep = 0;
	bIsExecutingSequence = false;
	SequenceTargetLocation = FVector::ZeroVector;
	MouseClickTask = nullptr;
	SequenceRangeDecal = nullptr;

	// Range display default settings
	bShowSequenceRange = true;
	RangeReferenceSkillIndex = 0;
	bUseMaxRangeFromAllSkills = false;
	SequenceRangeDecalMaterial = nullptr;

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;

	CurrentStepAbilityHandle = FGameplayAbilitySpecHandle();
	OnAbilityEndedDelegateHandle.Reset();
}

void USequenceSkillGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// Validate skill sequence
	if (!IsSequenceValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid skill sequence"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Initialize execution state
	CurrentExecutionStep = 0;
	bIsExecutingSequence = false;
	SequenceTargetLocation = FVector::ZeroVector;
	TemporarySkillHandles.Empty();
	CurrentStepAbilityHandle = FGameplayAbilitySpecHandle();

	// Clean up any remaining delegate handles
	if (OnAbilityEndedDelegateHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
		}
		OnAbilityEndedDelegateHandle.Reset();
	}

	if (IsLocallyControlled())
	{
		ShowSkillRanges();
	}

	// Start mouse input handling
	StartMouseInput();
}

void USequenceSkillGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		if (OnAbilityEndedDelegateHandle.IsValid())
		{
			ASC->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
			OnAbilityEndedDelegateHandle.Reset();
		}

		for (const FGameplayAbilitySpecHandle& TempHandle : TemporarySkillHandles)
		{
			if (TempHandle.IsValid())
			{
				ASC->ClearAbility(TempHandle);
			}
		}
	}
	TemporarySkillHandles.Empty();

	// Cleanup operations
	StopMouseInput();
	HideAllRanges();

	// Reset state
	CurrentExecutionStep = 0;
	bIsExecutingSequence = false;
	SequenceTargetLocation = FVector::ZeroVector;

	CachedTargetLocations.Empty();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ========================================
// Unified Decal Creation Functions
// ========================================

ADecalActor* USequenceSkillGameplayAbility::CreateUnifiedDecal(const FDecalCreationParams& Params)
{
	AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	// 1. Calculate position/rotation based on display mode
	FVector SpawnLocation;
	FRotator SpawnRotation;
	bool bShouldAttach = false;

	CalculateDecalTransform(Params, Character->GetActorLocation(), SpawnLocation, SpawnRotation, bShouldAttach);

	// 2. Spawn decal actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADecalActor* DecalActor = World->SpawnActor<ADecalActor>(ADecalActor::StaticClass(), SpawnLocation, SpawnRotation,
	                                                         SpawnParams);
	if (!DecalActor)
	{
		return nullptr;
	}

	// 3. Setup decal component properties
	SetupDecalComponent(DecalActor, Params);

	// 4. Handle attachment if needed
	if (bShouldAttach)
	{
		if (USceneComponent* FixedRoot = Character->GetFixedDecalRoot())
		{
			DecalActor->AttachToComponent(FixedRoot, FAttachmentTransformRules(
				                              EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld,
				                              EAttachmentRule::KeepWorld, false));
		}
	}

	return DecalActor;
}

void USequenceSkillGameplayAbility::CalculateDecalTransform(const FDecalCreationParams& Params,
                                                            const FVector& CharacterLocation, FVector& OutLocation,
                                                            FRotator& OutRotation, bool& bShouldAttach)
{
	const FVector& MouseLocation = Params.MouseLocation;
	const FSkillRangeData& RangeData = Params.RangeData;

	// Calculate base direction vector
	FVector Direction = (MouseLocation - CharacterLocation).GetSafeNormal();
	float BaseYaw = Direction.Rotation().Yaw;

	// Apply rotation offset to get final rotation
	float FinalYaw = Params.bApplyOffset ? GetFinalRotationYaw(BaseYaw, Params.RotationOffset) : BaseYaw;

	// Apply location offset to get base position
	FVector BaseLocation = CharacterLocation;
	if (Params.bApplyOffset && !Params.LocationOffset.IsZero())
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		if (Character)
		{
			BaseLocation = TransformLocationOffset(
				CharacterLocation,
				Params.LocationOffset,
				Character->GetActorRotation()
			);
		}
	}

	bShouldAttach = false;

	switch (Params.DisplayMode)
	{
	case EDecalDisplayMode::AtCharacter:
		OutLocation = BaseLocation - FVector(0.0f, 0.0f, 88.0f);
		OutRotation = FRotator(0.0f, FinalYaw, 0.0f);
		bShouldAttach = !Params.bApplyOffset; // No attachment if offset is applied
		break;

	case EDecalDisplayMode::AtMouseCursor:
		{
			// Calculate offset-adjusted mouse position
			FVector AdjustedMouseLocation = MouseLocation;
			if (Params.bApplyOffset)
			{
				FVector DirectionFromBase = (MouseLocation - BaseLocation).GetSafeNormal();
				float Distance = FVector::Dist(BaseLocation, MouseLocation);
				FVector OffsetDirection = FRotator(0, FinalYaw, 0).Vector();
				AdjustedMouseLocation = BaseLocation + (OffsetDirection * Distance);
			}

			OutLocation = AdjustedMouseLocation;
			OutLocation.Z = CharacterLocation.Z - 88.0f;
			OutRotation = FRotator(-90.0f, FinalYaw, 0.0f);
		}
		break;

	case EDecalDisplayMode::CharacterToMouse:
		{
			FVector TargetDirection = FRotator(0, FinalYaw, 0).Vector();
			float DistanceToMouse = FVector::Dist(BaseLocation, MouseLocation);

			OutLocation = BaseLocation + (TargetDirection * DistanceToMouse * 0.5f);
			OutLocation.Z = CharacterLocation.Z - 88.0f;
			OutRotation = FRotator(90.0f, FinalYaw + 90.0f, 0.0f);
		}
		break;

	case EDecalDisplayMode::MaxRangeFixed:
		{
			FVector TargetDirection = FRotator(0, FinalYaw, 0).Vector();
			OutLocation = BaseLocation + (TargetDirection * RangeData.MaxRange * 0.5f);
			OutLocation.Z = CharacterLocation.Z - 88.0f;
			OutRotation = FRotator(90.0f, FinalYaw + 90.0f, 0.0f);
			bShouldAttach = !Params.bApplyOffset;
		}
		break;
	}
}

void USequenceSkillGameplayAbility::SetupDecalComponent(ADecalActor* DecalActor, const FDecalCreationParams& Params)
{
	UDecalComponent* DecalComponent = DecalActor->GetDecal();
	if (!DecalComponent)
	{
		return;
	}

	DecalComponent->SetDecalMaterial(Params.RangeData.DecalMaterial);

	// Calculate size based on display mode
	float DistanceToMouse = FVector::Dist(GetAvatarActorFromActorInfo()->GetActorLocation(), Params.MouseLocation);
	DecalComponent->DecalSize = CalculateDecalSize(Params.RangeData, Params.DisplayMode, DistanceToMouse);

	// Set layer order (different for each mode)
	int32 SortOrder = 10 + Params.SkillIndex;
	switch (Params.DisplayMode)
	{
	case EDecalDisplayMode::AtCharacter: SortOrder += 0;
		break;
	case EDecalDisplayMode::MaxRangeFixed: SortOrder += 5;
		break;
	case EDecalDisplayMode::CharacterToMouse: SortOrder += 10;
		break;
	case EDecalDisplayMode::AtMouseCursor: SortOrder += 15;
		break;
	}

	DecalComponent->SetSortOrder(SortOrder);
	DecalComponent->SetFadeScreenSize(0.0f);
}

// ========================================
// Array Processing Utility Functions
// ========================================

void USequenceSkillGameplayAbility::DestroyDecalArray(TArray<TObjectPtr<ADecalActor>>& DecalArray)
{
	for (ADecalActor* Decal : DecalArray)
	{
		if (Decal && IsValid(Decal))
		{
			Decal->Destroy();
		}
	}
	DecalArray.Empty();
}

void USequenceSkillGameplayAbility::ShowDecalArray(const TArray<TObjectPtr<ADecalActor>>& DecalArray)
{
	for (ADecalActor* Decal : DecalArray)
	{
		if (Decal && IsValid(Decal))
		{
			Decal->SetActorHiddenInGame(false);
		}
	}
}

// ========================================
// Range Display Functions
// ========================================

void USequenceSkillGameplayAbility::ShowSkillRanges()
{
	HideAllRanges();

	bHasReceivedFirstMouseMove = false;
	DecalToSkillIndexMap.Empty();

	// 1. Create range decal
	if (bShowSequenceRange)
	{
		SequenceRangeDecal = CreateSequenceRangeDecal();
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	FVector InitialMouseLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 500.0f;

	for (int32 i = 0; i < SkillSequence.Num(); i++)
	{
		const FSkillExecutionStep& Step = SkillSequence[i];
		if (!Step.SkillClass)
		{
			continue;
		}

		USkillGameplayAbility* SkillCDO = Step.SkillClass->GetDefaultObject<USkillGameplayAbility>();
		if (!SkillCDO)
		{
			continue;
		}

		UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
		FSkillRangeData RangeData = SkillCDO->GetSkillRangeData(MyASC);
		if (!RangeData.DecalMaterial)
		{
			continue;
		}

		// Create using existing method then apply offset settings
		FDecalCreationParams Params(RangeData, i, InitialMouseLocation);

		// Apply offset if present
		if (!Step.LocationOffset.IsZero() || Step.RotationOffset != 0.0f)
		{
			Params.LocationOffset = Step.LocationOffset;
			Params.RotationOffset = Step.RotationOffset;
			Params.bApplyOffset = Step.bShowOffsetInPreview;
		}

		Params.DisplayMode = SkillSequence[i].DecalDisplayMode;

		ADecalActor* NewDecal = CreateUnifiedDecal(Params);

		if (NewDecal)
		{
			NewDecal->SetActorHiddenInGame(true);
			DecalToSkillIndexMap.Add(NewDecal, i);

			// Add to appropriate array based on mode
			switch (Step.DecalDisplayMode)
			{
			case EDecalDisplayMode::AtCharacter: AtCharacterDecals.Add(NewDecal);
				break;
			case EDecalDisplayMode::AtMouseCursor: AtMouseDecals.Add(NewDecal);
				break;
			case EDecalDisplayMode::CharacterToMouse: CharacterToMouseDecals.Add(NewDecal);
				break;
			case EDecalDisplayMode::MaxRangeFixed: MaxRangeFixedDecals.Add(NewDecal);
				break;
			}
		}
	}
}

void USequenceSkillGameplayAbility::HideAllRanges()
{
	// Remove existing range decal
	if (SequenceRangeDecal && IsValid(SequenceRangeDecal))
	{
		SequenceRangeDecal->Destroy();
		SequenceRangeDecal = nullptr;
	}

	// Clean up using utility functions
	DestroyDecalArray(AtCharacterDecals);
	DestroyDecalArray(AtMouseDecals);
	DestroyDecalArray(CharacterToMouseDecals);
	DestroyDecalArray(MaxRangeFixedDecals);

	DecalToSkillIndexMap.Empty();
}

void USequenceSkillGameplayAbility::ShowAllDecals()
{
	ShowDecalArray(AtCharacterDecals);
	ShowDecalArray(AtMouseDecals);
	ShowDecalArray(CharacterToMouseDecals);
	ShowDecalArray(MaxRangeFixedDecals);
}

void USequenceSkillGameplayAbility::RotateDecalsToMouse(const FVector& MouseWorldLocation)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	FVector CharacterLocation = Character->GetActorLocation();
	FVector Direction = (MouseWorldLocation - CharacterLocation).GetSafeNormal();
	float BaseYaw = FMath::Atan2(Direction.Y, Direction.X) * 180.0f / PI;

	// Use existing DecalToSkillIndexMap to get offset information
	auto RotateDecalArrayWithOffset = [&](const TArray<TObjectPtr<ADecalActor>>& DecalArray,
	                                      const FRotator& BaseRotation)
	{
		for (ADecalActor* Decal : DecalArray)
		{
			if (Decal && IsValid(Decal))
			{
				FRotator FinalRotation = BaseRotation;

				// Find skill index using existing map to get offset
				if (int32* SkillIndex = DecalToSkillIndexMap.Find(Decal))
				{
					if (SkillSequence.IsValidIndex(*SkillIndex))
					{
						const FSkillExecutionStep& Step = SkillSequence[*SkillIndex];
						if (Step.RotationOffset != 0.0f)
						{
							FinalRotation.Yaw = GetFinalRotationYaw(BaseRotation.Yaw, Step.RotationOffset);
						}
					}
				}

				Decal->SetActorRotation(FinalRotation);
			}
		}
	};

	FRotator TargetRotation = FRotator(90.0f, BaseYaw + 90.0f, 0.0f);

	// AtCharacter decals don't rotate (same as before)
	RotateDecalArrayWithOffset(AtMouseDecals, TargetRotation);
	RotateDecalArrayWithOffset(CharacterToMouseDecals, TargetRotation);
	RotateDecalArrayWithOffset(MaxRangeFixedDecals, TargetRotation);
}

// ========================================
// Existing Functions (No Changes)
// ========================================

void USequenceSkillGameplayAbility::UpdateDecalsByMode(const FVector& MouseWorldLocation)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	FVector CharacterLocation = Character->GetActorLocation();
	float FixedDecalZ = CharacterLocation.Z - 88.0f;

	// Update AtMouseCursor mode decals
	for (ADecalActor* MouseDecal : AtMouseDecals)
	{
		if (MouseDecal && IsValid(MouseDecal))
		{
			FVector NewLocation = MouseWorldLocation;

			// Find skill index only once
			if (int32* SkillIndexPtr = DecalToSkillIndexMap.Find(MouseDecal))
			{
				int32 SkillIndex = *SkillIndexPtr;
				if (SkillSequence.IsValidIndex(SkillIndex))
				{
					const FSkillExecutionStep& Step = SkillSequence[SkillIndex];
					FSkillRangeData SkillSpecificRangeData = GetRangeFromSpecificSkill(SkillIndex);

					// Maintain original distance even with rotation offset
					FVector BaseLocation = CharacterLocation;

					// Apply location offset
					if (!Step.LocationOffset.IsZero())
					{
						BaseLocation = TransformLocationOffset(
							CharacterLocation,
							Step.LocationOffset,
							Character->GetActorRotation()
						);
					}

					// Calculate original distance to mouse
					float OriginalDistance = FVector::Dist(BaseLocation, MouseWorldLocation);

					// Apply rotation offset - maintain distance, only change direction
					if (Step.RotationOffset != 0.0f)
					{
						FVector OriginalDirection = (MouseWorldLocation - BaseLocation).GetSafeNormal();
						float OriginalYaw = OriginalDirection.Rotation().Yaw;
						float FinalYaw = GetFinalRotationYaw(OriginalYaw, Step.RotationOffset);
						FVector RotatedDirection = FRotator(0, FinalYaw, 0).Vector();

						NewLocation = BaseLocation + (RotatedDirection * OriginalDistance);
					}
					else if (!Step.LocationOffset.IsZero())
					{
						// Only location offset, no rotation offset
						NewLocation = MouseWorldLocation;
					}

					// Clamp to range (based on BaseLocation)
					FVector Direction = NewLocation - BaseLocation;
					Direction.Z = 0.0f;
					float Distance = Direction.Size();

					if (Distance > SkillSpecificRangeData.MaxRange)
					{
						Direction = Direction.GetSafeNormal() * SkillSpecificRangeData.MaxRange;
						NewLocation = BaseLocation + Direction;
					}
				}
			}

			NewLocation.Z = FixedDecalZ;
			MouseDecal->SetActorLocation(NewLocation);
		}
	}

	// Update CharacterToMouse mode decals
	for (ADecalActor* CharToMouseDecal : CharacterToMouseDecals)
	{
		if (CharToMouseDecal && IsValid(CharToMouseDecal))
		{
			// Find skill index only once
			if (int32* SkillIndexPtr = DecalToSkillIndexMap.Find(CharToMouseDecal))
			{
				int32 SkillIndex = *SkillIndexPtr;
				if (SkillSequence.IsValidIndex(SkillIndex))
				{
					const FSkillExecutionStep& Step = SkillSequence[SkillIndex];
					FSkillRangeData SkillSpecificRangeData = GetRangeFromSpecificSkill(SkillIndex);

					// Calculate base point
					FVector BaseLocation = CharacterLocation;
					if (!Step.LocationOffset.IsZero())
					{
						BaseLocation = TransformLocationOffset(CharacterLocation, Step.LocationOffset,
						                                       Character->GetActorRotation());
					}

					// Calculate original distance to mouse (before rotation)
					float OriginalDistance = FVector::Dist(BaseLocation, MouseWorldLocation);

					// Calculate direction with rotation offset applied
					FVector DirectionToMouse = (MouseWorldLocation - BaseLocation).GetSafeNormal();
					if (Step.RotationOffset != 0.0f)
					{
						float BaseYaw = DirectionToMouse.Rotation().Yaw;
						float FinalYaw = GetFinalRotationYaw(BaseYaw, Step.RotationOffset);
						DirectionToMouse = FRotator(0, FinalYaw, 0).Vector();
					}

					// Use rotated direction but original distance
					float FinalDistance = FMath::Min(OriginalDistance, SkillSpecificRangeData.MaxRange);

					// Set new position (midpoint)
					FVector NewLocation = BaseLocation + (DirectionToMouse * FinalDistance * 0.5f);
					NewLocation.Z = FixedDecalZ;

					CharToMouseDecal->SetActorLocation(NewLocation);

					// Update size - use original distance
					UDecalComponent* DecalComp = CharToMouseDecal->GetDecal();
					if (DecalComp)
					{
						DecalComp->DecalSize = CalculateDecalSize(SkillSpecificRangeData,
						                                          EDecalDisplayMode::CharacterToMouse, FinalDistance);
					}
				}
			}
		}
	}

	// Update MaxRangeFixed mode decals
	for (ADecalActor* MaxRangeDecal : MaxRangeFixedDecals)
	{
		if (MaxRangeDecal && IsValid(MaxRangeDecal))
		{
			// Find skill index only once
			if (int32* SkillIndexPtr = DecalToSkillIndexMap.Find(MaxRangeDecal))
			{
				int32 SkillIndex = *SkillIndexPtr;
				if (SkillSequence.IsValidIndex(SkillIndex))
				{
					const FSkillExecutionStep& Step = SkillSequence[SkillIndex];
					FSkillRangeData SkillSpecificRangeData = GetRangeFromSpecificSkill(SkillIndex);

					// Calculate base point
					FVector BaseLocation = CharacterLocation;
					if (!Step.LocationOffset.IsZero())
					{
						BaseLocation = TransformLocationOffset(CharacterLocation, Step.LocationOffset,
						                                       Character->GetActorRotation());
					}

					// Calculate mouse direction then apply only rotation offset
					FVector MouseDirection = (MouseWorldLocation - BaseLocation).GetSafeNormal();

					if (Step.RotationOffset != 0.0f)
					{
						float MouseYaw = MouseDirection.Rotation().Yaw;
						float FinalYaw = GetFinalRotationYaw(MouseYaw, Step.RotationOffset);
						MouseDirection = FRotator(0, FinalYaw, 0).Vector();
					}

					// Place at half of max range (fixed distance)
					FVector NewLocation = BaseLocation + (MouseDirection * SkillSpecificRangeData.MaxRange * 0.5f);
					NewLocation.Z = FixedDecalZ;

					MaxRangeDecal->SetActorLocation(NewLocation);
				}
			}
		}
	}
}

FVector USequenceSkillGameplayAbility::CalculateDecalSize(const FSkillRangeData& RangeData, EDecalDisplayMode Mode,
                                                          float DistanceToMouse) const
{
	UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	float Scale = ASC ? ASC->SkillScale : 1.0f;

	switch (Mode)
	{
	case EDecalDisplayMode::AtCharacter:
	case EDecalDisplayMode::AtMouseCursor:
		return RangeData.GetSkillSize() * Scale * 2.0f;

	case EDecalDisplayMode::CharacterToMouse:
		{
			FVector SkillSize = RangeData.GetSkillSize() * Scale;
			return FVector(SkillSize.X * 2.0f, DistanceToMouse, SkillSize.Y * 2.0f);
		}

	case EDecalDisplayMode::MaxRangeFixed:
		{
			FVector SkillSize = RangeData.GetSkillSize() * Scale;
			return FVector(SkillSize.X * 2.0f, RangeData.MaxRange, SkillSize.Y * 2.0f);
		}

	default:
		return RangeData.GetSkillSize() * Scale * 2.0f;
	}
}

FVector USequenceSkillGameplayAbility::TransformLocationOffset(
	const FVector& BaseLocation,
	const FVector& LocalOffset,
	const FRotator& PlayerRotation) const
{
	// Transform player-relative local coordinates to world coordinates
	FVector WorldOffset = PlayerRotation.RotateVector(LocalOffset);
	return BaseLocation + WorldOffset;
}

float USequenceSkillGameplayAbility::GetFinalRotationYaw(
	float BaseYaw,
	float RotationOffset) const
{
	return BaseYaw + RotationOffset;
}

// ========================================
// Range Related Functions
// ========================================

FSkillRangeData USequenceSkillGameplayAbility::GetSequenceRangeData() const
{
	if (SkillSequence.Num() == 0)
	{
		FSkillRangeData DefaultRange;
		DefaultRange.MaxRange = 1000.0f;
		UE_LOG(LogTemp, Warning, TEXT("Empty skill sequence, using default range"));
		return DefaultRange;
	}

	if (bUseMaxRangeFromAllSkills)
	{
		return GetMaxRangeFromAllSkills();
	}
	return GetRangeFromSpecificSkill(RangeReferenceSkillIndex);
}

FSkillRangeData USequenceSkillGameplayAbility::GetMaxRangeFromAllSkills() const
{
	FSkillRangeData MaxRangeData;
	MaxRangeData.MaxRange = 0.0f;
	MaxRangeData.MinRange = 0.0f;

	for (int32 i = 0; i < SkillSequence.Num(); i++)
	{
		FSkillRangeData SkillRange = GetRangeFromSpecificSkill(i);

		if (SkillRange.MaxRange > MaxRangeData.MaxRange)
		{
			MaxRangeData = SkillRange;
		}
	}

	return MaxRangeData;
}

FSkillRangeData USequenceSkillGameplayAbility::GetRangeFromSpecificSkill(int32 SkillIndex) const
{
	if (SkillIndex < 0 || SkillIndex >= SkillSequence.Num())
	{
		FSkillRangeData DefaultRange;
		DefaultRange.MaxRange = 1000.0f;
		UE_LOG(LogTemp, Warning, TEXT("Invalid skill index %d, using default range"), SkillIndex);
		return DefaultRange;
	}

	const FSkillExecutionStep& Step = SkillSequence[SkillIndex];
	if (!Step.SkillClass)
	{
		FSkillRangeData DefaultRange;
		DefaultRange.MaxRange = 1000.0f;
		UE_LOG(LogTemp, Warning, TEXT("Null skill class at index %d, using default range"), SkillIndex);
		return DefaultRange;
	}

	USkillGameplayAbility* SkillCDO = Step.SkillClass->GetDefaultObject<USkillGameplayAbility>();
	if (SkillCDO)
	{
		UAbilitySystemComponent* MyASC = GetAbilitySystemComponentFromActorInfo();
		return SkillCDO->GetSkillRangeData(MyASC);
	}

	FSkillRangeData DefaultRange;
	DefaultRange.MaxRange = 1000.0f;
	return DefaultRange;
}

ADecalActor* USequenceSkillGameplayAbility::CreateSequenceRangeDecal()
{
	AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return nullptr;
	}

	USceneComponent* FixedRoot = Character->GetFixedDecalRoot();
	if (!FixedRoot)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null in CreateSequenceRangeDecal"));
		return nullptr;
	}

	FSkillRangeData RangeData = GetSequenceRangeData();
	FVector SpawnLocation = Character->GetActorLocation() - FVector(0.0f, 0.0f, 88.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADecalActor* DecalActor = World->SpawnActor<ADecalActor>(
		ADecalActor::StaticClass(),
		SpawnLocation,
		FRotator(0.0f, -90.0f, 0.0f),
		SpawnParams
	);

	if (!DecalActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn DecalActor"));
		return nullptr;
	}

	UDecalComponent* DecalComponent = DecalActor->GetDecal();
	if (DecalComponent)
	{
		UMaterialInterface* MaterialToUse = SequenceRangeDecalMaterial
			                                    ? SequenceRangeDecalMaterial
			                                    : RangeData.DecalMaterial;

		if (MaterialToUse)
		{
			DecalComponent->SetDecalMaterial(MaterialToUse);
		}

		float DecalRadius = RangeData.MaxRange * 2.0f;
		DecalComponent->DecalSize = FVector(RangeData.Size.Z, DecalRadius, DecalRadius);

		DecalComponent->SetSortOrder(1);
		DecalComponent->SetFadeScreenSize(0.0f);
	}

	DecalActor->AttachToComponent(FixedRoot,
	                              FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld,
	                                                        EAttachmentRule::KeepWorld, false));

	return DecalActor;
}

// ========================================
// Mouse Input Handling
// ========================================

void USequenceSkillGameplayAbility::StartMouseInput()
{
	StopMouseInput();

	MouseClickTask = UWaitForMouseClickTask::WaitForMouseClickWithTracking(this, false, false, true);
	if (MouseClickTask)
	{
		MouseClickTask->OnValidClick.AddDynamic(this, &USequenceSkillGameplayAbility::OnMouseInputReceived);
		MouseClickTask->OnMouseMoved.AddDynamic(this, &USequenceSkillGameplayAbility::OnMouseMoved);

		MouseClickTask->ReadyForActivation();
	}
}

void USequenceSkillGameplayAbility::StopMouseInput()
{
	if (MouseClickTask && IsValid(MouseClickTask))
	{
		MouseClickTask->OnValidClick.RemoveDynamic(this, &USequenceSkillGameplayAbility::OnMouseInputReceived);
		MouseClickTask->OnMouseMoved.RemoveDynamic(this, &USequenceSkillGameplayAbility::OnMouseMoved);
		MouseClickTask->EndTask();
		MouseClickTask = nullptr;
	}
}

void USequenceSkillGameplayAbility::OnMouseInputReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	OnBlueprintCommit();
	if (bIsExecutingSequence)
	{
		return;
	}

	FVector TargetLocation = FVector::ZeroVector;
	if (TargetDataHandle.Num() > 0)
	{
		const FHitResult* HitResult = TargetDataHandle.Get(0)->GetHitResult();
		if (HitResult && HitResult->bBlockingHit)
		{
			TargetLocation = HitResult->ImpactPoint;
		}
	}

	if (GetAvatarActorFromActorInfo()->HasAuthority() && IsLocallyControlled())
	{
		// Execute directly on server
		ExecuteSkillSequence(TargetLocation);
	}
	else if (IsLocallyControlled())
	{
		// Send mouse position + decal target data from client to server
		TArray<FSkillTargetData> DecalTargetData = CollectDecalTargetData();
		ServerExecuteSkillSequenceWithDecalData(TargetLocation, DecalTargetData);

		StopMouseInput();
		HideAllRanges();
	}
}

void USequenceSkillGameplayAbility::OnMouseMoved(const FVector& MouseWorldLocation)
{
	if (bIsExecutingSequence)
	{
		return;
	}

	// Show all decals when first mouse movement is received
	if (!bHasReceivedFirstMouseMove)
	{
		bHasReceivedFirstMouseMove = true;
		ShowAllDecals();
	}

	RotateDecalsToMouse(MouseWorldLocation);
	UpdateDecalsByMode(MouseWorldLocation);
}

// ========================================
// Skill Sequence Execution
// ========================================

void USequenceSkillGameplayAbility::ExecuteSkillSequence(const FVector& TargetLocation)
{
	if (bIsExecutingSequence)
	{
		return;
	}

	// Only execute caching if cache is empty (skip if already received from server)
	if (CachedTargetLocations.Num() == 0)
	{
		CacheAllTargetLocations();
	}

	bIsExecutingSequence = true;
	SequenceTargetLocation = TargetLocation;
	CurrentExecutionStep = 0;

	StopMouseInput();
	HideAllRanges();

	ExecuteNextSkillInSequence();
}

void USequenceSkillGameplayAbility::ExecuteNextSkillInSequence()
{
	if (CurrentExecutionStep >= SkillSequence.Num())
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	const FSkillExecutionStep& CurrentStep = SkillSequence[CurrentExecutionStep];

	if (CurrentStep.DelayAfterPreviousStep > 0.0f)
	{
		FTimerHandle DelayTimer;
		GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &USequenceSkillGameplayAbility::ExecuteSkillStep,
		                                       CurrentStep.DelayAfterPreviousStep, false);
	}
	else
	{
		ExecuteSkillStep();
	}
}

void USequenceSkillGameplayAbility::ExecuteSkillStep()
{
	if (!SkillSequence.IsValidIndex(CurrentExecutionStep) || !SkillSequence[CurrentExecutionStep].SkillClass)
	{
		OnSkillStepCompleted();
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	OnAbilityEndedDelegateHandle = ASC->AbilityEndedCallbacks.AddUObject(
		this, &USequenceSkillGameplayAbility::OnSubAbilityEnded);

	const FSkillExecutionStep& Step = SkillSequence[CurrentExecutionStep];
	FGameplayAbilitySpec Spec(Step.SkillClass, 1, INDEX_NONE, this);
	CurrentStepAbilityHandle = ASC->GiveAbility(Spec);

	if (CurrentStepAbilityHandle.IsValid())
	{
		TemporarySkillHandles.Add(CurrentStepAbilityHandle);

		// Set target location
		FVector StepTargetLocation = GetTargetLocationForStep(CurrentExecutionStep);
		if (auto* OmochaASC = Cast<UOmochaAbilitySystemComponent>(ASC))
		{
			OmochaASC->CachedSkillTargetLocation = StepTargetLocation;
		}

		// Try to activate ability
		if (!ASC->TryActivateAbility(CurrentStepAbilityHandle))
		{
			ASC->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
			OnAbilityEndedDelegateHandle.Reset();
			CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		}
	}
	else
	{
		ASC->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
		OnAbilityEndedDelegateHandle.Reset();
		OnSkillStepCompleted();
	}
}

void USequenceSkillGameplayAbility::OnSubAbilityEnded(UGameplayAbility* EndedAbility)
{
	if (EndedAbility && EndedAbility->GetCurrentAbilitySpecHandle() == CurrentStepAbilityHandle)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->AbilityEndedCallbacks.Remove(OnAbilityEndedDelegateHandle);
		}
		OnAbilityEndedDelegateHandle.Reset();
		CurrentStepAbilityHandle = FGameplayAbilitySpecHandle();

		FTimerHandle DelayTimer;
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimer,
			this,
			&USequenceSkillGameplayAbility::OnSkillStepCompleted,
			0.001f,
			false
		);

		//OnSkillStepCompleted();
	}
}

void USequenceSkillGameplayAbility::OnSkillStepCompleted()
{
	CurrentExecutionStep++;
	ExecuteNextSkillInSequence();
}

// ========================================
// Validation and Utility Functions
// ========================================

bool USequenceSkillGameplayAbility::IsSequenceValid() const
{
	if (SkillSequence.Num() == 0)
	{
		return false;
	}

	for (const FSkillExecutionStep& Step : SkillSequence)
	{
		if (!Step.SkillClass)
		{
			return false;
		}
	}

	return true;
}

FVector USequenceSkillGameplayAbility::GetTargetLocationForStep(int32 StepIndex) const
{
	// if (StepIndex < 0 || StepIndex >= SkillSequence.Num())
	// {
	// 	return SequenceTargetLocation;
	// }

	FVector BaseTargetLocation = SequenceTargetLocation;
	if (const FVector* CachedLocation = CachedTargetLocations.Find(StepIndex))
	{
		BaseTargetLocation = *CachedLocation;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		FVector StartLocation = Character->GetActorLocation();
		UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
		const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
		const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		TArray<AActor*> ActorsToIgnore = {Character};

		return UOmochaAbilitySystemLibrary::AdjustLocationForWalls(
			this,
			StartLocation,
			BaseTargetLocation,
			CapsuleRadius,
			CapsuleHalfHeight,
			ActorsToIgnore,
			false
		);
	}

	return BaseTargetLocation;
}

void USequenceSkillGameplayAbility::CacheAllTargetLocations()
{
	TArray<FSkillTargetData> CollectedData = CollectDecalTargetData();

	CachedTargetLocations.Empty();
	for (const FSkillTargetData& Data : CollectedData)
	{
		CachedTargetLocations.Add(Data.SkillIndex, Data.TargetLocation);
	}
}

TArray<FSkillTargetData> USequenceSkillGameplayAbility::CollectDecalTargetData() const
{
	TArray<FSkillTargetData> Result;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return Result;
	}

	for (const auto& DecalPair : DecalToSkillIndexMap)
	{
		ADecalActor* Decal = DecalPair.Key;
		int32 SkillIndex = DecalPair.Value;

		if (!Decal || !IsValid(Decal) || !SkillSequence.IsValidIndex(SkillIndex))
		{
			continue;
		}

		const FSkillExecutionStep& Step = SkillSequence[SkillIndex];
		FVector DecalLocation = Decal->GetActorLocation();

		FVector BaseLocation = Character->GetActorLocation();
		if (!Step.LocationOffset.IsZero())
		{
			BaseLocation = TransformLocationOffset(
				Character->GetActorLocation(),
				Step.LocationOffset,
				Character->GetActorRotation()
			);
		}

		FVector FinalTargetLocation = ConvertMidpointToEndpoint(
			DecalLocation,
			BaseLocation,
			Step.DecalDisplayMode
		);

		Result.Add(FSkillTargetData(SkillIndex, FinalTargetLocation));
	}

	return Result;
}

void USequenceSkillGameplayAbility::ServerExecuteSkillSequenceWithDecalData_Implementation(const FVector& MouseLocation,
	const TArray<FSkillTargetData>& DecalTargetData)
{
	// Set cache directly with client decal data on server
	CachedTargetLocations.Empty();
	for (const FSkillTargetData& Data : DecalTargetData)
	{
		CachedTargetLocations.Add(Data.SkillIndex, Data.TargetLocation);
	}

	bIsExecutingSequence = true;
	SequenceTargetLocation = MouseLocation;
	CurrentExecutionStep = 0;

	ExecuteNextSkillInSequence();
}

TArray<TObjectPtr<ADecalActor>>& USequenceSkillGameplayAbility::GetDecalArrayForMode(EDecalDisplayMode Mode)
{
	switch (Mode)
	{
	case EDecalDisplayMode::AtCharacter: return AtCharacterDecals;
	case EDecalDisplayMode::AtMouseCursor: return AtMouseDecals;
	case EDecalDisplayMode::CharacterToMouse: return CharacterToMouseDecals;
	case EDecalDisplayMode::MaxRangeFixed: return MaxRangeFixedDecals;
	default: return AtCharacterDecals; // 기본값
	}
}

FVector USequenceSkillGameplayAbility::ConvertMidpointToEndpoint(
	const FVector& DecalLocation,
	const FVector& BaseLocation,
	EDecalDisplayMode Mode) const
{
	if (Mode != EDecalDisplayMode::MaxRangeFixed && Mode != EDecalDisplayMode::CharacterToMouse)
	{
		return DecalLocation;
	}

	FVector Direction = DecalLocation - BaseLocation;
	Direction.Z = 0.0f;
	FVector EndpointLocation = BaseLocation + (Direction * 2.0f);
	EndpointLocation.Z = DecalLocation.Z;
	return EndpointLocation;
}
