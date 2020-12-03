load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        "//util:check",
        "//util:io",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
    ]
)
