/**
 * SLET
 * Copyright (C) 2020  Hong Huang and Fan Zhong and Yuqing Sun and Xueying Qin (Shandong University)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
* Prototypes and definitions for the common M-estimators
* It can be integrated into Lev-Mar least-squares as reweighted least-squares problem
* For detail, please refer to
*   https://www.microsoft.com/en-us/research/wp-content/uploads/2016/11/RR-2676.pdf
*/

#pragma once

#include <cmath>

#define ME_L2           0
#define ME_L1           1
#define ME_L1L2         2
#define ME_FAIR         3
#define ME_HUBER        4
#define ME_CAUCHY       5
#define ME_GEMANMCCLURE 6
#define ME_WELSCH       7
#define ME_TUKEY        8

float l2_cost(float x, float c);
float l2_influence(float x, float c);
float l2_weight(float x, float c);

float l1_cost(float x, float c);
float l1_influence(float x, float c);
float l1_weight(float x, float c);

float l1l2_cost(float x, float c);
float l1l2_influence(float x, float c);
float l1l2_weight(float x, float c);

float fair_cost(float x, float c);
float fair_influence(float x, float c);
float fair_weight(float x, float c);

float huber_cost(float x, float c);
float huber_influence(float x, float c);
float huber_weight(float x, float c);

float cauchy_cost(float x, float c);
float cauchy_influence(float x, float c);
float cauchy_weight(float x, float c);

float gemanmcclure_cost(float x, float c);
float gemanmcclure_influence(float x, float c);
float gemanmcclure_weight(float x, float c);

float welsch_cost(float x, float c);
float welsch_influence(float x, float c);
float welsch_weight(float x, float c);

float tukey_cost(float x, float c);
float tukey_influence(float x, float c);
float tukey_weight(float x, float c);

inline float ecweight(float x, float c) {
	return (1 - c * (x - 1) * c * (x - 1)) * (1 - c * (x - 1) * c * (x - 1));
}