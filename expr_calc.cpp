/**
 * Calculator of an arbitrary mathematical expression.
 * 
 * The program is designed to put the shunting-yard algorithm into practice in C++.
 * 
 * It supports arithmetic operators (+, -, *, /, ^) and mathematical functions (sin, cos, tan, log) and constants (pi, e).
 * You can easily create additional functions and operators.
 * 
 * @author Igor Belov, first-year student of Innopolis University
 * @date February 2020
 */

#include <iostream>
#include <stdlib.h>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include <math.h>

#define PI  3.14159265358979323846 // Mathematical constant pi.
#define EXP 2.71828182845904523536 // Mathematical constant e.
#define EPS 0.00000001

const std::string DIGITS = "0123456789"; // Set of allowed digits.
const std::string OPS = "+-*/()^"; // Set of allowed operators.
const std::string LETTERS = "abcdefghijklmnopqrstuvwxqz"; // Set of allowed letters.
const std::string FUNCTIONS = "sin cos tan log"; // Set of allowed mathematical functions.

/**
 * Splits the source string into numbers and operators according to the rules.
 * 
 * @param line The source line.
 * @return Vector consisting of elements suitable for calculation.
 */ 
std::vector<std::string> SplitLine(std::string line);

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

/**
 * Checking the correct placement of parentheses.
 * 
 * @param line The mathematical expression to check.
 * @return Returns true if the expression is correct and false otherwise.
 */
bool ValidateParentheses(std::string line);

/**
 * Checking if a string is a valid number without any mistakes.
 * 
 * @param line Text string to check.
 */
bool IsNumber(std::string line);

/**
 * Additional function to display an error message and terminate program execution.
 * 
 * @param message Mandatory message for an explanation of an error. 
 */
void ThrowError(std::string message);

int main() {
    std::cout << "[Expression calculator]" << std::endl;
    std::cout << "# Enter an expression: ";
    std::string input_string;
    std::vector<std::string> splitted_line;

    std::getline(std::cin, input_string);

    if (input_string.length() > 255) ThrowError("The expression is too long"); // Max length is custom.
    if (input_string.empty()) ThrowError("An empty string was entered");
    if (!ValidateParentheses(input_string)) ThrowError("Missed parentheses");
    splitted_line = SplitLine(input_string);
    
    double result = EvaluateExpression(ParseExpression(splitted_line));
    std::cout << "# Result: " <<  result << std::endl;
    return 0;
}

void ThrowError(std::string message) {
    std::cout << "~ Error: " << message << std::endl;
    exit(0);
}

bool IsNumber(std::string line) {
    int dot_counter = 0;
    bool flag = true;
    for (int i = 0; i < line.length(); i++) {
        if (DIGITS.find(line.at(i)) == std::string::npos) {
            if (line.at(i) == '.') dot_counter += 1;
            else flag = false;
        } 
    }
    if (dot_counter > 1) flag = false;
    return flag;
}

bool ValidateParentheses(std::string line) {
    std::stack<char> p_stack;
    bool flag = true;
    for (int i = 0; i < line.length(); i++) {
        if (line.at(i) == '(') p_stack.push(line.at(i));
        else if (line.at(i) == ')') {
            if (!p_stack.empty()) {
               p_stack.pop();
            } else {
                flag = false;
            }
        }
    }
    if (!p_stack.empty()) flag = false; 
    return flag;
}

std::vector<std::string> SplitLine(std::string line) {
    std::vector<std::string> result;
    result.push_back("(");
    int i = 0;
    std::string curr = "", buff = "";
    
    while (i < line.length()) {
        curr = line.substr(i, 1);
        if (DIGITS.find(curr) != std::string::npos || LETTERS.find(curr) != std::string::npos) {
            // The current item is a digit or letter.
            int j = i;
            bool complete = false;
            buff = "";
            while (j < line.length() && !complete) {
                // Continue to collect a full number or function.
                if (DIGITS.find(curr) != std::string::npos) {
                    std::string tmp = line.substr(j, 1);
                    if ((DIGITS.find(line.substr(j, 1)) != std::string::npos) || (tmp == ".")) {
                        buff.append(line.substr(j, 1));
                        j += 1;
                    } else {
                        complete = true;
                    } 
                } else if (LETTERS.find(curr) != std::string::npos) {
                    std::string tmp = line.substr(j, 1);
                    if (LETTERS.find(line.substr(j, 1)) != std::string::npos) {
                        buff.append(line.substr(j, 1));
                        j += 1;
                    } else {
                        complete = true;
                    } 
                } 
                i = j - 1;
            }
            result.push_back(buff);
        } else if (OPS.find(curr) != std::string::npos) {
            // The current item is an operator.
            if (curr == "-" && result.back() == "(") result.push_back("0");
            result.push_back(curr);
        } else if (curr != " ") {
            ThrowError("Incorrect input \'" + curr + "\'");
        }
        i += 1;
    }
    result.push_back(")");
    return result;
}

std::stack<std::string> ParseExpression(std::vector<std::string> items) {
    // Prioritization of functions and operators using hashmap.
    std::map<std::string, int> pr_ops;
    pr_ops["+"] = 0; pr_ops["-"] = 0;
    pr_ops["*"] = 1; pr_ops["/"] = 1;
    pr_ops["^"] = 2;
    pr_ops["sin"] = 3; pr_ops["cos"] = 3; pr_ops["tan"] = 3; pr_ops["log"] = 3;
    pr_ops["("] = -1; pr_ops[")"] = -1;

    std::stack<std::string> main_stack, op_stack, result;

    for (auto p = items.begin(); p < items.end(); p++) {
        std::string curr = *p;
        if (OPS.find(curr) != std::string::npos || FUNCTIONS.find(curr) != std::string::npos) {
            // The current item is an operator or function.
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
            else if (curr == "/") {
                if (second != 0) {
                    eval_stack.push(first / second);
                } else {
                    ThrowError("Division by zero");
                }
            }
            else if (curr == "^") eval_stack.push(pow(first, second));
        } else if (FUNCTIONS.find(curr) != std::string::npos) {
            // Calculation of mathematical single argument functions.
            // Argument is taken from the top of the stack.
            double value = eval_stack.top(); eval_stack.pop();
            if (curr == "sin") eval_stack.push(sin(value));
            else if (curr == "cos") eval_stack.push(cos(value));
            else if (curr == "tan") { 
                if (abs(value - PI/2) > EPS) {
                    eval_stack.push(tan(value));
                } else {
                    ThrowError("Invalid tangent argument value");
                }
            }
            else if (curr == "log") { 
                if (value > 0) {
                    eval_stack.push(log(value)); 
                } else {
                    ThrowError("Invalid logarithm argument value");
                }
            }
        } else {
            // Moving number onto evaluation stack.
            // Mathematical constants are replaced by their values.
            if (curr == "pi") eval_stack.push(PI);
            else if (curr == "e") eval_stack.push(EXP);
            else if (IsNumber(curr)) eval_stack.push(std::stod(curr));
            else ThrowError("Invalid value \'" + curr + "\'");
        }
    }
    return eval_stack.top(); // The final result is on top of the stack.
}