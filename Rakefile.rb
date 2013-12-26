task :default => [:build]

task :build do
	file "ekg" do
        	sh "g++ -o ekg main.cpp"
	end
end