//---------------------------------------------------------------------------//
//!
//! \file   Utility_Tuple.hpp
//! \author Alex Robinson
//! \brief  Helper function and class declarations for std::tuple
//!
//---------------------------------------------------------------------------//

#ifndef UTILITY_TUPLE_HPP
#define UTILITY_TUPLE_HPP

// Std Lib Includes
#include <iostream>
#include <sstream>
#include <tuple>
#include <utility>
#include <type_traits>

// FRENSIE Includes
#include "Utility_ToStringTraits.hpp"
#include "Utility_FromStringTraits.hpp"
#include "Utility_ComparisonTraits.hpp"
#include "Utility_TypeTraits.hpp"

/*! \defgroup tuple Tuple.
 *
 * Additional helper functions and classes for use with std::tuple and
 * std::pair types have been defined that do not appear in the standard 
 * library. For example, Utility::get, Utility::TupleElement and 
 * Utility::TupleSize can all be used instead of std::get, std::tuple_element 
 * and std::tuple_size respectively to create methods that can take both tuple
 * types and arithmetic types. 
 */

namespace Utility{

/*! \brief Enum for refering to commonly used tuple elements (previously 
 * referred to as members)
 * \ingroup tuple
 */
enum TupleMember{
  FIRST = 0,
  SECOND,
  THIRD,
  FOURTH
};

/*! The tuple size struct (the default is 1)
 * \ingroup tuple
 */
template<typename T, typename Enabled = void>
struct TupleSize : public std::integral_constant<size_t,1>
{ /* ... */ };

/*! \brief Partial specialization of TupleSize for all const types
 * \ingroup tuple
 */
template<typename T>
struct TupleSize<const T> : public TupleSize<T>
{ /* ... */ };

/*! \brief Partial specialization of TupleSize for all volatile types
 * \ingroup tuple
 */
template<typename T>
struct TupleSize<volatile T> : public TupleSize<T>
{ /* ... */ };

/*! \brief Partial specialization of TupleSize for all const volatile types
 * \ingroup tuple
 */
template<typename T>
struct TupleSize<const volatile T> : public TupleSize<T>
{ /* ... */ };

/*! \brief Partial specialization of TupleSize for std::tuple and std::pair types
 * \ingroup tuple
 */
template<typename T>
struct TupleSize<T,typename std::enable_if<Utility::IsTuple<T>::value && !std::is_const<T>::value && !std::is_volatile<T>::value>::type> : public std::tuple_size<T>
{ /* ... */ };

/*! The tuple element struct (the default is undefined)
 * \ingroup tuple
 */
template<size_t I, typename T, typename Enabled = void>
struct TupleElement 
{ /* ... */ };

/*! \brief Partial specialization of TupleElement for all const types
 * \ingroup tuple
 */
template<size_t I, typename T>
struct TupleElement<I, T, typename std::enable_if<std::is_const<T>::value && !std::is_volatile<T>::value>::type> 
{
  typedef typename std::add_const<typename TupleElement<I,typename std::remove_const<T>::type>::type>::type type;
};

/*! \brief Partial specialization of TupleElement for all volatile types
 * \ingroup tuple
 */
template<size_t I, typename T>
struct TupleElement<I, T, typename std::enable_if<std::is_volatile<T>::value && !std::is_const<T>::value>::type>
{
  typedef typename std::add_volatile<typename TupleElement<I,typename std::remove_volatile<T>::type>::type>::type type;
};

/*! \brief Partial specialization of TupleElement for all const volatile types
 * \ingroup tuple
 */
template<size_t I, typename T>
struct TupleElement<I, T, typename std::enable_if<std::is_const<T>::value && std::is_volatile<T>::value>::type>
{
  typedef typename std::add_cv<typename TupleElement<I,typename std::remove_cv<T>::type>::type>::type type;
};

/*! \brief Partial specialization of TupleElement for all non-tuple types (as 
 * long as I == 0).
 *
 * Unlike std::tuple_element, this specialization allows one to create
 * methods that take both std::tuple types and non-tuple types (Utility::get
 * must also be used instead of std::get). See the 
 * Utility::Search::binaryLowerBound method for an example.
 * \ingroup tuple
 */
template<size_t I, typename T>
struct TupleElement<I,T,typename std::enable_if<I==0 && !Utility::IsTuple<T>::value && !std::is_const<T>::value && !std::is_volatile<T>::value>::type>
{ typedef T type; };

/*! \brief Partial specialization of TupleElement for std::tuple and std::pair 
 * types
 * \ingroup tuple
 */
template<size_t I, typename T>
struct TupleElement<I,T,typename std::enable_if<Utility::IsTuple<T>::value && !std::is_const<T>::value && !std::is_volatile<T>::value>::type> : public std::tuple_element<I,T>
{ /* ... */ };

/*! Return a reference to the desired tuple element (std::get)
 * \ingroup tuple
 */
template<size_t I, typename T>
inline typename std::enable_if<Utility::IsTuple<T>::value,typename TupleElement<I,T>::type>::type&
get( T& tuple ) noexcept
{ return std::get<I>( tuple ); }

/*! Return a const reference to the desired tuple element (std::get)
 * \ingroup tuple
 */
template<size_t I, typename T>
inline const typename std::enable_if<Utility::IsTuple<T>::value,typename TupleElement<I,T>::type>::type&
get( const T& tuple ) noexcept
{ return std::get<I>( tuple ); }

/*! Return a reference to the desired tuple member 
 *
 * This can be used with all types 
 * (e.g. double my_double = 1.0; Utility::get<0>( my_double ) == 1.0; }
 * \ingroup tuple
 */
template<size_t I, typename T>
inline typename std::enable_if<I==0 && !Utility::IsTuple<T>::value,T>::type&
get( T& value ) noexcept
{ return value; }

/*! Return a const reference to the desired tuple member 
 *
 * This can be used with all types 
 * (e.g. double my_double = 1.0; Utility::get<0>( my_double ) == 1.0; }
 * \ingroup tuple
 */
template<size_t I, typename T>
inline const typename std::enable_if<I==0 && !Utility::IsTuple<T>::value,T>::type&
get( const T& value ) noexcept
{ return value; }

/*! Set the head tuple member value
 *
 * The ValueType must be implicitly convertable to the tuple member type.
 * \ingroup tuple
 */
template<size_t I, typename TupleType, typename ValueType>
inline void set( TupleType& tuple, ValueType value )
{ Utility::get<I>( tuple ) = value; }

/*! Partial specialization of ToStringTraits for tuple types
 * \ingroup tuple
 * \ingroup to_string_traits
 */
template<typename T>
struct ToStringTraits<T,typename std::enable_if<Utility::IsTuple<T>::value>::type>
{
  //! Convert the tuple to a string
  static std::string toString( const T& tuple );

