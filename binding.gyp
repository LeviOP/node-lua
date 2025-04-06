{
    "targets": [
        {
            "target_name": "addon",
            "sources": [ "addon.cc" ],
            "defines": [ "NODE_ADDON_API_DISABLE_CPP_EXCEPTIONS" ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<!@(pkg-config lua --cflags-only-I | sed s/-I//g)"
            ],
            "libraries": ["<!@(pkg-config lua --libs)"],
            "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-exceptions" ]
        }
    ]
}
