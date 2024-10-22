require_relative "lib/fasttext/version"

Gem::Specification.new do |spec|
  spec.name          = "fasttext"
  spec.version       = FastText::VERSION
  spec.summary       = "Efficient text classification and representation learning for Ruby"
  spec.homepage      = "https://github.com/ankane/fastText-ruby"
  spec.license       = "MIT"

  spec.author        = "Andrew Kane"
  spec.email         = "andrew@ankane.org"

  spec.files         = Dir["*.{md,txt}", "{lib,ext}/**/*", "vendor/fastText/{LICENSE,README.md}", "vendor/fastText/src/**/*"]
  spec.require_path  = "lib"
  spec.extensions    = ["ext/fasttext/extconf.rb"]

  spec.required_ruby_version = ">= 3.1"

  spec.add_dependency "rice", ">= 4.3.3"
end
