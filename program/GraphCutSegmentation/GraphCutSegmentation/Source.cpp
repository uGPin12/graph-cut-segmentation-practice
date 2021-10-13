#include "graphcut.h"
#include "util.h"
#include "itkImageIO.h"
#include "dataIO.h"

double calc_likelihood(double value, double ave, double var) {
	constexpr double PI = 3.14159265357;
	return -(std::log(1 / std::sqrt(2.0 * PI * var)) + (-std::pow(value - ave, 2.0) / 2.0 / var));
}

int main(int argc, char* argv[]) {

	if (argc > 7) {
		std::cerr 
			<< "Usage: " << argv[0]
			<< "<inputFileName> <seedFileName> <outputFilename>"
			<< "<sigma> <lambda> [maskFileName(option)]" << std::endl;
		return EXIT_FAILURE;
	}

	// Parse arguments
	std::string inputFileName = argv[1];
	std::string seedFileName = argv[2];
	std::string outputFilename = argv[3];
	double sigma = std::stod(argv[4]);
	double lambda = std::stod(argv[5]);

	std::cout << "Segmentation starts : lambda = " << lambda << ", sigma = " << sigma << std::endl;
	std::cout << std::endl;

	std::vector<ImagePixelType> image;
	std::vector<LabelPixelType>	mask;
	std::vector<LabelPixelType>	seed;
	ImageIO<3> imageIO, maskIO, labelIO;
	imageIO.Read(image, inputFileName);
	labelIO.Read(seed, seedFileName);
	if (argc == 7) {
		maskIO.Read(mask, argv[6]);
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
	
	std::cout << "Object		: " << ave_obj << "(+- " << std::sqrt(var_obj) << " )" << std::endl;
	std::cout << "Background	: " << ave_bkg << "(+- " << std::sqrt(var_bkg) << " )" << std::endl;

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

	labelIO.Write(label, outputFilename);
	return EXIT_SUCCESS;

}