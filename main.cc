#include <cstdio>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <Eigen/Dense>

#include "auxiliary.h"
#include "datasets.h"
#include "LogRegOracle.h"
#include "optim.h"
#include "logger.h"

#include <tclap/CmdLine.h>

int main(int argc, char* argv[])
{
    std::string method;
    std::string dataset;
    double max_epochs = 1.0;

    try {
        TCLAP::CmdLine cmd("Run numerical optimiser for training logistic regression.", ' ', "0.1");

        TCLAP::ValueArg<std::string> arg_method("", "method", "Optimisation method (SGD, SAG, SO2)", true, "", "string");
        TCLAP::ValueArg<std::string> arg_dataset("", "dataset", "Dataset (a9a, mushrooms, w8a, covtype, quantum, alpha)", true, "", "string");
        TCLAP::ValueArg<double> arg_max_epochs("", "max_epochs", "Maximum number of epochs", true, -1.0, "double");

        cmd.add(arg_method);
        cmd.add(arg_dataset);
        cmd.add(arg_max_epochs);

        cmd.parse(argc, argv);

        method = arg_method.getValue();
        dataset = arg_dataset.getValue();
        max_epochs = arg_max_epochs.getValue();
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    /* ========================================================================== */
    /* ========================================================================== */
    /* ========================================================================== */

    Eigen::MatrixXd Z;
    Eigen::VectorXi y;
    double lambda;

    if (dataset == "a9a") {
        fprintf(stderr, "Load a9a\n");
        load_a9a(Z, y);
    } else if (dataset == "mushrooms") {
        fprintf(stderr, "Load mushrooms\n");
        load_mushrooms(Z, y);
    } else if (dataset == "w8a") {
        fprintf(stderr, "Load w8a\n");
        load_w8a(Z, y);
    } else if (dataset == "covtype") {
        fprintf(stderr, "Load covtype\n");
        load_covtype(Z, y);
    } else if (dataset == "quantum") {
        fprintf(stderr, "Load quantum\n");
        load_quantum(Z, y);
    } else if (dataset == "alpha") {
        fprintf(stderr, "Load alpha, might take a lot of time\n");
        load_alpha(Z, y);
    } else {
        fprintf(stderr, "Unknown dataset %s\n", dataset.c_str());
        return 1;
    }

    /* multiply each sample Z[i] by -y[i] */
    Z.array().colwise() *= -y.cast<double>().array();

    lambda = 1.0 / Z.rows();

    LogRegOracle func(Z, lambda);
    Eigen::VectorXd w0 = Eigen::VectorXd::Zero(Z.cols());

    int maxiter = max_epochs * Z.rows();

    Logger logger(func);
    Eigen::VectorXd w;

    if (method == "SAG") {
        fprintf(stderr, "Use method SAG\n");

        /* estimate the Lipschitz constant */
        double L = 0.25 * Z.rowwise().squaredNorm().maxCoeff() + lambda;

        double alpha = 1.0 / L;
        fprintf(stderr, "SAG: L=%g, alpha=%g\n", L, alpha);
        w = SAG(func, logger, w0, alpha, maxiter);
    } else if (method == "SGD") {
        fprintf(stderr, "Use method SGD\n");

        double alpha = 1e-4;
        w = SGD(func, logger, w0, alpha, maxiter);
    } else if (method == "SO2") {
        fprintf(stderr, "Use method SO2\n");

        w = SO2(func, logger, w0, maxiter);
    } else {
        fprintf(stderr, "Unknown method %s\n", method.c_str());
        return 1;
    }

    printf("%9s %9s %15s %15s\n", "epoch", "elapsed", "val", "norm_grad");
    for (int i = 0; i < int(logger.trace_epoch.size()); ++i) {
        printf("%9.2f %9.2f %15.6e %15.6e\n", logger.trace_epoch[i], logger.trace_elaps[i], logger.trace_val[i], logger.trace_norm_grad[i]);
    }

    return 0;
}
