load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "part1",
    srcs = ["part1.cc"],
    deps = [
        ":tree_map",
        "//util:check",
        "//util:io",
    ],
)

cc_binary(
    name = "part2",
    srcs = ["part2.cc"],
    deps = [
        ":tree_map",
        "//util:check",
        "//util:io",
    ],
)

cc_library(
    name = "tree_map",
    srcs = ["tree_map.cc"],
    hdrs = ["tree_map.h"],
    deps = [
        "@com_google_absl//absl/types:optional",
    ],
)
