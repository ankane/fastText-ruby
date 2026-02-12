require "bundler/gem_tasks"
require "rake/testtask"
require "rake/extensiontask"
require "ruby_memcheck"

test_config = lambda do |t|
  t.pattern = "test/**/*_test.rb"
end
Rake::TestTask.new(&test_config)

namespace :test do
  RubyMemcheck::TestTask.new(:valgrind, &test_config)
end

task default: :test

Rake::ExtensionTask.new("fasttext") do |ext|
  ext.name = "ext"
  ext.lib_dir = "lib/fasttext"
end

task :check_license do
  raise "Missing vendor license" unless File.exist?("vendor/fastText/LICENSE")
end

task :remove_ext do
  Dir["lib/fasttext/ext.{bundle,so}"].each do |path|
    File.unlink(path)
  end
end

Rake::Task["build"].enhance [:check_license, :remove_ext]
