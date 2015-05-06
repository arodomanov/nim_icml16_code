#ifndef LOG_REG_ORACLE_H_
#define LOG_REG_ORACLE_H_

#include <Eigen/Dense>

/* class for computing Hessian-vector products */
class LogRegHessVec
{
public:
    LogRegHessVec(const Eigen::MatrixXd& Z, double lambda);

    void prepare(const Eigen::VectorXd& w); // calcuate the diagonal part s=sigma(w)*(1-sigma(w))
    Eigen::VectorXd calculate(const Eigen::VectorXd& d) const; // calculate the actual product at point `w`

private:
    const Eigen::MatrixXd& Z;
    double lambda;

    Eigen::VectorXd s;
};

class LogRegOracle
{
public:
    LogRegOracle(const Eigen::MatrixXd& Z, double lambda);

    int n_samples() const;

    double single_val(const Eigen::VectorXd& w, int i) const;
    Eigen::VectorXd single_grad(const Eigen::VectorXd& w, int i) const;

    double full_val(const Eigen::VectorXd& w) const;
    Eigen::VectorXd full_grad(const Eigen::VectorXd& w) const;
    Eigen::MatrixXd full_hess(const Eigen::VectorXd& w) const; // the elements will be stored in the **upper** triangular part

    LogRegHessVec hessvec() const; // return the corresponding LogRegHessVec object

    double phi_prime(double mu) const;
    double phi_double_prime(double mu) const;

    const Eigen::MatrixXd& Z;
    double lambda;
};

#endif
