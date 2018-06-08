#pragma once
#include <cmath>
#include <type_traits>

namespace ds
{
	//长度，质量，时间，电流，温度，物质的量，光强
	template <int L, int M, int T, int I, int Θ, int N, int J>
	struct Unit
	{
		double value;
		Unit operator-() const { return Unit(-value); }
		Unit &operator+=(Unit rhs) { return value += rhs.value, *this; }
		Unit &operator-=(Unit rhs) { return value -= rhs.value, *this; }
		friend Unit operator+(Unit lhs, Unit rhs) { return lhs += rhs; }
		friend Unit operator-(Unit lhs, Unit rhs) { return lhs -= rhs; }
		template <int L1, int M1, int T1, int I1, int Θ1, int N1, int J1>
		friend auto operator*(Unit lhs, Unit<L1, M1, T1, I1, Θ1, N1, J1> rhs)
		{
			return Unit<L + L1, M + M1, T + T1, I + I1, Θ + Θ1, N + N1, J + J1>(lhs.value * rhs.value);
		}
		template <int L1, int M1, int T1, int I1, int Θ1, int N1, int J1>
		friend auto operator/(Unit lhs, Unit<L1, M1, T1, I1, Θ1, N1, J1> rhs)
		{
			return Unit<L - L1, M - M1, T - T1, I - I1, Θ - Θ1, N - N1, J - J1>(lhs.value / rhs.value);
		}
		Unit &operator*=(double rhs) { return value *= rhs, *this; }
		Unit &operator/=(double rhs) { return value /= rhs, *this; }
		friend Unit operator*(Unit lhs, double rhs) { return lhs *= rhs; }
		friend Unit operator*(double lhs, Unit rhs) { return rhs *= lhs; }
		friend Unit operator/(Unit lhs, double rhs) { return Unit(lhs.value / rhs); }

		bool operator<(Unit rhs) const { return value < rhs.value; }
		bool operator>(Unit rhs) const { return value > rhs.value; }
		bool operator<=(Unit rhs) const { return value <= rhs.value; }
		bool operator>=(Unit rhs) const { return value >= rhs.value; }
		bool operator==(Unit rhs) const { return value == rhs.value; }
		bool operator!=(Unit rhs) const { return value != rhs.value; }

		//一个物理量的实数次方没有意义(纯数值除外),而且每一个量纲都必须整除
		//因为MSVC的日常bug，没办法写成SFINAE的语法，只能用static_assert
		template <int Num, int Denom = 1>
		friend auto pow(Unit x)
		{
			static_assert(L * Num % Denom == 0 && M * Num % Denom == 0 && T * Num % Denom == 0 && I * Num % Denom == 0 && Θ * Num % Denom == 0 && N * Num % Denom == 0 && J * Num % Denom == 0,
				"fractional dimen is illgeal");
			return Unit<L * Num / Denom, M * Num / Denom, T * Num / Denom, I * Num / Denom, Θ * Num / Denom, N * Num / Denom, J * Num / Denom>(std::pow(x.value, Num * 1.0 / Denom));
		}

		//纯数值允许非explicit初始化，有单位数值必须explicit初始化
		template <typename = std::enable_if_t<L == 0 && M == 0 && T == 0 && I == 0 && Θ == 0 && N == 0 && J == 0>>
		constexpr Unit(double value = 0.0) :value(value) {}
		constexpr explicit Unit(double value = 0.0) :value(value) {}

		//同上
		template <typename = std::enable_if_t<L == 0 && M == 0 && T == 0 && I == 0 && Θ == 0 && N == 0 && J == 0>>
		operator double() const { return value; }
		explicit operator double() const { return value; }
	};
	namespace unit
	{
		using Numeric = Unit<0, 0, 0, 0, 0, 0, 0>;
		using Length = Unit<1, 0, 0, 0, 0, 0, 0>;
		using Mass = Unit<0, 1, 0, 0, 0, 0, 0>;
		using Time = Unit<0, 0, 1, 0, 0, 0, 0>;
		using ElectricCurrent = Unit<0, 0, 0, 1, 0, 0, 0>;
		using Temperature = Unit<0, 0, 0, 0, 1, 0, 0>;
		using Amount = Unit<0, 0, 0, 0, 0, 1, 0>;
		using LuminousIntensity = Unit<0, 0, 0, 0, 0, 0, 1>;

