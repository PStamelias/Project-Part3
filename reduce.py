#exw mono ta data arxeia
import sys
from matplotlib import pyplot as plt
import numpy as np
import six
import numpy as np
import pandas as pd
import keras
from sklearn.model_selection import train_test_split
from keras.layers import Input,Dense,Flatten,Conv2D,MaxPooling2D,UpSampling2D,Reshape
from keras.layers.normalization import BatchNormalization
from keras.models import Model, load_model
from keras.optimizers import RMSprop
from tensorflow.keras import backend as K
import ast


#Getting useful Info from file with file descriptor f
def getInfo(f):
	temp = f.read(4) #pass first 4 bytes that are the magic number

	for pos in range(3):
		temp = f.read(4) #read 4 bytes
		temp = np.frombuffer(temp,dtype = np.uint8).astype(np.int64) #convert the string that stored in temp into an array(numpy) of integers
		if pos == 0: #temp array has 4 integers, each integer represents a byte of number number_of_images
			img_number = (temp[0]<<24)|(temp[1]<<16)|(temp[2]<<8)|temp[3] #convert the temp array into an integer
		elif pos == 1: #temp has number of rows
			rows_number = (temp[0]<<24)|(temp[1]<<16)|(temp[2]<<8)|temp[3]
		elif pos == 2: #temp has number of columns
			columns_number = (temp[0]<<24)|(temp[1]<<16)|(temp[2]<<8)|temp[3]

	return img_number,rows_number,columns_number


#Creating NumPy Array with images from file with file descriptor f
def createNParray(f,image_number,rows_number,columns_number):
	e = f.read(image_number*rows_number*columns_number)     #at the beginning e is string with pixels of image_number images
	e = np.frombuffer(e,dtype = np.uint8).astype(np.int64)  #convert string to numpy array of integers
	e = e.reshape(image_number, rows_number,columns_number) #convert the numpy array into three-dimensional array with dimensions
															#image_numberxrows_numberxcolumns_number
	return e

#Converting each rows_number x columns_number image of train set into a matrix of size rows_number x columns_number x 1, which you can feed into the Neural network
def DataProcess1(e,rows_number,columns_number):
	e = e.reshape(-1,rows_number,columns_number,1) #convert every image rows_numberxcolumns_number to vector rows_numberxcolumns_numberx1
	return e

#Rescaling the training  data with the maximum pixel value of the training
def DataProcess2(e): #convert every pixel to 0 or 1
	e = e/np.max(e)
	return e

#Split the dataset into training and validation set (this reduce the probability of overfitting)
def DataProcess3(e):
	train_X,valid_X,train_ground,valid_ground = train_test_split(e,e,test_size = 0.2, random_state = 13)
	return train_X,valid_X,train_ground,valid_ground

