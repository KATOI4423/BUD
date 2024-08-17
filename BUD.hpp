#ifndef __BOTTOM_UP_DIFFERENCE_HPP__
#define __BOTTOM_UP_DIFFERENCE_HPP__

#include <cmath>
#include <concepts>
#include <compare>
#include <complex>
#include <iostream>
#include <type_traits>
#include <vector>

namespace BottomUpDiff
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T> || std::is_same_v<T, std::complex<typename T::value_type>>;

	template <Arithmetic Type>
	class BUD
	{
	protected:
		Type x;   /* variable value */
		Type dx; /* partial derivative value */

	public:
		using value_type = Type;

		constexpr BUD() noexcept = default;

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		constexpr BUD(const type &x = 0, const type &dx = 0) noexcept
			: x(static_cast<Type>(x))
			, dx(static_cast<Type>(dx)) {}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		constexpr BUD(const BUD<type> &bud) noexcept
			: x(static_cast<Type>(bud.get_x()))
			, dx(static_cast<Type>(bud.get_dx())) {}

		constexpr BUD(const BUD<Type> &bud) noexcept
			: x(bud.x), dx(bud.dx) {}

		~BUD() = default;

		BUD(BUD<Type> &&bud) = default;

		/**
		 * @brief use this method if the BUD object is used as variable.
		 *        the BUD class is changed from constant to variable by this method.
		 */
		void select(void) noexcept
		{
			this->dx = 1;
		}


		constexpr Type get_x(void) const noexcept { return this->x; }
		constexpr Type get_dx(void) const noexcept { return this->dx; }


		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type> operator=(const BUD<type> &bud) & noexcept
		{
			this->x = static_cast<Type>(bud.get_x());
			this->dx = static_cast<Type>(bud.get_dx());
			return *this;
		}

		BUD<Type> operator=(const BUD<Type> &bud) & noexcept
		{
			this->x = std::move(bud.x);
			this->dx = std::move(bud.dx);
			return *this;
		}

		BUD<Type> operator=(BUD<Type> &&bud) & noexcept
		{
			if (this != &bud) {
				this->x = std::move(bud.x);
				this->dx = std::move(bud.dx);
			}
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type> operator=(const type &right) & noexcept
		{
			this->x = static_cast<Type>(right);
			this->dx = static_cast<Type>(0);
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator+=(const BUD<type> &right)
		{
			this->x += static_cast<Type>(right.get_x());
			this->dx += static_cast<Type>(right.get_dx());
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator-=(const BUD<type> &right)
		{
			this->x -= static_cast<Type>(right.get_x());
			this->dx -= static_cast<Type>(right.get_dx());
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator*=(const BUD<type> &right)
		{
			this->dx = (this->dx * static_cast<Type>(right.get_x()) + this->x * static_cast<Type>(right.get_dx()));
			this->x *= static_cast<Type>(right.get_x());
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator/=(const BUD<type> &right)
		{
			Type w = this->x / static_cast<Type>(right.get_x());
			this->dx = (this->dx - w * static_cast<Type>(right.get_dx())) / static_cast<Type>(right.get_x());
			this->x = w;
			return *this;
		}


		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator+=(const type &right)
		{
			this->x += static_cast<Type>(right);
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator-=(const type &right)
		{
			this->x -= static_cast<Type>(right);
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator*=(const type &right)
		{
			this->x *= static_cast<Type>(right);
			this->dx *= static_cast<Type>(right);
			return *this;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		BUD<Type>& operator/=(const type &right)
		{
			this->x /= static_cast<Type>(right);
			this->dx /= static_cast<Type>(right);
			return *this;
		}


		BUD<Type> operator+() const noexcept { return *this; }
		BUD<Type> operator-() const noexcept { return BUD<Type>(-(this->x), -(this->dx)); }


		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		std::strong_ordering operator<=>(const BUD<type> &right) const noexcept
		{
			if (this->x < static_cast<Type>(right.get_x())) {
				return std::strong_ordering::less;
			}
			if (this->x > static_cast<Type>(right.get_x())) {
				return std::strong_ordering::greater;
			}
			return std::strong_ordering::equal;
		}

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		bool operator==(const BUD<type> &right) const noexcept
		{
			return (this->x == static_cast<Type>(right.get_x()));
		}


		friend std::ostream& operator<<(std::ostream &os, const BUD<Type> &bud)
		{
			os << "{ " << bud.x << ", " << bud.dx << " }";
			return os;
		}
	};

	template <Arithmetic Type, Arithmetic type>
	auto operator+(const BUD<Type> &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) += right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator-(const BUD<Type> &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) -= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator*(const BUD<Type> &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) *= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator/(const BUD<Type> &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) /= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator^(const BUD<Type> &left, const BUD<type> &right) { return pow(left, right); }

	template <Arithmetic Type, Arithmetic type>
	auto operator+(const BUD<Type> &left, const type &right) { return BUD<std::common_type_t<Type, type>>(left) += right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator-(const BUD<Type> &left, const type &right) { return BUD<std::common_type_t<Type, type>>(left) -= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator*(const BUD<Type> &left, const type &right) { return BUD<std::common_type_t<Type, type>>(left) *= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator/(const BUD<Type> &left, const type &right) { return BUD<std::common_type_t<Type, type>>(left) /= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator^(const BUD<Type> &left, const type &right) { return pow(left, BUD<std::common_type_t<Type, type>>(right)); }

	template <Arithmetic Type, Arithmetic type>
	auto operator+(const Type &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) += right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator-(const Type &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) -= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator*(const Type &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) *= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator/(const Type &left, const BUD<type> &right) { return BUD<std::common_type_t<Type, type>>(left) /= right; }

	template <Arithmetic Type, Arithmetic type>
	auto operator^(const Type &left, const BUD<type> &right) { return pow(BUD<std::common_type_t<Type, type>>(left), right); }

	template <Arithmetic Type>
	inline BUD<Type> sqrt(const BUD<Type> &bud)
	{
		return BUD<Type>(std::sqrt(bud.get_x()),
		                 0.5 * bud.get_dx() / std::sqrt(bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> cbrt(const BUD<Type> &bud)
	{
		return BUD<Type>(std::cbrt(bud.get_x()),
		                 bud.get_dx() * std::cbrt(bud.get_x()) / (3.0 * bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> sin(const BUD<Type> &bud)
	{
		return BUD<Type>(std::sin(bud.get_x()),
		                 std::cos(bud.get_x()) * bud.get_dx());
	}

	template <Arithmetic Type>
	inline BUD<Type> cos(const BUD<Type> &bud)
	{
		return BUD<Type>(std::cos(bud.get_x()),
		                 -std::sin(bud.get_x()) * bud.get_dx());
	}

	template <Arithmetic Type>
	inline BUD<Type> tan(const BUD<Type> &bud)
	{
		Type COS = std::cos(bud.get_x());
		return BUD<Type>(std::tan(bud.get_x()),
		                 bud.get_dx() / (COS * COS));
	}

	template <Arithmetic Type>
	inline BUD<Type> asin(const BUD<Type> &bud)
	{
		return BUD<Type>(std::asin(bud.get_x()),
		                 bud.get_dx() / std::sqrt(1.0 - bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> acos(const BUD<Type> &bud)
	{
		return BUD<Type>(std::acos(bud.get_x()),
		                 -bud.get_dx() / std::sqrt(1.0 - bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> atan(const BUD<Type> &bud)
	{
		return BUD<Type>(std::atan(bud.get_x()),
		                 bud.get_dx() / (1.0 + bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> sinh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::sinh(bud.get_x()),
		                 bud.get_dx() * std::cosh(bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> cosh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::cosh(bud.get_x()),
		                 bud.get_dx() * std::sinh(bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> tanh(const BUD<Type> &bud)
	{
		Type COSH = std::cosh(bud.get_x());
		return BUD<Type>(std::tanh(bud.get_x()),
		                 bud.get_dx() / (COSH * COSH));
	}

	template <Arithmetic Type>
	inline BUD<Type> asinh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::asinh(bud.get_x()),
		                 bud.get_dx() / std::sqrt(bud.get_x() * bud.get_x() + 1.0));
	}

	template <Arithmetic Type>
	inline BUD<Type> acosh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::acosh(bud.get_x()),
		                 bud.get_dx() / std::sqrt(bud.get_x() * bud.get_x() - 1.0));
	}

	template <Arithmetic Type>
	inline BUD<Type> atanh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::atanh(bud.get_x()),
		                 bud.get_dx() / (1.0 - bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	inline BUD<Type> exp(const BUD<Type> &bud)
	{
		Type EXP = std::exp(bud.get_x());
		return BUD<Type>(EXP, bud.get_dx() * EXP);
	}

	template <Arithmetic Type>
	inline BUD<Type> log(const BUD<Type> &bud)
	{
		return BUD<Type>(std::log(bud.get_x()), bud.get_dx() / bud.get_x());
	}

	template <Arithmetic Type>
	inline BUD<Type> log10(const BUD<Type> &bud)
	{
		const Type LOG10e = 1.0 / std::log(static_cast<Type>(10.0));
		return LOG10e * log(bud);
	}

	template <Arithmetic Type, Arithmetic type>
	inline BUD<std::common_type_t<Type, type>> pow(const BUD<Type> &right, const BUD<type> &left)
	{
		using rettype = std::common_type_t<Type, type>;
		const rettype POW = std::pow(static_cast<rettype>(right.get_x()), static_cast<rettype>(left.get_x()));
		return BUD<rettype>(POW, (std::log(right.get_x()) * left.get_dx() + right.get_dx() * left.get_x() / right.get_x()) * POW);
	}
}

/** @brief overload std function */
namespace std
{
	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> sqrt(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::sqrt(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> cbrt(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::cbrt(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> sin(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::sin(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> cos(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::cos(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> tan(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::tan(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> asin(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::asin(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> acos(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::acos(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> atan(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::atan(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> sinh(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::sinh(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> cosh(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::cosh(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> tanh(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::tanh(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> asinh(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::asinh(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> acosh(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::acosh(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> atanh(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::atanh(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> exp(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::exp(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> log(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::log(bud); }

	template <Arithmetic Type>
	inline BottomUpDiff::BUD<Type> log10(const BottomUpDiff::BUD<Type> &bud) { return BottomUpDiff::log10(bud); }

	template <Arithmetic Type, Arithmetic type>
	inline BottomUpDiff::BUD<std::common_type_t<Type, type>> pow(const BottomUpDiff::BUD<Type> &right, const BottomUpDiff::BUD<type> &left)
	{ return BottomUpDiff::pow(right, left); }

}

#endif //__BOTTOM_UP_DIFFERENCE_HPP__
