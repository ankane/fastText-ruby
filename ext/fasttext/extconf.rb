require "mkmf-rice"

# abort "Missing stdc++" unless have_library("stdc++")

# TODO use -std=c++14 when available
# $CXXFLAGS << " -pthread -std=c++11 -funroll-loops -O3 -march=native"
$CXXFLAGS << " -std=c++11"

ext = File.expand_path(".", __dir__)
fasttext = File.expand_path("../../vendor/fastText/src", __dir__)

$srcs = Dir["{#{ext},#{fasttext}}/*.{cc,cpp}"]
$INCFLAGS << " -I#{fasttext}"
$VPATH << fasttext

create_makefile("fasttext/ext")
