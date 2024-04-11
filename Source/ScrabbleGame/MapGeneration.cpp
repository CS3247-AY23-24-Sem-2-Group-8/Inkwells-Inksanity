// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGeneration.h"
#include "MapGenerator/GridMap.h"

constexpr uint32_t Rows = 3;
constexpr uint32_t Cols = 10;

void UMapGeneration::GenerateCoordinates(const int32 Width, const int32 Height, const int32 NumPaths,
	const double WidthScale, const double HeightScale, const TArray<int32> Probabilities,
	TArray<FVector>& Vertices, TArray<FVector2D>& Edges)
{
	std::vector<int32> ProbabilitiesVector;
	
	for (auto Probability: Probabilities)
	{
		ProbabilitiesVector.push_back(Probability);
	}
	
	FGridMap GridMap = FGridMap(Rows, Cols, Width, Height, WidthScale, HeightScale, NumPaths, ProbabilitiesVector);
	const std::tuple<TArray<FVector>, TArray<FVector2D>> Graph = GridMap.GenerateGraph();

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

int32 UMapGeneration::GenerateNodeType(const FVector2D Node, const int32 Height, const double HeightScale,
	const TArray<int32> Probabilities)
{
	const int32 NodeLevel = GetNodeLevel(Node, Height, HeightScale);

	if (NodeLevel == 0)
	{
		return 0;
	}

	if (NodeLevel == Rows - 1)
	{
		return 3;
	}

	if (NodeLevel == Rows - 2)
	{
		return 1;
	}
	
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
