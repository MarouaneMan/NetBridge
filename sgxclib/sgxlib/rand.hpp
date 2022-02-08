#ifndef SGX_RAND_H_
# define SGX_RAND_H_

#include <random>

namespace SGX
{
    namespace Rand
    {
        template<typename Ty>
        class Bounded
        {

        public:

            Bounded(Ty start, Ty end) : gen(rd()), dist(start, end) {}
            
            Ty Next() { return dist(gen); }

        private:

            thread_local static std::random_device rd;  //Will be used to obtain a seed for the random number engine
            std::mt19937 gen;                           //Standard mersenne_twister_engine seeded with rd()
            std::uniform_int_distribution<Ty> dist;
        };

        template <typename Ty> thread_local std::random_device Bounded<Ty>::rd;

    };
    
};

#endif // RAND