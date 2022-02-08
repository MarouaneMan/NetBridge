#ifndef STRUCT_CASTER_H_
# define STRUCT_CASTER_H_

#include <vector>
#include <functional>
#include <sstream>

namespace SGX
{
    namespace StructCaster
    {

		namespace Exception
		{

			// Bad size exception, buffer size does not match struct size
			class BadSizeException : public std::exception
			{
			public:
				BadSizeException(size_t expected, size_t actual)
				{
					std::stringstream ss;
					ss << "expected size = " << expected << ", actual size = " << actual;
					what_message = ss.str();
				}

				const char *what() const throw ()
				{
					return what_message.c_str();
				}

			private:
				std::string	what_message;
			};

		}; //EXCEPTION


        namespace Vector
        {
			// Any : Provide (vector size, struct size) functor comparator
			template<class StructType, template<typename Ty> class ComparatorFunctor>
			static StructType *Any(std::vector<char> &buffer)
			{
				if (ComparatorFunctor<size_t>()(buffer.size(), sizeof(StructType)))
				{
					return reinterpret_cast<StructType*>(&buffer[0]);
				}
				throw StructCaster::Exception::BadSizeException(sizeof(StructType), buffer.size());
			}

            // Size of vector must be equal to struct size
            template<class StructType>
            StructType	*Eq(std::vector<char> &buffer)
            {
                return Any<StructType, std::equal_to >(buffer);
            };

            // Size of vector must be greater or equal to struct size
            template<class StructType>
            StructType	*Gte(std::vector<char> &buffer)
            {
                return Any<StructType, std::greater_equal>(buffer);
            };

        }; //VECTOR
     
    }; //STRUCT_CASTER
}; //SGX

#endif // STRUCT_CASTER_H_