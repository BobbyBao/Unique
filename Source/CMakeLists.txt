if(PLATFORM_WIN32 OR PLATFORM_LINUX)
    cmake_minimum_required (VERSION 3.13)
    option(INSTALL_DILIGENT_SAMPLES "Enable installation of samples and tutorials" ON)
else()
    cmake_minimum_required (VERSION 3.6)
    set(INSTALL_DILIGENT_SAMPLES OFF)
endif()

add_subdirectory(UniqueBase)
add_subdirectory(UniqueEngine)
add_subdirectory(UniqueSample)
