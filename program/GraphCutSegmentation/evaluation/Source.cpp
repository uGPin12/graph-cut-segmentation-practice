#include "dataIO.h"
#include "itkImageIO.h"
#include "common.h"
#include "util.h"

#include <ios>

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Usage: " << argv[0]
			<< "input_info.ini\n"
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	std::cout << "Evaluation starts." << std::endl;

	std::string dir			= read_param_boost<std::string>(argv[1], "path"	, "dir");
	std::string dir_i		= read_param_boost<std::string>(argv[1], "path"	, "dir_i");
	std::string param		= read_param_boost<std::string>(argv[1], "param", "param");
	std::string mask_name	= read_param_boost<std::string>(argv[1], "param", "mask_name");
	std::string ans_name	= read_param_boost<std::string>(argv[1], "param", "ans_name");

	std::vector <input_type>	imgI;
	std::vector <mask_type>		mask;
	std::vector <output_type>	imgAns;

	ImageIO<3> mhdI;
	ImageIO<3> mhdM;
	ImageIO<3> mhdAns;
	mhdI.	Read(imgI, dir + param + "/label_obj.mhd");
	mhdM.	Read(mask, dir_i + mask_name);
	mhdAns.	Read(imgAns, dir_i + ans_name);
	
	enum eval
	{
		TP = 1,
		FP,
		FN
	};

	std::vector<output_type> imgO(imgI.size(), 0);
	double tp = 0, fn = 0, fp = 0;
	// Starts raster scan
	for (unsigned int s = 0; s < mhdI.Size(0) * mhdI.Size(1) * mhdI.Size(2); s++) {

		// Starts mask processing 
		if (mask[s] != 0) {
			if (imgI[s] != 0 && imgAns[s] != 0) {
				tp++;
				imgO[s] = eval::TP;
			}
			else if (imgI[s] != 0 && imgAns[s] == 0) {
				fp++;
				imgO[s] = eval::FP;
			}
			else if (imgI[s] == 0 && imgAns[s] != 0) {
				fn++;
				imgO[s] = eval::FN;
			}
		}
		// Ends mask processing 

	}
	// Ends raster scan

	double ji = tp / (tp + fp + fn);

	std::vector<unsigned int>	siz		= { mhdI.Size(0), mhdI.Size(1), mhdI.Size(2) };
	std::vector<double>			spacing = { mhdI.Spacing(0), mhdI.Spacing(1), mhdI.Spacing(2) };
	mkdir_boost(dir + param + "/evaluation/");
	save_vector(dir + param + "/evaluation/label.mhd", imgO, siz, spacing, true);

	std::ofstream ofs(dir + "evaluation.csv",std::ios::app);
	if (!ofs) {
		std::cerr
			<< "Cannot open : "
			<< dir + param + "/evaluation/evaluation.csv"
			<< "Hit any key to exist..."
			<< std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	ofs << param << ","
		<< ji << ","
		<< tp << ","
		<< fp << ","
		<< fn << ","
		<< std::endl;

	std::cout << "Evaluation completes." << std::endl;
	system("pause");
	return 0;

}