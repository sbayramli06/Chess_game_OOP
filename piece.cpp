#include "piece.h"
#include <iostream>
#include <cstdlib>   // abs

// =============================================================================
// Piece (base)
// =============================================================================

Piece::Piece(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : color_(color), symbol_(symbol), id_(id), pos_(pos) {}

void Piece::affiche() const {
    std::cout << symbol_;
}

// =============================================================================
// Tour (Rook)
// =============================================================================

Tour::Tour(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : Piece(color, symbol, id, pos), moved_(false) {}

bool Tour::is_legal_move_geometry(const Square& origin, const Square& dest) const {
    // Rook moves along a rank (same row) or a file (same column).
    return (origin.get_row() == dest.get_row()) != (origin.get_col() == dest.get_col());
    // The XOR ensures at least one matches but not both (which would mean no move).
}

// =============================================================================
// Cavalier (Knight)
// =============================================================================

Cavalier::Cavalier(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : Piece(color, symbol, id, pos) {}

bool Cavalier::is_legal_move_geometry(const Square& origin, const Square& dest) const {
    int dr = std::abs(dest.get_row() - origin.get_row());
    int dc = std::abs(dest.get_col() - origin.get_col());
    // L-shape: (2,1) or (1,2)
    return (dr == 2 && dc == 1) || (dr == 1 && dc == 2);
}

// =============================================================================
// Fou (Bishop)
// =============================================================================

Fou::Fou(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : Piece(color, symbol, id, pos) {}

bool Fou::is_legal_move_geometry(const Square& origin, const Square& dest) const {
    int dr = std::abs(dest.get_row() - origin.get_row());
    int dc = std::abs(dest.get_col() - origin.get_col());
    // Bishop moves diagonally: row delta == col delta, and must move.
    return dr == dc && dr != 0;
}

// =============================================================================
// Dame (Queen)
// =============================================================================

Dame::Dame(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : Piece(color, symbol, id, pos) {}

bool Dame::is_legal_move_geometry(const Square& origin, const Square& dest) const {
    int dr = std::abs(dest.get_row() - origin.get_row());
    int dc = std::abs(dest.get_col() - origin.get_col());
    // Queen = Rook OR Bishop
    bool rook_move   = (dr == 0) != (dc == 0);          // same row XOR same col
    bool bishop_move = (dr == dc) && (dr != 0);
    return rook_move || bishop_move;
}

// =============================================================================
// Roi (King)
// =============================================================================

Roi::Roi(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : Piece(color, symbol, id, pos), moved_(false) {}

bool Roi::is_legal_move_geometry(const Square& origin, const Square& dest) const {
    int dr = std::abs(dest.get_row() - origin.get_row());
    int dc = std::abs(dest.get_col() - origin.get_col());
    // King moves exactly one square in any direction.
    return dr <= 1 && dc <= 1 && (dr + dc) > 0;
}

// =============================================================================
// Pion (Pawn)
// =============================================================================

Pion::Pion(couleur_t color, const std::string& symbol, int id, const Square& pos)
    : Piece(color, symbol, id, pos), moved_(false), en_passant_vulnerable_(false) {}

bool Pion::is_legal_move_geometry(const Square& origin, const Square& dest) const {
    // Direction: white (+1) or black (-1) in the row axis.
    int direction = (color_ == Blanc) ? 1 : -1;
    int dr = dest.get_row() - origin.get_row();
    int dc = dest.get_col() - origin.get_col();

    // Forward one square (non-capture)
    if (dc == 0 && dr == direction) return true;

    // Forward two squares on first move (non-capture; collision checked in Echiquier)
    if (dc == 0 && dr == 2 * direction && !moved_) return true;

    // Diagonal capture (one square diagonally forward)
    if (std::abs(dc) == 1 && dr == direction) return true;

    return false;
}

bool Pion::is_capture_move(const Square& origin, const Square& dest) const {
    int direction = (color_ == Blanc) ? 1 : -1;
    int dr = dest.get_row() - origin.get_row();
    int dc = dest.get_col() - origin.get_col();
    return (std::abs(dc) == 1 && dr == direction);
}
