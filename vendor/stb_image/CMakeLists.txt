add_library(stb_img INTERFACE
        stb_image.h
)

target_include_directories(stb_img INTERFACE .)

set_target_properties(stb_img PROPERTIES
        LINKER_LANGUAGE C
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON)