#include "GridMap.h"

FGridMap::FGridMap(const uint32_t Rows, const uint32_t Cols, const double Width, const double Height,
                   const double WidthScale, const double HeightScale, const uint32_t NumPaths,
                   const std::vector<int32>& Probabilities)
{
	this->Rows = Rows;
	this->Cols = Cols;
	this->BlockWidth = WidthScale * Width / Cols;
	this->BlockHeight = HeightScale * Height / Rows;
	this->RandomNumberGenerator.seed(DefaultSeed());
	this->Probabilities = Probabilities;

	CreatePaths(NumPaths);
}

FGridMap::FGridMap(const uint32_t Rows, const uint32_t Cols, const double Width, const double Height,
	const double WidthScale, const double HeightScale, const uint32_t NumPaths,
	const std::vector<int32>& Probabilities, const uint32_t Seed)
{
	this->Rows = Rows;
	this->Cols = Cols;
	this->BlockWidth = WidthScale * Width / Cols;
	this->BlockHeight = HeightScale * Height / Rows;
	this->RandomNumberGenerator.seed(Seed);
	this->Probabilities = Probabilities;

	CreatePaths(NumPaths);
}

uint64_t FGridMap::HashBlock(const uint32_t Row, const uint32_t Col)
{
	return static_cast<uint64_t>(Row) << 32 | Col;
}

std::tuple<uint32_t, uint32_t> FGridMap::UnhashBlock(const uint64_t Hash)
{
	uint32_t Row = static_cast<uint32_t>(Hash >> 32);
	uint32_t Col = static_cast<uint32_t>(Hash);
	
	return std::make_tuple(Row, Col);
}

void FGridMap::CreatePaths(const uint32_t NumPaths)
{
	std::uniform_int_distribution<int> StartGenerator(0, Cols - 1);

	for (uint32_t i = 0; i < NumPaths; i++)
	{
		// randomly select a starting node path
		uint32_t CurrentCol = StartGenerator(RandomNumberGenerator);
		uint64_t PrevBlock = HashBlock(0, CurrentCol);
		NodeList.insert(PrevBlock);
		
		for (uint32_t CurrentRow = 1; CurrentRow < Rows - 1; CurrentRow++)
		{
			int LeftBound = -1;
			int RightBound = 1;

			if (CurrentCol == 0)
			{
				LeftBound = 0;
			} else if (std::vector<uint64_t> To = EdgeList[HashBlock(CurrentRow - 1, CurrentCol - 1)];
				std::ranges::find(To, HashBlock(CurrentRow, CurrentCol)) != To.end())
			{
				UE_LOGFMT(LogTemp, Log, "Left");
				LeftBound = 0;
			}

			if (CurrentCol == Cols - 1)
			{
				RightBound = 0;
			} else if (std::vector<uint64_t> To = EdgeList[HashBlock(CurrentRow - 1, CurrentCol + 1)];
				std::ranges::find(To, HashBlock(CurrentRow, CurrentCol)) != To.end())
			{
				UE_LOGFMT(LogTemp, Log, "Right");
				RightBound = 0;
			}

			std::uniform_int_distribution<> NextSquare(LeftBound, RightBound);
			
			CurrentCol += NextSquare(RandomNumberGenerator);

			uint64_t CurrentBlock = HashBlock(CurrentRow, CurrentCol);
			
			NodeList.insert(CurrentBlock);
			if (!EdgeList.contains(PrevBlock))
			{
				EdgeList[PrevBlock] = std::vector<uint64_t>();
			}
			
			EdgeList[PrevBlock].push_back(CurrentBlock);
			std::tuple<uint32_t, uint32_t> Temp = UnhashBlock(PrevBlock);
			UE_LOGFMT(LogTemp, Log, "({0}, {1}) -> ({2}, {3})", std::get<0>(Temp), std::get<1>(Temp), CurrentRow, CurrentCol);
			PrevBlock = CurrentBlock;
		}
	}
}

std::tuple<TArray<FVector>, TArray<FVector2D>> FGridMap::GenerateGraph() 
{
	std::unordered_map<uint64_t, std::set<double>> DisallowedNodeTypes;
	
	TArray<FVector> Vertices = TArray<FVector>();
	TArray<FVector2D> Edges = TArray<FVector2D>();
	const FVector BossNode = FVector(BlockWidth * Cols * 0.5, BlockHeight * (Rows - 0.5), 3);
	Vertices.Add(BossNode);

	std::random_device RandomDevice;
	std::mt19937 Generator(RandomDevice());

	std::unordered_map<uint64_t, uint32_t> IndexMap = std::unordered_map<uint64_t, uint32_t>();

	std::vector<double> Events(Probabilities.size() + 1);
	int32 StartNum = -1;
	int32 Step = 1;

	std::ranges::generate(Events.begin(), Events.end(),[&StartNum, &Step]{ return StartNum += Step; });
	std::piecewise_constant_distribution<> EventSelector(Events.begin(), Events.end(),Probabilities.begin());
	std::uniform_real_distribution<> XCoordinateGenerator(0.25 * BlockWidth, 0.75 * BlockWidth);
	std::uniform_real_distribution<> YCoordinateGenerator(0.25 * BlockHeight, 0.75 * BlockHeight);

	for (uint64_t NodeHash : NodeList)
	{
		std::tuple<uint32_t, uint32_t> Coords = UnhashBlock(NodeHash);
		const uint32_t Row = std::get<0>(Coords);
		const uint32_t Col = std::get<1>(Coords);
		double NodeType;

		const double XCoordinate = Col * BlockWidth + XCoordinateGenerator(Generator);
		const double YCoordinate = Row * BlockHeight + YCoordinateGenerator(Generator);

		if (Row == 0 || Row == Rows - 3)
		{
			// set to battle
			NodeType = 0;
		}
		else if (Row == Rows - 2)
		{
			// set to rest point
			NodeType = 1;
		}
		else
		{
			// if the node is not a battle, and it is the same as next, change it
			do
			{
				NodeType = std::floor(EventSelector(Generator));
			}
			while (DisallowedNodeTypes.contains(NodeHash) && DisallowedNodeTypes[NodeHash].contains(NodeType));
		}

		for (std::vector<uint64_t> DestinationNodes = EdgeList[NodeHash]; uint64_t Dest : DestinationNodes)
		{
			if (NodeType == 0)
			{
				break;
			}
			
			if (!DisallowedNodeTypes.contains(Dest))
			{
				DisallowedNodeTypes[Dest] = std::set<double>();
			}
			DisallowedNodeTypes[Dest].insert(NodeType);
		}
		
		IndexMap[NodeHash] = Vertices.Num();
		Vertices.Add(FVector(XCoordinate, YCoordinate, NodeType));
	}

	
	for (const auto& [Key, Value] : EdgeList)
	{
		const int FromIndex = IndexMap[Key];
		
		for (const uint64_t Node : Value)
		{
			const int ToIndex = IndexMap[Node];

			Edges.Add(FVector2D(FromIndex, ToIndex));
			
			// connect to boss node
			std::tuple<uint32_t, uint32_t> Coords = UnhashBlock(Node);
			if (const uint32_t Row = std::get<0>(Coords); Row == Rows - 2)
			{
				Edges.Add(FVector2D(ToIndex, 0));
			}
		}
	}

	return std::tuple(Vertices, Edges);
}
