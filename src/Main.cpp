#include <iostream>

#ifdef DEBUG
static const char* __message = "Debug";
#elif defined(NDEBUG)
static const char* __message = "Release";
#endif // DEBUG


int main(int argc, char** argv)
{
	std::cout << "Hello! EsoxTorrent " << __message << std::endl;
	return 0;
}