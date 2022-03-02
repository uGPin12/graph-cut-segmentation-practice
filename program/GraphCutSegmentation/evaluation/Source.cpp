#include "dataIO.h"
#include "itkImageIO.h"
#include "common.h"
#include "util.h"

#include <ios>

int main(int argc, char* argv[]) {


	if (argc > 5) {
		std::cout << "Usage: " << argv[0]
			<< "[出力フォルダ] [結果ファイル名] [正解ファイル名] [マスクファイル名(option)]"<<std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	std::cout << "------ 評価開始 ------" << std::endl;
	std::string outputFolder = argv[1];
	std::string resultFilename = argv[2];
	std::string answerFilename = argv[3];

	std::vector <input_type> result;
	std::vector <mask_type>	mask;
	std::vector <output_type> answer;

	ImageIO<3> resultIO;
	ImageIO<3> maskIO;
	ImageIO<3> ansIO;
	resultIO.Read(result, resultFilename);
	ansIO.Read(answer, answerFilename);
	if (argc == 4) {
		maskIO.Read(mask, argv[4]);
	}
	else {
		mask.resize(resultIO.NumOfPixels());
		std::fill(mask.begin(), mask.end(), 1);
	}
	
	enum eval
	{
		TP = 1,
		FP,
		FN
	};

	std::vector<output_type> evaluate(resultIO.NumOfPixels(), 0);
	double tp = 0, fn = 0, fp = 0;
	// Starts raster scan
	for (unsigned int s = 0; s < resultIO.NumOfPixels(); s++) {

		// Starts mask processing 
		if (mask[s] != 0) {
			if (result[s] != 0 && answer[s] != 0) {
				tp++;
				evaluate[s] = eval::TP;
			}
			else if (result[s] != 0 && answer[s] == 0) {
				fp++;
				evaluate[s] = eval::FP;
			}
			else if (result[s] == 0 && answer[s] != 0) {
				fn++;
				evaluate[s] = eval::FN;
			}
		}
		// Ends mask processing 

	}
	// Ends raster scan

	double ji = tp / (tp + fp + fn);
	mkdir_boost(outputFolder);
	resultIO.Write(evaluate, outputFolder + "evaluation_label.mhd");

	std::ofstream ofs(outputFolder + "evaluation.csv", std::ios::app);
	if (!ofs) {
		std::cerr
			<< "Cannot open : "
			<< outputFolder + "evaluation.csv"
			<< std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	ofs << "JI" << ji << std::endl;
	ofs << "TP" << tp << std::endl;
	ofs << "FP" << fp << std::endl;
	ofs << "FN" << fn << std::endl;

	system("pause");
	return EXIT_SUCCESS;

}