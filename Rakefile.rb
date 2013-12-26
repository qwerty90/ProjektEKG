require 'rake'

task :default => :build

task :build do
	file "ekg" do
		puts "Compiling main.cpp..."
        	sh "g++ -o ekg main.cpp"
	end
end