  //! Place the tuple in a stream
  static void toStream( std::ostream& os, const T& tuple );
};

namespace Details{

/*! Remove references from tuple element types (default is undefined)
 * \ingroup tuple
 */
template<typename T>
struct RemoveTupleElementReferences;

/*! Partial specialization of RemoveTupleElementReferences for std::tuple types
 * \ingroup tuple
 */
template<typename... Types>
struct RemoveTupleElementReferences<std::tuple<Types...> >
{
  typedef std::tuple<typename std::remove_reference<Types>::type...> type;
};

/*! Partial specialization of RemoveTupleElementReferences for std::pair types
 * \ingroup tuple
 */
template<typename T1, typename T2>
struct RemoveTupleElementReferences<std::pair<T1,T2> >
{
  typedef std::pair<typename std::remove_reference<T1>::type, typename std::remove_reference<T2>::type> type;
};
  
} // end Details namespace

/*! Partial specialization of FromStringTraits for std::tuple
 * \ingroup tuple
 * \ingroup from_string_traits
 */
template<typename T>
struct FromStringTraits<T,typename std::enable_if<Utility::IsTuple<T>::value>::type>
{
  //! The type that a string will be converted to
  typedef typename Details::RemoveTupleElementReferences<T>::type ReturnType;
  
  //! Convert the string to an object of type T
  static ReturnType fromString( const std::string& obj_rep );

  //! Extract the object from a stream
  static void fromStream( std::istream& is,
                          T& obj,
                          const std::string& = std::string() );
};

/*! \brief The partial specialization of the Utility::ComparisonTraits for
 * tuple types (with at least one element).
 * \ingroup comparison_traits
 */
template<typename... Types>
struct ComparisonTraits<std::tuple<Types...> >
{
  //! The extra data type (usually a comparison tolerance)
  typedef double ExtraDataType;

