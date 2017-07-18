#include "shading utils\PhotonMap.h"

PhotonMap::PhotonMap()
{
	pos[0] = 0.0f;
	pos[1] = 0.0f;
	pos[2] = 0.0f;
	power[0] = 0.0f;
	power[1] = 0.0f;
	power[2] = 0.0f;

	incidentDirection[0] = 0;
	incidentDirection[1] = 0;
	incidentDirection[2] = 0;
	index = -1;
}
PhotonMap::~PhotonMap()
{
	
}

MaterialFace::MaterialFace()
{
	diffuseP = 1.0;
	specularP = 0.0;
	indexOfRefraction = 1.0;
}
MaterialFace::~MaterialFace()
{

}

PhotonMapKdTree::PhotonMapKdTree(std::vector<PhotonMap*> &photonMap)
{
	__bounds = computeBounds(photonMap, 0, photonMap.size());

	__children[0] = NULL;
	__children[1] = NULL;
	build(photonMap, 0, photonMap.size(), 0);
}

PhotonMapKdTree::PhotonMapKdTree(std::vector<PhotonMap*> &photonMap, Bounds* parentBounds, int start, int end, int sortOn)
{
	__children[0] = NULL;
	__children[1] = NULL;

	__bounds = computeBounds(photonMap, start, end);

	switch(sortOn)
	{
	case SORT_ON_X:
		__bounds->low->y = parentBounds->low->y;
		__bounds->high->y = parentBounds->high->y;
		
		__bounds->low->x = parentBounds->low->x;
		__bounds->high->x = parentBounds->high->x;

		break;
	case SORT_ON_Y:

		__bounds->low->y = parentBounds->low->y;
		__bounds->high->y = parentBounds->high->y;

		__bounds->low->z = parentBounds->low->z;
		__bounds->high->z = parentBounds->high->z;

		break;
	case SORT_ON_Z:

		__bounds->low->x = parentBounds->low->x;
		__bounds->high->x = parentBounds->high->x;

		__bounds->low->z = parentBounds->low->z;
		__bounds->high->z = parentBounds->high->z;

		break;
	}

	build(photonMap, start, end, sortOn);
}

PhotonMapKdTree::~PhotonMapKdTree()
{
	delete __node;
	delete __bounds->low;
	delete __bounds->high;
	delete __bounds;
	if (children(0) != NULL)
		delete children(0);
	if (children(1) != NULL)
		delete children(1);
}

void PhotonMapKdTree::build(std::vector<PhotonMap*> &photonMap, int start, int end, int sortOn)
{
	__sortOn = sortOn;
	__photonIndex = -1;
	__start = start;
	__end = end;
	numTreePhotons = end - start;

	if (end - start == 1)
	{
		__node = new Point(photonMap[start]->pos[0], photonMap[start]->pos[1], photonMap[start]->pos[2]);
		__photonIndex = start;//photonMap[start]->index;
		return;
	}

	insertionSortPhotons(photonMap, start, end, sortOn);

	int numPoints = (end - start);
	int mid = (int)floor((float)numPoints*0.5) + start;

	__node = new Point(photonMap[mid]->pos[0], photonMap[mid]->pos[1], photonMap[mid]->pos[2]);

	int numPointsHigh = (end - mid);
	int numPointsLow = (mid - start);

	if (numPointsHigh > 0)
		__children[1] = new PhotonMapKdTree(photonMap, __bounds, mid, end, nextSortOn(sortOn));


	if (numPointsLow > 0)
		__children[0] = new PhotonMapKdTree(photonMap, __bounds, start, mid, nextSortOn(sortOn));
}

void PhotonMapKdTree::insertionSortPhotons(std::vector<PhotonMap*> &photonMap, int start, int end, int sortOn)
{
	for (int i = start; i < end; i++)
	{
		for (int j = i + 1; j < end; j++)
		{
			if (photonMap[j]->pos[__sortOn] < photonMap[i]->pos[__sortOn])
			{
				PhotonMap* temp = photonMap[i];
				photonMap[i] = photonMap[j];
				photonMap[j] = temp;
			}
		}
	}
}

