load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "part1",
    srcs = ["part1.cc"],
    deps = [
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
    ],
)

cc_binary(
    name = "part2",
    srcs = ["part2.cc"],
    deps = [
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_googlesource_code_re2//:re2",
    ],
)
