load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "part1",
    srcs = ["part1.cc"],
    deps = [
        ":rule_parser",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "part2",
    srcs = ["part2.cc"],
    deps = [
        ":rule_parser",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "rule_parser",
    srcs = ["rule_parser.cc"],
    hdrs = ["rule_parser.h"],
    deps = [
        "//util:check",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)
