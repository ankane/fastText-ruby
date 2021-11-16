require "mkmf-rice"

# -march=native not supported with ARM Mac
default_optflags = RbConfig::CONFIG["host_os"] =~ /darwin/i && RbConfig::CONFIG["host_cpu"] =~ /arm/i ? "" : "-march=native"
# -pthread and -O3 set by default
$CXXFLAGS << " -std=c++17 $(optflags) -funroll-loops " << with_config("optflags", default_optflags)

ext = File.expand_path(".", __dir__)
fasttext = File.expand_path("../../vendor/fastText/src", __dir__)

$srcs = Dir["{#{ext},#{fasttext}}/*.{cc,cpp}"]
$INCFLAGS << " -I#{fasttext}"
$VPATH << fasttext

create_makefile("fasttext/ext")
