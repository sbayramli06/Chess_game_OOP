# Chess Game OOP — Project Report

**Course:** Object-Oriented Programming — Mini-project: Chessboard  
**Instructor:** Ali Ayadi  
**Language:** C++17  
**Repository:** https://github.com/sbayramli06/Chess_game_OOP  
**Branch:** `main` (merged from `my-feature`)

---

## 1. Project Overview

This project implements a fully functional, text-mode two-player chess engine in C++ using object-oriented design principles. Players enter moves in algebraic notation (e.g. `e2e4`). The engine enforces all standard chess rules across the four required implementation levels.

The project is structured around four classes as described in the assignment:  
`Square` → `Piece` (abstract, with 6 subclasses) → `Echiquier` → `Jeu`.

---

## 2. Class Design

### 2.1 `Square` — Board Coordinate Abstraction

**Files:** `square.h`, `square.cpp`

`Square` encapsulates a board position as zero-based `(row, col)` indices, with bidirectional conversion from/to algebraic notation.

```cpp
explicit Square(const std::string& notation); // "e2" → (row=1, col=4)
Square(int row, int col);
std::string to_string() const;                // (1,4) → "e2"
bool is_valid() const;                        // checks 0 ≤ row,col ≤ 7
bool operator==(const Square& other) const;
```

This removes the need to pass raw `(row, col)` pairs everywhere, simplifying all method signatures.

---

### 2.2 `Piece` — Abstract Base Class

**Files:** `piece.h`, `piece.cpp`

`Piece` is a **pure abstract class** enabling polymorphic dispatch over all piece types. `Echiquier` stores a `Piece*` per square and calls virtual methods without knowing the concrete type.

**Key attributes:** `color_` (`Blanc`/`Noir`), `symbol_` (UTF-8 glyph), `id_`, `pos_` (current `Square`).

**Pure virtual interface:**
```cpp
virtual bool is_legal_move_geometry(const Square& origin,
                                    const Square& dest) const = 0;
virtual bool is_sliding() const { return false; }
```

`is_legal_move_geometry()` checks **geometry only** — not collisions, captures, or check. This clean separation means each subclass is small and testable.  
`is_sliding()` tells `Echiquier` whether to call `path_is_clear()` (Rook, Bishop, Queen return `true`).

---

### 2.3 Concrete Piece Subclasses

All defined in `piece.h` / `piece.cpp`:

| Class | Piece | Movement | Sliding |
|---|---|---|---|
| `Tour` | Rook | Along ranks or files | Yes |
| `Cavalier` | Knight | L-shape ±(1,2) | No |
| `Fou` | Bishop | Diagonally | Yes |
| `Dame` | Queen | Rank, file, or diagonal | Yes |
| `Roi` | King | One square, any direction | No |
| `Pion` | Pawn | Forward 1 or 2; captures diagonally | No |

**`Roi` and `Tour`** carry a `moved_` flag (`has_moved()` / `set_moved()`) for castling eligibility.  
**`Pion`** carries `moved_` (enables the 2-square advance only on first move) and `en_passant_vulnerable_` (active for exactly one opponent turn after a 2-square advance).

---

### 2.4 `Echiquier` — The Chessboard

**Files:** `echiquier.h`, `echiquier.cpp`

As recommended by the assignment, the board stores:
- `Piece* board_[8][8]` — O(1) square lookup; `nullptr` for empty squares
- `std::vector<Piece*> pieces_[2]` — ownership lists per color; used for check detection

**Copy is explicitly disabled** (raw pointer ownership).

#### Constructor
Allocates all 32 pieces with `new`, stores them in `pieces_[Blanc]` / `pieces_[Noir]`, then calls `pose_piece()` to fill `board_[i][j]` per the piece's initial `Square`.

#### Destructor
Deletes every pointer in both `pieces_` vectors. Captured pieces are also `delete`d immediately on capture, and promoted pawns are `delete`d before the replacement piece is created — ensuring no memory leaks.

