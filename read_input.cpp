#include "read_input.h"

void readInputData(const std::string& filename, int& capacity, int& num_var, std::vector<int>& weights, std::vector<int>& values) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file >> capacity;

    file >> num_var;

    weights.resize(num_var);
    values.resize(num_var);

    for (int i = 0; i < num_var; i++) {
        file >> weights[i] >> values[i];
    }

    file.close();
}