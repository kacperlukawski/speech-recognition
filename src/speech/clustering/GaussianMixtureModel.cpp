//
// Created by kacper on 20.05.15.
//

#include "GaussianMixtureModel.h"

void speech::clustering::GaussianMixtureModel::fit(vector<valarray<double>> &vectors, vector<int> &labels) {
    VL_PRINT("Hello world");
}

int speech::clustering::GaussianMixtureModel::predict(const valarray<double> &vector) {
    return 0;
}

void speech::clustering::GaussianMixtureModel::serialize(std::ostream &out) const {
    // TODO: add model serialization
}