#### `deplace()` — Full Move Validation Pipeline
```
1. Origin square contains a piece of the current player's color?
2. Destination is on the board?
3. Destination not occupied by a friendly piece?
4. Pawn special rules: capture direction, 2-square advance, en passant
5. is_legal_move_geometry() — geometry check (dispatched polymorphically)
6. path_is_clear() — for sliding pieces (Rook, Bishop, Queen)
7. move_is_safe() — simulate move, verify king not left in check
8. Execute: update board_[i][j] and piece->pos_
9. Update flags: en passant vulnerability, moved_, promotion
```

Each failed check prints a relevant error and returns `false` without modifying the board.

#### `is_in_check(color)`
Scans all enemy pieces and tests whether any can geometrically reach the king's square with a clear path. Pawns are handled separately (attack diagonally, not forward).

#### `move_is_safe(origin, dest, color)`
Temporarily performs the move, calls `is_in_check`, then **fully undoes** the move. Used to reject any move that would expose the own king to check.

#### `has_any_legal_move(color)` — Level 4
Exhaustive search: for every piece of `color` and every target square, checks whether a geometrically valid, safe move exists. Returns `false` when no legal move is available (used for checkmate and stalemate detection).

---

### 2.5 `Jeu` — Game Controller

**Files:** `jeu.h`, `jeu.cpp`

`Jeu` owns an `Echiquier` object and drives the game loop as described in section 4.4 of the assignment.

```cpp
int main() {
    Jeu monjeu;
    bool stop = false;
    monjeu.affiche();
    do { stop = monjeu.coup(); } while (!stop);
}
```

**`coup()`** handles per-turn logic:
1. Clears en passant flags from the previous turn
2. Announces "Check!" if the current player is in check
3. Reads a command from stdin (validated with `std::regex`)
4. Dispatches: normal move / castling O-O or O-O-O / `/quit` / `/resign` / `/draw`
5. After a valid move: switches turn, displays board, checks for checkmate/stalemate (Level 4)
6. On game end: calls `print_final_line()` — canonical position + result on its own stdout line

---

## 3. Features by Level

### Level 1 — Base (8 pts)

