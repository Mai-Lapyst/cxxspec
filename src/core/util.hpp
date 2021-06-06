#pragma once

#include <string>
#include <ostream>
#include <functional>

#if defined(__GNUG__) || defined(__clang__)
    #include <cxxabi.h>
    #include <memory>
#else
    #warning "Platform not fully supported; missing symbol demangling support!"
#endif

namespace cxxspec {
    namespace util {

        #if defined(__GNUG__) || defined(__clang__)
            std::string demangle(const char* mangledName) {
                int status;
                std::unique_ptr<char[], void (*)(void *)> result{
                    abi::__cxa_demangle(mangledName, NULL, NULL, &status), std::free};
                return (status == 0) ? result.get() : mangledName;
            }
        #else
            std::string demangle(const char* mangledName) {
                return mangledName;
            }
        #endif

        namespace is_streamable_imp {

            // check for operator<< in std::ostream first (fundamental types)
            template <typename C>
            auto test(void*) -> std::is_same<
                decltype(std::declval<std::ostream>().operator<<(std::declval<C>())),
                std::ostream &
            >;

            // check for an operator<< via SFINAE (const)
            template <class C>
            auto test(int) -> std::is_same<
                decltype(operator<<(std::declval<std::ostream&>(),
                                    std::declval<C const &>())),
                std::ostream&
            >;

            // check for an operator<< via SFINAE
            template <class C>
            auto test(int) -> std::is_same<
                decltype(operator<<(std::declval<std::ostream&>(),
                                    std::declval<C&>())),
                std::ostream&
            >;

            // TODO: we should also check for 'operator<<' as method inside the class 'C' itself

            template <class>
            auto test(...) -> std::false_type;  // fallthrough
        }

        template <class T>
        struct is_streamable : public decltype( is_streamable_imp::test<T>(0) ) {};

    }
}