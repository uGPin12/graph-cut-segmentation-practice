#include <iostream>
#include <vector>

#include "common.h"
#include "util.h"
#include "dataIO.h"
#include "graphcut.h"

double calc_likelihood(double value, double ave, double var) {
	return -(std::log(1 / std::sqrt(2.0 * 3.14159265357 * var)) + (-(value - ave) * (value - ave) / 2.0 / var));
}

int main() {

	std::vector<input_type>	label	= { 100,90,90,20,20,10 };
	std::vector<seed_type>		seed	= { 1,1,0,0,2,2 };

	std::vector<mask_type> mask(label.size(), 1);

	mkdir_boost("../../../../result/debug/");
	//save_vector("../../../../result/d/*ebug/1D_label.mhd"	, label	, { 6 }, { 1 }, true);
	//save_vector("../../../../result/debug/1D_seed.mhd"	, seed	, { 6 }, { 1 }, true);
	//save_vector("../../../../result/debug/1D_mask.mhd"	, mask	, { 6 }, { 1 }, true);

	double lambda = 1; double sigma = 1;
	std::cout << "Segmentation starts : lambda = " << lambda << ", sigma = " << sigma << std::endl;
	std::cout << std::endl;

	double ave_obj = 0, ave_bkg = 0, var_obj = 0, var_bkg = 0;
	unsigned int obj_seed = 0, bkg_seed = 0;
	for (unsigned int s = 0; s < 6; s++) {
		if (seed[s] == 1) {
			ave_obj += static_cast<double>(label[s]);
			obj_seed++;
		}
		else if (seed[s] == 2) {
			ave_bkg += static_cast<double>(label[s]);
			bkg_seed++;
		}
	}
	ave_obj /= static_cast<double>(obj_seed);
	ave_bkg /= static_cast<double>(bkg_seed);

	for (unsigned int s = 0; s < 6; s++) {
		if (seed[s] == 1) {
			var_obj += (static_cast<double>(label[s]) - ave_obj)* (static_cast<double>(label[s]) - ave_obj);
		}
		else if (seed[s] == 2) {
			var_bkg += (static_cast<double>(label[s]) - ave_bkg)* (static_cast<double>(label[s]) - ave_bkg);
		}
	}
	var_obj /= static_cast<double>(obj_seed);
	var_bkg /= static_cast<double>(bkg_seed);

	std::cout << "Object		: " << ave_obj << "(+- " << std::sqrt(var_obj) << " )" << std::endl;
	std::cout << "Background	: " << ave_bkg << "(+- " << std::sqrt(var_bkg) << " )" << std::endl;
	std::cout << std::endl;

	std::vector<feat_type> imgObj, imgBkg;
	for (unsigned int s = 0; s < 6; s++) {
		imgObj.push_back(calc_likelihood(static_cast<feat_type>(label[s]), ave_obj, var_obj));
		imgBkg.push_back(calc_likelihood(static_cast<feat_type>(label[s]), ave_bkg, var_bkg));
		std::cout << "val = " << label[s] << ", obj = " << imgObj[s] << " , bkg = " << imgBkg[s] << std::endl;
	}

	std::vector<output_type> imgO(label.size(), 0), labelO(label.size(), 0);
	graph_cut_segmentation_6n(imgO, labelO, label, mask, imgObj, imgBkg, { 6,1,1 }, lambda, sigma);;

	//save_vector("../../../../result/debug/1D_result.mhd", imgO, { 6 }, { 1 }, true);

	system("pause");
	return 0;

}