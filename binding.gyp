{
    "targets": [
    {
        "target_name": "ezmath",
        "sources": [
            "ezmath.cc"
        ],
        "include_dirs": [
            "src/parsing/include",
            "src/tree/include",
            "build-cmake/_deps/boost-src/include",
            "build-cmake/_deps/fmt-src/include",
            "<!@(node -p \"require('node-addon-api').include\")" 
        ],
        "libraries": [
            "-lfmt",
            "-lparsing",
            "-ltree",
            "-L<(module_root_dir)/build-cmake/_deps/fmt-build",
            "-L<(module_root_dir)/build-cmake/src/parsing",
            "-L<(module_root_dir)/build-cmake/src/tree"
        ],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "OTHER_CFLAGS": [ "-std=c++20"],
        },
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-std=c++20", "-fno-exceptions" ],
        "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]
    }
    ]
}
