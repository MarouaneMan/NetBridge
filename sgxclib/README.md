# SGXCLIB

Set of C++ libraries
- **SGX::Net::Server** : *boost based* TCP Server (UPD : WIP)
- **SGX::Net::Client** : *boost based* TCP Client (UDP : WIP)
- **SGX::Net::Socket** : *boost based* Socket Wrapper
- **SGX::StructCaster::Vector** : Cast vector buffer to struct
- **SGX::Callback::Registrator**: Callback registry
- **SGX::Functional::placeholder_template** : Generic PlaceHolder Template
- **SGX::Utility::make_int_sequence,int_sequence** : int sequence utilities
- **SGX::Date::getDateTime** : format current time(0), const char *fmt* "%d-%m %H:%M:%S"
- **SGX::String::SafeCopyStdArrayToString** : copy std::array to std::string while preventing buffer overflow
- **SGX::String::SafeCopyCArrayToString** : copy c style array to std::string while preventing buffer overflow
- **SGX::Logger::SimpleLogger** : Simple thread safe implementation of SGX::Logger::Interface


#### SGX::Callback::Registrator Example
```cpp
#include "sgxlib/callback_registrator.hpp"
#include <iostream>
#include <string>
class Test
{
public:

	void TestCallbackArray()
	{
		// CallbackRegistrator
		SGX::Callback::Registrator<
			int,                    // KeyType
			void,                   // ReturnType
			std::string>            // Callback Args
			callbackRegistrator(this);

		// Register MyFuncEnum
		const int MyFuncEnum = 0;
		callbackRegistrator.Register(this, MyFuncEnum, &Test::Callback);

		// Print mapType
		std::cout << callbackRegistrator.Typeid() << std::endl;

		// Call MyFuncEnum
		callbackRegistrator.Call(MyFuncEnum, "Test CallBackRegistrator with std::array");
	}

	void TestUnorderedMap()
	{
		// CallbackRegistrator
		SGX::Callback::Registrator<
			std::string,			// KeyType
			void,					// ReturnType
			std::string>			// Callback Args
			callbackRegistrator(this);

		// Register MyFuncEnum
		callbackRegistrator.Register(this, "MyFuncEnum", &Test::Callback);
		
		// Print mapType
		std::cout << callbackRegistrator.Typeid() << std::endl;

		// Call MyFuncEnum
		callbackRegistrator.Call("MyFuncEnum", "Test CallBackRegistrator with std::unordered_map");
	}

	void	Callback(std::string str)
	{
		std::cout << str << std::endl;
	}
};

int		main()
{
	Test t;
	t.TestCallbackArray();
	t.TestUnorderedMap();
}
```
