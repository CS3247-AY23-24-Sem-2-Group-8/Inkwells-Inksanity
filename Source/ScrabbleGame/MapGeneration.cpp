// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration.h"
#include "MapGenerator/GridMap.h"

constexpr uint32_t Rows = 8;
constexpr uint32_t Cols = 10;

void UMapGeneration::GenerateCoordinates(const int32 Width, const int32 Height, const int32 NumPaths,
	const double WidthScale, const double HeightScale, TArray<FVector2D>& Vertices, TArray<FVector2D>& Edges)
{
	const FGridMap GridMap = FGridMap(Rows, Cols, Width, Height, WidthScale, HeightScale, NumPaths);
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
	std::vector<int32> ProbabilitiesVector;

	for (auto Probability: Probabilities)
	{
		ProbabilitiesVector.push_back(Probability);
	}

	std::vector<double> Events(Probabilities.Num() + 1);
	int32 StartNum = -1;
	int32 Step = 1;

	std::ranges::generate(Events.begin(), Events.end(),[&StartNum, &Step]{ return StartNum += Step; });

	std::piecewise_constant_distribution<> EventSelector(Events.begin(), Events.end(),ProbabilitiesVector.begin());
	
	return std::floor(EventSelector(NumberGenerator));
}

int32 UMapGeneration::GetNodeLevel(const FVector2D Node, const int32 Height, const double HeightScale)
{
	const double BlockHeight = Height * HeightScale / Rows;

	return static_cast<int32>(std::floor(Node.Y / BlockHeight));
}

bool UMapGeneration::IsBossNode(const FVector2D Node, const int32 Height, const double HeightScale)
{
	return GetNodeLevel(Node, Height, HeightScale) == Rows;
}
