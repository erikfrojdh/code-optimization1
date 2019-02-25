find_package(ROOT CONFIG REQUIRED)
target_include_directories(ROOT::Core INTERFACE "${ROOT_INCLUDE_DIRS}")

add_library(ROOT::Flags_CXX IMPORTED INTERFACE)

separate_arguments(ROOT_CXX_FLAGS)
target_compile_options(ROOT::Flags_CXX INTERFACE ${ROOT_CXX_FLAGS})

separate_arguments(ROOT_DEFINITIONS)
target_compile_definitions(ROOT::Flags_CXX INTERFACE ${ROOT_DEFINITIONS})

# This fixes a bug in the linker flags
string(REPLACE "-L " "-L" ROOT_EXE_LINKER_FLAGS "${ROOT_EXE_LINKER_FLAGS}")
separate_arguments(ROOT_EXE_LINKER_FLAGS)
# Stuck into using old property method due to separate -L and -l arguments
# (A full path to -l is better!)
set_property(TARGET ROOT::Flags_CXX PROPERTY
    INTERFACE_LINK_LIBRARIES ${ROOT_EXE_LINKER_FLAGS})


include_directories(../include)

# add_executable(b0 benchmark_0.cpp)
# target_link_libraries(b0 PUBLIC ROOT::Hist  project_options project_warnings)

# add_executable(b1 benchmark_1.cpp)
# target_link_libraries(b1 PUBLIC ROOT::Hist  project_options project_warnings)

# add_executable(b2 benchmark_2.cpp)
# target_link_libraries(b2 PUBLIC ROOT::Hist  project_options project_warnings fmt::fmt)

add_executable(b3 benchmark_3.cpp)
target_link_libraries(b3 PUBLIC ROOT::Hist project_options project_warnings fmt::fmt)

add_executable(b4 benchmark_4.cpp)
target_link_libraries(b4 PUBLIC ROOT::Hist project_options project_warnings fmt::fmt)