#user gives hyperparameters
def GiveHyperparameters():

	numOfLayers = int(input("Give number of layers(at least 5): "))
	x_filter = int(input("Give the x coordinate of filter: "))
	y_filter = int(input("Give the y coordinate of filter: "))

	filtersPerLayer = [] #list with number of filters at each layer
	for val in range(numOfLayers):
		flt = int(input("Give number of filters in {} layer: ".format(val+1)))
		filtersPerLayer.append(flt)

	epochs = int(input("Give epochs: "))
	batch_size = int(input("Give batch size: "))
	latent_dimension = int(input("Give latent dimension: "))

	if numOfLayers%2 == 0: #if numOfLayers = even number
		ConvLayersEnc = numOfLayers//2 #how many Conv Layers the encoder has
		ConvLayersDec = numOfLayers//2 #how many Conv Layers the decoder has
	elif numOfLayers%2 != 0: #if numOfLayers = odd number
		ConvLayersEnc = numOfLayers//2
		ConvLayersDec = (numOfLayers//2) + 1

	return numOfLayers,x_filter,y_filter,filtersPerLayer,epochs,batch_size,ConvLayersEnc,ConvLayersDec,latent_dimension

#Get minimun and maximum value at list of lists
def getMinMax(listoflist):

	maxList = [] #maxList consists of maximum values of every list that listoflist includes
	minList = [] #minList consists of minimum values of every list that listoflist includes

	for element in listoflist:
		maxList.append(max(element))
		minList.append(min(element))

	maximum = max(maxList) #maximum value at list of lists
	minimum = min(minList) #minimum value at list of lists

	return minimum,maximum

#Normalize vectors at range [0,25500]
def NormalizeVectors(listoflists,minimum,maximum):

	normalizedVectors = listoflists
	diff = maximum - minimum

	for eachlist in range(len(normalizedVectors)): #for each vector in listoflists I'll make the normalization at range [0,25500]
		for element in range(len(normalizedVectors[eachlist])):
			normalizedVectors[eachlist][element] = 25500*((normalizedVectors[eachlist][element]-minimum)/diff)
			normalizedVectors[eachlist][element] = round(normalizedVectors[eachlist][element])

	return normalizedVectors

#Create binary file that contains vectors of images
def createBinFile(filename,imagesVectorsList,magic_number,number_of_images,rows_number,columns_number):

	binary_file = open(filename, "wb")

	magic_number = magic_number.to_bytes(4,byteorder='big',signed=False) #Create four bytes from the integer magic_number
	binary_file.write(magic_number)
	number_of_images = number_of_images.to_bytes(4,byteorder='big',signed=False)
	binary_file.write(number_of_images)
	rows_number = rows_number.to_bytes(4,byteorder='big',signed=False)
	binary_file.write(rows_number)
	columns_number = columns_number.to_bytes(4,byteorder='big',signed=False)
	binary_file.write(columns_number)

	imagesVectorsList = imagesVectorsList.astype(int) #convert float array to int array

	for eachlist in imagesVectorsList:
		for value in eachlist:
			temp = value.item() #take the content of numpy array value(for to_bytes)
			temp = temp.to_bytes(2,byteorder='big',signed=False) #Create 2 bytes from the integer
			binary_file.write(temp)

	binary_file.close()

#Built Encoder of autoencoder
def Encoder(input_img, filtersPerLayer, ConvLayersEnc, x_filter, y_filter, latent_dimension):

	count = 0

	for value in filtersPerLayer:
		if count == ConvLayersEnc:
			break
		elif count == 0: #for first Conv Layer
			x = Conv2D(value, (x_filter, y_filter), activation='relu', padding='same')(input_img)
			x = BatchNormalization()(x)
			x = MaxPooling2D(pool_size=(2, 2))(x) #first MaxPooling
		elif count != 0:
			x = Conv2D(value, (x_filter, y_filter), activation='relu', padding='same')(x)
			x = BatchNormalization()(x)
			if count == 1:
				x = MaxPooling2D(pool_size=(2, 2))(x) #second MaxPooling (we have 2 MaxPooling Layers)
		count = count + 1

	volumeSize = K.int_shape(x) #we keep the three dimensions of x before flatten layer(for example: if x dimensions
								#are 3x3x128 then flatten layer reshapes the x to have the shape 1152 ( = 3x3x128)
								#and some layers later we will not be able to find these x dimensions(3x3x128), so we save them)
	x = Flatten()(x)
	latentVector = Dense(latent_dimension, name = 'latent_vectors')(x)

	list = []
	list.append(latentVector)
	list.append(volumeSize)

	return list #return list with latent vector and volumeSize(previously dimensions of x)

#Built Decoder of autoencoder
def Decoder(x, filtersPerLayer, ConvLayersEnc, ConvLayersDec, x_filter, y_filter):

	latentVector = x[0]
	volumeSize = x[1]
	x = Dense(np.prod(volumeSize[1:]))(latentVector) #reshape from latent vector to previously layer with one dimension(for example 1152)
	x = Reshape((volumeSize[1], volumeSize[2], volumeSize[3]))(x) #reshape from one dimension to three dimesions(for example from 1152 to 3x3x128)

	layerUpSampl1 = ConvLayersEnc + ConvLayersDec - 2
	layerUpSampl2 = ConvLayersEnc + ConvLayersDec - 3

	for value in range(ConvLayersEnc, ConvLayersEnc + ConvLayersDec - 1): #take the number of filters which belong in decoder convolutional layers
		x = Conv2D(filtersPerLayer[value], (x_filter, y_filter), activation='relu', padding='same')(x)               #except the last layer
		x = BatchNormalization()(x)
		if value == layerUpSampl1 or value == layerUpSampl2: #layers followed by UpSampling layers
			x = UpSampling2D((2,2))(x)

	decoded = Conv2D(1, (x_filter, y_filter), activation='sigmoid', padding='same')(x)
	return decoded


#onlyLatentOut function helps us to built model which has only the encoder part of autoencoder and stops at layer with the latent vectors
def onlyLatentOut(x): #x is list
	return x[0]		  #we return the first element of x


def main():

	###
	for pos in range(1,len(sys.argv)-1,2): #take values in range [1,len(sys.argv)-2] with step 2
		if sys.argv[pos] == '-d': #if this argument is '-d'
			datafile = sys.argv[pos+1]  #the next argument has the datafile name of dataset
		if sys.argv[pos] == '-q':
			queryfile = sys.argv[pos+1]
		if sys.argv[pos] == '-od':
			DataOutput = sys.argv[pos+1]
		if sys.argv[pos] == '-oq':
			QueryOutput = sys.argv[pos+1]
	###

	#Preprocessing of dataset:
	fd = open(datafile,"rb")
	image_number,rows_number,columns_number = getInfo(fd) #Getting info about Dataset
	e = createNParray(fd,image_number,rows_number,columns_number) #Creating NumPy Array
	fd.close()

	e = DataProcess1(e,rows_number,columns_number) #Data Processing
	e = DataProcess2(e) #Data Processing

	DataSet = e #keep training_set for predict function

	train_X,valid_X,train_ground,valid_ground = DataProcess3(e)
	###

	#Preprocessing of queryset:
	queryfd = open(queryfile,"rb")
	q_image_number,q_rows_number,q_columns_number = getInfo(queryfd) #Getting info about Queryset
	q_e = createNParray(queryfd,q_image_number,q_rows_number,q_columns_number) #Creating NumPy Array
	queryfd.close()

	q_e = DataProcess1(q_e,q_rows_number,q_columns_number) #Data Processing
	q_e = DataProcess2(q_e) #Data Processing

	QuerySet = q_e #keep query set for predict function
	###

	inChannel = 1
	x, y = rows_number, columns_number
	input_img = Input(shape = (x, y, inChannel)) #input of autoencoder

	print("Give 1 if you want to use pre-trained autoencoder model\nGive 2 if you want to create models and train them")
	saveModelorNot = int(input("Choose 1 or 2: "))

	####
	if saveModelorNot == 1:
		#Read hyperparameters that we use in autoencoder:
		infofd = open("info.txt","r")
		newlist = [line.rstrip() for line in infofd.readlines()]
		numOfLayers = int(newlist[0])
		x_filter = int(newlist[1])
		y_filter = int(newlist[2])
		res = newlist[3]
		filtersPerLayer = ast.literal_eval(res)
		epochs = int(newlist[4])
		batch_size = int(newlist[5])
		ConvLayersEnc = int(newlist[6])
		ConvLayersDec = int(newlist[7])
		latent_dimension = int(newlist[8])
		infofd.close()

		giveAutoencModel = input("Give pre-trained autoencoder: ")
		autoencoder_model = keras.models.load_model(giveAutoencModel) #load autoencoder

		autoencoder_model.summary()

		latent_model = Model(input_img, onlyLatentOut(Encoder(input_img,filtersPerLayer, ConvLayersEnc, x_filter, y_filter, latent_dimension)))
		latent_model.summary()
		for l1,l2 in zip(latent_model.layers[:5 + 2*ConvLayersEnc],autoencoder_model.layers[0:5 + 2*ConvLayersEnc]): #copy the encoder weights
			l1.set_weights(l2.get_weights())																#of autoencoder to latent_model
		pred = latent_model.predict(DataSet) #if DataSet has 10000 images then pred has 10000 vectors(one latent vector at every image) and
											  #each vector has 10 dimensions (if latent vector dimension that I gave is 10 )
		q_pred = latent_model.predict(QuerySet)

		d_min,d_max = getMinMax(pred)
		q_min,q_max = getMinMax(q_pred)

		tmpmin = [d_min,q_min]
		tmpmax = [d_max,q_max]
		minimum = min(tmpmin)
		maximum = max(tmpmax)

		newpred = NormalizeVectors(pred,minimum,maximum)
		newq_pred = NormalizeVectors(q_pred,minimum,maximum)

		createBinFile(DataOutput,newpred,26,image_number.item(),1,latent_dimension) #image_number and q_image_number are numpy arrays so I want their content(function item)
		createBinFile(QueryOutput,newq_pred,27,q_image_number.item(),1,latent_dimension) #because I want to call the function to_bytes

		sys.exit()
	####

	listNumOfLayers = []
	list_xfilter = []
	list_yfilter = []
	listFiltersPerLayer = [] #list of lists,that list consists of filtersPerLayer lists
	listEpochs = []
	listBatchSize = []
	listLoss = []
	listValLoss = []


	while True:

		numOfLayers,x_filter,y_filter,filtersPerLayer,epochs,batch_size,ConvLayersEnc,ConvLayersDec,latent_dimension = GiveHyperparameters()

		autoencoder = Model(input_img, Decoder(Encoder(input_img,filtersPerLayer, ConvLayersEnc, x_filter, y_filter, latent_dimension),filtersPerLayer, ConvLayersEnc, ConvLayersDec, x_filter, y_filter))
		autoencoder.compile(loss='mean_squared_error', optimizer = RMSprop())

		autoencoder.summary()

		autoencoder_train = autoencoder.fit(train_X, train_ground, batch_size=batch_size,epochs=epochs,verbose=1,validation_data=(valid_X, valid_ground))

		#latent_model has the encoder part of autoencoder and returns the latent vector
		latent_model = Model(input_img, onlyLatentOut(Encoder(input_img,filtersPerLayer, ConvLayersEnc, x_filter, y_filter, latent_dimension)))
		latent_model.summary()
		for l1,l2 in zip(latent_model.layers[:5 + 2*ConvLayersEnc],autoencoder.layers[0:5 + 2*ConvLayersEnc]): #copy the encoder weights
			l1.set_weights(l2.get_weights())																#of autoencoder to latent_model
		pred = latent_model.predict(DataSet) #if DataSet has 10000 images then pred has 10000 vectors(one latent vector at every image) and
											  #each vector has 10 dimensions (if latent vector dimension that I gave is 10 )
		q_pred = latent_model.predict(QuerySet)

		d_min,d_max = getMinMax(pred)
		q_min,q_max = getMinMax(q_pred)

		tmpmin = [d_min,q_min]
		tmpmax = [d_max,q_max]
		minimum = min(tmpmin)
		maximum = max(tmpmax)

		newpred = NormalizeVectors(pred,minimum,maximum)
		newq_pred = NormalizeVectors(q_pred,minimum,maximum)

		createBinFile(DataOutput,newpred,26,image_number.item(),1,latent_dimension) #image_number and q_image_number are numpy arrays so I want their content(function item)
		createBinFile(QueryOutput,newq_pred,27,q_image_number.item(),1,latent_dimension) #because I want to call the function to_bytes
		######

		listNumOfLayers.append(numOfLayers)
		list_xfilter.append(x_filter)
		list_yfilter.append(y_filter)
		listFiltersPerLayer.append(filtersPerLayer)
		listEpochs.append(epochs)
		listBatchSize.append(batch_size)

		loss = autoencoder_train.history['loss']
		val_loss = autoencoder_train.history['val_loss']
		listLoss.append(loss[epochs-1]) #add at list loss value from last epoch
		listValLoss.append(val_loss[epochs-1]) #add at list val_loss value from last epoch

		print("Give 1 if you want to do another experiment\nGive 2 if you want plots\nGive 3 if you want to save the current model")
		doNext = int(input("Choose 1,2 or 3: "))


		if doNext == 2:

			transpose = list(map(list,six.moves.zip_longest(*listFiltersPerLayer,fillvalue = -1)))
			#if listFiltersPerLayer = [[32,64,64,1],[56,40,4,12,1],[27,6,1]]
			#then transpose = [[32,56,27],[64,40,6],[64,4,1],[1,12,-1],[-1,1,-1]]
			#first experiment -> layer_1 has 32 filters, layer_2 has 64 filters ...
			#second experiment -> layer_1 has 56 filters, layer_2 has 40 filters...
			numplots = len(transpose) + 5
			#plt.figure()
			f, axs = plt.subplots(numplots,1,figsize=(25,numplots*7))

			xs = list(range(len(listEpochs)))
			int2str = []
			for value in listEpochs:
				int2str.append(str(value)) #if listEpochs=[50,25,50] then with this way x axis at subplot will be [50,25,50], instead of [25,50]

			plt.subplot(numplots, 1, 1)
			plt.xticks(xs,int2str)
			plt.plot(xs, listLoss, 'bo', label='training loss')
			plt.plot(xs, listValLoss, 'gs', label='validation loss')
			plt.xlabel('epochs at each experiment')
			plt.ylabel('loss/val_loss')
			plt.title('Loss/Val_Loss per number of epochs')
			plt.legend()

			xs.clear()
			int2str.clear()

			xs = list(range(len(listBatchSize)))
			for value in listBatchSize:
				int2str.append(str(value))

			plt.subplot(numplots, 1, 2)
			plt.xticks(xs,int2str)
			plt.plot(xs, listLoss, 'bo', label='training loss')
			plt.plot(xs, listValLoss, 'gs', label='validation loss')
			plt.xlabel('batch_size at each experiment')
			plt.ylabel('loss/val_loss')
			plt.title('Loss/Val_Loss per batch_size')
			plt.legend()

			xs.clear()
			int2str.clear()

			xs = list(range(len(listNumOfLayers)))
			for value in listNumOfLayers:
				int2str.append(str(value))

			plt.subplot(numplots, 1, 3)
			plt.xticks(xs,int2str)
			plt.plot(xs, listLoss, 'bo', label='training loss')
			plt.plot(xs, listValLoss, 'gs', label='validation loss')
			plt.xlabel('number of layers at each experiment')
			plt.ylabel('loss/val_loss')
			plt.title('Loss/Val_Loss per num_of_layers')
			plt.legend()

			xs.clear()
			int2str.clear()

			xs = list(range(len(list_xfilter)))
			for value in list_xfilter:
				int2str.append(str(value))

			plt.subplot(numplots, 1, 4)
			plt.xticks(xs,int2str)
			plt.plot(xs, listLoss, 'bo', label='training loss')
			plt.plot(xs, listValLoss, 'gs', label='validation loss')
			plt.xlabel('x coordinate of filter at each experiment')
			plt.ylabel('loss/val_loss')
			plt.title('Loss/Val_Loss per filter size (x coordinate)')
			plt.legend()

			xs.clear()
			int2str.clear()

			xs = list(range(len(list_yfilter)))
			for value in list_yfilter:
				int2str.append(str(value))

			plt.subplot(numplots, 1, 5)
			plt.xticks(xs,int2str)
			plt.plot(xs, listLoss, 'bo', label='training loss')
			plt.plot(xs, listValLoss, 'gs', label='validation loss')
			plt.xlabel('y coordinate of filter at each experiment')
			plt.ylabel('loss/val_loss')
			plt.title('Loss/Val_Loss per filter size (y coordinate)')
			plt.legend()

			xs.clear()
			int2str.clear()

			cnt = 6
			templistflt = []
			templistLoss = []
			templistValLoss = []
			for value in range(len(transpose)):
				temp1 = value + 1

				cnt1 = 0
				for pos in transpose[value]:
					if pos != -1:
						templistflt.append(transpose[value][cnt1])
						templistLoss.append(listLoss[cnt1])
						templistValLoss.append(listValLoss[cnt1])
					cnt1 = cnt1 + 1

				xs = list(range(len(templistflt)))
				for fltvalue in templistflt:
					int2str.append(str(fltvalue))

				plt.subplot(numplots, 1, cnt)
				plt.xticks(xs,int2str)
				plt.plot(xs, templistLoss, 'bo', label='training loss')
				plt.plot(xs, templistValLoss, 'gs', label='validation loss')
				plt.xlabel('number of filters')
				plt.ylabel('loss/val_loss')
				plt.title('Loss/Val_Loss per number_of_filters at layer %d' %temp1)
				plt.legend()

				xs.clear()
				int2str.clear()

				templistflt.clear()  #templistflt = []
				templistLoss.clear()
				templistValLoss.clear()
				cnt = cnt + 1


			plt.show()
			plt.savefig("autoencoderPlots.png")
			break
		if doNext == 3:
			path = input("Give the path: ")
			autoencoder.save(path) #save model(autoencoder)
			#save hyperparameters:
			fd1 = open("info.txt", "w")
			fd1.write(str(numOfLayers))
			fd1.write("\n")
			fd1.write(str(x_filter))
			fd1.write("\n")
			fd1.write(str(y_filter))
			fd1.write("\n")
			fd1.write(str(filtersPerLayer))
			fd1.write("\n")
			fd1.write(str(epochs))
			fd1.write("\n")
			fd1.write(str(batch_size))
			fd1.write("\n")
			fd1.write(str(ConvLayersEnc))
			fd1.write("\n")
			fd1.write(str(ConvLayersDec))
			fd1.write("\n")
			fd1.write(str(latent_dimension))
			fd1.write("\n")
			fd1.close()

			break

	#ta numOfLayers,x_filter,y_filter,filtersPerLayer,epochs,batch_size,ConvLayersEnc,ConvLayersDec
	#exoyn tis times toy teleytaioy peiramatos


if __name__ == "__main__":
	main()
