#include "graphcut.h"
#include "util.h"
#include "itkImageIO.h"
#include "dataIO.h"

double calc_likelihood(double value, double ave, double var) {
	constexpr double PI = 3.14159265357;
	constexpr double EPS = std::numeric_limits<double>::epsilon();
	return -(std::log(1 / std::sqrt(2.0 * PI * std::max(EPS, var))) + (-std::pow(value - ave, 2.0) / 2.0 / std::max(EPS, var)));
}

int main(int argc, char* argv[]) {

	// �R�}���h���C�������̃`�F�b�N
	if (argc < 2) {
		std::cerr << "�R�}���h���C������������܂���" << std::endl;
		std::cerr << "�g����: .exe [�摜�t�@�C����] [options]" << std::endl;
		std::cerr << "�ڍׂ�--help(or -h)�R�}���h�Ŋm�F�\" << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	std::string filename = argv[1];

	std::string root = "../data";
	std::string inputFolder = "input";
	std::string outputFolder = "result";
	std::string seedFolder = "seed";
	std::string maskFolder = "none";

	double sigma = 0.1;
	double lambda = 1;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if ((arg == "-h") || (arg == "--help")) {
			std::cout << "�g����: .exe [�摜�t�@�C����] [options]" << std::endl;
			std::cout << "<options>" << std::endl;
			std::cout << "--root (-r)	:�f�[�^������t�H���_��(�f�t�H���g [exe�̂���t�H���_��1��]/data)" << std::endl;
			std::cout << "--input (-i)	:���͉摜������t�H���_��(�f�t�H���g input)" << std::endl;
			std::cout << "--output (-o)	:�o�̓t�H���_(�f�t�H���g result)" << std::endl;
			std::cout << "--seed (-s)	:�V�[�h�摜������t�H���_��(�f�t�H���g seed)" << std::endl;
			std::cout 
				<< "--mask (-m)	:�}�X�N�摜������t�H���_���D"
				<< "--mask�I�v�V�������w�肳��Ă��Ȃ���΁C�摜�S�̂ɓK�p" << std::endl;
			std::cout << "--sigma		:���������̃p�����[�^(�f�t�H���g�l��0.1)" << std::endl;
			std::cout << "--lambda	:�f�[�^���ɂ�����d��(�f�t�H���g�l��1)" << std::endl;
			system("pause");
			return EXIT_SUCCESS;
		}

		if ((arg == "--input") || (arg == "-i")) {
			if (i + 1 < argc) {
				inputFolder = argv[++i];
			}
			else {
				std::cerr << "--input�I�v�V�����̈�����1�����ł�" << std::endl;
			}
		}

		if ((arg == "--output") || (arg == "-o")) {
			if (i + 1 < argc) {
				outputFolder = argv[++i];
			}
			else {
				std::cerr << "--output�I�v�V�����̈�����1�����ł�" << std::endl;
			}
		}

		if ((arg == "--seed") || (arg == "-s")) {
			if (i + 1 < argc) {
				seedFolder = argv[++i];
			}
			else {
				std::cerr << "--seed�I�v�V�����̈�����1�����ł�" << std::endl;
			}
		}

		if ((arg == "--mask") || (arg == "-m")) {
			if (i + 1 < argc) {
				maskFolder = argv[++i];
			}
			else {
				std::cerr << "--mask�I�v�V�����̈�����1�����ł�" << std::endl;
			}
		}

		if (arg == "--sigma") {
			if (i + 1 < argc) {
				sigma = std::atof(argv[++i]);
			}
			else {
				std::cerr << "--sigma�I�v�V�����̈�����1�����ł�" << std::endl;
			}
		}

		if (arg == "--lambda") {
			if (i + 1 < argc) {
				lambda = std::atof(argv[++i]);
			}
			else {
				std::cerr << "--lambda�I�v�V�����̈�����1�����ł�" << std::endl;
			}
		}
	}

	std::cout << "<�f�[�^�̃t�H���_�\��>" << std::endl;
	std::cout << root << std::endl;
	std::cout << " |-" << inputFolder << std::endl;
	std::cout << " |-" << seedFolder << std::endl;
	if (maskFolder != "none")
		std::cout << " |-" << maskFolder << std::endl;
	
	std::cout << "<�n�C�p�[�p�����[�^>" << std::endl;
	std::cout << "�f�[�^���ɂ�����d�� lambda = " << lambda << std::endl;
	std::cout << "���������̃p�����[�^ sigma = " << sigma << std::endl;

	std::cout << "- �O���t�J�b�g�J�n -" << std::endl;
	std::vector<ImagePixelType> image;
	std::vector<LabelPixelType>	mask;
	std::vector<LabelPixelType>	seed;
	ImageIO<3> imageIO, maskIO, labelIO;
	imageIO.Read(image, root + "/" + inputFolder + "/" + filename);
	labelIO.Read(seed, root + "/" + seedFolder + "/" + filename);
	if (maskFolder != "none") {
		maskIO.Read(mask, root + "/" + maskFolder + "/" + filename);
	}
	else {
		mask.resize(imageIO.NumOfPixels());
		std::fill(mask.begin(), mask.end(), 1);
	}

	// Calculate average and variance of object and background.
	double ave_obj = 0, ave_bkg = 0;
	unsigned int numOfObj = 0, numOfBkg = 0;
	for (unsigned int s = 0; s < imageIO.NumOfPixels(); s++) {
		if (seed[s] == 1) {
			ave_obj += static_cast<double>(image[s]);
			numOfObj++;
		}
		else if (seed[s] == 2) {
			ave_bkg += static_cast<double>(image[s]);
			numOfBkg++;
		}

	}
	ave_obj /= numOfObj;
	ave_bkg /= numOfBkg;

	double var_obj = 0, var_bkg = 0;
	for (unsigned int s = 0; s < imageIO.NumOfPixels(); s++) {
		if (seed[s] == 1) {
			var_obj += std::pow(static_cast<double>(image[s]) - ave_obj, 2.0);
		}
		else if (seed[s] == 2) {
			var_bkg += std::pow(static_cast<double>(image[s]) - ave_bkg, 2.0);
		}
	}
	var_obj /= numOfObj;
	var_bkg /= numOfBkg;
	
	std::cout << "<�}�`�V�[�h�Ɣw�i�V�[�h�̔Z�x���z>" << std::endl;
	std::cout << "�}�`: " << ave_obj << "(+- " << std::sqrt(var_obj) << " )" << std::endl;
	std::cout << "�w�i: " << ave_bkg << "(+- " << std::sqrt(var_bkg) << " )" << std::endl;

	// Create likelihood image.
	std::vector<double> imgObj, imgBkg;
	for (unsigned int s = 0; s < imageIO.NumOfPixels(); s++) {
		imgObj.push_back(calc_likelihood(static_cast<double>(image[s]), ave_obj, var_obj));
		imgBkg.push_back(calc_likelihood(static_cast<double>(image[s]), ave_bkg, var_bkg));
	}

	std::vector<unsigned int> siz = { imageIO.Size(0), imageIO.Size(1) ,imageIO.Size(2) };
	std::vector<double> spacing = { imageIO.Spacing(0), imageIO.Spacing(1) ,imageIO.Spacing(2) };
	std::vector<LabelPixelType> imgO(imageIO.NumOfPixels(), 0), label(imageIO.NumOfPixels(), 0);
	graph_cut_segmentation(imgO, label, image, mask, seed, imgObj, imgBkg, siz, lambda, sigma);

	labelIO.Write(label, root + "/" + outputFolder + "/" + filename);
	return EXIT_SUCCESS;

}