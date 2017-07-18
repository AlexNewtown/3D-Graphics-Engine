#pragma once

#include "../../header/openCL/ClKernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>

#define NN_DEBUG 1

#define NN_KENRNEL_SOURCE "../bin/kernels/NeuralNetwork_v2.cl"

#define NN_INPUT_SIZE __inputSize
#define NN_LAYER_1_SIZE __activationLayer1Size
#define NN_LAYER_2_SIZE __activationLayer2Size
#define NN_OUTPUT_SIZE __activationOutputSize
#define NN_ACTIVATION_SIZE (NN_LAYER_1_SIZE + NN_LAYER_2_SIZE + NN_OUTPUT_SIZE)

#define STOCHASTIC_SAMPLING_SIZE __minibatchSize
#define SAMPLING_ITERATIONS __epochs

#define NN_LAYER_1_WEIGHT_SIZE		(NN_INPUT_SIZE*NN_LAYER_1_SIZE)
#define NN_LAYER_1_BIAS_SIZE	NN_LAYER_1_SIZE
#define NN_LAYER_2_WEIGHT_SIZE		(NN_LAYER_1_SIZE*NN_LAYER_2_SIZE)
#define NN_LAYER_2_BIAS_SIZE	NN_LAYER_2_SIZE
#define NN_LAYER_OUTPUT_WEIGHT_SIZE		(NN_LAYER_2_SIZE*NN_OUTPUT_SIZE)
#define NN_LAYER_OUTPUT_BIAS_SIZE		NN_OUTPUT_SIZE

#define NN_WEIGHT_SIZE (NN_LAYER_1_WEIGHT_SIZE + NN_LAYER_1_BIAS_SIZE + NN_LAYER_2_WEIGHT_SIZE + NN_LAYER_2_BIAS_SIZE + NN_LAYER_OUTPUT_WEIGHT_SIZE + NN_LAYER_OUTPUT_BIAS_SIZE)

/*
class NNKernel
Neural Network class used for digit recognition.

Contructor:
@param CLContext*
@param vector<Mat*> input image training set
@param vector<unsigned char> image label training set.

void train()
trains the neural network parameters using the training set.

unsigned char predict(Mat* image)
predicts the label of the image using the neural network.
@param Mat* input image

@return redicted label.
*/
class NNKernel : public ClKernel
{
public:
	/*CONSTRUCTOR*/
	NNKernel(ClContext* context, size_t inputSize, size_t activationLayer1Size, size_t activationLayer2Size, size_t outputLayerSize, float learningRate, size_t minibatchSize, size_t epochs);
	/*DESTRUCTOR*/
	virtual ~NNKernel();
	
	/*
	void train
	trains the Neural Network parameters using the training images and training labels.
	*/
	virtual void train(float* trainingSetInput, float* trainingSetOutput, size_t numTrainingSamples);

	/*
	float totalCost
	computes the total cost to the neural network with the training set.
	*/
	float totalCost();

	/*
	unsigned char predict
	predicts the digit classification using the neural network.
	@return unsigned char, predicted digit [0,9]
	*/
	void predict(float* inputVector, float* outputVector, size_t numSamples);
	float* nnParams();

	void exportNNParams(char* filePath);
	void importNNParams(char* filePath);

#if NN_DEBUG
	void exportReport(char* filePath);
#endif
private:

	size_t __inputSize;
	size_t __activationLayer1Size;
	size_t __activationLayer2Size;
	size_t __activationOutputSize;
	float __learningRate;
	size_t __minibatchSize;
	size_t __epochs;

	/*BUFFERS AND VARIABLES*/
	float* __nnParams;
	float* __activations;
	float* __activationDeltas;
	float* __gradient;
	float __cost;

	float* __inputImageVector;
	float* __trainingLabelsVector;
	size_t __numTrainingSample;

	/*
	void addNNKernelArg
	adds kernel arguments to each kernel function
	*/
	void addNNKernelArg();

	/*
	void createBuffers
	creates all the buffers necessary for the Neural Network
	@param vector<Mat*> : image training set.
	@param vector<unsigned char> : label training set.
	*/
	void createBuffers(float* inputImage, float* trainingLabels, size_t numTrainingSamples);

	/*
	float* createImageVector
	vectorizes the image training set.
	@param vector<Mat*> : image training set

	@return float* : pointer to the images vector 
	*/
	float* createImageVector(float* inputImage);

