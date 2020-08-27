#include "graphcut.h"
#include "util.h"
#include "itkImageIO.h"
#include "dataIO.h"

double calc_likelihood(double value, double ave, double var) {
	return -(std::log(1 / std::sqrt(2.0 * 3.14159265357 * var)) + (-(value - ave) * (value - ave) / 2.0 / var));
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Usage: " << argv[0]
			<< "input_info.ini\n"
			<< "Hit any key to exit..." << std::endl;
		system("pause");
		return EXIT_FAILURE;
	}

	std::string dir_i		= read_param_boost<std::string>(argv[1], "path"	, "dir_i");
	std::string dir_o		= read_param_boost<std::string>(argv[1], "path"	, "dir_o");
	std::string param		= read_param_boost<std::string>(argv[1], "param", "param");
	std::string img_name	= read_param_boost<std::string>(argv[1], "param", "img_name");
	std::string mask_name	= read_param_boost<std::string>(argv[1], "param", "mask_name");
	std::string seed_name	= read_param_boost<std::string>(argv[1], "param", "seed_name");

	double lambda	= read_param_boost<double>(argv[1], "param", "lambda");
	double sigma	= read_param_boost<double>(argv[1], "param", "sigma");

	std::cout << "Segmentation starts : lambda = " << lambda << ", sigma = " << sigma << std::endl;
	std::cout << std::endl;

	std::vector<input_type> imgI;
	std::vector<mask_type>	mask;
	std::vector<seed_type>	seed;
	ImageIO<3> mhdI, mhdM, mhdS;
	mhdI.Read(imgI, dir_i + img_name);
	mhdM.Read(mask, dir_i + mask_name);
	mhdS.Read(seed, dir_i + seed_name);

	std::vector<unsigned int> siz = { mhdI.Size(0),mhdI.Size(1), mhdI.Size(2) };

	double ave_obj = 0, ave_bkg = 0, var_obj = 0, var_bkg = 0;
	unsigned int obj_seed = 0, bkg_seed = 0;
	for (unsigned int s = 0; s < siz[0] * siz[1] * siz[2]; s++) {
		if (seed[s] == 1) {
			ave_obj += static_cast<double>(imgI[s]);
			obj_seed++;
		}
		else if (seed[s] == 2) {
			ave_bkg += static_cast<double>(imgI[s]);
			bkg_seed++;
		}
	}
	ave_obj /= static_cast<double>(obj_seed);
	ave_bkg /= static_cast<double>(bkg_seed);

	for (unsigned int s = 0; s < siz[0] * siz[1] * siz[2]; s++) {
		if (seed[s] == 1) {
			var_obj += ((static_cast<double>(imgI[s]) - ave_obj) * (static_cast<double>(imgI[s]) - ave_obj));
		}
		else if (seed[s] == 2) {
			var_bkg += ((static_cast<double>(imgI[s]) - ave_bkg) * (static_cast<double>(imgI[s]) - ave_bkg));
		}
	}
	var_obj /= static_cast<double>(obj_seed);
	var_bkg /= static_cast<double>(bkg_seed);
	
	std::cout << "Object		: " << ave_obj << "(+- " << std::sqrt(var_obj) << " )" << std::endl;
	std::cout << "Background	: " << ave_bkg << "(+- " << std::sqrt(var_bkg) << " )" << std::endl;
	std::cout << std::endl;

	std::vector<feat_type> imgObj, imgBkg;
	for (unsigned int s = 0; s < siz[0] * siz[1] * siz[2]; s++) {
		imgObj.push_back(calc_likelihood(static_cast<double>(imgI[s]), ave_obj, var_obj));
		imgBkg.push_back(calc_likelihood(static_cast<double>(imgI[s]), ave_bkg, var_bkg));
	}

	std::vector<output_type> imgO(imgI.size(), 0), label(imgI.size(), 0);
	graph_cut_segmentation(imgO, label, imgI, mask, seed, imgObj, imgBkg, siz, lambda, sigma);

	std::vector<double> spacing = { mhdI.Spacing(0),mhdI.Spacing(1), mhdI.Spacing(2) };
	mkdir_boost(dir_o + param + "/");
	save_vector(dir_o + param + "/label.mhd"	, imgO	, siz, spacing, true);
	save_vector(dir_o + param + "/label_obj.mhd", label	, siz, spacing, true);
	std::cout << std::endl;

	std::cout << "Segmentation complete." << std::endl;
	system("pause");
	return 0;
}