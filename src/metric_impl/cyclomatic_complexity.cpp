#include "metric_impl/cyclomatic_complexity.hpp"

#include <cstddef>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyzer::metric::metric_impl {
std::string CyclomaticComplexityMetric::Name() const { return kName; }
MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    // Получаем строковое представление AST (абстрактного синтаксического дерева) функции.
    // Это S-выражение, сгенерированное утилитой tree-sitter, например:
    // "(function_definition name: (identifier) ... (if_statement ...) (for_statement ...))"
    auto &function_ast = f.ast;

    // Список типов узлов AST, каждый из которых увеличивает цикломатическую сложность на 1.
    // Эти узлы соответствуют управляющим конструкциям языка Python:
    // - if / elif
    // - циклы (for, while)
    // - обработка исключений (try, finally)
    // - case в match-выражениях
    // - assert
    // - тернарный оператор (conditional_expression)
    constexpr std::array<std::string_view, 9> complexity_nodes = {
        "if_statement",            // if
        "elif_statement",          // elif
        "for_statement",           // for
        "while_statement",         // while
        "try_statement",           // try
        "finally_clause",          // finally
        "case_clause",             // case
        "assert",                  // assert
        "conditional_expression",  // для тернарного оператора
    };

    auto node_type_count = [&](const std::string_view node_type) {
        std::size_t count = 0;
        std::size_t current_pos = 0;
        while ((current_pos = function_ast.find(node_type, current_pos)) != std::string::npos) {
            count++;
            current_pos += node_type.length();
        }

        return count;
    };

    auto counts_view = complexity_nodes | std::views::transform(node_type_count);

    return std::ranges::fold_left(counts_view, 1, std::plus<int>());
}
}  // namespace analyzer::metric::metric_impl
