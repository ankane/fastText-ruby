require "bundler/gem_tasks"
require "rake/testtask"
require "rake/extensiontask"

Rake::TestTask.new do |t|
  t.pattern = "test/**/*_test.rb"
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
  path = "lib/fasttext/ext.bundle"
  File.unlink(path) if File.exist?(path)
end

Rake::Task["build"].enhance [:check_license, :remove_ext]
