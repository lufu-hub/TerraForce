// Terraforge - yapi yerlesim kurallari (saf C++17, Unreal tipi kullanmaz).
// Izgara hucreleri HeightField hucreleriyle birebir eslesir.
#pragma once

#include "Sim/TFHeightField.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace TF
{
namespace Sim
{

enum class PlacementResult : uint8_t
{
	Ok,
	OutOfBounds,
	Overlap,
	TooSteep,
	InvalidSize
};

// Hucre cinsinden eksen hizali alan; (X, Y) en kucuk kose.
struct Footprint
{
	int X = 0;
	int Y = 0;
	int W = 1;
	int H = 1;
};

// Merkez hucre etrafinda, 90 derecelik donus sayisina gore footprint uretir.
inline Footprint MakeCenteredFootprint(int CenterX, int CenterY, int SizeX, int SizeY, int QuarterTurns)
{
	const bool bSwap = ((QuarterTurns % 2) + 2) % 2 == 1;
	Footprint F;
	F.W = bSwap ? SizeY : SizeX;
	F.H = bSwap ? SizeX : SizeY;
	F.X = CenterX - F.W / 2;
	F.Y = CenterY - F.H / 2;
	return F;
}

inline void FootprintHeightRange(const HeightField& Field, const Footprint& F, double& OutMin, double& OutMax)
{
	OutMin = Field.HeightAtVertex(F.X, F.Y);
	OutMax = OutMin;
	for (int IY = F.Y; IY <= F.Y + F.H; ++IY)
	{
		for (int IX = F.X; IX <= F.X + F.W; ++IX)
		{
			const double V = Field.HeightAtVertex(IX, IY);
			OutMin = std::min(OutMin, V);
			OutMax = std::max(OutMax, V);
		}
	}
}

class OccupancyGrid
{
public:
	void Reset(int InCellsX, int InCellsY)
	{
		CellsX = std::max(0, InCellsX);
		CellsY = std::max(0, InCellsY);
		Owners.assign(static_cast<size_t>(CellsX) * static_cast<size_t>(CellsY), 0);
	}

	int GetCellsX() const { return CellsX; }
	int GetCellsY() const { return CellsY; }

	bool InBounds(const Footprint& F) const
	{
		return F.W > 0 && F.H > 0 && F.X >= 0 && F.Y >= 0 && F.X + F.W <= CellsX && F.Y + F.H <= CellsY;
	}

	// 0 = bos. Sinir disi hucreler -1 dondurur.
	int32_t OwnerAt(int X, int Y) const
	{
		if (X < 0 || Y < 0 || X >= CellsX || Y >= CellsY)
		{
			return -1;
		}
		return Owners[Index(X, Y)];
	}

	bool IsFree(const Footprint& F) const
	{
		if (!InBounds(F))
		{
			return false;
		}
		for (int Y = F.Y; Y < F.Y + F.H; ++Y)
		{
			for (int X = F.X; X < F.X + F.W; ++X)
			{
				if (Owners[Index(X, Y)] != 0)
				{
					return false;
				}
			}
		}
		return true;
	}

	// OwnerId > 0 olmali. Cagiran once IsFree ile dogrulamalidir.
	void Occupy(const Footprint& F, int32_t OwnerId)
	{
		if (!InBounds(F) || OwnerId <= 0)
		{
			return;
		}
		for (int Y = F.Y; Y < F.Y + F.H; ++Y)
		{
			for (int X = F.X; X < F.X + F.W; ++X)
			{
				Owners[Index(X, Y)] = OwnerId;
			}
		}
	}

	int Release(int32_t OwnerId)
	{
		int Count = 0;
		for (int32_t& O : Owners)
		{
			if (O == OwnerId)
			{
				O = 0;
				++Count;
			}
		}
		return Count;
	}

private:
	size_t Index(int X, int Y) const
	{
		return static_cast<size_t>(Y) * static_cast<size_t>(CellsX) + static_cast<size_t>(X);
	}

	std::vector<int32_t> Owners;
	int CellsX = 0;
	int CellsY = 0;
};

// Kurallar sirasiyla: boyut, sinir, egim, cakisma.
inline PlacementResult EvaluatePlacement(const OccupancyGrid& Grid, const HeightField& Field, const Footprint& F, double MaxHeightDelta)
{
	if (F.W <= 0 || F.H <= 0)
	{
		return PlacementResult::InvalidSize;
	}
	if (!Grid.InBounds(F))
	{
		return PlacementResult::OutOfBounds;
	}
	double MinH = 0.0;
	double MaxH = 0.0;
	FootprintHeightRange(Field, F, MinH, MaxH);
	if (MaxH - MinH > MaxHeightDelta)
	{
		return PlacementResult::TooSteep;
	}
	if (!Grid.IsFree(F))
	{
		return PlacementResult::Overlap;
	}
	return PlacementResult::Ok;
}

inline const char* ToString(PlacementResult R)
{
	switch (R)
	{
	case PlacementResult::Ok: return "Ok";
	case PlacementResult::OutOfBounds: return "OutOfBounds";
	case PlacementResult::Overlap: return "Overlap";
	case PlacementResult::TooSteep: return "TooSteep";
	case PlacementResult::InvalidSize: return "InvalidSize";
	}
	return "Unknown";
}

} // namespace Sim
} // namespace TF
