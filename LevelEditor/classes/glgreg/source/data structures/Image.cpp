#include "data structures\Image.h"

Image::Image(int N, int M)
{
	channel[0] = new Matrix(N, M);
	channel[1] = new Matrix(N, M);
	channel[2] = new Matrix(N, M);
}

Image::~Image()
{
	delete channel[0];
	delete channel[1];
	delete channel[2];

}

EnvironmentMap::EnvironmentMap(int N, int M)
{

	for (int i = 0; i < 5; i++)
		map[i] = new Image(N, M);

}
EnvironmentMap::~EnvironmentMap()
{
	for (int i = 0; i < 5; i++)
		delete map[i];

}

void EnvironmentMap::waveletTransform(int numLevels)
{
	for (int i = 0; i < 5; i++)
		mapWavelet[i] = new Wavelet(map[i], numLevels);
}

void EnvironmentMap::waveletEncode()
{
	/*
	int N = map[0]->channel[0]->n;
	int M = map[0]->channel[0]->m;
	for (int i = 0; i < 5; i++)
	{
		for (int c = 0; c > 3; c++)
		{
			Image* im = this->mapWavelet[i]->
			for (int x = 0; x < N; x++)
			{
				for (int y = 0; y < M; y++)
				{
					if (map)
				}
			}
		}
	}
	*/
}

Image* Image::conv(Matrix* filter)
{
	int N = this->channel[0]->n;
	int M = this->channel[0]->m;
	Image* filtered;
	filtered = new Image(N, M);

	for (int c = 0; c < 3; c++)
	{
		int filterN = filter->n;
		int filterM = filter->m;

		int filterMidN = floor(filterN / 2);
		int filterMidM = floor(filterM / 2);

		int sx, sy;
		for (int x = 0; x < N; x++)
		{
			for (int y = 0; y < M; y++)
			{
				float sum = 0;

				for (int filterX = 0; filterX < filterN; filterX++)
				{
					for (int filterY = 0; filterY < filterM; filterY++)
					{

						sx = x - filterMidN + filterX;
						sy = y - filterMidM + filterY;

						if (sx >= 0 && sx < N && sy >= 0 && sy < M)
						{
							sum = sum + channel[c]->get(sx, sy) * filter->get(filterX, filterY);
						}


					}
				}

				filtered->channel[c]->set(x, y, sum);

			}
		}
	}
	return filtered;
}

void Image::exportImage(char* filePath)
{
	std::string fp(filePath);
	
	int N = this->channel[0]->n;
	int M = this->channel[0]->m;
	std::vector<uchar> buf;
	std::vector<float> f;
	float sum = 0.0;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
		{
			for (int c = 0; c < 3; c++)
			{
				float p = this->channel[c]->get(j, i);
				if (p < 0.0)
					p = 0.0;

				f.push_back(this->channel[c]->get(j, i));
				unsigned char b = (unsigned char)floor(p*(unsigned char)255);
				buf.push_back(b);
			}
		}
	}
	if (sum > 0.0)
	{
		int pp = 0;
	}
	std::vector<uchar> bufout;
	lodepng::encode(bufout, buf, N, M, LCT_RGB, 8U);
	lodepng::save_file(bufout, fp);
}

