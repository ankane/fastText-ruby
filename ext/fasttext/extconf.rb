require "mkmf-rice"

# -pthread and -O3 set by default
$CXXFLAGS << " -std=c++17 $(optflags) -funroll-loops "

# Add -march=native if we're not on arm64
if defined?(RUBY_PLATFORM) == 'constant' && RUBY_PLATFORM.split('-')[0] != 'arm64'
  $CXXFLAGS << with_config("optflags", "-march=native")
end

ext = File.expand_path(".", __dir__)
fasttext = File.expand_path("../../vendor/fastText/src", __dir__)

$srcs = Dir["{#{ext},#{fasttext}}/*.{cc,cpp}"]
$INCFLAGS << " -I#{fasttext}"
$VPATH << fasttext

create_makefile("fasttext/ext")