		constexpr Length operator""_m(long double value) { return Length(static_cast<double>(value)); }
		constexpr Length operator""_m(unsigned long long value) { return Length(static_cast<double>(value)); }
		constexpr Mass operator""_kg(long double value) { return Mass(static_cast<double>(value)); }
		constexpr Mass operator""_kg(unsigned long long value) { return Mass(static_cast<double>(value)); }
		constexpr Time operator""_s(long double value) { return Time(static_cast<double>(value)); }
		constexpr Time operator""_s(unsigned long long value) { return Time(static_cast<double>(value)); }
		constexpr ElectricCurrent operator""_A(long double value) { return ElectricCurrent(static_cast<double>(value)); }
		constexpr ElectricCurrent operator""_A(unsigned long long value) { return ElectricCurrent(static_cast<double>(value)); }
		constexpr Temperature operator""_K(long double value) { return Temperature(static_cast<double>(value)); }
		constexpr Temperature operator""_K(unsigned long long value) { return Temperature(static_cast<double>(value)); }
		constexpr Amount operator""_mol(long double value) { return Amount(static_cast<double>(value)); }
		constexpr Amount operator""_mol(unsigned long long value) { return Amount(static_cast<double>(value)); }
		constexpr LuminousIntensity operator""_cd(long double value) { return LuminousIntensity(static_cast<double>(value)); }
		constexpr LuminousIntensity operator""_cd(unsigned long long value) { return LuminousIntensity(static_cast<double>(value)); }

		using Velocity = decltype(Length() / Time());
		using Acceleration = decltype(Velocity() / Time());
		using Force = decltype(Mass() * Acceleration());
		using Work = decltype(Force() * Length());
		using Power = decltype(Work() / Time());
		using Area = decltype(Length() * Length());
		using Volume = decltype(Length() * Length() * Length());
		using Pressure = decltype(Force() / Area());
		
		using Momentum = decltype(Mass() * Velocity());
		using AngularMomentum = decltype(Length() * Momentum());
		using Moment = decltype(Length() * Force()); //力矩

		constexpr Velocity operator""_m_s(long double value) { return Velocity(static_cast<double>(value)); }
		constexpr Velocity operator""_m_s(unsigned long long value) { return Velocity(static_cast<double>(value)); }
		constexpr Acceleration operator""_m_s2(long double value) { return Acceleration(static_cast<double>(value)); }
		constexpr Acceleration operator""_m_s2(unsigned long long value) { return Acceleration(static_cast<double>(value)); }
		constexpr Force operator""_N(long double value) { return Force(static_cast<double>(value)); }
		constexpr Force operator""_N(unsigned long long value) { return Force(static_cast<double>(value)); }
		constexpr Work operator""_J(long double value) { return Work(static_cast<double>(value)); }
		constexpr Work operator""_J(unsigned long long value) { return Work(static_cast<double>(value)); }
		constexpr Power operator""_W(long double value) { return Power(static_cast<double>(value)); }
		constexpr Power operator""_W(unsigned long long value) { return Power(static_cast<double>(value)); }
		constexpr Area operator""_m2(long double value) { return Area(static_cast<double>(value)); }
		constexpr Area operator""_m2(unsigned long long value) { return Area(static_cast<double>(value)); }
		constexpr Volume operator""_m3(long double value) { return Volume(static_cast<double>(value)); }
		constexpr Volume operator""_m3(unsigned long long value) { return Volume(static_cast<double>(value)); }
		constexpr Pressure operator""_pa(long double value) { return Pressure(static_cast<double>(value)); }
		constexpr Pressure operator""_pa(unsigned long long value) { return Pressure(static_cast<double>(value)); }
	}
}