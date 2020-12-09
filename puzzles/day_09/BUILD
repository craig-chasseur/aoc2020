load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "part1",
    srcs = ["part1.cc"],
    deps = [
        ":xmas_cipher",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "part2",
    srcs = ["part2.cc"],
    deps = [
        ":xmas_cipher",
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "xmas_cipher",
    srcs = ["xmas_cipher.cc"],
    hdrs = ["xmas_cipher.h"],
    deps = [
        "//util:check",
        "@com_google_absl//absl/types:span",
    ],
)
