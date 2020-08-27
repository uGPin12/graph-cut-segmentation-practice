#pragma once

#include "common.h"

#include <vector>
#include <iostream>

#include "../maxflow/graph.h"

/*
	Implementation of graph cut segmentation.
		imgO			:output image(obj_seed + bkg_seed + obj + bkg)
		label			:binary label image(obj + bkg)
		seed			:seed
		imgI			:input image
		mask			:mask image
		obj, bkg		:weight of t-link
		siz				:size of image
		lambda, sigma	:hyper parameter
*/
void graph_cut_segmentation(
	std::vector<output_type>& imgO, std::vector<output_type>& label,
	const std::vector<input_type>& imgI, const std::vector<mask_type>& mask, std::vector<seed_type>& seed, 
	const std::vector<feat_type>& obj, const std::vector<feat_type>& bkg,
	const std::vector<unsigned int>& siz, const double lambda, const double sigma
)
{
	if (siz.size() != 3) {
		std::cerr
			<< "This function supports only 3D image now.\n"
			<< "Hit any key to exit..."
			<< std::endl;
		system("pause");
		std::exit(EXIT_FAILURE);
	}

	unsigned int xe = siz[0];
	unsigned int ye = siz[1];
	unsigned int ze = siz[2];
	unsigned int x, y, z, s;

	// Set node index
	std::vector<unsigned int> node_idx(xe * ye * ze, 0);
	unsigned int n_mask = 0;
	for (s = 0; s < xe * ye * ze; s++) {
		if ((mask[s] != 0) & (seed[s] == 0)) { // ignore out of mask or seed
			node_idx[s] = ++n_mask;
		}
	}
	std::cout << "There are " << n_mask << " voxel in the mask(ignore seed)" << std::endl;

	// Construct and initialize st-graph
	using graph_type = maxflow::Graph<feat_type, feat_type, feat_type>;
	graph_type* g = new graph_type(n_mask, n_mask * 3);
	g->add_node(n_mask);

	unsigned int dx[] = { 1, 0, 0 }, dy[] = { 0, 1, 0 }, dz[] = { 0, 0, 1 };

	// Start raster scan
	for (z = 0; z < ze; z++) {
		for (y = 0; y < ye; y++) {
			for (x = 0; x < xe; x++) {
				
				s = z * xe * ye + y * xe + x;

				// Start set st-graph's edges 
				if (node_idx[s] != 0) { // node_idx==0 means the voxel is ignored

					// Set terminal-link(t-link)
					feat_type g_src		= static_cast<feat_type>(bkg[s]);
					feat_type g_sink	= static_cast<feat_type>(obj[s]);
					g->add_tweights(node_idx[s] - 1, g_src * static_cast<feat_type>(lambda), g_sink * static_cast<feat_type>(lambda));

					// Set neighborhood-link(n-link)
					for (unsigned int d = 0; d < sizeof(dx) / sizeof(unsigned int); d++) {
						unsigned int nx = x + dx[d], ny = y + dy[d], nz = z + dz[d];
						if (0 <= nx && nx < xe && 0 <= ny && ny < ye && 0 <= nz && nz < ze) {
							unsigned int ss = nz * xe * ye + ny * xe + nx;
							if (node_idx[ss] != 0) {

								feat_type cap = static_cast<feat_type>(
									std::exp(-(imgI[s] - imgI[ss]) * (imgI[s] - imgI[ss]) / 2 * (sigma * sigma))
									);
								feat_type rev_cap = static_cast<feat_type>(
									std::exp(-(imgI[ss] - imgI[s]) * (imgI[ss] - imgI[s]) / 2 * (sigma * sigma))
									);
								g->add_edge(node_idx[s] - 1, node_idx[ss] - 1, cap, rev_cap);

							}
						}
					}
				}
				// End set st-graph's edges
			}
		}
	}
	// End raster scan

	// Graph cut
	std::cout << std::endl;
	std::cout << "Max flow" << std::endl;
	feat_type cost = g->maxflow();
	std::cout << "Cost = " << cost << std::endl;
	std::cout << std::endl;

	// Starts raster scan
	for (z = 0; z < ze; z++) {
		for (y = 0; y < ye; y++) {
			for (x = 0; x < xe; x++) {

				s = z * xe * ye + y * xe + x;
				// Start setting output image's value
				if (node_idx[s] != 0) {
					if (g->what_segment(node_idx[s] - 1) == graph_type::SOURCE) {
						imgO[s] = 1;
						label[s] = 1;
					}
					else {
						imgO[s] = 2;
					}
				}

				if (seed[s] == 1) {
					imgO[s] = 1;
					label[s] = 1;
				}
				else if(seed[s] == 2) {
					imgO[s] = 2;
				}
				// End setting output image's value
				
			}
		}
	}
	// Ends raster scan

	delete g;
}