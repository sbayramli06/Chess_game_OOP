#include "echiquier.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>

using std::cout; using std::cin; using std::endl; using std::string;

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
    pieces_[Noir] = {
        new Tour    (Noir, "\u265C", 8,  Square(7, 0)),
        new Cavalier(Noir, "\u265E", 9,  Square(7, 1)),
        new Fou     (Noir, "\u265D", 10, Square(7, 2)),
        new Dame    (Noir, "\u265B", 11, Square(7, 3)),
        new Roi     (Noir, "\u265A", 12, Square(7, 4)),
        new Fou     (Noir, "\u265D", 13, Square(7, 5)),
        new Cavalier(Noir, "\u265E", 14, Square(7, 6)),
        new Tour    (Noir, "\u265C", 15, Square(7, 7))
    };
    for (int i = 0; i < 8; ++i) {
        pieces_[Blanc].push_back(new Pion(Blanc, "\u2659", 16 + i, Square(1, i)));
        pieces_[Noir].push_back(new Pion(Noir, "\u265F", 24 + i, Square(6, i)));
    }
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
    for (int r = NBCOL - 1; r >= 0; --r) {
        cout << r + 1 << " ";
        for (int c = 0; c < NBCOL; ++c) {
            cout << "|";
            if (board_[r][c]) {
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

Piece* Echiquier::find_king(couleur_t color) const {
    for (auto p : pieces_[color])
        if (dynamic_cast<Roi*>(p)) return p;
    return nullptr;
}

// =============================================================================
// Path-clear check (for sliding pieces)
// =============================================================================

bool Echiquier::path_is_clear(const Square& origin, const Square& dest) const {
    int dr = Piece::sign(dest.get_row() - origin.get_row());
    int dc = Piece::sign(dest.get_col() - origin.get_col());
    int r = origin.get_row() + dr;
    int c = origin.get_col() + dc;
    while (r != dest.get_row() || c != dest.get_col()) {
        if (board_[r][c] != nullptr) return false;
        r += dr; c += dc;
    }
    return true;
}

// =============================================================================
// En passant bookkeeping
// =============================================================================

void Echiquier::clear_en_passant(couleur_t color) {
    for (auto p : pieces_[color]) {
        Pion* pawn = dynamic_cast<Pion*>(p);
        if (pawn) pawn->set_en_passant_vulnerable(false);
    }
}

// =============================================================================
// Check detection (Level 2)
// =============================================================================

bool Echiquier::is_in_check(couleur_t color) const {
    Piece* king = find_king(color);
    if (!king) return false;
    Square king_sq = king->get_pos();
    couleur_t enemy = (color == Blanc) ? Noir : Blanc;

    for (auto p : pieces_[enemy]) {
        Square from = p->get_pos();
        // Check if this enemy piece can geometrically attack the king
        if (!p->is_legal_move_geometry(from, king_sq)) continue;

        // Pawn: only attacks diagonally, not forward
        Pion* epawn = dynamic_cast<Pion*>(p);
        if (epawn) {
            if (epawn->is_capture_move(from, king_sq))
                return true;
            continue;
        }
        // Sliding pieces need a clear path
        if (p->is_sliding()) {
            if (path_is_clear(from, king_sq)) return true;
        } else {
            // Knight or King – no path check needed
            return true;
        }
    }
    return false;
}

// =============================================================================
// Safe-move test: temporarily do the move, check for check, undo
// =============================================================================

bool Echiquier::move_is_safe(const Square& origin, const Square& dest, couleur_t color) {
    Piece* moving = board_[origin.get_row()][origin.get_col()];
    Piece* target = board_[dest.get_row()][dest.get_col()];

    // Temporarily execute
    board_[dest.get_row()][dest.get_col()] = moving;
    board_[origin.get_row()][origin.get_col()] = nullptr;
    Square old_pos = moving->get_pos();
    moving->set_pos(dest);

    // If target was captured, temporarily remove from enemy list
    couleur_t enemy = (color == Blanc) ? Noir : Blanc;
    bool removed = false;
    if (target) {
        auto& vec = pieces_[enemy];
        auto it = std::find(vec.begin(), vec.end(), target);
        if (it != vec.end()) { vec.erase(it); removed = true; }
    }

    bool safe = !is_in_check(color);

    // Undo
    moving->set_pos(old_pos);
    board_[origin.get_row()][origin.get_col()] = moving;
    board_[dest.get_row()][dest.get_col()] = target;
    if (removed) pieces_[enemy].push_back(target);

    return safe;
}



// =============================================================================
// Castling (Level 3)
// =============================================================================

bool Echiquier::do_kingside_castling(couleur_t color) {
    int row = (color == Blanc) ? 0 : 7;
    Roi* king = dynamic_cast<Roi*>(board_[row][4]);
    Tour* rook = dynamic_cast<Tour*>(board_[row][7]);

    if (!king || !rook) {
        cout << "Error: king or rook not in place for castling.\n"; return false;
    }
    if (king->has_moved()) {
        cout << "Error: king has already moved.\n"; return false;
    }
    if (rook->has_moved()) {
        cout << "Error: rook has already moved.\n"; return false;
    }
    if (board_[row][5] || board_[row][6]) {
        cout << "Error: squares between king and rook are not empty.\n"; return false;
    }

    // Execute castling
    board_[row][6] = king;  board_[row][4] = nullptr;
    king->set_pos(Square(row, 6)); king->set_moved();
    board_[row][5] = rook;  board_[row][7] = nullptr;
    rook->set_pos(Square(row, 5)); rook->set_moved();

    cout << "\n-> O-O (" << (color == Blanc ? "White" : "Black") << ")\n";
    return true;
}

bool Echiquier::do_queenside_castling(couleur_t color) {
    int row = (color == Blanc) ? 0 : 7;
    Roi* king = dynamic_cast<Roi*>(board_[row][4]);
    Tour* rook = dynamic_cast<Tour*>(board_[row][0]);

    if (!king || !rook) {
        cout << "Error: king or rook not in place for castling.\n"; return false;
    }
    if (king->has_moved()) {
        cout << "Error: king has already moved.\n"; return false;
    }
    if (rook->has_moved()) {
        cout << "Error: rook has already moved.\n"; return false;
    }
    if (board_[row][1] || board_[row][2] || board_[row][3]) {
        cout << "Error: squares between king and rook are not empty.\n"; return false;
    }

    board_[row][2] = king;  board_[row][4] = nullptr;
    king->set_pos(Square(row, 2)); king->set_moved();
    board_[row][3] = rook;  board_[row][0] = nullptr;
    rook->set_pos(Square(row, 3)); rook->set_moved();

    cout << "\n-> O-O-O (" << (color == Blanc ? "White" : "Black") << ")\n";
    return true;
}

// =============================================================================
// Promotion (Level 3)
// =============================================================================

void Echiquier::handle_promotion(const Square& sq, couleur_t color) {
    int promo_row = (color == Blanc) ? 7 : 0;
    if (sq.get_row() != promo_row) return;

    Piece* p = get_piece(sq);
    Pion* pawn = dynamic_cast<Pion*>(p);
    if (!pawn) return;

    cout << "Promotion! Choose piece (Q, R, B, N): ";
    string choice;
    cin >> choice;

    // Remove old pawn from pieces vector
    auto& vec = pieces_[color];
    vec.erase(std::remove(vec.begin(), vec.end(), pawn), vec.end());
    board_[sq.get_row()][sq.get_col()] = nullptr;
    int old_id = pawn->get_id();
    delete pawn;

    // Create the new piece
    Piece* promoted = nullptr;
    if (color == Blanc) {
        if      (choice == "Q") promoted = new Dame(Blanc, "\u2655", old_id, sq);
        else if (choice == "R") promoted = new Tour(Blanc, "\u2656", old_id, sq);
        else if (choice == "B") promoted = new Fou(Blanc, "\u2657", old_id, sq);
        else if (choice == "N") promoted = new Cavalier(Blanc, "\u2658", old_id, sq);
        else                    promoted = new Dame(Blanc, "\u2655", old_id, sq); // default Q
    } else {
        if      (choice == "Q") promoted = new Dame(Noir, "\u265B", old_id, sq);
        else if (choice == "R") promoted = new Tour(Noir, "\u265C", old_id, sq);
        else if (choice == "B") promoted = new Fou(Noir, "\u265D", old_id, sq);
        else if (choice == "N") promoted = new Cavalier(Noir, "\u265E", old_id, sq);
        else                    promoted = new Dame(Noir, "\u265B", old_id, sq);
    }

    vec.push_back(promoted);
    board_[sq.get_row()][sq.get_col()] = promoted;
}

// =============================================================================
// Move execution
// =============================================================================

bool Echiquier::deplace(const Square& origin, const Square& dest,
                        couleur_t current_player) {

    Piece* moving = get_piece(origin);
    if (!moving) {
        cout << "Error: no piece on " << origin.to_string() << ".\n";
        return false;
    }
    if (moving->get_color() != current_player) {
        cout << "Error: that piece does not belong to "
             << (current_player == Blanc ? "White" : "Black") << ".\n";
        return false;
    }
    if (!dest.is_valid()) {
        cout << "Error: destination " << dest.to_string() << " is off the board.\n";
        return false;
    }

    Piece* target = get_piece(dest);

    if (target && target->get_color() == current_player) {
        cout << "Error: destination " << dest.to_string()
             << " is occupied by your own piece.\n";
        return false;
    }

    // --- Pawn special handling ---
    Pion* pawn = dynamic_cast<Pion*>(moving);
    bool is_en_passant = false;

    if (pawn) {
        bool is_capture = pawn->is_capture_move(origin, dest);

        if (is_capture && !target) {
            // Could be en passant
            int ep_row = (current_player == Blanc) ? 4 : 3;
            if (origin.get_row() == ep_row) {
                Piece* adj = board_[origin.get_row()][dest.get_col()];
                Pion* adj_pawn = dynamic_cast<Pion*>(adj);
                if (adj_pawn && adj_pawn->get_color() != current_player &&
                    adj_pawn->is_en_passant_vulnerable()) {
                    is_en_passant = true;
                } else {
                    cout << "Error: pawn can only move diagonally when capturing.\n";
                    return false;
                }
            } else {
                cout << "Error: pawn can only move diagonally when capturing.\n";
                return false;
            }
        }
        if (!is_capture && target) {
            cout << "Error: pawn's forward path is blocked.\n";
            return false;
        }
        if (!pawn->is_legal_move_geometry(origin, dest)) {
            cout << "Error: illegal pawn move.\n";
            return false;
        }
        // 2-square advance path check
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
        if (!moving->is_legal_move_geometry(origin, dest)) {
            cout << "Error: illegal move for " << moving->to_string() << ".\n";
            return false;
        }
        if (moving->is_sliding() && !path_is_clear(origin, dest)) {
            cout << "Error: path from " << origin.to_string()
                 << " to " << dest.to_string() << " is blocked.\n";
            return false;
        }
    }

    // --- Safety check: move must not leave own king in check ---
    // For en passant we need a special simulation
    if (is_en_passant) {
        // Temporarily do en passant
        Piece* captured_ep = board_[origin.get_row()][dest.get_col()];
        board_[dest.get_row()][dest.get_col()] = moving;
        board_[origin.get_row()][origin.get_col()] = nullptr;
        board_[origin.get_row()][dest.get_col()] = nullptr;
        Square old_pos = moving->get_pos();
        moving->set_pos(dest);

        couleur_t enemy = (current_player == Blanc) ? Noir : Blanc;
        auto& evec = pieces_[enemy];
        auto it = std::find(evec.begin(), evec.end(), captured_ep);
        bool removed = false;
        if (it != evec.end()) { evec.erase(it); removed = true; }

        bool safe = !is_in_check(current_player);

        // Undo
        moving->set_pos(old_pos);
        board_[origin.get_row()][origin.get_col()] = moving;
        board_[dest.get_row()][dest.get_col()] = nullptr;
        board_[origin.get_row()][dest.get_col()] = captured_ep;
        if (removed) evec.push_back(captured_ep);

        if (!safe) {
            cout << "Error: move would leave your king in check.\n";
            return false;
        }
    } else {
        if (!move_is_safe(origin, dest, current_player)) {
            cout << "Error: move would leave your king in check.\n";
            return false;
        }
    }

    // ---- All checks passed: execute the move --------------------------------
    cout << "\n-> move of " << moving->to_string() << "  "
         << origin.to_string() << dest.to_string() << "\n";

    // Handle en passant capture
    if (is_en_passant) {
        Piece* captured_ep = board_[origin.get_row()][dest.get_col()];
        couleur_t enemy = (current_player == Blanc) ? Noir : Blanc;
        auto& vec = pieces_[enemy];
        vec.erase(std::remove(vec.begin(), vec.end(), captured_ep), vec.end());
        board_[origin.get_row()][dest.get_col()] = nullptr;
        delete captured_ep;
        cout << "  (en passant capture)\n";
    }

    // Remove normally captured piece
    if (target) {
        couleur_t enemy = (current_player == Blanc) ? Noir : Blanc;
        auto& vec = pieces_[enemy];
        vec.erase(std::remove(vec.begin(), vec.end(), target), vec.end());
        delete target;
    }

    // Move on the board
    board_[dest.get_row()][dest.get_col()] = moving;
    board_[origin.get_row()][origin.get_col()] = nullptr;
    moving->set_pos(dest);

    // Mark pawn as moved + set en passant vulnerability
    if (pawn) {
        int direction = (pawn->get_color() == Blanc) ? 1 : -1;
        if (dest.get_row() - origin.get_row() == 2 * direction) {
            pawn->set_en_passant_vulnerable(true);
        }
        pawn->set_moved();
    }

    // Mark king/rook as moved (for castling tracking)
    Roi* king = dynamic_cast<Roi*>(moving);
    if (king) king->set_moved();
    Tour* rook = dynamic_cast<Tour*>(moving);
    if (rook) rook->set_moved();

    // Handle promotion
    handle_promotion(dest, current_player);

    return true;
}

// =============================================================================
// Canonical position
// =============================================================================

string Echiquier::pgn_piece_name(const string& name, bool view_pawn, bool view_color) const {
    string psymb;
    if      (name == "\u2656") psymb = "R";
    else if (name == "\u2658") psymb = "N";
    else if (name == "\u2657") psymb = "B";
    else if (name == "\u2655") psymb = "Q";
    else if (name == "\u2654") psymb = "K";
    else if (name == "\u2659" && view_pawn) psymb = "P";
    if (!psymb.empty()) return view_color ? "w" + psymb : psymb;

    if      (name == "\u265C") psymb = "R";
    else if (name == "\u265E") psymb = "N";
    else if (name == "\u265D") psymb = "B";
    else if (name == "\u265B") psymb = "Q";
    else if (name == "\u265A") psymb = "K";
    else if (name == "\u265F" && view_pawn) psymb = "P";
    if (!psymb.empty()) return view_color ? "b" + psymb : psymb;

    return "";
}

string Echiquier::canonical_position() const {
    string output;
    for (int row = 0; row < NBCOL; ++row) {
        for (int col = 0; col < NBCOL; ++col) {
            Piece* p = board_[row][col];
            if (p) output += pgn_piece_name(p->to_string(), true, true);
            output += ",";
        }
    }
    return output;
}
