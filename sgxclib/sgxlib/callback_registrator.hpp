#ifndef SGX_CALLBACK_REGISTRATOR_
# define SGX_CALLBACK_REGISTRATOR_

#include <functional>
#include <unordered_map>
#include <array>
#include <stdexcept>
#include <type_traits>
#include "placeholder_template.hpp"
#include "utility.hpp"

namespace SGX
{
    namespace Callback
    {
        namespace Exception
        {
            // Unknown Callback Exception
            class UnknownCallbackException : std::exception
            {
            };
        };

        template<class KeyType, class ReturnType, class ...Args>
        class Registrator
        {

        public:

            // Register new Callback
            template<typename CallerType>
            void	Register(CallerType *caller, KeyType type, ReturnType(CallerType::*Func)(Args...))
            {
                registerInternal<CallerType>(caller, type, Func, SGX::Utility::make_int_sequence<sizeof...(Args)>{});
            }

            // Call callback -- unordered_map
            void	Call(std::string key, Args... args)
            {
                if (callbacks.find(key) == callbacks.end())
                {
                    throw Exception::UnknownCallbackException();
                }
                callbacks[key](args...);
            }

            // Call callback -- std::array
            void	Call(int key, Args... args)
            {
                if (key >= callbacks.size() || !callbacks[key])
                {
                    throw Exception::UnknownCallbackException();
                }
                callbacks[key](args...);
            }

            std::string Typeid()
            {
                return typeid(callbacks).name();
            }

        private:

            // Implement generic placeholders
            template<typename CallerType, int... Is>
            void	registerInternal(CallerType *caller, KeyType type, ReturnType(CallerType::*Func)(Args...), SGX::Utility::int_sequence<Is...>)
            {
                callbacks[type] = std::bind(Func, caller, SGX::Functional::placeholder_template<Is>{}...);
            }

			// Choose map type based on key type
			using MapType = 
				typename std::conditional<std::is_integral<KeyType>::value,
					std::array<std::function<ReturnType(Args...)>, 64>,
					std::unordered_map<KeyType, std::function<ReturnType(Args...)>>
				>::type;

			MapType callbacks;
        };
    }  //Callback
}; //SGX

#endif // SGX_CALLBACK_REGISTRATOR_