licenses(["notice"])  # Apache 2.0

exports_files(["LICENSE"])

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        "@github_nlohmann_json//:json",
        "//lib:service"
    ]
)
# load(":foo.bzl", "foo_binary")
# print("BUILD file")
# foo_binary(name = "bin1")
# foo_binary(name = "bin2")
# foo_binary(
#     name = "bin",
#     username = "Alice",
# )