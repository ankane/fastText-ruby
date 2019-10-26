require "mkmf-rice"

abort "Missing stdc++" unless have_library("stdc++")

# TODO use -std=c++14 when available
$CXXFLAGS << " -pthread -std=c++11 -funroll-loops -O3 -march=native"

src1 = File.expand_path(".", __dir__)
src2 = File.expand_path("../../vendor/fastText/src", __dir__)

$srcs = Dir["{#{src1},#{src2}}/*.{cc,cpp}"]
$INCFLAGS << " -I$(srcdir)/../../vendor/fastText/src"
$VPATH << "$(srcdir)/../../vendor/fastText/src"

create_makefile("fasttext/ext")
