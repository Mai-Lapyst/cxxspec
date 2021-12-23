#include "./util.hpp"

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

            std::string current_exception_typename() {
                return demangle(abi::__cxa_current_exception_type()->name());
            }
        #else
            std::string demangle(const char* mangledName) {
                return mangledName;
            }
            std::string current_exception_typename() {
                return "got unknown exception type";
            }
        #endif

    }
}