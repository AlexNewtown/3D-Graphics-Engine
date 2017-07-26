#include "../../header/openCL/NNKernel.h"

NNKernel::NNKernel(ClContext* context, size_t inputSize, size_t activationLayer1Size, size_t activationLayer2Size, size_t outputLayerSize, 
	float learningRate, size_t minibatchSize, size_t epochs) : 
	ClKernel(NN_KENRNEL_SOURCE, context, kernelGlobals(inputSize, activationLayer1Size, activationLayer2Size, outputLayerSize, learningRate, minibatchSize))
{
	cl_int ret;
	
	createKernel("activationLayer1");
	createKernel("activationLayer2");
	createKernel("activationLayerOutput");

	createKernel("activationLayer1Delta");
	createKernel("activationLayer2Delta");
	createKernel("activationOutputDelta");
	
	createKernel("addGradientWeightLayer1");
	createKernel("addGradientWeightLayer2");
	createKernel("addGradientWeightLayerOutput");

	createKernel("addGradientBiasLayer1");
	createKernel("addGradientBiasLayer2");
	createKernel("addGradientBiasLayerOutput");

	createKernel("updateNNparams");
	createKernel("cost");
	createKernel("normalizeGradient");

	this->__inputSize = inputSize;
	this->__activationLayer1Size = activationLayer1Size;
	this->__activationLayer2Size = activationLayer2Size;
	this->__activationOutputSize = outputLayerSize;
	this->__learningRate = learningRate;
	this->__minibatchSize = minibatchSize;
	this->__epochs = epochs;
	this->__numTrainingSample = 0;

	addNNKernelArg();

	__inputImageVector = nullptr;
	__trainingLabelsVector = nullptr;
	__nnParams = nullptr;
	__activations = nullptr;
	__activationDeltas = nullptr;
	__gradient = nullptr;

	__inputVector = new float[NN_INPUT_SIZE];
	__sampleIndex = new int[STOCHASTIC_SAMPLING_SIZE];
}

NNKernel::~NNKernel()
{
	clReleaseMemObject(__memobjInputVector);
	clReleaseMemObject(__memobjOutputTruthVector);
	clReleaseMemObject(__memobjActivationVector);
	clReleaseMemObject(__memobjNNParamsVector);
	clReleaseMemObject(__memobjActivationDeltaVector);
	clReleaseMemObject(__memobjGradientVector);
	clReleaseMemObject(__memobjCost);
	clReleaseMemObject(__memobjTrainingIndex);

	delete __inputImageVector;
	delete __trainingLabelsVector;

	delete __nnParams;
	delete __activations;
	delete __activationDeltas;
	delete __gradient;

	delete __inputVector;
	delete __sampleIndex;
}

void NNKernel::train(float* trainingSetInput, float* trainingSetOutput, size_t numTrainingSamples)
{
#if NN_DEBUG
	__elapsedTime = time(0);
#endif

	this->__numTrainingSample = numTrainingSamples;
	createBuffers(trainingSetInput, trainingSetOutput, numTrainingSamples);
	addNNKernelArg();
	readBuffers();

	srand(time(NULL));

	int* trainingSamples = new int[STOCHASTIC_SAMPLING_SIZE];

	for (int stochSampIndex = 0; stochSampIndex < SAMPLING_ITERATIONS; stochSampIndex++)
	{
		for (int tsIndex = 0; tsIndex < STOCHASTIC_SAMPLING_SIZE; tsIndex++)
		{
			float r =  ((float)__numTrainingSample - 1.0f)*((float)rand() / (float)RAND_MAX);
			trainingSamples[tsIndex] = (int)floor(r);
		}

		/*Clear Buffers*/
		clearGradient();
		clearCost();

		/*Set the sample indices*/
		setImageIndex(trainingSamples);

		/*Calculate the Activations*/
		calculateActivationsLayer1();
		calculateActivationsLayer2();
		calculateActivationsLayerOutput();

		/*Calculate the Activation Deltas*/
		calculateActivationsDeltaLayerOutput();
		calculateActivationsDeltaLayer2();
		calculateActivationsDeltaLayer1();

		/*Calculate the Gradients*/
		addGradientBiasLayerOutput();
		addGradientWeightLayerOutput();
		addGradientBiasLayer2();
		addGradientWeightLayer2();
		addGradientBiasLayer1();
		addGradientWeightLayer1();

		/*Compute Cost*/
		computeCost();
		normalizeGradient();
		readCost();
		__cost = __cost;// (float)STOCHASTIC_SAMPLING_SIZE;
		updateNNParams();
		readActivations();

#if NN_DEBUG
		__miniBatchCostHistory.push_back(__cost);
#endif
	}
	delete trainingSamples;
#if NN_DEBUG
	__elapsedTime = time(0) - __elapsedTime;
#endif
}

