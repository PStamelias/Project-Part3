import sys
def main():
	if sys.argv[1]=='-d':
		dataset=sys.argv[2]
		output=sys.argv[4]
	if sys.argv[1]=='-o':
		dataset=sys.argv[4]
		output=sys.argv[2]

if __name__ == "__main__":
	main()
