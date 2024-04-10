#pragma once
#include <random>
#include <tuple>
#include <set>
#include <vector>
#include "Logging/StructuredLog.h"

struct FGridMap
{
private:
	std::mt19937 RandomNumberGenerator;
	
	std::random_device DefaultSeed;

	// the key represents the start point while the vector represents all the end points.
	std::unordered_map<uint64_t, std::vector<uint64_t>> EdgeList = std::unordered_map<uint64_t, std::vector<uint64_t>>();

	std::set<uint64_t> NodeList = std::set<uint64_t>();

	void CreatePaths(uint32_t NumPaths);
	
public:
	uint32_t Rows;
	
	uint32_t Cols;
	
	double BlockWidth;
	
	double BlockHeight;

	std::vector<int32> Probabilities;

	FGridMap(const uint32_t Rows, const uint32_t Cols, const double Width, const double Height,
		const double WidthScale, const double HeightScale, const uint32_t NumPaths,
		const std::vector<int32>& Probabilities);

	FGridMap(const uint32_t Rows, const uint32_t Cols, const double Width, const double Height,
		const double WidthScale, const double HeightScale, const uint32_t NumPaths,
		const std::vector<int32>& Probabilities, const uint32_t Seed);

	// gives a tuple of points and edges
	std::tuple<TArray<FVector>, TArray<FVector2D>> GenerateGraph();

	static uint64_t HashBlock(const uint32_t Row, const uint32_t Col);

	static std::tuple<uint32_t, uint32_t> UnhashBlock(const uint64_t Hash);
};