	/*
	float* createOutputVector
	vectorizes the output training labels.
	@param vector<unsigned char> : training labels

	@return float* : pointer to the labels vector
	*/
	float* createOutputVector(float* trainingLabels);

	/*
	void initNNParams
	initializes the neural network weights. The weights are sampled from a uniform distribution. 
	*/
	void initNNParams();

	/*
	void initGradientVector
	sets the gradient memory objects to zero.
	*/
	void initGradientVector();

	/*
	void setImageIndex
	sets the image index of the relevant kernels
	*/
	inline void setImageIndex(int* index)
	{
		clEnqueueWriteBuffer(__context->commandQueue, __memobjTrainingIndex, CL_TRUE, 0, sizeof(int)*STOCHASTIC_SAMPLING_SIZE, index, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	/*functions that read memory object buffers into host memory*/
	inline void readNNParams()
	{
		cl_int ret;
		ret = clEnqueueReadBuffer(__context->commandQueue, __memobjNNParamsVector, CL_TRUE, 0, NN_WEIGHT_SIZE * sizeof(float), (void*)__nnParams, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	inline void readActivations()
	{
		cl_int ret;
		ret = clEnqueueReadBuffer(__context->commandQueue, __memobjActivationVector, CL_TRUE, 0, STOCHASTIC_SAMPLING_SIZE*NN_ACTIVATION_SIZE * sizeof(float), (void*)__activations, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	inline void readGradient()
	{
		cl_int ret;
		ret = clEnqueueReadBuffer(__context->commandQueue, __memobjGradientVector, CL_TRUE, 0, NN_WEIGHT_SIZE * sizeof(float), (void*)__gradient, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	inline void readActivationDelta()
	{
		cl_int ret;
		ret = clEnqueueReadBuffer(__context->commandQueue, __memobjActivationDeltaVector, CL_TRUE, 0, STOCHASTIC_SAMPLING_SIZE*NN_ACTIVATION_SIZE * sizeof(float), (void*)__activationDeltas, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	inline void readCost()
	{
		cl_int ret;
		ret = clEnqueueReadBuffer(__context->commandQueue, __memobjCost, CL_TRUE, 0, sizeof(float), (void*)&__cost, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	inline void readBuffers()
	{
		readNNParams();
		readActivations();
		readGradient();
		readActivationDelta();
		readCost();
	}

	/* 
	void clearBuffers
	clears the gradient buffer.
	*/
	inline void clearGradient()
	{
		for (int i = 0; i < NN_WEIGHT_SIZE; i++)
		{
			__gradient[i] = 0.0;
		}
		clEnqueueWriteBuffer(__context->commandQueue, __memobjGradientVector, CL_TRUE, 0, NN_WEIGHT_SIZE*sizeof(float), __gradient, 0, NULL, NULL);
		clFinish(__context->commandQueue);
	}

	/*
	void clearCost
	clears the cost buffer.
	*/
	inline void clearCost()
	{
		__cost = 0;
		clEnqueueWriteBuffer(__context->commandQueue, __memobjCost, CL_TRUE, 0, sizeof(float), &__cost, 0, NULL, NULL);
	}
	
	/*
	void computeCost
	computes the NN cost from the activation buffer and the label buffer
	and adds it to the cost buffer.
	*/
	inline void computeCost()
	{
		enqueueKernel(13, 1, 1);
	}

	/*
	void normalizeGradient
	divides the gradient vector by the total number of training samples.
	(the cost is the average l2 norm)
	*/
	inline void normalizeGradient()
	{
		enqueueKernel(14, NN_WEIGHT_SIZE, 1);
	}

	/*
	void updateNNParams
	updates the NNweights of the Neural Network.
	*/
	inline void updateNNParams()
	{
		enqueueKernel(12, NN_WEIGHT_SIZE, 1);
	}

	/*
	float gradientInnerProduct
	computes the inner product of the gradient vector.
	*/
	inline float gradientInnerProduct()
	{
		float ip = 0.0;
		for (int i = 0; i < NN_WEIGHT_SIZE; i++)
		{
			ip += __gradient[i] * __gradient[i];
		}
		return ip;
	}

	/*
	void calculateActivationsLayer1
	computes the activations in layer 1 and places them in the activation buffer.
	*/
	inline void calculateActivationsLayer1()
	{
		enqueueKernel(0, NN_LAYER_1_SIZE, STOCHASTIC_SAMPLING_SIZE);
	}

	/*
	void calculateActivationsLayer2
	computes the activations in layer 2 and places them in the activation buffer.
	*/
	inline void calculateActivationsLayer2()
	{
		enqueueKernel(1, NN_LAYER_2_SIZE, STOCHASTIC_SAMPLING_SIZE);
	}

	/*
	void calculateActivationsLayerOutput
	computes the activations in the output layer and places them in the activation buffer.
	*/
	inline void calculateActivationsLayerOutput()
	{
		enqueueKernel(2, NN_OUTPUT_SIZE, STOCHASTIC_SAMPLING_SIZE);
	}

	/*
	void calculateActivationsDeltaLayer1
	computes the activations delta in layer 1 and places them in the activation delta buffer.
	*/
	inline void calculateActivationsDeltaLayer1()
	{
		enqueueKernel(3, NN_LAYER_1_SIZE, STOCHASTIC_SAMPLING_SIZE);
	}

	/*
	void calculateActivationsDeltaLayer2
	computes the activations delta in layer 2 and places them in the activation delta buffer.
	*/
	inline void calculateActivationsDeltaLayer2()
	{
		enqueueKernel(4, NN_LAYER_2_SIZE, STOCHASTIC_SAMPLING_SIZE);
	}

	/*
	void calculateActivationsDeltaLayerOutput
	computes the activations delta in the output layer and places them in the activation delta buffer.
	*/
	inline void calculateActivationsDeltaLayerOutput()
	{
		enqueueKernel(5, NN_OUTPUT_SIZE, STOCHASTIC_SAMPLING_SIZE);
	}

	/*
	void addGradientWeightLayer1
	computes the gradient of each weight in layer 1 and adds it to the gradient buffer
	*/
	inline void addGradientWeightLayer1()
	{
		enqueueKernel(6, NN_LAYER_1_WEIGHT_SIZE, 1);
	}

	/*
	void addGradientWeightLayer2
	computes the gradient of each weight in layer 2 and adds it to the gradient buffer
	*/
	inline void addGradientWeightLayer2()
	{
		enqueueKernel(7, NN_LAYER_2_WEIGHT_SIZE, 1);
	}


	/*
	void addGradientWeightLayerOutput
	computes the gradient of each weight in the output layer and adds it to the gradient buffer
	*/
	inline void addGradientWeightLayerOutput()
	{
		enqueueKernel(8, NN_LAYER_OUTPUT_WEIGHT_SIZE, 1);
	}

	/*
	void addGraidnetBiasLayer1
	computes the gradient of the biases in layer 1 and adds it to the gradient buffer.
	*/
	inline void addGradientBiasLayer1()
	{
		enqueueKernel(9, NN_LAYER_1_BIAS_SIZE, 1);
	}

	/*
	void addGraidnetBiasLayer2
	computes the gradient of the biases in layer 2 and adds it to the gradient buffer.
	*/
	inline void addGradientBiasLayer2()
	{
		enqueueKernel(10, NN_LAYER_2_BIAS_SIZE, 1);
	}

	/*
	void addGraidnetBiasLayerOutput
	computes the gradient of the biases in the output layer and adds it to the gradient buffer.
	*/
	inline void addGradientBiasLayerOutput()
	{
		enqueueKernel(11, NN_LAYER_OUTPUT_BIAS_SIZE, 1);
	}

	std::vector<std::string> kernelGlobals(int inputSize, int layer1Size, int layer2Size, int outputSize, float learningRate, int minibatchSize);

	/*MEMORY OBJECTS*/
	cl_mem __memobjInputVector;
	cl_mem __memobjOutputTruthVector;
	cl_mem __memobjActivationVector;
	cl_mem __memobjNNParamsVector;
	cl_mem __memobjActivationDeltaVector;
	cl_mem __memobjGradientVector;
	cl_mem __memobjCost;
	cl_mem __memobjLearningParameter;
	cl_mem __memobjTrainingIndex;

	/* REPORT VARIABLES*/
#if NN_DEBUG
	time_t __elapsedTime;
	std::vector<float> __miniBatchCostHistory;
#endif
	/*reused variables, avoids leaks*/
	float* __inputVector;
	int* __sampleIndex;
};