//
// Created by kacper on 20.05.15.
//

#include "GaussianMixtureModel.h"

speech::clustering::GaussianMixtureModel::GaussianMixtureModel(std::istream &in) {
    in.read((char *) &this->nbClusters, sizeof(int));
    in.read((char *) &this->dimension, sizeof(int));

    this->means = new double[this->nbClusters * this->dimension];
    this->covariances = new double[this->nbClusters * this->dimension];
    this->priors = new double[this->nbClusters];
    this->posteriors = new double[this->nbClusters];

    in.read((char *) this->means, sizeof(double) * this->nbClusters * this->dimension);
    in.read((char *) this->covariances, sizeof(double) * this->nbClusters * this->dimension);
    in.read((char *) this->priors, sizeof(double) * this->nbClusters);
    in.read((char *) this->posteriors, sizeof(double) * this->nbClusters);
    in.read((char *) &this->logLikelihood, sizeof(double));

    this->gmm = vl_gmm_new(VL_TYPE_DOUBLE, this->dimension, this->nbClusters);
    vl_gmm_set_max_num_iterations(this->gmm, 100);
    vl_gmm_set_initialization(this->gmm, VlGMMCustom);
    vl_gmm_set_means(this->gmm, this->means);
    vl_gmm_set_covariances(this->gmm, this->covariances);
    vl_gmm_set_priors(this->gmm, this->priors);
    vl_gmm_set_verbosity(this->gmm, 1);
}

speech::clustering::GaussianMixtureModel::GaussianMixtureModel(int nbClusters, int dimension) {
    this->nbClusters = nbClusters;
    this->dimension = dimension;
    this->gmm = vl_gmm_new(VL_TYPE_DOUBLE, this->dimension, this->nbClusters);
    vl_gmm_set_max_num_iterations(this->gmm, 100); // max number of EM iterations TODO: allow to change that
    vl_gmm_set_initialization(this->gmm, VlGMMRand);
    vl_gmm_set_verbosity(this->gmm, 1); // enables verbosity
//    vl_gmm_set_num_repetitions(this->gmm, 3); // number of repetitions TODO: allow to change that
}

speech::clustering::GaussianMixtureModel::~GaussianMixtureModel() {
    vl_gmm_reset(this->gmm);
}

void speech::clustering::GaussianMixtureModel::fit(vector<valarray<double>> &vectors) {
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
    double *clustersPosteriors = new double[this->nbClusters];
    vl_get_gmm_data_posteriors_d(clustersPosteriors,
                                 this->nbClusters,
                                 1, // get the posterior for one vector only
                                 this->priors,
                                 this->means,
                                 this->dimension,
                                 this->covariances,
                                 data);

    int maxIndex = 0;
    for (int i = 1; i < this->nbClusters; i++) {
        if (clustersPosteriors[i] > clustersPosteriors[maxIndex]) {
            maxIndex = i;
        }
    }

    delete[] data;
    delete[] clustersPosteriors;

    return maxIndex;
}

void speech::clustering::GaussianMixtureModel::serialize(std::ostream &out) const {
    uint32_t type = TYPE_IDENTIFIER;
    out.write((char const *) &type, sizeof(type));
    out.write((char const *) &this->nbClusters, sizeof(this->nbClusters));
    out.write((char const *) &this->dimension, sizeof(this->dimension));
    out.write((char const *) this->means, sizeof(double) * this->nbClusters * this->dimension);
    out.write((char const *) this->covariances, sizeof(double) * this->nbClusters * this->dimension);
    out.write((char const *) this->priors, sizeof(double) * this->nbClusters);
    out.write((char const *) this->posteriors, sizeof(double) * this->nbClusters);
    out.write((char const *) &this->logLikelihood, sizeof(double));
}

// TODO: check if the format of the data is correct (it might be a problem that matrix should be transposed...)
double *speech::clustering::GaussianMixtureModel::createData(const std::vector<std::valarray<double>>& vectors){
    int nbVectors = vectors.size();
    double * data = new double[nbVectors * this->dimension];

    int pos = 0;
    for (auto it = vectors.begin(); it != vectors.end(); it++) {
        for (int i = 0; i < this->dimension; i++) {
            *(data + pos) = (*it)[i];
            pos++;
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
