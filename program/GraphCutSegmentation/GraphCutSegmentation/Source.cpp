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

	// コマンドライン引数のチェック
	if (argc < 2) {
		std::cerr << "コマンドライン引数が足りません" << std::endl;
		std::cerr << "使い方: .exe [画像ファイル名] [options]" << std::endl;
		std::cerr << "詳細は--help(or -h)コマンドで確認可能" << std::endl;
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
			std::cout << "使い方: .exe [画像ファイル名] [options]" << std::endl;
			std::cout << "<options>" << std::endl;
			std::cout << "--root (-r)	:データがあるフォルダ名(デフォルト [exeのあるフォルダの1つ上]/data)" << std::endl;
			std::cout << "--input (-i)	:入力画像があるフォルダ名(デフォルト input)" << std::endl;
			std::cout << "--output (-o)	:出力フォルダ(デフォルト result)" << std::endl;
			std::cout << "--seed (-s)	:シード画像があるフォルダ名(デフォルト seed)" << std::endl;
			std::cout 
				<< "--mask (-m)	:マスク画像があるフォルダ名．"
				<< "--maskオプションが指定されていなければ，画像全体に適用" << std::endl;
			std::cout << "--sigma		:平滑化項のパラメータ(デフォルト値は0.1)" << std::endl;
			std::cout << "--lambda	:データ項にかかる重み(デフォルト値は1)" << std::endl;
			system("pause");
			return EXIT_SUCCESS;
		}

		if ((arg == "--input") || (arg == "-i")) {
			if (i + 1 < argc) {
				inputFolder = argv[++i];
			}
			else {
				std::cerr << "--inputオプションの引数は1つだけです" << std::endl;
			}
		}

		if ((arg == "--output") || (arg == "-o")) {
			if (i + 1 < argc) {
				outputFolder = argv[++i];
			}
			else {
				std::cerr << "--outputオプションの引数は1つだけです" << std::endl;
			}
		}

		if ((arg == "--seed") || (arg == "-s")) {
			if (i + 1 < argc) {
				seedFolder = argv[++i];
			}
			else {
				std::cerr << "--seedオプションの引数は1つだけです" << std::endl;
			}
		}

		if ((arg == "--mask") || (arg == "-m")) {
			if (i + 1 < argc) {
				maskFolder = argv[++i];
			}
			else {
				std::cerr << "--maskオプションの引数は1つだけです" << std::endl;
			}
		}

		if (arg == "--sigma") {
			if (i + 1 < argc) {
				sigma = std::atof(argv[++i]);
			}
			else {
				std::cerr << "--sigmaオプションの引数は1つだけです" << std::endl;
			}
		}

		if (arg == "--lambda") {
			if (i + 1 < argc) {
				lambda = std::atof(argv[++i]);
			}
			else {
				std::cerr << "--lambdaオプションの引数は1つだけです" << std::endl;
			}
		}
	}

	std::cout << "<データのフォルダ構成>" << std::endl;
	std::cout << root << std::endl;
	std::cout << " |-" << inputFolder << std::endl;
	std::cout << " |-" << seedFolder << std::endl;
	if (maskFolder != "none")
		std::cout << " |-" << maskFolder << std::endl;
	
	std::cout << "<ハイパーパラメータ>" << std::endl;
	std::cout << "データ項にかける重み lambda = " << lambda << std::endl;
	std::cout << "平滑化項のパラメータ sigma = " << sigma << std::endl;

	std::cout << "- グラフカット開始 -" << std::endl;
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
	
	std::cout << "<図形シードと背景シードの濃度分布>" << std::endl;
	std::cout << "図形: " << ave_obj << "(+- " << std::sqrt(var_obj) << " )" << std::endl;
	std::cout << "背景: " << ave_bkg << "(+- " << std::sqrt(var_bkg) << " )" << std::endl;

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