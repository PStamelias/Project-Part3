import sys
def main():
	dataset=""
	queryset=""
	outputdatasetfile=""
	outputqueryfile=""
	for i in range(1,len(sys.argv)):
		if sys.argv[i]=="-d":
			dataset=sys.argv[i+1]
		elif sys.argv[i]=="-q":
			queryset=sys.argv[i+1]
		elif sys.argv[i]=="-od":
			outputdatasetfile=sys.argv[i+1]
		elif sys.argv[i]=="-oq":
			outputqueryfile=sys.argv[i+1]
	
if __name__ == "__main__":
	main()
