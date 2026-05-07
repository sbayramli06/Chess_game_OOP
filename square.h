#ifndef SQUARE_H
#define SQUARE_H

#include <string>
#include <stdexcept>

/**
 * @brief Represents a single square on the chessboard.
 *
 * Provides conversion between algebraic notation (e.g. "a1") and
 * zero-based (row, col) array indices used internally.
 *  row 0 == rank 1  (white's first rank)
 *  col 0 == file 'a'
 */
class Square {
public:
    // ---- Constructors -------------------------------------------------------

    /// Construct from algebraic notation, e.g. Square("a1") -> row=0, col=0.
    explicit Square(const std::string& notation);

    /// Construct directly from zero-based indices.
    Square(int row, int col);

    // ---- Accessors ----------------------------------------------------------

    int get_row() const { return row_; }
    int get_col() const { return col_; }

    /// Returns algebraic notation, e.g. "a1" for (0,0).
    std::string to_string() const;

    /// True if the square is within the 8×8 board.
    bool is_valid() const;

    // ---- Operators ----------------------------------------------------------
    bool operator==(const Square& other) const;
    bool operator!=(const Square& other) const;

private:
    int row_; ///< 0-based rank  (0 = rank 1)
    int col_; ///< 0-based file  (0 = file 'a')
};

#endif // SQUARE_H
