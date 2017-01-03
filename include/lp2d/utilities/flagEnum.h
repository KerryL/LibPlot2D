/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  flagEnum.h
// Date:  12/29/2016
// Auth:  K. Loux
// Desc:  Collection of template methods for performing bitwise operations on
//        enum classes.

#ifndef FLAG_ENUM_H_
#define FLAG_ENUM_H_

namespace LibPlot2D
{

template<typename E>
struct EnableBitwiseOperators
{
	static constexpr bool mEnable = false;
};

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type operator|(
	const E& lhs, const E& rhs)
{
	typedef typename std::underlying_type<E>::type T;
	return static_cast<E>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type operator&(
	const E& lhs, const E& rhs)
{
	typedef typename std::underlying_type<E>::type T;
	return static_cast<E>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type operator^(
	const E& lhs, const E& rhs)
{
	typedef typename std::underlying_type<E>::type T;
	return static_cast<E>(static_cast<T>(lhs) ^ static_cast<T>(rhs));
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type operator~(
	const E& lhs)
{
	typedef typename std::underlying_type<E>::type T;
	return static_cast<E>(~static_cast<T>(lhs));
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type& operator|=(
	E& lhs, const E& rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type& operator&=(
	E& lhs, const E& rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, E>::type& operator^=(
	E& lhs, const E& rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, bool>::type
	operator==(const E& lhs, const typename std::underlying_type<E>::type& rhs)
{
	return static_cast<typename std::underlying_type<E>::type>(lhs) == rhs;
}

template<typename E>
typename std::enable_if<EnableBitwiseOperators<E>::mEnable, bool>::type
	operator!=(const E& lhs, const typename std::underlying_type<E>::type& rhs)
{
	return !(lhs == rhs);
}

}

#endif// FLAG_ENUM_H_