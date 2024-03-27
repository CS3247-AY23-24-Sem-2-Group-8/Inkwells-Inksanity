// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration.h"
#include "MapGenerator/GridMap.h"

void UMapGeneration::GenerateCoordinates(const int32 Width, const int32 Height, const int32 NumPaths,
                                         TArray<FVector2D>& Vertices, TArray<FVector2D>& Edges)
{
	FGridMap GridMap = FGridMap(8, 16, Width, Height, NumPaths);
	std::tuple<TArray<FVector2D>, TArray<FVector2D>> Graph = GridMap.GenerateGraph();

	Vertices = std::get<0>(Graph);
	Edges = std::get<1>(Graph);
}
