#pragma once

#include <string>
#include <ostream>
#include <functional>
#include <type_traits>

namespace cxxspec {
    namespace util {

        std::string demangle(const char* mangledName);

        template<typename T>
        const T& unmove(T&& param) { return param; }

        namespace is_streamable_imp {

            // check for operator<< in std::ostream first (fundamental types)
            template <typename C>
            auto test(void*) -> std::is_same<
                decltype(std::declval<std::ostream>().operator<<(std::declval<C>())),
                std::ostream &
            >;

            // check for an operator<< via SFINAE
            template <class C>
            auto test(int) -> std::is_same<
                decltype(operator<<(std::declval<std::ostream&>(),
                                    std::declval<const C&>())),
                std::ostream&
            >;

            // TODO: we should also check for 'operator<<' as method inside the class 'C' itself

            template <class>
            auto test(...) -> std::false_type;  // fallthrough
        }

        template <class T>
        struct is_streamable : public decltype( is_streamable_imp::test<T>(0) ) {};

        namespace is_iterateable_impl {
            template <class C>
            auto test(void *) -> decltype(
                std::declval<C>().begin(),
                std::declval<C>().end(),
                std::true_type{}
            );

            template <class C>
            auto test(int) -> decltype(
                std::declval<C>().cbegin(),
                std::declval<C>().cend(),
                std::true_type{}
            );

            template <class>
            auto test(...) -> std::false_type;
        }

        template <class T>
        struct is_iterateable : public decltype( is_iterateable_impl::test<T>(0) ) {};

        template<class T>
        struct is_c_str
        : std::integral_constant<
            bool,
            std::is_same<char const *, typename std::decay<T>::type>::value ||
            std::is_same<char *, typename std::decay<T>::type>::value
        > {};

        template<typename...>
            struct voider { using type = void; };

        template<typename... T>
            using void_t = typename voider<T...>::type;

        namespace mappish_impl {
            template<typename T, typename U = void>
                struct is_mappish_impl : public std::false_type { };
            
            template<typename T>
                struct is_mappish_impl<T, void_t<typename T::key_type,
                                                typename T::mapped_type,
                                                decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>>
                : public std::true_type { };
        }

        template<class T>
        struct is_mappish : public mappish_impl::is_mappish_impl<T>::type { };

        namespace pairish_impl {
            template<typename T, typename U = void>
                struct is_pairish_impl : std::false_type { };

            template<typename T>
                struct is_pairish_impl<T, void_t<typename T::first_type,
                                                typename T::second_type>>
                : std::true_type { };
        }

        template<class T>
        struct is_pairish : pairish_impl::is_pairish_impl<T>::type { };

        template<class T>
        struct is_bounded_array: std::false_type {};
        
        template<class T, std::size_t N>
        struct is_bounded_array<T[N]> : std::true_type {};

        template<class T>
        struct get_array_bound : std::integral_constant<std::size_t, 0> {};

        template<class T, std::size_t N>
        struct get_array_bound<T[N]> : std::integral_constant<std::size_t, N> {};

    }
}