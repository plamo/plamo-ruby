require "bundler/gem_tasks"
require "rake/extensiontask"

task :build => :compile

Rake::ExtensionTask.new("plamo") do |ext|
  ext.lib_dir = "lib/plamo"
end

task :default => [:clobber, :compile, :spec]
