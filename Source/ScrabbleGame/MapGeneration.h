// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapGeneration.generated.h"

/**
 * 
 */
UCLASS()
class SCRABBLEGAME_API UMapGeneration : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MapGeneration")
	static void GenerateCoordinates(const int32 Width, const int32 Height, const int32 NumPaths,
		TArray<FVector2D> &Vertices, TArray<FVector2D> &Edges);
};
