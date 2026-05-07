#ifndef PIECE_H
#define PIECE_H

#include "square.h"
#include <string>

/// Color enumeration shared across all classes.
typedef enum { Blanc = 0, Noir = 1 } couleur_t;

/**
 * @brief Abstract base class for all chess pieces.
 *
 * Stores color, UTF-8 display symbol, unique id, and current position.
 * Subclasses must implement is_legal_move_geometry() which checks only the
 * geometric legality of the move (no collision / capture logic here).
 */
class Piece {
public:
    // ---- Constructor / Destructor -------------------------------------------

    /**
     * @param color   Blanc or Noir
     * @param symbol  UTF-8 string used for display (e.g. "\u2656" for ♖)
     * @param id      Unique identifier for the piece
     * @param pos     Initial square
     */
    Piece(couleur_t color, const std::string& symbol, int id, const Square& pos);

    virtual ~Piece() = default;

    // ---- Pure virtual -------------------------------------------------------

    /**
     * @brief Checks whether the move from @p origin to @p dest is
     *        geometrically legal for this piece type.
     *
     * This does NOT check collisions, captures, or turn order.
     * @return true if the move shape is valid for this piece.
     */
    virtual bool is_legal_move_geometry(const Square& origin,
                                        const Square& dest) const = 0;

    /**
     * @brief Returns true if the piece moves in a sliding fashion
     *        (Rook, Bishop, Queen) so the caller can check for obstacles.
     */
    virtual bool is_sliding() const { return false; }

    // ---- Non-virtual accessors ----------------------------------------------

    couleur_t   get_color()  const { return color_; }
    Square      get_pos()    const { return pos_; }
    int         get_id()     const { return id_; }
    std::string to_string()  const { return symbol_; }

    /// Update internal position (called by Echiquier after a successful move).
    void set_pos(const Square& sq) { pos_ = sq; }

    /// Print the UTF-8 symbol to stdout.
    void affiche() const;

    // ---- Helpers ------------------------------------------------------------

    /// Direction helpers: sign of an integer.
    static int sign(int x) { return (x > 0) - (x < 0); }

protected:
    couleur_t   color_;
    std::string symbol_;
    int         id_;
    Square      pos_;
};

// =============================================================================
// Concrete piece subclasses
// =============================================================================

/// Rook – moves along ranks or files.
class Tour : public Piece {
public:
    Tour(couleur_t color, const std::string& symbol, int id, const Square& pos);
    bool is_legal_move_geometry(const Square& origin, const Square& dest) const override;
    bool is_sliding() const override { return true; }
};

/// Knight – L-shaped jump, can leap over pieces.
class Cavalier : public Piece {
public:
    Cavalier(couleur_t color, const std::string& symbol, int id, const Square& pos);
    bool is_legal_move_geometry(const Square& origin, const Square& dest) const override;
};

/// Bishop – moves diagonally.
class Fou : public Piece {
public:
    Fou(couleur_t color, const std::string& symbol, int id, const Square& pos);
    bool is_legal_move_geometry(const Square& origin, const Square& dest) const override;
    bool is_sliding() const override { return true; }
};

/// Queen – combines Rook and Bishop.
class Dame : public Piece {
public:
    Dame(couleur_t color, const std::string& symbol, int id, const Square& pos);
    bool is_legal_move_geometry(const Square& origin, const Square& dest) const override;
    bool is_sliding() const override { return true; }
};

/// King – moves one square in any direction.
class Roi : public Piece {
public:
    Roi(couleur_t color, const std::string& symbol, int id, const Square& pos);
    bool is_legal_move_geometry(const Square& origin, const Square& dest) const override;
};

/**
 * @brief Pawn – moves forward, captures diagonally.
 *
 * Direction depends on color: white pawns go up (row increases),
 * black pawns go down (row decreases).
 * The "first move" flag allows a 2-square advance.
 */
class Pion : public Piece {
public:
    Pion(couleur_t color, const std::string& symbol, int id, const Square& pos);

    bool is_legal_move_geometry(const Square& origin, const Square& dest) const override;

    bool has_moved() const { return moved_; }
    void set_moved()       { moved_ = true; }

    /**
     * @brief Checks if dest is a valid diagonal capture square for this pawn.
     * @param origin current square of the pawn
     * @param dest   target square
     */
    bool is_capture_move(const Square& origin, const Square& dest) const;

private:
    bool moved_; ///< false until the pawn makes its first move
};

#endif // PIECE_H
