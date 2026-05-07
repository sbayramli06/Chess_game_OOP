#include "square.h"
#include <stdexcept>

// ---- Constructors -----------------------------------------------------------

Square::Square(const std::string& notation) {
    if (notation.size() != 2)
        throw std::invalid_argument("Square: notation must be 2 characters, got: " + notation);

    col_ = notation[0] - 'a';   // 'a'->0 … 'h'->7
    row_ = notation[1] - '1';   // '1'->0 … '8'->7

    if (!is_valid())
        throw std::invalid_argument("Square: out-of-board notation: " + notation);
}

Square::Square(int row, int col)
    : row_(row), col_(col) {}

// ---- Methods ----------------------------------------------------------------

std::string Square::to_string() const {
    std::string s;
    s += static_cast<char>('a' + col_);
    s += static_cast<char>('1' + row_);
    return s;
}

bool Square::is_valid() const {
    return row_ >= 0 && row_ < 8 && col_ >= 0 && col_ < 8;
}

// ---- Operators --------------------------------------------------------------

bool Square::operator==(const Square& other) const {
    return row_ == other.row_ && col_ == other.col_;
}

bool Square::operator!=(const Square& other) const {
    return !(*this == other);
}
