require "bundler/gem_tasks"
require "rake/testtask"
require "rake/extensiontask"

task default: :test
Rake::TestTask.new do |t|
  t.libs << "test"
  t.pattern = "test/**/*_test.rb"
  t.warning = false
end

Rake::ExtensionTask.new("fasttext") do |ext|
  ext.name = "ext"
  ext.lib_dir = "lib/fasttext"
end

Rake::Task[:release].enhance [:clean]
