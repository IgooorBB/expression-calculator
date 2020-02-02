/**
 * Simplified version of calculator of an arbitrary mathematical expression.
 * 
 * The program is designed to put the shunting-yard algorithm into practice in C++.
 * 
 * It supports arithmetic operators (+, -, *, /).
 * Each element in an expression must be separated by space character from the others. Expression validation checks removed.
 * For examle: ( 1 + 2.5 ) * ( 10 - 3 * ( 5.2 - 2 ) )
 * 
 * @author Igor Belov, first-year student of Innopolis University
 * @date February 2020
 */

#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <map>

const std::string OPS = "+-*/()"; // Set of allowed operators.

/**
 * Translates a collection of elements in Reverse Polish Notation for further calculation.
 * 
 * @param items Given collection. Can be obtained using the function EvaluateExpression.
 * @return Stack of elements in Reverse Polish Notation.
 */
std::stack<std::string> ParseExpression(std::vector<std::string> items);

/**
 * Evaluates expression by using stack with elements in Reverse Polish Notation.
 * 
 * @param main_stack Stack of elements in Reverse Polish Notation.
 * @return Calculated result.
 */
double EvaluateExpression(std::stack<std::string> main_stack);

int main() {
    std::cout << "[Simplified expression calculator]" << std::endl;
    std::cout << "# Enter an expression: ";
    
    std::string input_string;
    std::vector<std::string> splitted_line;

    std::getline(std::cin, input_string);
    
    // Split entered line using getline and stream
    std::string tmp;
    std::istringstream iss(input_string);
    while (std::getline(iss, tmp, ' ')) {
        splitted_line.push_back(tmp);
    }
    
    double result = EvaluateExpression(ParseExpression(splitted_line));
    std::cout << "# Result: " <<  result << std::endl;
    
    return 0;
}

std::stack<std::string> ParseExpression(std::vector<std::string> items) {
    std::map<std::string, int> pr_ops;
    pr_ops["+"] = 0; pr_ops["-"] = 0;
    pr_ops["*"] = 1; pr_ops["/"] = 1;
    pr_ops["("] = -1; pr_ops[")"] = -1;

    std::stack<std::string> main_stack, op_stack, result;

    for (auto p = items.begin(); p < items.end(); p++) {
        std::string curr = *p;
        if (OPS.find(curr) != std::string::npos) {
            if (curr == ")") {
                // Moving items inside brackets on the main stack.
                std::string tmp = "";
                while (!op_stack.empty()) {
                    tmp = op_stack.top();
                    op_stack.pop();
                    if (tmp == "(") break; 
                    main_stack.push(tmp);
                }
            } else if (curr == "(") {
                op_stack.push(curr);
            } else {
                // The current item is an arithmetic operator.
                if (!op_stack.empty()) {
                    std::string prev = op_stack.top();
        
                    if (pr_ops[curr] > pr_ops[prev]) {
                        op_stack.push(curr);
                    } else {
                        bool complete = false;
                        while (!op_stack.empty() && !complete) {
                            std::string tmp = op_stack.top();
                            op_stack.pop();
                            if (pr_ops[curr] <= pr_ops[tmp]) {
                                main_stack.push(tmp);
                            } else {
                                op_stack.push(tmp);
                                complete = true;
                            }
                        }
                        op_stack.push(curr);
                    }
                } else {
                    op_stack.push(curr);
                }
            }
        } else {
            // The current item is a number.
            main_stack.push(curr);
        }
    }

     // Moving the remaining items onto the main stack.
    while (!op_stack.empty()) {
        main_stack.push(op_stack.top()); 
        op_stack.pop();
    }

    // Reverse the main stack and return result.
    while (!main_stack.empty()) { 
        result.push(main_stack.top());
        main_stack.pop();
    }

    return result;
}

double EvaluateExpression(std::stack<std::string> main_stack) {
    std::stack<double> eval_stack;

    while (!main_stack.empty()) {
        std::string curr = main_stack.top();
        main_stack.pop();

        if (OPS.find(curr) != std::string::npos) {
            // Calculation of binary mathematical operators.
            // Operands are taken from the top of the stack.
            double second = eval_stack.top(); eval_stack.pop();
            double first = eval_stack.top(); eval_stack.pop();
            if (curr == "+") eval_stack.push(first + second);
            else if (curr == "-") eval_stack.push(first - second);
            else if (curr == "*") eval_stack.push(first * second);
            else if (curr == "/") eval_stack.push(first / second);
        } else {
            // Moving number onto evaluation stack.
            eval_stack.push(std::stod(curr));
        }
    }
    return eval_stack.top(); // The final result is on top of the stack.
}