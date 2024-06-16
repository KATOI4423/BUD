#ifndef __BOTTUM_UP_DIFFERENCE_HPP__
#define __BOTTUM_UP_DIFFERENCE_HPP__

#include <cmath>
#include <concepts>
#include <compare>
#include <complex>
#include <iostream>
#include <type_traits>

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
		explicit constexpr BUD(const Type &x = 0, const Type &dx = 0) noexcept
			: x(x), dx(dx) {};

		template <Arithmetic type>
		requires std::is_convertible_v<type, Type>
		explicit constexpr BUD(const BUD<type> &bud) noexcept
			: x(static_cast<Type>(bud.get_x()))
			, dx(static_cast<Type>(bud.get_dx())) {};

		constexpr BUD(const BUD<Type> &bud) noexcept
			: x(bud.x), dx(bud.dx) {};

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
	BUD<Type> sqrt(const BUD<Type> &bud)
	{
		return BUD<Type>(std::sqrt(bud.get_x()),
		                 0.5 * bud.get_dx() / std::sqrt(bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> cbrt(const BUD<Type> &bud)
	{
		return BUD<Type>(std::cbrt(bud.get_x()),
		                 bud.get_dx() * std::cbrt(bud.get_x()) / (3.0 * bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> sin(const BUD<Type> &bud)
	{
		return BUD<Type>(std::sin(bud.get_x()),
		                 std::cos(bud.get_x()) * bud.get_dx());
	}

	template <Arithmetic Type>
	BUD<Type> cos(const BUD<Type> &bud)
	{
		return BUD<Type>(std::cos(bud.get_x()),
		                 -std::sin(bud.get_x()) * bud.get_dx());
	}

	template <Arithmetic Type>
	BUD<Type> tan(const BUD<Type> &bud)
	{
		Type COS = std::cos(bud.get_x());
		return BUD<Type>(std::tan(bud.get_x()),
		                 bud.get_dx() / (COS * COS));
	}

	template <Arithmetic Type>
	BUD<Type> asin(const BUD<Type> &bud)
	{
		return BUD<Type>(std::asin(bud.get_x()),
		                 bud.get_dx() / std::sqrt(1.0 - bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> acos(const BUD<Type> &bud)
	{
		return BUD<Type>(std::acos(bud.get_x()),
		                 -bud.get_dx() / std::sqrt(1.0 - bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> atan(const BUD<Type> &bud)
	{
		return BUD<Type>(std::atan(bud.get_x()),
		                 bud.get_dx() / (1.0 + bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> sinh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::sinh(bud.get_x()),
		                 bud.get_dx() * std::cosh(bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> cosh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::cosh(bud.get_x()),
		                 bud.get_dx() * std::sinh(bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> tanh(const BUD<Type> &bud)
	{
		Type COSH = std::cosh(bud.get_x());
		return BUD<Type>(std::tanh(bud.get_x()),
		                 bud.get_dx() / (COSH * COSH));
	}

	template <Arithmetic Type>
	BUD<Type> asinh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::asinh(bud.get_x()),
		                 bud.get_dx() / std::sqrt(bud.get_x() * bud.get_x() + 1.0));
	}

	template <Arithmetic Type>
	BUD<Type> acosh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::acosh(bud.get_x()),
		                 bud.get_dx() / std::sqrt(bud.get_x() * bud.get_x() - 1.0));
	}

	template <Arithmetic Type>
	BUD<Type> atanh(const BUD<Type> &bud)
	{
		return BUD<Type>(std::atanh(bud.get_x()),
		                 bud.get_dx() / (1.0 - bud.get_x() * bud.get_x()));
	}

	template <Arithmetic Type>
	BUD<Type> exp(const BUD<Type> &bud)
	{
		Type EXP = std::exp(bud.get_x());
		return BUD<Type>(EXP, bud.get_dx() * EXP);
	}

	template <Arithmetic Type>
	BUD<Type> log(const BUD<Type> &bud)
	{
		return BUD<Type>(std::log(bud.get_x()), bud.get_dx() / bud.get_x());
	}

	template <Arithmetic Type>
	BUD<Type> log10(const BUD<Type> &bud)
	{
		const Type LOG10e = 1.0 / std::log(static_cast<Type>(10.0));
		return LOG10e * log(bud);
	}

	template <Arithmetic Type, Arithmetic type>
	BUD<std::common_type_t<Type, type>> pow(const BUD<Type> &right, const BUD<type> &left)
	{
		using rettype = std::common_type_t<Type, type>;
		const rettype POW = std::pow(static_cast<rettype>(right.get_x()), static_cast<rettype>(left.get_x()));
		return BUD<rettype>(POW, (std::log(right.get_x()) * left.get_dx() + right.get_dx() * left.get_x() / right.get_x()) * POW);
	}

	/**
	 * @brief namespaceをtemplateで擬似的に使用するための構造体
	 * @details ex) template <typename namespace_struct>
	 *              func(){ namespace_struct::specity_func(); }
	 */
	template <Arithmetic Type>
	struct BUD_Space {
		using ret_type = BUD<Type>;

		static BUD<Type> _sqrt(const std::vector<BUD<Type>> &args){ return BottomUpDiff::sqrt<Type>(args[0]); }
		static BUD<Type> _cbrt(const std::vector<BUD<Type>> &args){ return BottomUpDiff::cbrt<Type>(args[0]); }
		static BUD<Type> _sin(const std::vector<BUD<Type>> &args){ return BottomUpDiff::sin<Type>(args[0]); }
		static BUD<Type> _cos(const std::vector<BUD<Type>> &args){ return BottomUpDiff::cos<Type>(args[0]); }
		static BUD<Type> _tan(const std::vector<BUD<Type>> &args){ return BottomUpDiff::tan<Type>(args[0]); }
		static BUD<Type> _asin(const std::vector<BUD<Type>> &args){ return BottomUpDiff::asin<Type>(args[0]); }
		static BUD<Type> _acos(const std::vector<BUD<Type>> &args){ return BottomUpDiff::acos<Type>(args[0]); }
		static BUD<Type> _atan(const std::vector<BUD<Type>> &args){ return BottomUpDiff::atan<Type>(args[0]); }
		static BUD<Type> _sinh(const std::vector<BUD<Type>> &args){ return BottomUpDiff::sinh<Type>(args[0]); }
		static BUD<Type> _cosh(const std::vector<BUD<Type>> &args){ return BottomUpDiff::cosh<Type>(args[0]); }
		static BUD<Type> _tanh(const std::vector<BUD<Type>> &args){ return BottomUpDiff::tanh<Type>(args[0]); }
		static BUD<Type> _asinh(const std::vector<BUD<Type>> &args){ return BottomUpDiff::asinh<Type>(args[0]); }
		static BUD<Type> _acosh(const std::vector<BUD<Type>> &args){ return BottomUpDiff::acosh<Type>(args[0]); }
		static BUD<Type> _atanh(const std::vector<BUD<Type>> &args){ return BottomUpDiff::atanh<Type>(args[0]); }
		static BUD<Type> _exp(const std::vector<BUD<Type>> &args){ return BottomUpDiff::exp<Type>(args[0]); }
		static BUD<Type> _log(const std::vector<BUD<Type>> &args){ return BottomUpDiff::log<Type>(args[0]); }
		static BUD<Type> _log10(const std::vector<BUD<Type>> &args){ return BottomUpDiff::log10<Type>(args[0]); }
		static BUD<Type> _pow(const std::vector<BUD<Type>> &args){ return BottomUpDiff::pow<Type>(args[0], args[1]); }

		static constexpr BUD<Type> (*sqrt)(const std::vector<BUD<Type>>&) = &_sqrt;
		static constexpr BUD<Type> (*cbrt)(const std::vector<BUD<Type>>&) = &_cbrt;
		static constexpr BUD<Type> (*sin)(const std::vector<BUD<Type>>&) = &_sin;
		static constexpr BUD<Type> (*cos)(const std::vector<BUD<Type>>&) = &_cos;
		static constexpr BUD<Type> (*tan)(const std::vector<BUD<Type>>&) = &_tan;
		static constexpr BUD<Type> (*asin)(const std::vector<BUD<Type>>&) = &_asin;
		static constexpr BUD<Type> (*acos)(const std::vector<BUD<Type>>&) = &_acos;
		static constexpr BUD<Type> (*atan)(const std::vector<BUD<Type>>&) = &_atan;
		static constexpr BUD<Type> (*sinh)(const std::vector<BUD<Type>>&) = &_sinh;
		static constexpr BUD<Type> (*cosh)(const std::vector<BUD<Type>>&) = &_cosh;
		static constexpr BUD<Type> (*tanh)(const std::vector<BUD<Type>>&) = &_tanh;
		static constexpr BUD<Type> (*asinh)(const std::vector<BUD<Type>>&) = &_asinh;
		static constexpr BUD<Type> (*acosh)(const std::vector<BUD<Type>>&) = &_acosh;
		static constexpr BUD<Type> (*atanh)(const std::vector<BUD<Type>>&) = &_atanh;
		static constexpr BUD<Type> (*exp)(const std::vector<BUD<Type>>&) = &_exp;
		static constexpr BUD<Type> (*log)(const std::vector<BUD<Type>>&) = &_log;
		static constexpr BUD<Type> (*log10)(const std::vector<BUD<Type>>&) = &_log10;
		static constexpr BUD<Type> (*pow)(const std::vector<BUD<Type>>&) = &_pow;
	};
}

#endif //__BOTTUM_UP_DIFFERENCE_HPP__
