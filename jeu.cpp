#include "jeu.h"
#include <iostream>
#include <regex>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::string;

// =============================================================================
// Constructor
// =============================================================================

Jeu::Jeu()
    : board_(), current_player_(Blanc), result_("?-?") {}

// =============================================================================
// Display
// =============================================================================

void Jeu::affiche() const {
    board_.affiche();
}

// =============================================================================
// Input validation
// =============================================================================

bool Jeu::is_valid_move_input(const string& cmd) const {
    // Exactly "[a-h][1-8][a-h][1-8]"
    static const std::regex move_pattern("[a-h][1-8][a-h][1-8]");
    return std::regex_match(cmd, move_pattern);
}

// =============================================================================
// Final output line (spec section 3)
// =============================================================================

void Jeu::print_final_line() const {
    cout << board_.canonical_position() << " " << result_ << "\n";
}

// =============================================================================
// deplace() — public wrapper used by the sample main()
// =============================================================================

void Jeu::deplace(const string& orig, const string& dest) {
    // Validate algebraic notation first
    string combined = orig + dest;
    if (!is_valid_move_input(combined)) {
        cout << "Error: \"" << combined
             << "\" is not valid algebraic notation (e.g. e2e4).\n";
        return;
    }

    Square from(orig);
    Square to(dest);

    bool moved = board_.deplace(from, to, current_player_);
    if (moved) {
        // Switch turns
        current_player_ = (current_player_ == Blanc) ? Noir : Blanc;
    }
}

// =============================================================================
// coup() — reads one command and acts on it; returns true when game ends
// =============================================================================

bool Jeu::coup() {
    string cmd;

    // Keep prompting until we get a valid move or a terminal command
    while (true) {
        cout << "Move (eg. a1a8) ? ";
        if (!(cin >> cmd)) {
            // EOF reached (e.g. piped input exhausted)
            result_ = "?-?";
            print_final_line();
            return true;
        }

        // ---- Terminal commands -----------------------------------------------

        if (cmd == "/quit") {
            result_ = "?-?";
            print_final_line();
            return true;
        }

        if (cmd == "/resign") {
            // The player who resigns loses
            result_ = (current_player_ == Blanc) ? "0-1" : "1-0";
            cout << (current_player_ == Blanc ? "White" : "Black")
                 << " resigns.\n";
            affiche();
            print_final_line();
            return true;
        }

        if (cmd == "/draw") {
            result_ = "1/2-1/2";
            cout << "Draw agreed.\n";
            affiche();
            print_final_line();
            return true;
        }

        // ---- Normal move ----------------------------------------------------

        if (!is_valid_move_input(cmd)) {
            cout << "Error: \"" << cmd
                 << "\" is not valid (expected e.g. e2e4, /quit, /resign, /draw).\n";
            continue;   // prompt again
        }

        string orig = cmd.substr(0, 2);
        string dest = cmd.substr(2, 2);

        Square from(orig);
        Square to(dest);

        bool moved = board_.deplace(from, to, current_player_);
        if (moved) {
            current_player_ = (current_player_ == Blanc) ? Noir : Blanc;
            affiche();
            return false;   // game continues
        }
        // If move was invalid, board_.deplace() already printed the error;
        // loop back and ask again.
    }
}