- [x] Board display with UTF-8 chess symbols (♔♕♖♗♘♙ / ♚♛♜♝♞♟)
- [x] Algebraic notation input, validated with `std::regex` (`[a-h][1-8][a-h][1-8]`)
- [x] Turn alternation: White → Black → White …
- [x] `/quit` (result `?-?`), `/resign` (loser's result), `/draw` (`1/2-1/2`)
- [x] Final line printed to stdout: `<canonical_position> <result>`
- [x] Coordinate validation (off-board → error, board unchanged)
- [x] Geometric legality per piece type (polymorphic dispatch)
- [x] Source square must contain a piece
- [x] Captures: enemy piece on destination = capture; friendly piece = illegal
- [x] Pawn rules: 2-square first advance, 1-square otherwise, diagonal capture only
- [x] No jumping over pieces for Rook, Bishop, Queen (`path_is_clear()`)

### Level 2 — Check Detection (+3 pts)

- [x] `is_in_check()` detects whether a king is under attack
- [x] `move_is_safe()` rejects any move that leaves own king in check
- [x] "Check!" announced at the start of the threatened player's turn

### Level 3 — Castling, En Passant, Promotion (+3 pts)

- [x] **Kingside castling** (`O-O`): king and rook unmoved + squares f1/g1 empty
- [x] **Queenside castling** (`O-O-O`): king and rook unmoved + squares b1/c1/d1 empty
- [x] **En passant**: pawn captures adjacent pawn that just advanced 2 squares; vulnerability expires after one turn
- [x] **Pawn promotion**: player prompted for Q / R / B / N when pawn reaches final rank; old pawn deleted, new piece allocated

### Level 4 — Checkmate and Stalemate (+4 pts)

- [x] `has_any_legal_move(color)` — exhaustive search across all pieces and all 64 squares
- [x] **Checkmate**: `in_check && !has_legal_move` → game ends, result set (`1-0` or `0-1`)
- [x] **Stalemate**: `!in_check && !has_legal_move` → game ends, result `1/2-1/2`

---

## 4. Canonical Output Format (Section 3.1)

The final line of stdout follows the spec exactly: squares are traversed in order  
`a1, b1, …, h1, a2, b2, …, h8`, each encoded as `{w|b}{Q|K|R|B|N|P}`, separated by commas, empty square = nothing printed.

**Example output for a game ending with `/quit`:**
```
wR,wN,wB,wQ,wK,wB,,wR,wP,wP,wP,wP,,wP,wP,wP,,,,,,wN,,,,,,,wP,,,,,,,,bP,,,,,,bN,,,,,,bP,bP,bP,bP,,bP,bP,bP,bR,,bB,bQ,bK,bB,bN,bR, ?-?
```

Game result strings: `1-0` (white wins), `0-1` (black wins), `1/2-1/2` (draw), `?-?` (interrupted).

This output is compatible with the provided `test-level.sh` test script.

---

## 5. Build Instructions

```bash
# Compile (C++17, all warnings enabled)
make

# Run interactively
./chess

# Run from a game file (as per spec section 3.3)
cat game.txt | ./chess

# Strip comments and run
grep -v "^#" game.txt | ./chess

# Clean build artifacts
make clean
```

> **Compiles with zero warnings and zero errors** under `-Wall -Wextra -pedantic`.

---

## 6. Gameplay Terminal Example

```
     a     b     c     d     e     f     g     h
  +-----+-----+-----+-----+-----+-----+-----+-----+
8 |  ♜  |  ♞  |  ♝  |  ♛  |  ♚  |  ♝  |  ♞  |  ♜  |
  +-----+-----+-----+-----+-----+-----+-----+-----+
...
1 |  ♖  |  ♘  |  ♗  |  ♕  |  ♔  |  ♗  |  ♘  |  ♖  |
  +-----+-----+-----+-----+-----+-----+-----+-----+

Move (eg. a1a8) ? e2e4
→ move of ♙  e2e4
[board redrawn]

Move (eg. a1a8) ? O-O
→ O-O (White)
[board redrawn]

Move (eg. a1a8) ? /resign
White resigns.
[board redrawn]
wR,..., 0-1
```

---

## 7. Git Collaboration Workflow

Two branches were developed and merged:

| Branch | Content |
|---|---|
| `main` | Level 1 + Level 2 + Level 3 + CI workflow + `.gitignore` |
| `my-feature` | Level 4 — checkmate, stalemate, memory management |

```bash
git fetch --all
git pull origin main          # fast-forward through Level 3 commits
git merge origin/my-feature   # conflict-free merge
make clean && make            # zero warnings/errors
git push origin main
```

The merge was **conflict-free**: the `my-feature` branch added new methods (`has_any_legal_move`) and new logic in `jeu.cpp` without overlapping any Level 3 code.

---

## 8. File Structure

```
Chess_game_OOP/
├── square.h / square.cpp        # Coordinate value type
├── piece.h  / piece.cpp         # Abstract Piece + 6 subclasses
├── echiquier.h / echiquier.cpp  # Board, move engine, check/endgame
├── jeu.h    / jeu.cpp           # Game loop, turn management, I/O
├── main.cpp                     # Entry point
├── Makefile                     # Separate compilation, all .o targets
├── .gitignore                   # Excludes *.o, chess binary
└── .github/workflows/main.yml   # CI pipeline
```

---

## 9. OOP Principles Applied

| Principle | Implementation |
|---|---|
| **Abstraction** | `Piece::is_legal_move_geometry()` hides move logic behind a uniform interface |
| **Inheritance** | `Tour`, `Cavalier`, `Fou`, `Dame`, `Roi`, `Pion` all inherit from `Piece` |
| **Polymorphism** | `Echiquier` calls `p->is_legal_move_geometry()` on any `Piece*` at runtime |
| **Encapsulation** | `board_[][]` and `pieces_[]` are private; all access via public methods |
| **RAII / Ownership** | `Echiquier` constructor allocates, destructor frees; copy/assign deleted |
| **Single Responsibility** | Each class has one role: coords / shape / rules / game loop |
| **Separation of Concerns** | Geometry, path-clear, capture, and safety checks are separate methods |

---

## 10. Conclusion

All four required levels are fully implemented and merged into the `main` branch. The program compiles cleanly under C++17 with strict warnings, produces the correct canonical output format on the last line of stdout, and handles all required commands (`/quit`, `/resign`, `/draw`, `O-O`, `O-O-O`). The codebase is modular, well-commented, and follows the OOP design described in section 4 of the assignment specification.
