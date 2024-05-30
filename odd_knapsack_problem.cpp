// Solve a multiple knapsack problem using a MIP solver.
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>
#include <algorithm>

#include "absl/strings/str_format.h"
#include "ortools/linear_solver/linear_expr.h"
#include "ortools/linear_solver/linear_solver.h"
#include "ortools/linear_solver/linear_solver_callback.h"

#include "ortools/algorithms/knapsack_solver.h"

#include "read_input.h"


namespace operations_research {
    void generateCombinations(int n, int k, int start, std::vector<int>& current, std::vector<std::vector<int>>& result) {
        if (current.size() == k) {
            // Create a mask of size n
            std::vector<int> mask(n, 0);
            for (int index : current) {
                mask[index] = 1;
            }
            result.push_back(mask);
            return;
        }

        for (int i = start; i < n; ++i) {
            current.push_back(i);
            generateCombinations(n, k, i + 1, current, result);
            current.pop_back();
        }
    }

    std::vector<std::vector<int>> getAllSubsets(int n, int k) {
        std::vector<std::vector<int>> result;
        std::vector<int> current;
        generateCombinations(n, k, 0, current, result);
        return result;
    }


    class OddNumberSeparationCallback : public MPCallback {

    public:
        OddNumberSeparationCallback(
            const std::vector<int>& all_items,
            const std::vector<const MPVariable*>& taken_flag
        )
            : _all_items(all_items), _taken_flag(taken_flag), MPCallback(false, true) {
        }

        // Inherited via MPCallback
        void RunCallback(MPCallbackContext* callback_context) override
        {
            auto event_type = callback_context->Event();
            if (event_type == MPCallbackEvent::kMipSolution) { // process found solution
                // Sum(x_j for j in S) - Sum(x_j for j in N\S) <= |S | -1
                size_t current_taken_items = 0;

                std::vector<int> subset_variables_flags;

                for (auto i : _all_items) {
                    auto current_value = callback_context->VariableValue(_taken_flag[i]);
                    //LOG(INFO) << current_value;
                    int is_taken = round(current_value);
                    subset_variables_flags.push_back(is_taken);
                    
                    current_taken_items += is_taken;
                }

                //LOG(INFO) << std::endl << "Current S: " << current_taken_items << std::endl;

                if ((current_taken_items > 0) && (current_taken_items % 2 == 0)) {
                    //LOG(INFO) << std::endl << "adding constraints:" << std::endl;
                    LinearExpr subset_sum;
                    LinearExpr complement_sum;

                    for (auto i : _all_items) {
                        if (subset_variables_flags[i] > 0) {
                            subset_sum += LinearExpr(_taken_flag[i]);
                        }
                        else {
                            complement_sum += LinearExpr(_taken_flag[i]);
                        }
                    }

                    LinearExpr diff = subset_sum - complement_sum;
                    auto constraint{ diff <= (current_taken_items - 1) };
                    //LOG(INFO) << diff;
                    //LOG(INFO) << "upper bound: " << constraint.upper_bound();
                    callback_context->AddLazyConstraint(constraint);
                }
            }
        }
    private:
        const std::vector<int>& _all_items;
        const std::vector<const MPVariable*>& _taken_flag;
    };

    void OddKnapsackMip(const std::string& input_file, const std::string& output_filename) {
        int capacity = 0;
        int num_items = 0;
        std::vector<int> weights;
        std::vector<int> values;
        readInputData(input_file, capacity, num_items, weights, values);


        std::vector<int> all_items(num_items);
        std::iota(all_items.begin(), all_items.end(), 0);

        // Create the mip solver with the SCIP backend.
        std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
        if (!solver) {
            LOG(WARNING) << "SCIP solver unavailable.";
            return;
        }

        // Variables.
        // x[i] = 1 if item i is taken
        std::vector<const MPVariable*> x(num_items);

        for (int i : all_items) {
            x[i] = solver->MakeBoolVar(absl::StrFormat("x_%d", i));
        }

        OddNumberSeparationCallback callback(all_items, x);
        solver->SetCallback(&callback);

        // The amount packed in each bin cannot exceed its capacity.
        LinearExpr items_weight;
        for (int i : all_items) {
            items_weight += LinearExpr(x[i]) * weights[i];
        }
        solver->MakeRowConstraint(items_weight <= capacity);

        // Objective.
        // Maximize total value of packed items.
        MPObjective* const objective = solver->MutableObjective();
        LinearExpr objective_value;
        for (int i : all_items) {
            objective_value += LinearExpr(x[i]) * values[i];
        }
        objective->MaximizeLinearExpr(objective_value);

        const MPSolver::ResultStatus result_status = solver->Solve();

        std::ofstream output_file(output_filename, std::ios::out);
        if (result_status == MPSolver::OPTIMAL) {
            double total_weight = 0.0;
            int items_value = 0;
            int items_count = 0;
            for (int i : all_items) {
                auto solution_value = x[i]->solution_value();
                if (solution_value > 0) {
                    output_file << "Item " << i << " weight: " << weights[i]
                        << " value: " << values[i] << " solution value: " << x[i]->solution_value() << std::endl;
                    total_weight += weights[i];
                    items_value += values[i];
                    ++items_count;
                }
            }
            output_file << "Total items value: " << items_value << std::endl;
            output_file << "Total weight: " << total_weight << std::endl;
            output_file << "Number of items: " << items_count << std::endl;

            output_file.close();

        }
        else {
            output_file << "The problem does not have an optimal solution." << std::endl;
            output_file.close();
        }
    }
}  // namespace operations_research


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    operations_research::OddKnapsackMip(input_file, output_file);
    return EXIT_SUCCESS;
}
