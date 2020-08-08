config_setting(
    name = "windows",
    constraint_values = [
        "@bazel_tools//platforms:windows",
    ],
)

cc_library(
    name = "memory_lock",
    srcs = ["memory_lock.cc"],
    hdrs = ["memory_lock.h"],
    visibility = ["//visibility:public"],
    copts = select({
        "windows": [
            "/DWINVER=0x602",
            "/D_WIN32_WINNT=0x602",
        ],
    }),
)