float NNKernel::totalCost()
{
	clearCost();
	int imageIndex = 0;
	int* trainingSetIndex = new int[STOCHASTIC_SAMPLING_SIZE];

	int numIters = __numTrainingSample / STOCHASTIC_SAMPLING_SIZE;

	for (int iterIndex = 0; iterIndex < numIters; iterIndex++)
	{
		for (int i = 0; i < STOCHASTIC_SAMPLING_SIZE; i++)
		{
			trainingSetIndex[i] = imageIndex;
			imageIndex++;
		}

		setImageIndex(trainingSetIndex);
		calculateActivationsLayer1();
		calculateActivationsLayer2();
		calculateActivationsLayerOutput();
		computeCost();
	}
	float totalCost;
	readCost();
	totalCost = __cost;

	int remainder = __numTrainingSample % STOCHASTIC_SAMPLING_SIZE;
	for (int i = 0; i < remainder; i++)
	{
		trainingSetIndex[i] = imageIndex;
		imageIndex++;
	}

	setImageIndex(trainingSetIndex);
	calculateActivationsLayer1();
	calculateActivationsLayer2();
	calculateActivationsLayerOutput();
	readActivations();

	for (int i = 0; i < remainder; i++)
	{
		for (int j = 0; j < NN_OUTPUT_SIZE; j++)
		{
			float outTruth = __trainingLabelsVector[trainingSetIndex[i] * NN_OUTPUT_SIZE + j];
			float outAct = __activations[i*NN_ACTIVATION_SIZE + NN_LAYER_1_SIZE + NN_LAYER_2_SIZE + j];
			float err = pow((outTruth - outAct), 2.0);
			totalCost += err;
		}
	}
	delete trainingSetIndex;
	return totalCost / (float)__numTrainingSample;
}

void NNKernel::predict(float* inputVector, float* outputVector, size_t numSamples)
{
	float* in;
	size_t ns;

	for (size_t iter = 0; iter < numSamples; iter = iter + STOCHASTIC_SAMPLING_SIZE)
	{
		if (iter + STOCHASTIC_SAMPLING_SIZE > numSamples)
			ns = numSamples % STOCHASTIC_SAMPLING_SIZE;
		else
			ns = STOCHASTIC_SAMPLING_SIZE;
		
		in = &(inputVector[iter*NN_INPUT_SIZE]);

		cl_int ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjInputVector, CL_TRUE, 0, ns*NN_INPUT_SIZE*sizeof(float), in, 0, NULL, NULL);
		for (int i = 0; i < ns; i++)
		{
			__sampleIndex[i] = i;
		}
		setImageIndex(__sampleIndex);
		calculateActivationsLayer1();
		calculateActivationsLayer2();
		calculateActivationsLayerOutput();
		readActivations();

		for (int actIndex = 0; actIndex < ns; actIndex++)
		{
			for (int i = 0; i < NN_OUTPUT_SIZE; i++)
			{
				size_t offset = NN_OUTPUT_SIZE*(iter + actIndex);
				size_t actOffset = NN_ACTIVATION_SIZE*actIndex + NN_LAYER_1_SIZE + NN_LAYER_2_SIZE;
				outputVector[offset + i] = __activations[actOffset + i];
			}
		}
		
	}
}

float* NNKernel::createImageVector(float* inputImage)
{
	size_t vectorSize = __numTrainingSample*NN_INPUT_SIZE;
	float* imVec = new float[vectorSize];
	for (size_t i = 0; i < vectorSize; i++)
	{
		imVec[i] = inputImage[i];
	}
	return imVec;
}

float* NNKernel::createOutputVector(float* trainingLabels)
{
	size_t vectorSize = __numTrainingSample*NN_OUTPUT_SIZE;
	float* outVec = new float[vectorSize];
	int outVecIndex = 0;
	for (size_t i = 0; i < vectorSize; i++)
	{
		outVec[i] = trainingLabels[i];
	}
	return outVec;
}

