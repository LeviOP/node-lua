{
    "targets": [
        {
            "target_name": "addon",
            "sources": [ "addon.cc" ],
            "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")", "/usr/include/lua"],
            "libraries": ["-llua"],
            "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
            "cflags!": [ "-fno-exceptions" ],
            "cflags_cc!": [ "-fno-exceptions" ]
        }
    ]
}
