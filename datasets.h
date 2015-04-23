#ifndef DATASETS_H
#define DATASETS_H

#include <vector>

void load_mushrooms(std::vector<std::vector<double>>& X, std::vector<int>& y);
void load_a9a(std::vector<std::vector<double>>& X, std::vector<int>& y);

#endif