import sys
from matplotlib import pyplot as plt

try:
	fp = open(sys.argv[1], 'r')
except:
	print("Invalid arguments. usage:", sys.argv[0], "<graphFileName>")
	exit()

# You can use an of the following styles in your plot by:
plt.style.use("seaborn-darkgrid")

fileContents = fp.read()
fp.close()
fileContents = fileContents.split('\n')[:-1]; #Each line

for line in fileContents:
	fileContents = line.split(':') #Separating the name of file with the data
	fileContents[1] = fileContents[1].split(",")[:-1] #Splitting the data
	tempDict = {}
	for dic in fileContents[1]:
		#Key: num of threads, Value: time taken
		tempDict[int(dic.split("=")[0])] = float(dic.split("=")[1])
	fileContents[1] = tempDict
	print(fileContents)

	# Graphing
	plt.plot(list(fileContents[1].keys()), list(fileContents[1].values()), label=fileContents[0])
	plt.legend() # Gets the values of legends from the labels in each plot
	
	plt.xlabel('Number of threads')
	plt.ylabel('Time taken')
	plt.title('Time taken for matrix multiplication on dimension matrices using different number of threads')

plt.show()

