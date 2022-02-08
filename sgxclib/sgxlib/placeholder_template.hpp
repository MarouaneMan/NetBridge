#ifndef SGX_PLACEHOLDER_TEMPLATE_H_
# define SGX_PLACEHOLDER_TEMPLATE_H_

namespace SGX
{
    namespace Functional
    {
        template<int>
        struct placeholder_template
        {
        };

    } // FUNCTIONAL
} // SGX

namespace std
{
    template<int N>
    struct is_placeholder < SGX::Functional::placeholder_template<N> > : std::integral_constant<int, N + 1>
    {
    };
}

#endif // SGX_PLACEHOLDER_TEMPLATE_H_