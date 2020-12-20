load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "bit_square",
    hdrs = ["bit_square.h"],
    deps = [
        "//util:check",
    ],
)

cc_binary(
    name = "part1",
    srcs = ["part1.cc"],
    deps = [
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_binary(
    name = "part2",
    srcs = ["part2.cc"],
    deps = [
        ":bit_square",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)
