#ifndef JEU_H
#define JEU_H

#include "echiquier.h"
#include <string>

/**
 * @brief Top-level game controller.
 *
 * Owns the Echiquier, manages turn alternation, handles user commands
 * (/quit, /resign, /draw) and drives the game loop.
 */
class Jeu {
public:
    /// Initialises a new game at the standard starting position.
    Jeu();

    // ---- Public interface used by main() ------------------------------------

    /// Prints the current board position to stdout.
    void affiche() const;

    /**
     * @brief Reads one command from stdin and processes it.
     *
     * Handles:
     *  - Normal moves in algebraic notation (e.g. "e2e4")
     *  - /quit  → sets result to ?-? and returns true
     *  - /resign → current player resigns; returns true
     *  - /draw   → mutual draw agreed; returns true
     *
     * @return true if the game should end, false to continue.
     */
    bool coup();

    /**
     * @brief Wrapper used by the main() shown in the assignment.
     *
     * Calls deplace() on the chessboard with the correct player color,
     * then switches turns if the move was valid.
     */
    void deplace(const std::string& orig, const std::string& dest);

private:
    Echiquier board_;          ///< The chessboard (owns all piece memory)
    couleur_t current_player_; ///< Whose turn it is
    std::string result_;       ///< Game result string (1-0, 0-1, 1/2-1/2, ?-?)

    /// Validates that @p cmd matches the "[a-h][1-8][a-h][1-8]" pattern.
    bool is_valid_move_input(const std::string& cmd) const;

    /// Prints the final line required by section 3 of the spec.
    void print_final_line() const;
};

#endif // JEU_H
