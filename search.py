import sys
import numpy as np
from scipy.spatial import distance
from scipy.optimize import linprog
from reduce import getInfo
import time

#Creating NumPy Array with images from file with file descriptor f
def secCreateNParray(f,image_number,rows_number,columns_number):
	e = f.read(image_number*rows_number*columns_number)     #at the beginning e is string with pixels of image_number images
	e = np.frombuffer(e,dtype = np.uint8).astype(np.int64)  #convert string to numpy array of integers
	return e

def readLabelFile(f):
	f.read(4) #magic number
	temp = f.read(4) #read 4 bytes
	temp = np.frombuffer(temp,dtype = np.uint8).astype(np.int64) #convert the string that stored in temp into an array(numpy) of integers
	img_number = (temp[0]<<24)|(temp[1]<<16)|(temp[2]<<8)|temp[3]
	e = f.read(img_number)
	e = np.frombuffer(e,dtype = np.uint8).astype(np.int64)
	return e


#Weight computation of cluster
def weight_computation(my_list):
	sum=0
	for k in range(0,len(my_list)):
		sum+=my_list[k]
	return sum

#Creating cluster method
def blockshaped(arr, nrows, ncols):
	"""
	Return an array of shape (n, nrows, ncols) where
	n * nrows * ncols = arr.size

	If arr is a 2D array, the returned array should look like n subblocks with
	each subblock preserving the "physical" layout of arr.
	"""
	h, w = arr.shape
	assert h % nrows == 0, "{} rows is not evenly divisble by {}".format(h, nrows)
	assert w % ncols == 0, "{} cols is not evenly divisble by {}".format(w, ncols)
	return (arr.reshape(h//nrows, nrows, -1, ncols).swapaxes(1,2).reshape(-1, nrows, ncols))

def compute_d(num_of_columns,cluster_size):
	#suppose num_of_columns == num_of_rows at each image
	centroids_place1 = [] #centroids_place1 consists of centroids that image1 has
	centroids_place2 = [] #centroids_place2 consists of centroids that image2 has

	for i in range(0,num_of_columns,cluster_size):
		for j in range(0,num_of_columns,cluster_size):
			k = [i,j]
			centroids_place1.append(k)
			centroids_place2.append(k)

	euclDistanceList = []

	for centroid1 in centroids_place1:
		for centroid2 in centroids_place2:
			dst = distance.euclidean(centroid1, centroid2) #ipologizei tin eukleidia apostasi meta3i dio simeiwn
			euclDistanceList.append(dst)

	return euclDistanceList


def cluster_info(file_array,num_of_images,row_num,column_num,cluster_size):
	list = []
	temp = int(row_num/cluster_size)
	num_of_clusters = temp*temp

	for i in range(0,num_of_images): #for each image

		cur_image = []
		w_list_of_cur_image = [] #calculate for the current image the weights of its clusters
		start = i*row_num*column_num
		for j in range(0,row_num*column_num): #cur_image has current image of
			cur_image.append(file_array[start+j]) #file file_array

		all_sum = weight_computation(cur_image) #sum of pixels at current image
		npa = np.asarray(cur_image).reshape((row_num,column_num))
		result = blockshaped(npa, cluster_size, cluster_size)

		for k in range(0,num_of_clusters): #for each cluster of the current image
			sec_list = []
			for a in range(0,cluster_size):
				for b in range(0,cluster_size):
					sec_list.append(result[k][a][b])
			wi = weight_computation(sec_list) #wi = weight of current cluster of image
			new_w = wi/all_sum
			w_list_of_cur_image.append(new_w)

		list.append(w_list_of_cur_image)

	return list


def printFun(array,number,num_of_rows,num_of_columns):

	for k in range(number*num_of_rows*num_of_columns,(number+1)*num_of_rows*num_of_columns): #ektypwnw tis 10 prwtes eikones
		if ((k+1) % (num_of_rows*num_of_columns)) == 0:
			print(array[k],end = " ")
			print()
			continue
		if (k+1)%num_of_columns == 0:
			print(array[k],end = " ")
			print()
			continue
		print(array[k],end = " ")

	print()
    ####


def main():

	for i in range(1,len(sys.argv)):
		if sys.argv[i]=="-d":
			inputfileoriginalspace=sys.argv[i+1]
		elif sys.argv[i]=='-q':
			queryfileoriginalspace=sys.argv[i+1]
		elif sys.argv[i]=='-l1':
			labelsofinputdataset=sys.argv[i+1]
		elif sys.argv[i]=='-l2':
			labelsofquerydataset=sys.argv[i+1]
		elif sys.argv[i]=='-o':
			outputfile=sys.argv[i+1]
		elif sys.argv[i]=='-EMD':
			continue

	f_input = open(inputfileoriginalspace,"rb") #opening input_file
	img_number_input,rows_number_input,columns_number_input = getInfo(f_input) #info from input file
	input_array = secCreateNParray(f_input,img_number_input,rows_number_input,columns_number_input) #getting the array with images from input
	f_input.close()

	f_query = open(queryfileoriginalspace,'rb') #opening query_file
	img_number_query,rows_number_query,columns_number_query = getInfo(f_query) #info from query file
	query_array = secCreateNParray(f_query,img_number_query,rows_number_query,columns_number_query) #getting the array with images from query
	f_query.close() #closing query file

	f_labels_input = open(labelsofinputdataset,"rb") #opening file with labels of input_file
	labels_input = readLabelFile(f_labels_input)
	f_labels_input.close()

	f_labels_query = open(labelsofquerydataset,"rb") #opening file with labels of query_file
	labels_query = readLabelFile(f_labels_query)
	f_labels_query.close()

	cluster_size = 7
	euclDistanceList = compute_d(columns_number_input,cluster_size) #euclDistanceList has distances between clusters centers from different images
	#information:images haven't got the same weight (weight = sum of pixels)

	list_input_array = cluster_info(input_array,img_number_input,rows_number_input,columns_number_input,cluster_size) #creating list of lists with clusters weights of images (input_array consists of these images)
	list_query_array = cluster_info(query_array,img_number_query,rows_number_query,columns_number_query,cluster_size)

	temp = int(rows_number_input/cluster_size)
	num_of_clusters = temp*temp
	num_of_f = num_of_clusters*num_of_clusters

	A = [] #coefficients of fij which are 1 or -1 or 0

	for value in range(num_of_clusters):

		plusone = []
		minusone = []
		start = value*num_of_clusters

		for fij in range(0,num_of_f,num_of_clusters):
			if fij == start:
				for val in range(num_of_clusters):
					plusone.append(1)
					minusone.append(-1)
				continue
			for val in range(num_of_clusters):
				plusone.append(0)
				minusone.append(0)

		A.append(plusone)
		A.append(minusone)


	for value in range(num_of_clusters):

		plusone = []
		minusone = []
		keep = value

		for fij in range(num_of_f):
			if fij == keep:
				plusone.append(1)
				minusone.append(-1)
				keep = keep + num_of_clusters
				continue
			plusone.append(0)
			minusone.append(0)

		A.append(plusone)
		A.append(minusone)


	f_bounds = []

	for fij in range(num_of_f):
		f_bounds.append((0,None))

	correct = 0
	set = 0
	EMD_time = 0.0

	for query in range(len(list_query_array)): #(100)
		#print("query = ",query)
		EMD_NN = [] #10 nearest neighbors of query
		#print("Query:",labels_query[query])
		#printFun(query_array,query,rows_number_query,columns_number_query)
		for imageData in range(len(list_input_array)): #(500)
			b = []
			for w in list_input_array[imageData]:
				b.append(w)
				b.append(w*(-1))
			for w in list_query_array[query]:
				b.append(w)
				b.append(w*(-1))

			tic = time.perf_counter()
			res = linprog(euclDistanceList, A_ub=A, b_ub=b, bounds=f_bounds)
			toc = time.perf_counter()
			difftime = toc-tic
			EMD_time = EMD_time+difftime

			EMD_NN.append((imageData,res.fun))

		EMD_NN.sort(key = lambda x: x[1]) #sort by second element of tuple
		for neighbor in range(10): #take 10 elements from the beginning (10 nearest neighbors)
			#print("neighbor = ",labels_input[EMD_NN[neighbor][0]])
			#printFun(input_array,EMD_NN[neighbor][0],rows_number_input,columns_number_input)
			if labels_input[EMD_NN[neighbor][0]] == labels_query[query]:
				correct = correct + 1
		set = set + 10

	#print("correct = ",correct,"\nset = ",set)
	f_out = open(outputfile,"a")
	avercorrEMD = correct/set
	f_out.write("Average Correct Search Results EMD:" + str(avercorrEMD) + "\n")
	f_out.close()
	print("EMD_time = ",EMD_time)


if __name__ == "__main__":
	main()
