
./b2.exe toolset=msvc address-model=64 variant=debug,release link=static threading=multi runtime-link=static --stagedir='./' cxxstd=latest define=BOOST_USE_WINAPI_VERSION=0x0a00
