# Purpose

	Parses .raml files to generate DUTDescriptor.json
	.raml files are passed as arguments in command line.
	prerequisite: .raml files and must be in the same directory with executable
		and required .json files must in the path specified in the related .raml files
	

# Build
	Import and build in eclipse

# Run
	run from command line
	
	1. export project as Runnable jar from eclipse
	2. go to the exported directory and run with arguments:
		java -jar RAMLParser.jar Audio.raml Door.raml. Temperature.raml
			or
		java -jar RAMLParser.jar *.raml
		
		# Important Note:
			the .raml files must be in the same directory as the executable (RAMLParser.jar)
			and .json files related to the .raml files must in the path specified in the .raml files
	3. Output:
		DUTDescriptor.json