int PhotonMapKdTree::nextSortOn(int sortOn)
{
	switch (sortOn)
	{
	case SORT_ON_X:
		return SORT_ON_Y;
		break;
	case SORT_ON_Y:
		return SORT_ON_Z;
		break;
	case SORT_ON_Z:
		return SORT_ON_X;
		break;
	}
}

int PhotonMapKdTree::photonIndex()
{
	return __photonIndex;
}

bool PhotonMapKdTree::isLeaf()
{
	if (__children[0] == NULL && __children[1] == NULL)
		return true;
	return false;
}

PhotonMapKdTree* PhotonMapKdTree::children(int index)
{
	if (index < 0 || index > 1)
		return NULL;
	return __children[index];
}

int PhotonMapKdTree::start()
{
	return __start;
}
int PhotonMapKdTree::end()
{
	return __end;
}

Point* PhotonMapKdTree::node()
{
	return __node;
}

Bounds* PhotonMapKdTree::getBoundary()
{
	return __bounds;
}

Bounds* PhotonMapKdTree::computeBounds(std::vector<PhotonMap*> &photonMap, int start, int end)
{
	PhotonMap* currentPhotonMap;
	currentPhotonMap = photonMap[start];
	Bounds* b = new Bounds();
	b->low = new Point();
	b->high = new Point();

	b->low->x = currentPhotonMap->pos[0];
	b->high->x = currentPhotonMap->pos[0];
	b->low->y = currentPhotonMap->pos[1];
	b->high->y = currentPhotonMap->pos[1];
	b->low->z = currentPhotonMap->pos[2];
	b->high->z = currentPhotonMap->pos[2];

	for (int i = start; i < end; i++)
	{
		currentPhotonMap = photonMap[i];
		b->low->x = minimum(currentPhotonMap->pos[0], b->low->x);
		b->low->y = minimum(currentPhotonMap->pos[1], b->low->y);
		b->low->z = minimum(currentPhotonMap->pos[2], b->low->z);

		b->high->x = maximum(currentPhotonMap->pos[0], b->high->x);
		b->high->y = maximum(currentPhotonMap->pos[1], b->high->y);
		b->high->z = maximum(currentPhotonMap->pos[2], b->high->z);
	}
	b->reCalculate();
	return b;
}

unsigned int PhotonMapKdTree::sortOn()
{
	return __sortOn;
}

void* createPhotonMapTexture(std::vector < PhotonMap*> photonMap)
{
	float* pmTex = new float[photonMap.size()*sizeof(PhotonMapTexture)];

	int texIndex = 0;
	for (int i = 0; i < photonMap.size(); i++)
	{
		pmTex[texIndex] = photonMap[i]->pos[0];
		pmTex[texIndex + 1] = photonMap[i]->pos[1];
		pmTex[texIndex + 2] = photonMap[i]->pos[2];

		pmTex[texIndex + 3] = photonMap[i]->incidentDirection[0];
		pmTex[texIndex + 4] = photonMap[i]->incidentDirection[1];
		pmTex[texIndex + 5] = photonMap[i]->incidentDirection[2];

		float p = pow(photonMap[i]->power[0], 2.0) + pow(photonMap[i]->power[1], 2.0) + pow(photonMap[i]->power[2], 2.0);
		pmTex[texIndex + 6] = sqrt(p);
		texIndex += sizeof(PhotonMapTexture)/4;
	}
	return (void*)pmTex;
}

PhotonMapKdTextureInt::PhotonMapKdTextureInt()
{
	treeIndex = -1;
	childIndex0 = -1;
	childIndex1 = -1;
	sortOn = -1;
	photonIndex = -1;
	numTreePhotons = -1;
	startIndex = -1;
	endIndex = -1;
}
PhotonMapKdTextureFloat::PhotonMapKdTextureFloat()
{
	minX = 0.0;
	minY = 0.0;
	minZ = 0.0;
	maxX = 0.0;
	maxY = 0.0;
	maxZ = 0.0;
}
PhotonMapTexture::PhotonMapTexture()
{
	index = -1;
	x = 0.0;
	y = 0.0;
	z = 0.0;
	dirX = 0.0;
	dirY = 0.0;
	dirZ = 0.0;
	illum = 0.0;
}


