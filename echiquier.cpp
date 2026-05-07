#include "echiquier.h"
#include <iostream>
#include <cstdlib>   // abs
#include <string>

using std::cout;
using std::endl;
using std::string;

// =============================================================================
// Constructor / Destructor
// =============================================================================

void Echiquier::alloc_board() {
    for (int r = 0; r < NBCOL; ++r)
        for (int c = 0; c < NBCOL; ++c)
            board_[r][c] = nullptr;
}

void Echiquier::pose_piece(Piece* p, const Square& sq) {
    board_[sq.get_row()][sq.get_col()] = p;
}

Echiquier::Echiquier() {
    alloc_board();

    // ---- White pieces (row 0 = rank 1) ----
    pieces_[Blanc] = {
        new Tour    (Blanc, "\u2656", 0,  Square(0, 0)),
        new Cavalier(Blanc, "\u2658", 1,  Square(0, 1)),
        new Fou     (Blanc, "\u2657", 2,  Square(0, 2)),
        new Dame    (Blanc, "\u2655", 3,  Square(0, 3)),
        new Roi     (Blanc, "\u2654", 4,  Square(0, 4)),
        new Fou     (Blanc, "\u2657", 5,  Square(0, 5)),
        new Cavalier(Blanc, "\u2658", 6,  Square(0, 6)),
        new Tour    (Blanc, "\u2656", 7,  Square(0, 7))
    };

    // ---- Black pieces (row 7 = rank 8) ----
    pieces_[Noir] = {
        new Tour    (Noir,  "\u265C", 8,  Square(7, 0)),
        new Cavalier(Noir,  "\u265E", 9,  Square(7, 1)),
        new Fou     (Noir,  "\u265D", 10, Square(7, 2)),
        new Dame    (Noir,  "\u265B", 11, Square(7, 3)),
        new Roi     (Noir,  "\u265A", 12, Square(7, 4)),
        new Fou     (Noir,  "\u265D", 13, Square(7, 5)),
        new Cavalier(Noir,  "\u265E", 14, Square(7, 6)),
        new Tour    (Noir,  "\u265C", 15, Square(7, 7))
    };

    // ---- Pawns ----
    for (int i = 0; i < 8; ++i) {
        // White pawns on rank 2 (row 1)
        pieces_[Blanc].push_back(
            new Pion(Blanc, "\u2659", 16 + i, Square(1, i)));
        // Black pawns on rank 7 (row 6)
        pieces_[Noir].push_back(
            new Pion(Noir,  "\u265F", 24 + i, Square(6, i)));
    }

    // Place every piece on the board
    for (auto p : pieces_[Blanc]) pose_piece(p, p->get_pos());
    for (auto p : pieces_[Noir])  pose_piece(p, p->get_pos());
}

Echiquier::~Echiquier() {
    for (auto p : pieces_[Blanc]) delete p;
    for (auto p : pieces_[Noir])  delete p;
}

// =============================================================================
// Display
// =============================================================================

void Echiquier::affiche() const {
    const string sep   = "  +-----+-----+-----+-----+-----+-----+-----+-----+";
    const string space5 = string(5, ' ');

    cout << "\n";
    cout << "     a     b     c     d     e     f     g     h    \n";
    cout << sep << "\n";

    // Print from row 7 (rank 8) down to row 0 (rank 1)
    for (int r = NBCOL - 1; r >= 0; --r) {
        cout << r + 1 << " ";          // rank number on the left
        for (int c = 0; c < NBCOL; ++c) {
            cout << "|";
            if (board_[r][c]) {
                // Two UTF-8 spaces so unicode chess glyphs align properly
                cout << "\u0020\u0020";
                board_[r][c]->affiche();
                cout << "\u0020 ";
            } else {
                cout << space5;
            }
        }
        cout << "|\n" << sep << "\n";
    }
}

// =============================================================================
// Query helpers
// =============================================================================

bool Echiquier::est_case_vide(const Square& sq) const {
    return board_[sq.get_row()][sq.get_col()] == nullptr;
}

Piece* Echiquier::get_piece(const Square& sq) const {
    return board_[sq.get_row()][sq.get_col()];
}

// =============================================================================
// Path-clear check (for sliding pieces: Rook, Bishop, Queen)
// =============================================================================

bool Echiquier::path_is_clear(const Square& origin, const Square& dest) const {
    int dr = Piece::sign(dest.get_row() - origin.get_row());
    int dc = Piece::sign(dest.get_col() - origin.get_col());

    int r = origin.get_row() + dr;
    int c = origin.get_col() + dc;

    // Walk every intermediate square (exclusive of origin and dest)
    while (r != dest.get_row() || c != dest.get_col()) {
        if (board_[r][c] != nullptr) return false;   // obstacle found
        r += dr;
        c += dc;
    }
    return true;
}

// =============================================================================
// Move execution
// =============================================================================

