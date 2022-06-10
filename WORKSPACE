##test
workspace(name = "nlohmann_json_demo")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "github_nlohmann_json",
    sha256 = "69cc88207ce91347ea530b227ff0776db82dcb8de6704e1a3d74f4841bc651cf",
    urls = [ 
        "https://github.com/nlohmann/json/releases/download/v3.6.1/include.zip",
    ],
    build_file = "//third_party:nlohmann_json.BUILD",
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
 name ="gtest",
 remote ="https://github.com/google/googletest",
 branch ="v1.10.x",
)
