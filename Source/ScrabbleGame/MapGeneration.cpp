// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration.h"
#include "MapGenerator/GridMap.h"

void UMapGeneration::GenerateCoordinates(const int32 Width, const int32 Height, const int32 NumPaths,
                                         TArray<FVector2D>& Vertices, TArray<FVector2D>& Edges)
{
	const FGridMap GridMap = FGridMap(8, 16, Width, Height, NumPaths);
	std::tuple<TArray<FVector2D>, TArray<FVector2D>> Graph = GridMap.GenerateGraph();

	Vertices = std::get<0>(Graph);
	Edges = std::get<1>(Graph);
}

FVector2D UMapGeneration::TranslateToMapCoordinates(const FVector2D Point, const FVector2D Midpoint,
	const int32 Width, const int32 Height)
{
	const double X = Point.X + Midpoint.X - 0.5 * Width;
	const double Y = Point.Y + Midpoint.Y - 0.5 * Height;
	
	return FVector2D(X, Y);
}
