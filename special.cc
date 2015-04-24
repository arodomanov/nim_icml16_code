#include "special.h"
#include <cmath>
#include <algorithm>

double logaddexp(double a, double b)
{
    double t = std::max(a, b);
    return t + log(exp(a - t) + exp(b - t));
}

double logaddexp0(double x)
{
    return logaddexp(0, x);
}

double sigm(double x)
{
    return 1.0 / (1 + exp(-x));
}