bool Echiquier::deplace(const Square& origin, const Square& dest,
                        couleur_t current_player) {

    // 1. Origin must contain a piece
    Piece* moving = get_piece(origin);
    if (!moving) {
        cout << "Error: no piece on " << origin.to_string() << ".\n";
        return false;
    }

    // 2. That piece must belong to the current player
    if (moving->get_color() != current_player) {
        cout << "Error: that piece does not belong to "
             << (current_player == Blanc ? "White" : "Black") << ".\n";
        return false;
    }

    // 3. Destination must be on the board (Square constructor already checked,
    //    but the Square passed here could have been built externally)
    if (!dest.is_valid()) {
        cout << "Error: destination " << dest.to_string() << " is off the board.\n";
        return false;
    }

    Piece* target = get_piece(dest);

    // 4. Cannot capture own piece
    if (target && target->get_color() == current_player) {
        cout << "Error: destination " << dest.to_string()
             << " is occupied by your own piece.\n";
        return false;
    }

    // 5. Geometric legality
    // Pawn needs special treatment: diagonal move only allowed when capturing
    Pion* pawn = dynamic_cast<Pion*>(moving);
    if (pawn) {
        bool is_capture = pawn->is_capture_move(origin, dest);

        if (is_capture && !target) {
            // Diagonal move but nothing to capture
            cout << "Error: pawn can only move diagonally when capturing.\n";
            return false;
        }
        if (!is_capture && target) {
            // Forward move blocked by a piece
            cout << "Error: pawn's forward path is blocked.\n";
            return false;
        }
        if (!pawn->is_legal_move_geometry(origin, dest)) {
            cout << "Error: illegal pawn move.\n";
            return false;
        }

        // For a 2-square advance, the intermediate square must be empty too
        if (!is_capture) {
            int direction = (pawn->get_color() == Blanc) ? 1 : -1;
            if (dest.get_row() - origin.get_row() == 2 * direction) {
                Square mid(origin.get_row() + direction, origin.get_col());
                if (!est_case_vide(mid)) {
                    cout << "Error: pawn's path is blocked.\n";
                    return false;
                }
            }
        }
    } else {
        // Non-pawn geometric check
        if (!moving->is_legal_move_geometry(origin, dest)) {
            cout << "Error: illegal move for "
                 << moving->to_string() << ".\n";
            return false;
        }

        // 6. Obstacle check for sliding pieces
        if (moving->is_sliding() && !path_is_clear(origin, dest)) {
            cout << "Error: path from " << origin.to_string()
                 << " to " << dest.to_string() << " is blocked.\n";
            return false;
        }
    }

    // ---- All checks passed: execute the move --------------------------------

    // Print move confirmation
    cout << "\n-> move of " << moving->to_string() << "  "
         << origin.to_string() << dest.to_string() << "\n";

    // Remove captured piece from ownership vector
    if (target) {
        couleur_t enemy = (current_player == Blanc) ? Noir : Blanc;
        auto& vec = pieces_[enemy];
        vec.erase(std::remove(vec.begin(), vec.end(), target), vec.end());
        delete target;
    }

    // Move on the board
    board_[dest.get_row()][dest.get_col()]     = moving;
    board_[origin.get_row()][origin.get_col()] = nullptr;
    moving->set_pos(dest);

    // Mark pawn as having moved (enables 2-square-advance restriction)
    if (pawn) pawn->set_moved();

    return true;
}

// =============================================================================
// Canonical position (for automated testing, section 3.1)
// =============================================================================

string Echiquier::pgn_piece_name(const string& name,
                                  bool view_pawn,
                                  bool view_color) const {
    string psymb;

    // White pieces
    if      (name == "\u2656") psymb = "R";
    else if (name == "\u2658") psymb = "N";
    else if (name == "\u2657") psymb = "B";
    else if (name == "\u2655") psymb = "Q";
    else if (name == "\u2654") psymb = "K";
    else if (name == "\u2659" && view_pawn) psymb = "P";

    if (!psymb.empty()) {
        return view_color ? "w" + psymb : psymb;
    }

    // Black pieces
    if      (name == "\u265C") psymb = "R";
    else if (name == "\u265E") psymb = "N";
    else if (name == "\u265D") psymb = "B";
    else if (name == "\u265B") psymb = "Q";
    else if (name == "\u265A") psymb = "K";
    else if (name == "\u265F" && view_pawn) psymb = "P";

    if (!psymb.empty()) {
        return view_color ? "b" + psymb : psymb;
    }

    return "";
}

string Echiquier::canonical_position() const {
    string output;
    // Traverse a1→h1, a2→h2, … a8→h8  (row 0 first, matching spec)
    for (int row = 0; row < NBCOL; ++row) {
        for (int col = 0; col < NBCOL; ++col) {
            Piece* p = board_[row][col];
            if (p) {
                output += pgn_piece_name(p->to_string(), true, true);
            }
            output += ",";
        }
    }
    return output;
}
