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
		const double WidthScale, const double HeightScale, TArray<FVector2D> &Vertices, TArray<FVector2D> &Edges);

	UFUNCTION(BlueprintCallable, Category = "MapGeneration")
	static FVector2D TranslateToMapCoordinates(const FVector2D Point, const FVector2D Midpoint,
		const int32 Width, const int32 Height, const double WidthScale, const double HeightScale);

	UFUNCTION(BlueprintCallable, Category = "MapGeneration")
	static int32 GenerateNodeType(const FVector2D Node, const int32 Height, const double HeightScale,
		const TArray<int32> Probabilities);

	UFUNCTION(BlueprintCallable, Category = "MapGeneration")
	static int32 GetNodeLevel(const FVector2D Node, const int32 Height, const double HeightScale);
};