void buildPhotonMapKdTreeArray(PhotonMapKdTree* tree, std::vector<PhotonMapKdTextureInt*> &texInt, std::vector<PhotonMapKdTextureFloat*> &texFloat)
{
	PhotonMapKdTextureInt* tInt = new PhotonMapKdTextureInt();
	PhotonMapKdTextureFloat* tFloat = new PhotonMapKdTextureFloat();
	tInt->sortOn = tree->sortOn();
	tInt->treeIndex = texInt.size();
	tInt->numTreePhotons = tree->numTreePhotons;
	tInt->startIndex = tree->start();
	tInt->endIndex = tree->end();
	texInt.push_back(tInt);

	tFloat->minX = tree->getBoundary()->low->x;
	tFloat->minY = tree->getBoundary()->low->y;
	tFloat->minZ = tree->getBoundary()->low->z;
	tFloat->maxX = tree->getBoundary()->high->x;
	tFloat->maxY = tree->getBoundary()->high->y;
	tFloat->maxZ = tree->getBoundary()->high->z;
	texFloat.push_back(tFloat);

	if (tree->isLeaf())
	{
		tInt->photonIndex = tree->photonIndex();
		tInt->childIndex0 = -1;
		tInt->childIndex1 = -1;
	}

	if (tree->children(0) != NULL)
	{
		tInt->childIndex0 = texInt.size();
		buildPhotonMapKdTreeArray(tree->children(0), texInt, texFloat);
	}
	if (tree->children(1) != NULL)
	{
		tInt->childIndex1 = texInt.size();
		buildPhotonMapKdTreeArray(tree->children(1), texInt, texFloat);
	}

}

void createPhotonMapKdTexture(PhotonMapKdTree* tree, void* &photonMapKdTextureInt, int* photonMapKdTextureIntSize, void* &photonMapKdTextureFloat, int* photonMapKdTextureFloatSize)
{
	std::vector<PhotonMapKdTextureInt*> texInt;
	std::vector<PhotonMapKdTextureFloat*> texFloat;
	buildPhotonMapKdTreeArray(tree, texInt, texFloat);

	int* textureDataI = new int[texInt.size()*(sizeof(PhotonMapKdTextureInt)/4)];
	float* textureDataF = new float[texFloat.size()*(sizeof(PhotonMapKdTextureFloat)/4)];
	*photonMapKdTextureIntSize = texInt.size();
	*photonMapKdTextureFloatSize = texFloat.size();

	int texIndex = 0;
	for (int i = 0; i < texInt.size(); i++)
	{
		/*
		int treeIndex;
		int childIndex0;
		int childIndex1;
		int sortOn;
		int photonIndex;
		*/
		PhotonMapKdTextureInt* ti = texInt[i];
		textureDataI[texIndex] = ti->treeIndex;
		textureDataI[texIndex + 1] = ti->childIndex0;
		textureDataI[texIndex + 2] = ti->childIndex1;
		textureDataI[texIndex + 3] = ti->sortOn;
		textureDataI[texIndex + 4] = ti->photonIndex;
		textureDataI[texIndex + 5] = ti->numTreePhotons;
		textureDataI[texIndex + 6] = ti->startIndex;
		textureDataI[texIndex + 7] = ti->endIndex;
		texIndex += sizeof(PhotonMapKdTextureInt)/4;
	}

	texIndex = 0;
	for (int i = 0; i < texFloat.size(); i++)
	{
		/*
		float minX;
		float minY;
		float minZ;
		float maxX;
		float maxY;
		float maxZ;
		*/
		PhotonMapKdTextureFloat* tf = texFloat[i];
		textureDataF[texIndex] = tf->maxX;
		textureDataF[texIndex + 1] = tf->maxY;
		textureDataF[texIndex + 2] = tf->maxZ;
		textureDataF[texIndex + 3] = tf->minX;
		textureDataF[texIndex + 4] = tf->minY;
		textureDataF[texIndex + 5] = tf->minZ;
		texIndex += sizeof(PhotonMapKdTextureFloat)/4;
	}

	photonMapKdTextureInt = (void*)textureDataI;
	photonMapKdTextureFloat = (void*)textureDataF;
	texInt.clear();
	texFloat.clear();
}

