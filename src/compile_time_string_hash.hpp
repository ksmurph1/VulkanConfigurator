// Copyright (C) 2018-2018 ChaosForge
// http://chaosforge.org/
//
// For conditions of distribution and use, see LICENSE file in root folder.

#ifndef NV_STRING_HASH_HPP
#define NV_STRING_HASH_HPP

#include <string>
#include <functional>
#include <string_view>


namespace detail
{
	using uint32 = unsigned;
	using uint64 = unsigned long long;

	template < typename H >
	class fnv_hash_values;

	template <>
	class fnv_hash_values< uint32 > final
	{
        template<typename H>
        friend class fnv_hash;
		   static constexpr uint32 basis = 2166136261UL;
		   static constexpr uint32 prime = 16777619UL;
	};

	template <>
	class fnv_hash_values< uint64 > final
	{
        template<typename H>
        friend class fnv_hash;
		   static constexpr uint64 basis = 14695981039346656037ULL;
		   static constexpr uint64 prime = 1099511628211ULL;
	};

    template <typename H>
	class string_hash;

	template < typename H > 
	class fnv_hash final
	{
        friend class string_hash<H>;

		static constexpr H hash_basis = fnv_hash_values< H >::basis;
		static constexpr H hash_prime = fnv_hash_values< H >::prime;
		static constexpr H string_hash( const char* str, H basis = hash_basis )
		{
			return str != nullptr ? str_hash_impl( str[0], str + 1, basis ) : 0;
		}
		template < typename T >
		static constexpr H hash( const T* p, std::size_t sz, H basis = hash_basis )
		{
			return p != nullptr ? hash_impl( static_cast<const char*>( p ), sizeof( T )*sz, basis ) : 0;
		}
	protected:
		static constexpr H str_hash_impl( char c, const char* remain, H value )
		{
			return c == 0 ? value : str_hash_impl( remain[0], remain + 1, static_cast<H>( value ^ static_cast<H>( c ) ) * hash_prime );
		}
		static constexpr H hash_impl( const char* current, std::size_t remain, H value )
		{
			return remain == 0 ? value : hash_impl( current + 1, remain - 1, static_cast<H>( value ^ static_cast<H>( current[0] ) ) * hash_prime );
		}
	};

	template < typename H >
	class string_hash final
	{
		using hash_type  = H;
		using hasher     = fnv_hash< H >;
        hash_type m_value;
    public:
	    using value_type = H;
		constexpr string_hash() {}
		constexpr explicit string_hash( hash_type value ) : m_value( value ) {}

		constexpr string_hash( const char* str, std::size_t len ) : m_value( hasher::hash( str, len ) ) {}
		//constexpr string_hash( const char* str ) : m_value( hasher::string_hash( str ) ) {}

		inline constexpr string_hash( const std::string_view& rhs ) : m_value( hasher::hash( rhs.data(), rhs.size() ) ) {}
		//inline string_hash( const std::string& rhs ) : m_value( hasher::hash( rhs.data(), rhs.size() ) ) {}

		// Literal constructors
		template< std::size_t N >
		constexpr string_hash( char( &s )[N] ) : m_value( hasher::hash( s, N - 1 ) ) {}
		template< std::size_t N >
		constexpr string_hash( const char( &s )[N] ) : m_value( hasher::hash( s, N - 1 ) ) {}

		constexpr bool is_valid() const { return m_value != 0; }
		constexpr explicit operator bool() const { return is_valid(); }
		constexpr H value() const { return m_value; }
	};

	template < typename H >
	constexpr bool operator==( const string_hash<H>& lhs, const string_hash<H>& rhs )
	{
		return lhs.value() == rhs.value();
	}

	template < typename H >
	constexpr bool operator!=( const string_hash<H>& lhs, const string_hash<H>& rhs )
	{
		return lhs.value() != rhs.value();
	}

	template < typename H >
	constexpr bool operator>( const string_hash<H>& lhs, const string_hash<H>& rhs )
	{
		return lhs.value() > rhs.value();
	}

	template < typename H >
	constexpr bool operator<( const string_hash<H>& lhs, const string_hash<H>& rhs )
	{
		return lhs.value() < rhs.value();
	}
}

namespace std
{

	template < typename H >
	struct hash<detail::string_hash<H>> final
	{
		constexpr std::size_t operator()( const detail::string_hash<H>& sh) const
		{
			return std::size_t( sh.value());
		}
	};
    
}
 using stringhash_uint32=detail::string_hash<uint32_t>;
 constexpr static std::hash<stringhash_uint32> stringview_hash = std::hash<stringhash_uint32>();
#endif // NV_STRING_HASH_HPP