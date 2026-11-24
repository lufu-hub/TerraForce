// Terraforge - motor bagimsiz simulasyon cekirdegi (saf C++17, Unreal tipi kullanmaz).
// Birimler: metre. Bu dosya hem Unreal modulunde hem de Tests/SimTests altinda derlenir.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace TF
{
namespace Sim
{

struct HeightFieldParams
{
	int CellsX = 100;            // hucre sayisi (X)
	int CellsY = 100;            // hucre sayisi (Y)
	double CellSize = 1.0;       // metre
	double Amplitude = 3.0;      // metre, +/- tepe yuksekligi
	double BaseFrequency = 0.02; // 1/metre
	int Octaves = 4;
	uint32_t Seed = 1337u;
};

inline uint32_t Hash2(int32_t X, int32_t Y, uint32_t Seed)
{
	uint32_t H = Seed ^ 0x9e3779b9u;
	H ^= static_cast<uint32_t>(X) * 0x27d4eb2du;
	H = (H ^ (H >> 15)) * 0x85ebca6bu;
	H ^= static_cast<uint32_t>(Y) * 0x165667b1u;
	H = (H ^ (H >> 13)) * 0xc2b2ae35u;
	H ^= H >> 16;
	return H;
}

inline double HashToUnit(uint32_t H)
{
	return static_cast<double>(H) / 4294967295.0;
}

inline double SmoothStep01(double T)
{
	return T * T * (3.0 - 2.0 * T);
}

// [-1, 1] araliginda deterministik deger gurultusu.
inline double ValueNoise2D(double X, double Y, uint32_t Seed)
{
	const double FX = std::floor(X);
	const double FY = std::floor(Y);
	const int32_t IX = static_cast<int32_t>(FX);
	const int32_t IY = static_cast<int32_t>(FY);
	const double TX = SmoothStep01(X - FX);
	const double TY = SmoothStep01(Y - FY);
	const double V00 = HashToUnit(Hash2(IX, IY, Seed));
	const double V10 = HashToUnit(Hash2(IX + 1, IY, Seed));
	const double V01 = HashToUnit(Hash2(IX, IY + 1, Seed));
	const double V11 = HashToUnit(Hash2(IX + 1, IY + 1, Seed));
	const double A = V00 + (V10 - V00) * TX;
	const double B = V01 + (V11 - V01) * TX;
	return (A + (B - A) * TY) * 2.0 - 1.0;
}

// Fraktal (fBm) gurultu, [-1, 1] araligina normalize.
inline double FractalNoise2D(double X, double Y, int Octaves, uint32_t Seed)
{
	double Sum = 0.0;
	double Amp = 1.0;
	double Freq = 1.0;
	double Norm = 0.0;
	const int Count = std::max(1, Octaves);
	for (int O = 0; O < Count; ++O)
	{
		Sum += Amp * ValueNoise2D(X * Freq, Y * Freq, Seed + static_cast<uint32_t>(O) * 101u);
		Norm += Amp;
		Amp *= 0.5;
		Freq *= 2.0;
	}
	return Sum / Norm;
}

class HeightField
{
public:
	void Generate(const HeightFieldParams& Params)
	{
		CellsX = std::max(1, Params.CellsX);
		CellsY = std::max(1, Params.CellsY);
		Cell = Params.CellSize > 0.0 ? Params.CellSize : 1.0;
		Heights.assign(static_cast<size_t>(VertsX()) * static_cast<size_t>(VertsY()), 0.0);
		for (int IY = 0; IY < VertsY(); ++IY)
		{
			for (int IX = 0; IX < VertsX(); ++IX)
			{
				const double WX = IX * Cell * Params.BaseFrequency;
				const double WY = IY * Cell * Params.BaseFrequency;
				Heights[Index(IX, IY)] = Params.Amplitude * FractalNoise2D(WX, WY, Params.Octaves, Params.Seed);
			}
		}
	}

	int GetCellsX() const { return CellsX; }
	int GetCellsY() const { return CellsY; }
	int VertsX() const { return CellsX + 1; }
	int VertsY() const { return CellsY + 1; }
	double CellSize() const { return Cell; }
	double SizeX() const { return CellsX * Cell; }
	double SizeY() const { return CellsY * Cell; }
	bool IsValid() const { return !Heights.empty(); }

	double HeightAtVertex(int IX, int IY) const
	{
		if (Heights.empty())
		{
			return 0.0;
		}
		IX = std::clamp(IX, 0, VertsX() - 1);
		IY = std::clamp(IY, 0, VertsY() - 1);
		return Heights[Index(IX, IY)];
	}

	// Bilinear ornekleme; X/Y metre cinsinden, sinir disi degerler kenara sabitlenir.
	double SampleHeight(double X, double Y) const
	{
		const double GX = std::clamp(X / Cell, 0.0, static_cast<double>(CellsX));
		const double GY = std::clamp(Y / Cell, 0.0, static_cast<double>(CellsY));
		const int IX = std::min(static_cast<int>(std::floor(GX)), CellsX - 1);
		const int IY = std::min(static_cast<int>(std::floor(GY)), CellsY - 1);
		const double TX = GX - IX;
		const double TY = GY - IY;
		const double A = HeightAtVertex(IX, IY) + (HeightAtVertex(IX + 1, IY) - HeightAtVertex(IX, IY)) * TX;
		const double B = HeightAtVertex(IX, IY + 1) + (HeightAtVertex(IX + 1, IY + 1) - HeightAtVertex(IX, IY + 1)) * TX;
		return A + (B - A) * TY;
	}

private:
	size_t Index(int IX, int IY) const
	{
		return static_cast<size_t>(IY) * static_cast<size_t>(VertsX()) + static_cast<size_t>(IX);
	}

	std::vector<double> Heights;
	int CellsX = 0;
	int CellsY = 0;
	double Cell = 1.0;
};

} // namespace Sim
} // namespace TF