void createPhotonMapClusterTexture(std::vector<PhotonMap*> photonMap, void* &photonMapClusterFloat, void* &photonMapClusterInt, void* &photonMapTexture)
{
	cv::Mat photonMat(photonMap.size(),3,CV_32F);

	for (int i = 0; i < photonMap.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			photonMat.at<float>(i, j) = photonMap[i]->pos[j];
		}
	}
	cv::Mat labels;
	cv::Mat centers;
	cv::TermCriteria tc(cv::TermCriteria::COUNT, 40, 1e-6);
	cv::kmeans(photonMat, K_PHOTON_MAP_CLUSTERS, labels, tc, 3, cv::KMEANS_PP_CENTERS, centers);

	float maxRadius[K_PHOTON_MAP_CLUSTERS];
	for (int i = 0; i < K_PHOTON_MAP_CLUSTERS; i++)
	{
		maxRadius[i] = 0.0;
	}

	std::vector<int> photonLabelBins[K_PHOTON_MAP_CLUSTERS];
	for (int i = 0; i < labels.rows; i++)
	{
		int labelIndex = labels.at<int>(i, 0);
		photonLabelBins[labelIndex].push_back(i);

		float photonx = photonMap[i]->pos[0];
		float photony = photonMap[i]->pos[1];
		float photonz = photonMap[i]->pos[2];

		float ccx = centers.at<float>(labelIndex, 0);
		float ccy = centers.at<float>(labelIndex, 1);
		float ccz = centers.at<float>(labelIndex, 2);

		float dist = pow(photonx - ccx, 2.0) + pow(photony - ccy, 2.0) + pow(photonz - ccz, 2.0);
		dist = sqrt(dist);

		if (dist > maxRadius[labelIndex])
		{
			maxRadius[labelIndex] = dist;
		}
	}

	PhotonMapClusterFloat* clusterTexFloat = new PhotonMapClusterFloat[K_PHOTON_MAP_CLUSTERS];
	PhotonMapClusterInt* clusterTexInt = new PhotonMapClusterInt[K_PHOTON_MAP_CLUSTERS];
	int photonStart = 0;
	int photonEnd;


	for (int i = 0; i < K_PHOTON_MAP_CLUSTERS; i++)
	{
		std::vector<int> bin = photonLabelBins[i];
		
		photonEnd = photonStart + bin.size();
		clusterTexInt[i].start = photonStart;
		clusterTexInt[i].end = photonEnd;

		clusterTexFloat[i].x = centers.at<float>(i, 0);
		clusterTexFloat[i].y = centers.at<float>(i, 1);
		clusterTexFloat[i].z = centers.at<float>(i, 2);
		clusterTexFloat[i].radius = maxRadius[i];

		photonStart = photonEnd;
	}

	int pmIndex = 0;
	PhotonMapTexture* pmt = new PhotonMapTexture[photonMap.size()];
	for (int i = 0; i < K_PHOTON_MAP_CLUSTERS; i++)
	{
		std::vector<int> bin = photonLabelBins[i];
		for (int pIndex = 0; pIndex < bin.size(); pIndex++)
		{
			PhotonMap* photonLookup = photonMap[bin[pIndex]];
			pmt[pmIndex].x = photonLookup->pos[0];
			pmt[pmIndex].y = photonLookup->pos[1];
			pmt[pmIndex].z = photonLookup->pos[2];
			pmt[pmIndex].dirX = photonLookup->incidentDirection[0];
			pmt[pmIndex].dirY = photonLookup->incidentDirection[1];
			pmt[pmIndex].dirZ = photonLookup->incidentDirection[2];
			
			float p = pow(photonLookup->power[0], 2.0) + pow(photonLookup->power[1], 2.0) + pow(photonLookup->power[2], 2.0);
			pmt[pmIndex].illum = sqrt(p);
			pmIndex++;
		}
	}

	photonMapClusterFloat = clusterTexFloat;
	photonMapClusterInt = clusterTexInt;
	photonMapTexture = pmt;
}