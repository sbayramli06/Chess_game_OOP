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

    /// Returns the vector of pieces for a given color.
    const std::vector<Piece*>& get_pieces(couleur_t color) const { return pieces_[color]; }

    // ---- Move execution -----------------------------------------------------

    /**
     * @brief Attempts to move the piece at @p origin to @p dest.
     *
     * Validates:
     *  - origin contains a piece of @p current_player's color
     *  - geometric legality for the piece type
     *  - no friendly piece on destination
     *  - no obstacle along the path (for sliding pieces)
     *  - pawn special rules (capture direction, 2-square advance, en passant)
     *  - the move does not leave the current player's king in check
     *
     * @param origin         Source square.
     * @param dest           Destination square.
     * @param current_player Color whose turn it is.
     * @return true if the move was executed, false if invalid.
     */
    bool deplace(const Square& origin, const Square& dest, couleur_t current_player);

    // ---- Check detection (Level 2) ------------------------------------------

    /**
     * @brief Returns true if the king of @p color is currently in check.
     *
     * Scans all pieces of the opposing color and tests whether any of them
     * could geometrically reach the king's square with a clear path.
     */
    bool is_in_check(couleur_t color) const;



    /**
     * @brief Attempts to perform kingside castling (O-O) for @p color.
     * @return true if castling was successfully performed.
     */
    bool do_kingside_castling(couleur_t color);

    /**
     * @brief Attempts to perform queenside castling (O-O-O) for @p color.
     * @return true if castling was successfully performed.
     */
    bool do_queenside_castling(couleur_t color);

    // ---- Promotion (Level 3) ------------------------------------------------

    /**
     * @brief Handles pawn promotion if the last moved pawn reached the final rank.
     *
     * Prompts the user for a piece type (Q, R, B, N), removes the pawn,
     * and replaces it with the chosen piece on the board.
     *
     * @param sq    The square where the pawn just arrived.
     * @param color The color of the pawn being promoted.
     */
    void handle_promotion(const Square& sq, couleur_t color);

    // ---- En passant bookkeeping ---------------------------------------------

    /// Clear en passant vulnerability for all pawns of @p color.
    void clear_en_passant(couleur_t color);

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

    /**
     * @brief Tests whether a hypothetical move from origin to dest would leave
     *        the current player's king in check.
     *
     * Temporarily performs the move, checks for check, and undoes it.
     * @return true if the move is safe (king NOT in check after).
     */
    bool move_is_safe(const Square& origin, const Square& dest, couleur_t color);

    /**
     * @brief Finds the king piece for @p color.
     * @return Pointer to the king, or nullptr if not found (should never happen).
     */
    Piece* find_king(couleur_t color) const;

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
