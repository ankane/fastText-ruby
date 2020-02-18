require "mkmf-rice"

# TODO use -std=c++14 when available
# -pthread and -O3 set by default
$CXXFLAGS << " -std=c++11 -funroll-loops -march=native"

ext = File.expand_path(".", __dir__)
fasttext = File.expand_path("../../vendor/fastText/src", __dir__)

$srcs = Dir["{#{ext},#{fasttext}}/*.{cc,cpp}"]
$INCFLAGS << " -I#{fasttext}"
$VPATH << fasttext

create_makefile("fasttext/ext")
