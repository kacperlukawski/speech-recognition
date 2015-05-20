//
// Created by kacper on 20.05.15.
//

#include "GaussianMixtureModel.h"


speech::clustering::GaussianMixtureModel::GaussianMixtureModel(int nbClusters, int dimension) {
    this->nbClusters = nbClusters;
    this->dimension = dimension;
    this->gmm = vl_gmm_new(VL_TYPE_DOUBLE, dimension, nbClusters);
    vl_gmm_set_max_num_iterations(this->gmm, 100000); // max number of EM iterations TODO: allow to change that
    vl_gmm_set_initialization(this->gmm, VlGMMRand);
//    vl_gmm_set_verbosity(this->gmm, 1); // enables verbosity
}

speech::clustering::GaussianMixtureModel::~GaussianMixtureModel() {
    vl_gmm_reset(this->gmm);
    // TODO: add means, convariances, priors and posteriors removal
}

void speech::clustering::GaussianMixtureModel::fit(vector<valarray<double>> &vectors, vector<int> &labels) {
    int nbVectors = vectors.size();
    double * data = this->createData(vectors);
    vl_gmm_cluster(this->gmm, data, nbVectors);
    delete[] data;

    this->means = (double *) vl_gmm_get_means(this->gmm);
    this->covariances = (double *) vl_gmm_get_covariances(this->gmm);
    this->priors = (double *) vl_gmm_get_priors(this->gmm);
    this->logLikelihood = vl_gmm_get_loglikelihood(this->gmm);
    this->posteriors = (double *) vl_gmm_get_posteriors(this->gmm);
}

int speech::clustering::GaussianMixtureModel::predict(const valarray<double> &vector) {
    double *data = createData(vector);
    double *samplePosteriors = new double[this->nbClusters];
    vl_get_gmm_data_posteriors_d(samplePosteriors,
                                 this->nbClusters,
                                 1, // get the posterior for one vector only
                                 this->priors,
                                 this->means,
                                 this->dimension,
                                 this->covariances,
                                 data);

    int maxIndex = 0;
    for (int i = 1; i < this->nbClusters; i++) {
        if (samplePosteriors[i] > samplePosteriors[maxIndex]) {
            maxIndex = i;
        }
    }

    delete[] data;
    delete[] samplePosteriors;

    return maxIndex;
}

void speech::clustering::GaussianMixtureModel::serialize(std::ostream &out) const {
    // TODO: add model serialization
}

double *speech::clustering::GaussianMixtureModel::createData(const std::vector<std::valarray<double>>& vectors){
    int nbVectors = vectors.size();
    double * data = new double[nbVectors * this->dimension];

    int pos = 0;
    for (std::valarray<double> vector : vectors) {
        for (int i = 0; i < this->dimension; i++) {
            *(data + pos) = vector[i];
        }
    }

    return data;
}

double *speech::clustering::GaussianMixtureModel::createData(const std::valarray<double> &vector) {
    double * data = new double[this->dimension];
    for (int i = 0; i < this->dimension; i++) {
        *(data + i) = vector[i];
    }

    return data;
}
