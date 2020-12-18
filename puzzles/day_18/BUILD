load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "expr",
    hdrs = ["expr.h"],
    deps = [
        "//util:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "parse",
    srcs = ["parse.cc"],
    hdrs = ["parse.h"],
    deps = [
        ":expr",
        "//util:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "part1",
    srcs = ["part1.cc"],
    deps = [
        ":expr",
        ":parse",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "part2",
    srcs = ["part2.cc"],
    deps = [
        ":expr",
        ":parse",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/strings",
    ],
)
