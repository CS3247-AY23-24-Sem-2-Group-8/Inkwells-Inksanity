// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration.h"
#include "MapGenerator/GridMap.h"

void UMapGeneration::GenerateCoordinates(const int32 Width, const int32 Height, const int32 NumPaths,
	const double WidthScale, const double HeightScale, TArray<FVector2D>& Vertices, TArray<FVector2D>& Edges)
{
	const FGridMap GridMap = FGridMap(8, 16, Width, Height, NumPaths, WidthScale, HeightScale);
	std::tuple<TArray<FVector2D>, TArray<FVector2D>> Graph = GridMap.GenerateGraph();

	Vertices = std::get<0>(Graph);
	Edges = std::get<1>(Graph);
}

FVector2D UMapGeneration::TranslateToMapCoordinates(const FVector2D Point, const FVector2D Midpoint,
	const int32 Width, const int32 Height, const double WidthScale, const double HeightScale)
{
	const double X = Point.X + Midpoint.X - 0.5 * Width * WidthScale;
	const double Y = Point.Y + Midpoint.Y - 0.5 * Height * HeightScale;
	
	return FVector2D(X, Y);
}

int32 UMapGeneration::GenerateNodeType(const FVector2D Node, const TArray<int32> Probabilities)
{
	std::random_device DefaultSeed;
	std::mt19937 NumberGenerator(DefaultSeed());

	std::vector<double> Events(Probabilities.Num() + 1);
	int32 StartNum = -1;
	int32 Step = 1;

	std::ranges::generate(Events.begin(), Events.end(),[&StartNum, &Step]{ return StartNum += Step; });

	std::piecewise_constant_distribution<> EventSelector(Events.begin(), Events.end(),Probabilities.begin());
	
	return std::floor(EventSelector(NumberGenerator));
}
