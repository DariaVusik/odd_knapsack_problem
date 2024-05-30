#pragma once
#include <iostream>
#include <fstream>
#include <random>

void readInputData(const std::string& filename, int& capacity, int& num_var, std::vector<int>& weights, std::vector<int>& values);