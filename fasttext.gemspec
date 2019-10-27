require_relative "lib/fasttext/version"

Gem::Specification.new do |spec|
  spec.name          = "fasttext"
  spec.version       = FastText::VERSION
  spec.summary       = "fastText - efficient text classification and representation learning - for Ruby"
  spec.homepage      = "https://github.com/ankane/fasttext"
  spec.license       = "MIT"

  spec.author        = "Andrew Kane"
  spec.email         = "andrew@chartkick.com"

  spec.files         = Dir["*.{md,txt}", "{lib,ext}/**/*", "vendor/fastText/{LICENSE,README.md}", "vendor/fastText/src/**/*"]
  spec.require_path  = "lib"
  spec.extensions    = ["ext/fasttext/extconf.rb"]

  spec.required_ruby_version = ">= 2.4"

  spec.add_dependency "rice"

  spec.add_development_dependency "bundler"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler"
  spec.add_development_dependency "minitest", ">= 5"
end