void NNKernel::createBuffers(float* inputImage, float* trainingLabels, size_t numTrainingSamples)
{
	cl_int ret;

	__nnParams = new float[NN_WEIGHT_SIZE];
	__activations = new float[STOCHASTIC_SAMPLING_SIZE*NN_ACTIVATION_SIZE];
	__activationDeltas = new float[STOCHASTIC_SAMPLING_SIZE*NN_ACTIVATION_SIZE];
	__gradient = new float[NN_WEIGHT_SIZE];

	size_t inputBufferSize = numTrainingSamples*NN_INPUT_SIZE*sizeof(float);
	size_t outputVectorSize = numTrainingSamples*NN_OUTPUT_SIZE*sizeof(float);
	size_t activationSize = STOCHASTIC_SAMPLING_SIZE*NN_ACTIVATION_SIZE*sizeof(float);
	size_t nnParamSize = NN_WEIGHT_SIZE*sizeof(float);

	float* inputImageVec = createImageVector(inputImage);
	float* outputImageVec = createOutputVector(trainingLabels);
	initNNParams();

	__memobjInputVector = clCreateBuffer(this->__context->context, CL_MEM_READ_WRITE, inputBufferSize, NULL, &ret);
	__memobjOutputTruthVector = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, outputVectorSize, NULL, &ret);
	__memobjActivationVector = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, activationSize, NULL, &ret);
	__memobjNNParamsVector = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, nnParamSize, NULL, &ret);
	__memobjActivationDeltaVector = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, activationSize, NULL, &ret);
	__memobjGradientVector = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, nnParamSize, NULL, &ret);
	__memobjCost = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, sizeof(float), NULL, &ret);
	__memobjTrainingIndex = clCreateBuffer(__context->context, CL_MEM_READ_WRITE, sizeof(int)*STOCHASTIC_SAMPLING_SIZE, NULL, &ret);

	ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjInputVector, CL_TRUE, 0, inputBufferSize, inputImageVec, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjOutputTruthVector, CL_TRUE, 0, outputVectorSize, outputImageVec, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjNNParamsVector, CL_TRUE, 0, nnParamSize, __nnParams, 0, NULL, NULL);
	__cost = 0.0;
	ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjCost, CL_TRUE, 0, sizeof(float), &__cost, 0, NULL, NULL);
	clFinish(__context->commandQueue);

	initGradientVector();

	__inputImageVector = inputImageVec;
	__trainingLabelsVector = outputImageVec;
}

void NNKernel::initNNParams()
{
	srand(time(NULL));

	float randRange = sqrt(6.0f / (float)(NN_INPUT_SIZE + NN_LAYER_1_SIZE));
	for (int i = 0; i < NN_LAYER_1_WEIGHT_SIZE; i++)
	{
		__nnParams[i] = randRange*(2.0*((float)rand() / (float)RAND_MAX) - 1.0);
	}

	randRange = sqrt(6.0f / (float)(NN_LAYER_2_SIZE + NN_LAYER_1_SIZE));
	for (int i = 0; i < NN_LAYER_2_WEIGHT_SIZE; i++)
	{
		__nnParams[NN_LAYER_1_WEIGHT_SIZE + NN_LAYER_1_BIAS_SIZE + i] = randRange*(2.0*((float)rand() / (float)RAND_MAX) - 1.0);
	}

	randRange = sqrt(6.0f / (float)(NN_OUTPUT_SIZE + NN_LAYER_1_SIZE));
	for (int i = 0; i < NN_LAYER_OUTPUT_WEIGHT_SIZE; i++)
	{
		__nnParams[NN_LAYER_1_WEIGHT_SIZE + NN_LAYER_1_BIAS_SIZE + NN_LAYER_2_WEIGHT_SIZE + NN_LAYER_2_BIAS_SIZE + i]
			= randRange*(2.0*((float)rand() / (float)RAND_MAX) - 1.0);
	}

	for (int i = 0; i < NN_LAYER_1_BIAS_SIZE; i++)
	{
		__nnParams[NN_LAYER_1_WEIGHT_SIZE + i] = 0.0;
	}

	for (int i = 0; i < NN_LAYER_2_BIAS_SIZE; i++)
	{
		__nnParams[NN_LAYER_1_WEIGHT_SIZE + NN_LAYER_1_BIAS_SIZE + NN_LAYER_2_WEIGHT_SIZE + i] = 0.0;
	}

	for (int i = 0; i < NN_LAYER_OUTPUT_BIAS_SIZE; i++)
	{
		__nnParams[NN_LAYER_1_WEIGHT_SIZE + NN_LAYER_1_BIAS_SIZE + NN_LAYER_2_WEIGHT_SIZE + NN_LAYER_2_BIAS_SIZE + NN_LAYER_OUTPUT_WEIGHT_SIZE + i] = 0.0;
	}
}

void NNKernel::initGradientVector()
{
	for (int i = 0; i < NN_WEIGHT_SIZE; i++)
	{
		__gradient[i] = 0.0;
	}
	cl_int ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjGradientVector, CL_TRUE, 0, NN_WEIGHT_SIZE*sizeof(float), __gradient, 0, NULL, NULL);
	clFinish(__context->commandQueue);
}