  //! Create a comparison header
  template<typename ComparisonPolicy>
  static std::string createComparisonHeader(
                           const std::tuple<Types...>& left_value,
                           const std::string& left_name,
                           const bool log_left_name,
                           const std::tuple<Types...>& right_value,
                           const std::string& right_name,
                           const bool log_right_name,
                           const std::string& name_suffix,
                           const ExtraDataType& extra_data = ExtraDataType() );

  //! Compare two tuples
  template<typename ComparisonPolicy>
  static bool compare( const std::tuple<Types...>& left_value,
                       const std::string& left_name,
                       const bool log_left_name,
                       const std::tuple<Types...>& right_value,
                       const std::string& right_name,
                       const bool log_right_name,
                       const std::string& name_suffix,
                       std::ostream& log,
                       const bool log_comparison_details = false,
                       const ExtraDataType& extra_data = ExtraDataType() );
};

/*! \brief The partial specialization of the Utility::ComparisonTraits for
 * empty tuple types.
 * \ingroup comparison_traits
 */
template<>
struct ComparisonTraits<std::tuple<> >
{
  //! The extra data type (usually a comparison tolerance)
  typedef double ExtraDataType;

  //! Create a comparison header
  template<typename ComparisonPolicy>
  static std::string createComparisonHeader(
                           const std::tuple<>& left_value,
                           const std::string& left_name,
                           const bool log_left_name,
                           const std::tuple<>& right_value,
                           const std::string& right_name,
                           const bool log_right_name,
                           const std::string& name_suffix,
                           const ExtraDataType& extra_data = ExtraDataType() );

  //! Compare two tuples
  template<typename ComparisonPolicy>
  static bool compare( const std::tuple<>& left_value,
                       const std::string& left_name,
                       const bool log_left_name,
                       const std::tuple<>& right_value,
                       const std::string& right_name,
                       const bool log_right_name,
                       const std::string& name_suffix,
                       std::ostream& log,
                       const bool log_comparison_header = false,
                       const ExtraDataType& extra_data = ExtraDataType() );
};

// /*! \brief The partial specialization of the Utility::ComparisonTraits for
//  * const tuple types (with at least one element).
//  * \ingroup comparison_traits
//  */
// template<typename... Types>
// struct ComparisonTraits<const std::tuple<Types...> > : public ComparisonTraits<std::tuple<Types...> >
// { /* ... */ };

// /*! \brief The partial specialization of the Utility::ComparisonTraits for
//  * volatile tuple types (with at least one element).
//  * \ingroup comparison_traits
//  */
// template<typename... Types>
// struct ComparisonTraits<volatile std::tuple<Types...> > : public ComparisonTraits<std::tuple<Types...> >
// { /* ... */ };

// /*! \brief The partial specialization of the Utility::ComparisonTraits for
//  * const volatile tuple types (with at least one element).
//  * \ingroup comparison_traits
//  */
// template<typename... Types>
// struct ComparisonTraits<const volatile std::tuple<Types...> > : public ComparisonTraits<std::tuple<Types...> >
// { /* ... */ };
  
} // end Utility namespace

namespace std{

/*! Place a std::tuple in a stream
 * \ingroup tuple
 */
template<typename... Types>
inline std::ostream& operator<<( std::ostream& os,
                                 const std::tuple<Types...>& tuple )
{
  Utility::toStream( os, tuple );

  return os;
}

/*! Extract a std::tuple from a stream
 * \ingroup tuple
 */
template<typename... Types>
inline std::istream& operator>>( std::istream& is,
                                 std::tuple<Types...>& tuple )
{
  Utility::fromStream( is, tuple );

  return is;
}

/*! Place a std::pair in a stream
 * \ingroup tuple
 */
template<typename T1, typename T2>
inline std::ostream& operator<<( std::ostream& os,
                                 const std::pair<T1,T2>& pair )
{
  Utility::toStream( os, pair );

  return os;
}

/*! Extract a std::pair from a stream
 * \ingroup tuple
 */
template<typename T1, typename T2>
inline std::istream& operator>>( std::istream& is,
                                 std::pair<T1,T2>& pair )
{
  Utility::fromStream( is, pair );

  return is;
}
  
} // end std namespace

//---------------------------------------------------------------------------//
// Template Includes.
//---------------------------------------------------------------------------//

#include "Utility_Tuple_def.hpp"

//---------------------------------------------------------------------------//

#endif // end UTILITY_TUPLE_HPP

//---------------------------------------------------------------------------//
// end Utility_Tuple.hpp
//---------------------------------------------------------------------------//
