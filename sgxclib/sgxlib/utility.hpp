#ifndef SGX_UTILITY_H_
# define SGX_UTILITY_H_

namespace SGX
{
    namespace Utility
    {

        // BEGIN : Int Sequence
        template<int...>
        struct int_sequence
        {
        };

        template<int N, int... Is>
        struct make_int_sequence : make_int_sequence<N - 1, N - 1, Is...>
        {
        };

        template<int... Is>
        struct make_int_sequence<0, Is...> : int_sequence<Is...>
        {
        };
        // END : Int Sequence

    }; // Utility
}; // SGX

#endif // SGX_UTILITY_H_