void Wavelet::createWaveletFilters()
{
	filterHH = new Matrix(13,13);
	filterGH = new Matrix(13,13);
	filterHG = new Matrix(13,13);
	filterGG = new Matrix(13,13);

	filterHH->set(0, 0, 1.81982762555e-006);
	filterHH->set(1, 0, -2.66358849668e-005);
	filterHH->set(2, 0, -3.7597206858e-005);
	filterHH->set(3, 0, 2.56157372229e-005);
	filterHH->set(4, 0, 0.000172891918843);
	filterHH->set(5, 0, 0.000335185948387);
	filterHH->set(6, 0, 0.000406449188081);
	filterHH->set(7, 0, 0.000335185948387);
	filterHH->set(8, 0, 0.000172891918843);
	filterHH->set(9, 0, 2.56157372229e-005);
	filterHH->set(10, 0, -3.7597206858e-005);
	filterHH->set(11, 0, -2.66358849668e-005);
	filterHH->set(12, 0, 1.81982762555e-006);
	filterHH->set(0, 1, -2.66358849668e-005);
	filterHH->set(1, 1, 0.000389855807224);
	filterHH->set(2, 1, 0.000550291062124);
	filterHH->set(3, 1, -0.000374924427145);
	filterHH->set(4, 1, -0.00253053047297);
	filterHH->set(5, 1, -0.00490594506776);
	filterHH->set(6, 1, -0.00594898861112);
	filterHH->set(7, 1, -0.00490594506776);
	filterHH->set(8, 1, -0.00253053047297);
	filterHH->set(9, 1, -0.000374924427145);
	filterHH->set(10, 1, 0.000550291062124);
	filterHH->set(11, 1, 0.000389855807224);
	filterHH->set(12, 1, -2.66358849668e-005);
	filterHH->set(0, 2, -3.7597206858e-005);
	filterHH->set(1, 2, 0.000550291062124);
	filterHH->set(2, 2, 0.000776749371029);
	filterHH->set(3, 2, -0.000529215051838);
	filterHH->set(4, 2, -0.00357190601218);
	filterHH->set(5, 2, -0.00692486214655);
	filterHH->set(6, 2, -0.00839714376627);
	filterHH->set(7, 2, -0.00692486214655);
	filterHH->set(8, 2, -0.00357190601218);
	filterHH->set(9, 2, -0.000529215051838);
	filterHH->set(10, 2, 0.000776749371029);
	filterHH->set(11, 2, 0.000550291062124);
	filterHH->set(12, 2, -3.7597206858e-005);
	filterHH->set(0, 3, 2.56157372229e-005);
	filterHH->set(1, 3, -0.000374924427145);
	filterHH->set(2, 3, -0.000529215051838);
	filterHH->set(3, 3, 0.000360564915194);
	filterHH->set(4, 3, 0.00243361178767);
	filterHH->set(5, 3, 0.00471804859655);
	filterHH->set(6, 3, 0.00572114383262);
	filterHH->set(7, 3, 0.00471804859655);
	filterHH->set(8, 3, 0.00243361178767);
	filterHH->set(9, 3, 0.000360564915194);
	filterHH->set(10, 3, -0.000529215051838);
	filterHH->set(11, 3, -0.000374924427145);
	filterHH->set(12, 3, 2.56157372229e-005);
	filterHH->set(0, 4, 0.000172891918843);
	filterHH->set(1, 4, -0.00253053047297);
	filterHH->set(2, 4, -0.00357190601218);
	filterHH->set(3, 4, 0.00243361178767);
	filterHH->set(4, 4, 0.0164255202975);
	filterHH->set(5, 4, 0.0318441928083);
	filterHH->set(6, 4, 0.0386145253831);
	filterHH->set(7, 4, 0.0318441928083);
	filterHH->set(8, 4, 0.0164255202975);
	filterHH->set(9, 4, 0.00243361178767);
	filterHH->set(10, 4, -0.00357190601218);
	filterHH->set(11, 4, -0.00253053047297);
	filterHH->set(12, 4, 0.000172891918843);
	filterHH->set(0, 5, 0.000335185948387);
	filterHH->set(1, 5, -0.00490594506776);
	filterHH->set(2, 5, -0.00692486214655);
	filterHH->set(3, 5, 0.00471804859655);
	filterHH->set(4, 5, 0.0318441928083);
	filterHH->set(5, 5, 0.0617364075689);
	filterHH->set(6, 5, 0.0748620664206);
	filterHH->set(7, 5, 0.0617364075689);
	filterHH->set(8, 5, 0.0318441928083);
	filterHH->set(9, 5, 0.00471804859655);
	filterHH->set(10, 5, -0.00692486214655);
	filterHH->set(11, 5, -0.00490594506776);
	filterHH->set(12, 5, 0.000335185948387);
	filterHH->set(0, 6, 0.000406449188081);
	filterHH->set(1, 6, -0.00594898861112);
	filterHH->set(2, 6, -0.00839714376627);
	filterHH->set(3, 6, 0.00572114383262);
	filterHH->set(4, 6, 0.0386145253831);
	filterHH->set(5, 6, 0.0748620664206);
	filterHH->set(6, 6, 0.0907783463512);
	filterHH->set(7, 6, 0.0748620664206);
	filterHH->set(8, 6, 0.0386145253831);
	filterHH->set(9, 6, 0.00572114383262);
	filterHH->set(10, 6, -0.00839714376627);
	filterHH->set(11, 6, -0.00594898861112);
	filterHH->set(12, 6, 0.000406449188081);
	filterHH->set(0, 7, 0.000335185948387);
	filterHH->set(1, 7, -0.00490594506776);
	filterHH->set(2, 7, -0.00692486214655);
	filterHH->set(3, 7, 0.00471804859655);
	filterHH->set(4, 7, 0.0318441928083);
	filterHH->set(5, 7, 0.0617364075689);
	filterHH->set(6, 7, 0.0748620664206);
	filterHH->set(7, 7, 0.0617364075689);
	filterHH->set(8, 7, 0.0318441928083);
	filterHH->set(9, 7, 0.00471804859655);
	filterHH->set(10, 7, -0.00692486214655);
	filterHH->set(11, 7, -0.00490594506776);
	filterHH->set(12, 7, 0.000335185948387);
	filterHH->set(0, 8, 0.000172891918843);
	filterHH->set(1, 8, -0.00253053047297);
	filterHH->set(2, 8, -0.00357190601218);
	filterHH->set(3, 8, 0.00243361178767);
	filterHH->set(4, 8, 0.0164255202975);
	filterHH->set(5, 8, 0.0318441928083);
	filterHH->set(6, 8, 0.0386145253831);
	filterHH->set(7, 8, 0.0318441928083);
	filterHH->set(8, 8, 0.0164255202975);
	filterHH->set(9, 8, 0.00243361178767);
	filterHH->set(10, 8, -0.00357190601218);
	filterHH->set(11, 8, -0.00253053047297);
	filterHH->set(12, 8, 0.000172891918843);
	filterHH->set(0, 9, 2.56157372229e-005);
	filterHH->set(1, 9, -0.000374924427145);
	filterHH->set(2, 9, -0.000529215051838);
	filterHH->set(3, 9, 0.000360564915194);
	filterHH->set(4, 9, 0.00243361178767);
	filterHH->set(5, 9, 0.00471804859655);
	filterHH->set(6, 9, 0.00572114383262);
	filterHH->set(7, 9, 0.00471804859655);
	filterHH->set(8, 9, 0.00243361178767);
	filterHH->set(9, 9, 0.000360564915194);
	filterHH->set(10, 9, -0.000529215051838);
	filterHH->set(11, 9, -0.000374924427145);
	filterHH->set(12, 9, 2.56157372229e-005);
	filterHH->set(0, 10, -3.7597206858e-005);
	filterHH->set(1, 10, 0.000550291062124);
	filterHH->set(2, 10, 0.000776749371029);
	filterHH->set(3, 10, -0.000529215051838);
	filterHH->set(4, 10, -0.00357190601218);
	filterHH->set(5, 10, -0.00692486214655);
	filterHH->set(6, 10, -0.00839714376627);
	filterHH->set(7, 10, -0.00692486214655);
	filterHH->set(8, 10, -0.00357190601218);
	filterHH->set(9, 10, -0.000529215051838);
	filterHH->set(10, 10, 0.000776749371029);
	filterHH->set(11, 10, 0.000550291062124);
	filterHH->set(12, 10, -3.7597206858e-005);
	filterHH->set(0, 11, -2.66358849668e-005);
	filterHH->set(1, 11, 0.000389855807224);
	filterHH->set(2, 11, 0.000550291062124);
	filterHH->set(3, 11, -0.000374924427145);
	filterHH->set(4, 11, -0.00253053047297);
	filterHH->set(5, 11, -0.00490594506776);
	filterHH->set(6, 11, -0.00594898861112);
	filterHH->set(7, 11, -0.00490594506776);
	filterHH->set(8, 11, -0.00253053047297);
	filterHH->set(9, 11, -0.000374924427145);
	filterHH->set(10, 11, 0.000550291062124);
	filterHH->set(11, 11, 0.000389855807224);
	filterHH->set(12, 11, -2.66358849668e-005);
	filterHH->set(0, 12, 1.81982762555e-006);
	filterHH->set(1, 12, -2.66358849668e-005);
	filterHH->set(2, 12, -3.7597206858e-005);
	filterHH->set(3, 12, 2.56157372229e-005);
	filterHH->set(4, 12, 0.000172891918843);
	filterHH->set(5, 12, 0.000335185948387);
	filterHH->set(6, 12, 0.000406449188081);
	filterHH->set(7, 12, 0.000335185948387);
	filterHH->set(8, 12, 0.000172891918843);
	filterHH->set(9, 12, 2.56157372229e-005);
	filterHH->set(10, 12, -3.7597206858e-005);
	filterHH->set(11, 12, -2.66358849668e-005);
	filterHH->set(12, 12, 1.81982762555e-006);


	filterHG->set(0, 0, 3.93616527977e-006);
	filterHG->set(1, 0, -5.50727074122e-006);
	filterHG->set(2, 0, -3.53214456274e-005);
	filterHG->set(3, 0, -9.36663408763e-005);
	filterHG->set(4, 0, -0.000171144089393);
	filterHG->set(5, 0, -0.000239413341243);
	filterHG->set(6, 0, 0.0010822326452);
	filterHG->set(7, 0, -0.000239413341243);
	filterHG->set(8, 0, -0.000171144089393);
	filterHG->set(9, 0, -9.36663408763e-005);
	filterHG->set(10, 0, -3.53214456274e-005);
	filterHG->set(11, 0, -5.50727074122e-006);
	filterHG->set(12, 0, 3.93616527977e-006);
	filterHG->set(0, 1, -5.76116353717e-005);
	filterHG->set(1, 1, 8.06071014008e-005);
	filterHG->set(2, 1, 0.000516981910477);
	filterHG->set(3, 1, 0.00137094626206);
	filterHG->set(4, 1, 0.00250494838843);
	filterHG->set(5, 1, 0.00350417046502);
	filterHG->set(6, 1, -0.015840084984);
	filterHG->set(7, 1, 0.00350417046502);
	filterHG->set(8, 1, 0.00250494838843);
	filterHG->set(9, 1, 0.00137094626206);
	filterHG->set(10, 1, 0.000516981910477);
	filterHG->set(11, 1, 8.06071014008e-005);
	filterHG->set(12, 1, -5.76116353717e-005);
	filterHG->set(0, 2, -8.13202405399e-005);
	filterHG->set(1, 2, 0.000113778906515);
	filterHG->set(2, 2, 0.000729732684093);
	filterHG->set(3, 2, 0.00193512437339);
	filterHG->set(4, 2, 0.00353579627054);
	filterHG->set(5, 2, 0.00494622281194);
	filterHG->set(6, 2, -0.0223586696119);
	filterHG->set(7, 2, 0.00494622281194);
	filterHG->set(8, 2, 0.00353579627054);
	filterHG->set(9, 2, 0.00193512437339);
	filterHG->set(10, 2, 0.000729732684093);
	filterHG->set(11, 2, 0.000113778906515);
	filterHG->set(12, 2, -8.13202405399e-005);
	filterHG->set(0, 3, 5.54051241211e-005);
	filterHG->set(1, 3, -7.75198695427e-005);
	filterHG->set(2, 3, -0.000497181632382);
	filterHG->set(3, 3, -0.00131843936252);
	filterHG->set(4, 3, -0.00240900949057);
	filterHG->set(5, 3, -0.00336996161111);
	filterHG->set(6, 3, 0.015233413684);
	filterHG->set(7, 3, -0.00336996161111);
	filterHG->set(8, 3, -0.00240900949057);
	filterHG->set(9, 3, -0.00131843936252);
	filterHG->set(10, 3, -0.000497181632382);
	filterHG->set(11, 3, -7.75198695427e-005);
	filterHG->set(12, 3, 5.54051241211e-005);
	filterHG->set(0, 4, 0.000373953641845);
	filterHG->set(1, 4, -0.000523215821473);
	filterHG->set(2, 4, -0.0033556983228);
	filterHG->set(3, 4, -0.008898729296);
	filterHG->set(4, 4, -0.0162594685334);
	filterHG->set(5, 4, -0.022745358617);
	filterHG->set(6, 4, 0.102817033898);
	filterHG->set(7, 4, -0.022745358617);
	filterHG->set(8, 4, -0.0162594685334);
	filterHG->set(9, 4, -0.008898729296);
	filterHG->set(10, 4, -0.0033556983228);
	filterHG->set(11, 4, -0.000523215821473);
	filterHG->set(12, 4, 0.000373953641845);
	filterHG->set(0, 5, 0.000724984758877);
	filterHG->set(1, 5, -0.00101435967918);
	filterHG->set(2, 5, -0.00650569981728);
	filterHG->set(3, 5, -0.0172519863188);
	filterHG->set(4, 5, -0.0315222678834);
	filterHG->set(5, 5, -0.0440964774435);
	filterHG->set(6, 5, 0.199331612766);
	filterHG->set(7, 5, -0.0440964774435);
	filterHG->set(8, 5, -0.0315222678834);
	filterHG->set(9, 5, -0.0172519863188);
	filterHG->set(10, 5, -0.00650569981728);
	filterHG->set(11, 5, -0.00101435967918);
	filterHG->set(12, 5, 0.000724984758877);
	filterHG->set(0, 6, 0.000879122373819);
	filterHG->set(1, 6, -0.00123002073927);
	filterHG->set(2, 6, -0.00788886414051);
	filterHG->set(3, 6, -0.0209198979426);
	filterHG->set(4, 6, -0.0382241566191);
	filterHG->set(5, 6, -0.053471744685);
	filterHG->set(6, 6, 0.241711123505);
	filterHG->set(7, 6, -0.053471744685);
	filterHG->set(8, 6, -0.0382241566191);
	filterHG->set(9, 6, -0.0209198979426);
	filterHG->set(10, 6, -0.00788886414051);
	filterHG->set(11, 6, -0.00123002073927);
	filterHG->set(12, 6, 0.000879122373819);
	filterHG->set(0, 7, 0.000724984758877);
	filterHG->set(1, 7, -0.00101435967918);
	filterHG->set(2, 7, -0.00650569981728);
	filterHG->set(3, 7, -0.0172519863188);
	filterHG->set(4, 7, -0.0315222678834);
	filterHG->set(5, 7, -0.0440964774435);
	filterHG->set(6, 7, 0.199331612766);
	filterHG->set(7, 7, -0.0440964774435);
	filterHG->set(8, 7, -0.0315222678834);
	filterHG->set(9, 7, -0.0172519863188);
	filterHG->set(10, 7, -0.00650569981728);
	filterHG->set(11, 7, -0.00101435967918);
	filterHG->set(12, 7, 0.000724984758877);
	filterHG->set(0, 8, 0.000373953641845);
	filterHG->set(1, 8, -0.000523215821473);
	filterHG->set(2, 8, -0.0033556983228);
	filterHG->set(3, 8, -0.008898729296);
	filterHG->set(4, 8, -0.0162594685334);
	filterHG->set(5, 8, -0.022745358617);
	filterHG->set(6, 8, 0.102817033898);
	filterHG->set(7, 8, -0.022745358617);
	filterHG->set(8, 8, -0.0162594685334);
	filterHG->set(9, 8, -0.008898729296);
	filterHG->set(10, 8, -0.0033556983228);
	filterHG->set(11, 8, -0.000523215821473);
	filterHG->set(12, 8, 0.000373953641845);
	filterHG->set(0, 9, 5.54051241211e-005);
	filterHG->set(1, 9, -7.75198695427e-005);
	filterHG->set(2, 9, -0.000497181632382);
	filterHG->set(3, 9, -0.00131843936252);
	filterHG->set(4, 9, -0.00240900949057);
	filterHG->set(5, 9, -0.00336996161111);
	filterHG->set(6, 9, 0.015233413684);
	filterHG->set(7, 9, -0.00336996161111);
	filterHG->set(8, 9, -0.00240900949057);
	filterHG->set(9, 9, -0.00131843936252);
	filterHG->set(10, 9, -0.000497181632382);
	filterHG->set(11, 9, -7.75198695427e-005);
	filterHG->set(12, 9, 5.54051241211e-005);
	filterHG->set(0, 10, -8.13202405399e-005);
	filterHG->set(1, 10, 0.000113778906515);
	filterHG->set(2, 10, 0.000729732684093);
	filterHG->set(3, 10, 0.00193512437339);
	filterHG->set(4, 10, 0.00353579627054);
	filterHG->set(5, 10, 0.00494622281194);
	filterHG->set(6, 10, -0.0223586696119);
	filterHG->set(7, 10, 0.00494622281194);
	filterHG->set(8, 10, 0.00353579627054);
	filterHG->set(9, 10, 0.00193512437339);
	filterHG->set(10, 10, 0.000729732684093);
	filterHG->set(11, 10, 0.000113778906515);
	filterHG->set(12, 10, -8.13202405399e-005);
	filterHG->set(0, 11, -5.76116353717e-005);
	filterHG->set(1, 11, 8.06071014008e-005);
	filterHG->set(2, 11, 0.000516981910477);
	filterHG->set(3, 11, 0.00137094626206);
	filterHG->set(4, 11, 0.00250494838843);
	filterHG->set(5, 11, 0.00350417046502);
	filterHG->set(6, 11, -0.015840084984);
	filterHG->set(7, 11, 0.00350417046502);
	filterHG->set(8, 11, 0.00250494838843);
	filterHG->set(9, 11, 0.00137094626206);
	filterHG->set(10, 11, 0.000516981910477);
	filterHG->set(11, 11, 8.06071014008e-005);
	filterHG->set(12, 11, -5.76116353717e-005);
	filterHG->set(0, 12, 3.93616527977e-006);
	filterHG->set(1, 12, -5.50727074122e-006);
	filterHG->set(2, 12, -3.53214456274e-005);
	filterHG->set(3, 12, -9.36663408763e-005);
	filterHG->set(4, 12, -0.000171144089393);
	filterHG->set(5, 12, -0.000239413341243);
	filterHG->set(6, 12, 0.0010822326452);
	filterHG->set(7, 12, -0.000239413341243);
	filterHG->set(8, 12, -0.000171144089393);
	filterHG->set(9, 12, -9.36663408763e-005);
	filterHG->set(10, 12, -3.53214456274e-005);
	filterHG->set(11, 12, -5.50727074122e-006);
	filterHG->set(12, 12, 3.93616527977e-006);



	filterGH->set(0, 0, 3.93616527977e-006);
	filterGH->set(1, 0, -5.76116353717e-005);
	filterGH->set(2, 0, -8.13202405399e-005);
	filterGH->set(3, 0, 5.54051241211e-005);
	filterGH->set(4, 0, 0.000373953641845);
	filterGH->set(5, 0, 0.000724984758877);
	filterGH->set(6, 0, 0.000879122373819);
	filterGH->set(7, 0, 0.000724984758877);
	filterGH->set(8, 0, 0.000373953641845);
	filterGH->set(9, 0, 5.54051241211e-005);
	filterGH->set(10, 0, -8.13202405399e-005);
	filterGH->set(11, 0, -5.76116353717e-005);
	filterGH->set(12, 0, 3.93616527977e-006);
	filterGH->set(0, 1, -5.50727074122e-006);
	filterGH->set(1, 1, 8.06071014008e-005);
	filterGH->set(2, 1, 0.000113778906515);
	filterGH->set(3, 1, -7.75198695427e-005);
	filterGH->set(4, 1, -0.000523215821473);
	filterGH->set(5, 1, -0.00101435967918);
	filterGH->set(6, 1, -0.00123002073927);
	filterGH->set(7, 1, -0.00101435967918);
	filterGH->set(8, 1, -0.000523215821473);
	filterGH->set(9, 1, -7.75198695427e-005);
	filterGH->set(10, 1, 0.000113778906515);
	filterGH->set(11, 1, 8.06071014008e-005);
	filterGH->set(12, 1, -5.50727074122e-006);
	filterGH->set(0, 2, -3.53214456274e-005);
	filterGH->set(1, 2, 0.000516981910477);
	filterGH->set(2, 2, 0.000729732684093);
	filterGH->set(3, 2, -0.000497181632382);
	filterGH->set(4, 2, -0.0033556983228);
	filterGH->set(5, 2, -0.00650569981728);
	filterGH->set(6, 2, -0.00788886414051);
	filterGH->set(7, 2, -0.00650569981728);
	filterGH->set(8, 2, -0.0033556983228);
	filterGH->set(9, 2, -0.000497181632382);
	filterGH->set(10, 2, 0.000729732684093);
	filterGH->set(11, 2, 0.000516981910477);
	filterGH->set(12, 2, -3.53214456274e-005);
	filterGH->set(0, 3, -9.36663408763e-005);
	filterGH->set(1, 3, 0.00137094626206);
	filterGH->set(2, 3, 0.00193512437339);
	filterGH->set(3, 3, -0.00131843936252);
	filterGH->set(4, 3, -0.008898729296);
	filterGH->set(5, 3, -0.0172519863188);
	filterGH->set(6, 3, -0.0209198979426);
	filterGH->set(7, 3, -0.0172519863188);
	filterGH->set(8, 3, -0.008898729296);
	filterGH->set(9, 3, -0.00131843936252);
	filterGH->set(10, 3, 0.00193512437339);
	filterGH->set(11, 3, 0.00137094626206);
	filterGH->set(12, 3, -9.36663408763e-005);
	filterGH->set(0, 4, -0.000171144089393);
	filterGH->set(1, 4, 0.00250494838843);
	filterGH->set(2, 4, 0.00353579627054);
	filterGH->set(3, 4, -0.00240900949057);
	filterGH->set(4, 4, -0.0162594685334);
	filterGH->set(5, 4, -0.0315222678834);
	filterGH->set(6, 4, -0.0382241566191);
	filterGH->set(7, 4, -0.0315222678834);
	filterGH->set(8, 4, -0.0162594685334);
	filterGH->set(9, 4, -0.00240900949057);
	filterGH->set(10, 4, 0.00353579627054);
	filterGH->set(11, 4, 0.00250494838843);
	filterGH->set(12, 4, -0.000171144089393);
	filterGH->set(0, 5, -0.000239413341243);
	filterGH->set(1, 5, 0.00350417046502);
	filterGH->set(2, 5, 0.00494622281194);
	filterGH->set(3, 5, -0.00336996161111);
	filterGH->set(4, 5, -0.022745358617);
	filterGH->set(5, 5, -0.0440964774435);
	filterGH->set(6, 5, -0.053471744685);
	filterGH->set(7, 5, -0.0440964774435);
	filterGH->set(8, 5, -0.022745358617);
	filterGH->set(9, 5, -0.00336996161111);
	filterGH->set(10, 5, 0.00494622281194);
	filterGH->set(11, 5, 0.00350417046502);
	filterGH->set(12, 5, -0.000239413341243);
	filterGH->set(0, 6, 0.0010822326452);
	filterGH->set(1, 6, -0.015840084984);
	filterGH->set(2, 6, -0.0223586696119);
	filterGH->set(3, 6, 0.015233413684);
	filterGH->set(4, 6, 0.102817033898);
	filterGH->set(5, 6, 0.199331612766);
	filterGH->set(6, 6, 0.241711123505);
	filterGH->set(7, 6, 0.199331612766);
	filterGH->set(8, 6, 0.102817033898);
	filterGH->set(9, 6, 0.015233413684);
	filterGH->set(10, 6, -0.0223586696119);
	filterGH->set(11, 6, -0.015840084984);
	filterGH->set(12, 6, 0.0010822326452);
	filterGH->set(0, 7, -0.000239413341243);
	filterGH->set(1, 7, 0.00350417046502);
	filterGH->set(2, 7, 0.00494622281194);
	filterGH->set(3, 7, -0.00336996161111);
	filterGH->set(4, 7, -0.022745358617);
	filterGH->set(5, 7, -0.0440964774435);
	filterGH->set(6, 7, -0.053471744685);
	filterGH->set(7, 7, -0.0440964774435);
	filterGH->set(8, 7, -0.022745358617);
	filterGH->set(9, 7, -0.00336996161111);
	filterGH->set(10, 7, 0.00494622281194);
	filterGH->set(11, 7, 0.00350417046502);
	filterGH->set(12, 7, -0.000239413341243);
	filterGH->set(0, 8, -0.000171144089393);
	filterGH->set(1, 8, 0.00250494838843);
	filterGH->set(2, 8, 0.00353579627054);
	filterGH->set(3, 8, -0.00240900949057);
	filterGH->set(4, 8, -0.0162594685334);
	filterGH->set(5, 8, -0.0315222678834);
	filterGH->set(6, 8, -0.0382241566191);
	filterGH->set(7, 8, -0.0315222678834);
	filterGH->set(8, 8, -0.0162594685334);
	filterGH->set(9, 8, -0.00240900949057);
	filterGH->set(10, 8, 0.00353579627054);
	filterGH->set(11, 8, 0.00250494838843);
	filterGH->set(12, 8, -0.000171144089393);
	filterGH->set(0, 9, -9.36663408763e-005);
	filterGH->set(1, 9, 0.00137094626206);
	filterGH->set(2, 9, 0.00193512437339);
	filterGH->set(3, 9, -0.00131843936252);
	filterGH->set(4, 9, -0.008898729296);
	filterGH->set(5, 9, -0.0172519863188);
	filterGH->set(6, 9, -0.0209198979426);
	filterGH->set(7, 9, -0.0172519863188);
	filterGH->set(8, 9, -0.008898729296);
	filterGH->set(9, 9, -0.00131843936252);
	filterGH->set(10, 9, 0.00193512437339);
	filterGH->set(11, 9, 0.00137094626206);
	filterGH->set(12, 9, -9.36663408763e-005);
	filterGH->set(0, 10, -3.53214456274e-005);
	filterGH->set(1, 10, 0.000516981910477);
	filterGH->set(2, 10, 0.000729732684093);
	filterGH->set(3, 10, -0.000497181632382);
	filterGH->set(4, 10, -0.0033556983228);
	filterGH->set(5, 10, -0.00650569981728);
	filterGH->set(6, 10, -0.00788886414051);
	filterGH->set(7, 10, -0.00650569981728);
	filterGH->set(8, 10, -0.0033556983228);
	filterGH->set(9, 10, -0.000497181632382);
	filterGH->set(10, 10, 0.000729732684093);
	filterGH->set(11, 10, 0.000516981910477);
	filterGH->set(12, 10, -3.53214456274e-005);
	filterGH->set(0, 11, -5.50727074122e-006);
	filterGH->set(1, 11, 8.06071014008e-005);
	filterGH->set(2, 11, 0.000113778906515);
	filterGH->set(3, 11, -7.75198695427e-005);
	filterGH->set(4, 11, -0.000523215821473);
	filterGH->set(5, 11, -0.00101435967918);
	filterGH->set(6, 11, -0.00123002073927);
	filterGH->set(7, 11, -0.00101435967918);
	filterGH->set(8, 11, -0.000523215821473);
	filterGH->set(9, 11, -7.75198695427e-005);
	filterGH->set(10, 11, 0.000113778906515);
	filterGH->set(11, 11, 8.06071014008e-005);
	filterGH->set(12, 11, -5.50727074122e-006);
	filterGH->set(0, 12, 3.93616527977e-006);
	filterGH->set(1, 12, -5.76116353717e-005);
	filterGH->set(2, 12, -8.13202405399e-005);
	filterGH->set(3, 12, 5.54051241211e-005);
	filterGH->set(4, 12, 0.000373953641845);
	filterGH->set(5, 12, 0.000724984758877);
	filterGH->set(6, 12, 0.000879122373819);
	filterGH->set(7, 12, 0.000724984758877);
	filterGH->set(8, 12, 0.000373953641845);
	filterGH->set(9, 12, 5.54051241211e-005);
	filterGH->set(10, 12, -8.13202405399e-005);
	filterGH->set(11, 12, -5.76116353717e-005);
	filterGH->set(12, 12, 3.93616527977e-006);


	filterGG->set(0, 0, 8.51366189421e-006);
	filterGG->set(1, 0, -1.191185779e-005);
	filterGG->set(2, 0, -7.63979214064e-005);
	filterGG->set(3, 0, -0.000202594022458);
	filterGG->set(4, 0, -0.000370173203797);
	filterGG->set(5, 0, -0.00051783502354);
	filterGG->set(6, 0, 0.00234079673419);
	filterGG->set(7, 0, -0.00051783502354);
	filterGG->set(8, 0, -0.000370173203797);
	filterGG->set(9, 0, -0.000202594022458);
	filterGG->set(10, 0, -7.63979214064e-005);
	filterGG->set(11, 0, -1.191185779e-005);
	filterGG->set(12, 0, 8.51366189421e-006);
	filterGG->set(0, 1, -1.191185779e-005);
	filterGG->set(1, 1, 1.66664307055e-005);
	filterGG->set(2, 1, 0.000106891862345);
	filterGG->set(3, 1, 0.000283458659108);
	filterGG->set(4, 1, 0.000517926435898);
	filterGG->set(5, 1, 0.000724526911657);
	filterGG->set(6, 1, -0.00327511688385);
	filterGG->set(7, 1, 0.000724526911657);
	filterGG->set(8, 1, 0.000517926435898);
	filterGG->set(9, 1, 0.000283458659108);
	filterGG->set(10, 1, 0.000106891862345);
	filterGG->set(11, 1, 1.66664307055e-005);
	filterGG->set(12, 1, -1.191185779e-005);
	filterGG->set(0, 2, -7.63979214064e-005);
	filterGG->set(1, 2, 0.000106891862345);
	filterGG->set(2, 2, 0.000685561920093);
	filterGG->set(3, 2, 0.00181799117671);
	filterGG->set(4, 2, 0.00332177430603);
	filterGG->set(5, 2, 0.00464682764261);
	filterGG->set(6, 2, -0.0210052979728);
	filterGG->set(7, 2, 0.00464682764261);
	filterGG->set(8, 2, 0.00332177430603);
	filterGG->set(9, 2, 0.00181799117671);
	filterGG->set(10, 2, 0.000685561920093);
	filterGG->set(11, 2, 0.000106891862345);
	filterGG->set(12, 2, -7.63979214064e-005);
	filterGG->set(0, 3, -0.000202594022458);
	filterGG->set(1, 3, 0.000283458659108);
	filterGG->set(2, 3, 0.00181799117671);
	filterGG->set(3, 3, 0.00482099693948);
	filterGG->set(4, 3, 0.00880876869382);
	filterGG->set(5, 3, 0.012322580071);
	filterGG->set(6, 3, -0.0557024030353);
	filterGG->set(7, 3, 0.012322580071);
	filterGG->set(8, 3, 0.00880876869382);
	filterGG->set(9, 3, 0.00482099693948);
	filterGG->set(10, 3, 0.00181799117671);
	filterGG->set(11, 3, 0.000283458659108);
	filterGG->set(12, 3, -0.000202594022458);
	filterGG->set(0, 4, -0.000370173203797);
	filterGG->set(1, 4, 0.000517926435898);
	filterGG->set(2, 4, 0.00332177430603);
	filterGG->set(3, 4, 0.00880876869382);
	filterGG->set(4, 4, 0.0160950954492);
	filterGG->set(5, 4, 0.0225154172299);
	filterGG->set(6, 4, -0.101777617822);
	filterGG->set(7, 4, 0.0225154172299);
	filterGG->set(8, 4, 0.0160950954492);
	filterGG->set(9, 4, 0.00880876869382);
	filterGG->set(10, 4, 0.00332177430603);
	filterGG->set(11, 4, 0.000517926435898);
	filterGG->set(12, 4, -0.000370173203797);
	filterGG->set(0, 5, -0.00051783502354);
	filterGG->set(1, 5, 0.000724526911657);
	filterGG->set(2, 5, 0.00464682764261);
	filterGG->set(3, 5, 0.012322580071);
	filterGG->set(4, 5, 0.0225154172299);
	filterGG->set(5, 5, 0.0314968006643);
	filterGG->set(6, 5, -0.142376634992);
	filterGG->set(7, 5, 0.0314968006643);
	filterGG->set(8, 5, 0.0225154172299);
	filterGG->set(9, 5, 0.012322580071);
	filterGG->set(10, 5, 0.00464682764261);
	filterGG->set(11, 5, 0.000724526911657);
	filterGG->set(12, 5, -0.00051783502354);
	filterGG->set(0, 6, 0.00234079673419);
	filterGG->set(1, 6, -0.00327511688385);
	filterGG->set(2, 6, -0.0210052979728);
	filterGG->set(3, 6, -0.0557024030353);
	filterGG->set(4, 6, -0.101777617822);
	filterGG->set(5, 6, -0.142376634992);
	filterGG->set(6, 6, 0.643592547943);
	filterGG->set(7, 6, -0.142376634992);
	filterGG->set(8, 6, -0.101777617822);
	filterGG->set(9, 6, -0.0557024030353);
	filterGG->set(10, 6, -0.0210052979728);
	filterGG->set(11, 6, -0.00327511688385);
	filterGG->set(12, 6, 0.00234079673419);
	filterGG->set(0, 7, -0.00051783502354);
	filterGG->set(1, 7, 0.000724526911657);
	filterGG->set(2, 7, 0.00464682764261);
	filterGG->set(3, 7, 0.012322580071);
	filterGG->set(4, 7, 0.0225154172299);
	filterGG->set(5, 7, 0.0314968006643);
	filterGG->set(6, 7, -0.142376634992);
	filterGG->set(7, 7, 0.0314968006643);
	filterGG->set(8, 7, 0.0225154172299);
	filterGG->set(9, 7, 0.012322580071);
	filterGG->set(10, 7, 0.00464682764261);
	filterGG->set(11, 7, 0.000724526911657);
	filterGG->set(12, 7, -0.00051783502354);
	filterGG->set(0, 8, -0.000370173203797);
	filterGG->set(1, 8, 0.000517926435898);
	filterGG->set(2, 8, 0.00332177430603);
	filterGG->set(3, 8, 0.00880876869382);
	filterGG->set(4, 8, 0.0160950954492);
	filterGG->set(5, 8, 0.0225154172299);
	filterGG->set(6, 8, -0.101777617822);
	filterGG->set(7, 8, 0.0225154172299);
	filterGG->set(8, 8, 0.0160950954492);
	filterGG->set(9, 8, 0.00880876869382);
	filterGG->set(10, 8, 0.00332177430603);
	filterGG->set(11, 8, 0.000517926435898);
	filterGG->set(12, 8, -0.000370173203797);
	filterGG->set(0, 9, -0.000202594022458);
	filterGG->set(1, 9, 0.000283458659108);
	filterGG->set(2, 9, 0.00181799117671);
	filterGG->set(3, 9, 0.00482099693948);
	filterGG->set(4, 9, 0.00880876869382);
	filterGG->set(5, 9, 0.012322580071);
	filterGG->set(6, 9, -0.0557024030353);
	filterGG->set(7, 9, 0.012322580071);
	filterGG->set(8, 9, 0.00880876869382);
	filterGG->set(9, 9, 0.00482099693948);
	filterGG->set(10, 9, 0.00181799117671);
	filterGG->set(11, 9, 0.000283458659108);
	filterGG->set(12, 9, -0.000202594022458);
	filterGG->set(0, 10, -7.63979214064e-005);
	filterGG->set(1, 10, 0.000106891862345);
	filterGG->set(2, 10, 0.000685561920093);
	filterGG->set(3, 10, 0.00181799117671);
	filterGG->set(4, 10, 0.00332177430603);
	filterGG->set(5, 10, 0.00464682764261);
	filterGG->set(6, 10, -0.0210052979728);
	filterGG->set(7, 10, 0.00464682764261);
	filterGG->set(8, 10, 0.00332177430603);
	filterGG->set(9, 10, 0.00181799117671);
	filterGG->set(10, 10, 0.000685561920093);
	filterGG->set(11, 10, 0.000106891862345);
	filterGG->set(12, 10, -7.63979214064e-005);
	filterGG->set(0, 11, -1.191185779e-005);
	filterGG->set(1, 11, 1.66664307055e-005);
	filterGG->set(2, 11, 0.000106891862345);
	filterGG->set(3, 11, 0.000283458659108);
	filterGG->set(4, 11, 0.000517926435898);
	filterGG->set(5, 11, 0.000724526911657);
	filterGG->set(6, 11, -0.00327511688385);
	filterGG->set(7, 11, 0.000724526911657);
	filterGG->set(8, 11, 0.000517926435898);
	filterGG->set(9, 11, 0.000283458659108);
	filterGG->set(10, 11, 0.000106891862345);
	filterGG->set(11, 11, 1.66664307055e-005);
	filterGG->set(12, 11, -1.191185779e-005);
	filterGG->set(0, 12, 8.51366189421e-006);
	filterGG->set(1, 12, -1.191185779e-005);
	filterGG->set(2, 12, -7.63979214064e-005);
	filterGG->set(3, 12, -0.000202594022458);
	filterGG->set(4, 12, -0.000370173203797);
	filterGG->set(5, 12, -0.00051783502354);
	filterGG->set(6, 12, 0.00234079673419);
	filterGG->set(7, 12, -0.00051783502354);
	filterGG->set(8, 12, -0.000370173203797);
	filterGG->set(9, 12, -0.000202594022458);
	filterGG->set(10, 12, -7.63979214064e-005);
	filterGG->set(11, 12, -1.191185779e-005);
	filterGG->set(12, 12, 8.51366189421e-006);

}

