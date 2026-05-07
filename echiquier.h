#ifndef ECHIQUIER_H
#define ECHIQUIER_H

#include "piece.h"
#include <vector>
#include <string>

/**
 * @brief Represents the 8×8 chessboard and all pieces on it.
 *
 * The Echiquier (French for "chessboard") owns piece memory and is the only
 * class that directly modifies the board array.
 */
class Echiquier {
public:
    static const int NBCOL = 8;

    // ---- Constructor / Destructor -------------------------------------------

    /// Initialises the board to the standard chess starting position.
    Echiquier();

    /// Frees all dynamically allocated pieces.
    ~Echiquier();

    // Disable copy (board owns raw pointers)
    Echiquier(const Echiquier&)            = delete;
    Echiquier& operator=(const Echiquier&) = delete;

    // ---- Display ------------------------------------------------------------

    /// Prints the board to stdout in the required text format.
    void affiche() const;

    // ---- Query helpers ------------------------------------------------------

    /// True if the square contains no piece.
    bool est_case_vide(const Square& sq) const;

    /// Returns the piece pointer at @p sq (may be nullptr).
    Piece* get_piece(const Square& sq) const;

    // ---- Move execution -----------------------------------------------------

    /**
     * @brief Attempts to move the piece at @p origin to @p dest.
     *
     * Validates:
     *  - origin contains a piece of @p current_player's color
     *  - geometric legality for the piece type
     *  - no friendly piece on destination
     *  - no obstacle along the path (for sliding pieces)
     *  - pawn special rules (capture direction, 2-square advance)
     *
     * @param origin         Source square in algebraic notation.
     * @param dest           Destination square in algebraic notation.
     * @param current_player Color whose turn it is.
     * @return true if the move was executed, false if invalid.
     */
    bool deplace(const Square& origin, const Square& dest, couleur_t current_player);

    // ---- Canonical position for testing ------------------------------------

    /// Returns the board state as a comma-separated string (section 3.1).
    std::string canonical_position() const;

private:
    Piece* board_[NBCOL][NBCOL]; ///< board_[row][col], row 0 = rank 1

    /// Two vectors of piece pointers (one per color) for ownership.
    std::vector<Piece*> pieces_[2];

    // ---- Initialisation helpers ---------------------------------------------

    void alloc_board();
    void pose_piece(Piece* p, const Square& sq);

    // ---- Move validation helpers --------------------------------------------

    /// True if no piece lies between @p origin and @p dest (exclusive).
    bool path_is_clear(const Square& origin, const Square& dest) const;

    // ---- Canonical position helper ------------------------------------------

    /**
     * @brief Converts a piece's UTF-8 symbol to its PGN letter.
     * @param name        Result of piece->to_string()
     * @param view_pawn   Include 'P' for pawns when true
     * @param view_color  Prefix with 'w'/'b' when true
     */
    std::string pgn_piece_name(const std::string& name,
                               bool view_pawn,
                               bool view_color) const;
};

#endif // ECHIQUIER_H