void NNKernel::addNNKernelArg()
{
	cl_int numSampleTemp = 0;

	/* ACTIVATION LAYER 1*/
	addKernelArg(0, 0, sizeof(cl_mem), (void*)&__memobjInputVector);
	addKernelArg(0, 1, sizeof(cl_mem), (void*)&__memobjTrainingIndex);
	addKernelArg(0, 2, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(0, 3, sizeof(cl_mem), (void*)&__memobjNNParamsVector);

	/* ACTIVATION LAYER 2*/
	addKernelArg(1, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(1, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);

	/* ACTIVATION LAYER OUTPUT */
	addKernelArg(2, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(2, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);



	/* ACTIVATION DELTA LAYER 1*/
	addKernelArg(3, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(3, 1, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(3, 2, sizeof(cl_mem), (void*)&__memobjNNParamsVector);


	/* ACTIVATION DELTA LAYER 2*/
	addKernelArg(4, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(4, 1, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(4, 2, sizeof(cl_mem), (void*)&__memobjNNParamsVector);

	/* ACTIVATION DELTA LAYER OUTPUT*/
	addKernelArg(5, 0, sizeof(cl_mem), (void*)&__memobjOutputTruthVector);
	addKernelArg(5, 1, sizeof(cl_mem), (void*)&__memobjTrainingIndex);
	addKernelArg(5, 2, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(5, 3, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);



	/* GRADIENT WEIGHT LAYER 1*/
	addKernelArg(6, 0, sizeof(cl_mem), (void*)&__memobjInputVector);
	addKernelArg(6, 1, sizeof(cl_mem), (void*)&__memobjTrainingIndex);
	addKernelArg(6, 2, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(6, 3, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(6, 4, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(6, 5, sizeof(cl_mem), (void*)&__memobjGradientVector);


	/* GRADIENT WEIGHT LAYER 2*/
	addKernelArg(7, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(7, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(7, 2, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(7, 3, sizeof(cl_mem), (void*)&__memobjGradientVector);


	/* GRADIENT WEIGHT LAYER OUTPUT*/
	addKernelArg(8, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(8, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(8, 2, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(8, 3, sizeof(cl_mem), (void*)&__memobjGradientVector);


	/* GRADIENT BIAS LAYER 1*/
	addKernelArg(9, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(9, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(9, 2, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(9, 3, sizeof(cl_mem), (void*)&__memobjGradientVector);

	/* GRADIENT BIAS LAYER 2*/
	addKernelArg(10, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(10, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(10, 2, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(10, 3, sizeof(cl_mem), (void*)&__memobjGradientVector);

	/* GRADIENT BIAS LAYER OUTPUT*/
	addKernelArg(11, 0, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(11, 1, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(11, 2, sizeof(cl_mem), (void*)&__memobjActivationDeltaVector);
	addKernelArg(11, 3, sizeof(cl_mem), (void*)&__memobjGradientVector);

	/*UPDATE VECTOR*/
	addKernelArg(12, 0, sizeof(cl_mem), (void*)&__memobjNNParamsVector);
	addKernelArg(12, 1, sizeof(cl_mem), (void*)&__memobjGradientVector);

	/*COST FUNCTION*/
	addKernelArg(13, 0, sizeof(cl_mem), (void*)&__memobjOutputTruthVector);
	addKernelArg(13, 1, sizeof(cl_mem), (void*)&__memobjTrainingIndex);
	addKernelArg(13, 2, sizeof(cl_mem), (void*)&__memobjActivationVector);
	addKernelArg(13, 3, sizeof(cl_mem), (void*)&__memobjCost);

	/*NORMALIZE GRADIENT*/
	addKernelArg(14, 0, sizeof(cl_mem), (void*)&__memobjGradientVector);
	int trainingSample = STOCHASTIC_SAMPLING_SIZE;
	addKernelArg(14, 1, sizeof(cl_int), (void*)&trainingSample);

}

void NNKernel::exportNNParams(char* filePath)
{
	//Neural network sizes are assumed to be the size they are defined.
	readBuffers();
	std::fstream file;
	file.open(filePath, std::fstream::out | std::fstream::binary);
	if (!file)
		return;

	file.write((char*)&__inputSize, sizeof(size_t));
	file.write((char*)&__activationLayer1Size, sizeof(size_t));
	file.write((char*)&__activationLayer2Size, sizeof(size_t));
	file.write((char*)&__activationOutputSize, sizeof(size_t));
	file.write((char*)&__learningRate, sizeof(float));
	file.write((char*)&__minibatchSize, sizeof(size_t));
	file.write((char*)&__epochs, sizeof(size_t));

	//write the weights to a file.
	for (int i = 0; i < NN_WEIGHT_SIZE; i++)
	{
		float w = __nnParams[i];
		file.write((char*)&w, 4);
	}
	
	file.close();
}

void NNKernel::importNNParams(char* filePath)
{
	std::fstream file;
	file.open(filePath, std::fstream::in | std::fstream::binary);
	
	if (!file)
		return;

	file.read((char*)&__inputSize, sizeof(size_t));
	file.read((char*)&__activationLayer1Size, sizeof(size_t));
	file.read((char*)&__activationLayer2Size, sizeof(size_t));
	file.read((char*)&__activationOutputSize, sizeof(size_t));
	file.read((char*)&__learningRate, sizeof(float));
	file.read((char*)&__minibatchSize, sizeof(size_t));
	file.read((char*)&__epochs, sizeof(size_t));

	for (int i = 0; i < NN_WEIGHT_SIZE; i++)
	{
		float w;
		file.read((char*)&w, 4);
		__nnParams[i] = w;
	}
	file.close();
	cl_int ret = clEnqueueWriteBuffer(__context->commandQueue, __memobjNNParamsVector, CL_TRUE, 0, NN_WEIGHT_SIZE*sizeof(float), __nnParams, 0, NULL, NULL);
}

#if NN_DEBUG

void NNKernel::exportReport(char* filePath)
{
	time_t t = time(0);

	std::string fp(filePath);
	std::ofstream myfile;
	myfile.open(fp, std::ofstream::out);

	struct tm * now = localtime(&t);
	myfile << "Date: ";
	myfile << (now->tm_year + 1900);
	myfile << '-';
	myfile << (now->tm_mon + 1);
	myfile << '-';
	myfile << now->tm_mday;
	myfile << "\n";


	myfile << "Training time: ";
	myfile << std::to_string((float)__elapsedTime / 60.0);
	myfile << "  min";
	myfile << "\n";


	myfile << "input vector size: ";
	myfile << std::to_string(NN_INPUT_SIZE);
	myfile << "\n";

	myfile << "layer 1 activation size: ";
	myfile << std::to_string(NN_LAYER_1_SIZE);
	myfile << "\n";

	myfile << "layer 2 activation size: ";
	myfile << std::to_string(NN_LAYER_2_SIZE);
	myfile << "\n";

	myfile << "output vector size: ";
	myfile << std::to_string(NN_OUTPUT_SIZE);
	myfile << "\n";

	myfile << "stochastic minibatch size: ";
	myfile << std::to_string(STOCHASTIC_SAMPLING_SIZE);
	myfile << "\n";

	myfile << "minibatch cost history:\n";
	for (int i = 0; i < __miniBatchCostHistory.size(); i++)
	{
		myfile << std::to_string(__miniBatchCostHistory[i]);
		myfile << "\n";
	}
	
	float c = totalCost();
	myfile << "total training set cost :";
	myfile << std::to_string(c);

	myfile.close();
}

#endif

std::vector<std::string> NNKernel::kernelGlobals(int inputSize, int layer1Size, int layer2Size, int outputSize, float learningRate, int minibatchSize)
{
	std::vector<std::string> defines;

	std::string defInput("#define INPUT_SIZE ");
	defInput.append(std::to_string(inputSize));
	defInput.append(" \n");

	std::string defLayer1Size("#define LAYER_1_SIZE ");
	defLayer1Size.append(std::to_string(layer1Size));
	defLayer1Size.append(" \n");

	std::string defLayer2Size("#define LAYER_2_SIZE ");
	defLayer2Size.append(std::to_string(layer2Size));
	defLayer2Size.append(" \n");

	std::string defLayerOutputSize("#define OUTPUT_SIZE ");
	defLayerOutputSize.append(std::to_string(outputSize));
	defLayerOutputSize.append(" \n");

	std::string defLearningRate("#define LEARNING_RATE ");
	defLearningRate.append(std::to_string(learningRate));
	defLearningRate.append(" \n");

	std::string defMinibatchSize("#define STOCHASTIC_SAMPLING_SIZE ");
	defMinibatchSize.append(std::to_string(minibatchSize));
	defMinibatchSize.append(" \n");

	
	defines.push_back(defInput);
	defines.push_back(defLayer1Size);
	defines.push_back(defLayer2Size);
	defines.push_back(defLayerOutputSize);
	defines.push_back(defLearningRate);
	defines.push_back(defMinibatchSize);

	return defines;
}

float* NNKernel::nnParams()
{
	return __nnParams;
}