Wavelet::Wavelet(Image* im, int levels)
{
	createWaveletFilters();
	
	highBand[0] = im->conv(filterGH);

	highBand[1] = im->conv(filterHG);

	highBand[2] = im->conv(filterGG);

	Image* lb = im->conv(filterHH);

	lowBand = lb->downsample(2);
	delete lb;

	
	if (levels > 1)
	{
		next = new Wavelet(lowBand, levels - 1);
	}
}

Image* Image::downsample(int rate)
{
	int N = channel[0]->n;
	int M = channel[0]->m;
	Image* down = new Image(floor(N/2), floor(M/2));
	for (int x = 0,xDown = 0; x < N; x=x+2,xDown++)
	{
		for (int y = 0, yDown = 0; y < M; y=y+2,yDown++)
		{
			for (int c = 0; c < 3; c++)
			{
				down->channel[c]->set(yDown, xDown, this->channel[c]->get(y, x));
			}
		}
	}
	return down;
}

Wavelet::~Wavelet()
{
	delete highBand[0];
	delete highBand[1];
	delete highBand[2];
	delete lowBand;
	delete next;
	delete filterHH;
	delete filterGH;
	delete filterHG;
	delete filterGG;
}

void Wavelet::waveletTreshold(float t)
{
	for (int i = 0; i < 3; i++)
	{
		for (int c = 0; c < 3; c++)
		{
			int N = highBand[i]->channel[c]->n;
			int M = highBand[i]->channel[c]->m;
			Matrix* im = highBand[i]->channel[c];
			for (int x = 0; x < N; x++)
			{
				for (int y = 0; y < M; y++)
				{
					if (abs(im->get(x, y)) < t)
					{
						im->set(x, y, 0);
					}
				}
			}
			
		}
	}
}