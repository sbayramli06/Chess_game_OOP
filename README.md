# ♟ Chess Game OOP

A fully functional two-player chess engine implemented in **C++17** using object-oriented programming principles.  
Built as part of the *Mini-project: Object Programming (Chessboard)* assignment.

---

## Features

| Level | Feature | Status |
|---|---|---|
| **Level 1** | Board display, move input, turn alternation, `/quit` `/resign` `/draw` | ✅ |
| **Level 1** | Geometric legality, captures, pawn rules, no-jumping | ✅ |
| **Level 2** | Check detection, safe-move filtering, "Check!" announcement | ✅ |
| **Level 3** | Castling (O-O / O-O-O), en passant, pawn promotion | ✅ |
| **Level 4** | Checkmate detection, stalemate detection | ✅ |

---

## Build

**Requirements:** g++ with C++17 support, `make`

```bash
make        # compile
make clean  # remove build artifacts
```

---

## Run

### Interactive mode
```bash
./chess
```

### From a game file
```bash
cat game.txt | ./chess

# Strip comment lines first
grep -v "^#" game.txt | ./chess
```

---

## How to Play

Enter moves in **algebraic notation** — source square followed by destination square:

```
Move (eg. a1a8) ? e2e4
```

### Special commands

| Command | Description |
|---|---|
| `e2e4` | Move piece from e2 to e4 |
| `O-O` | Kingside castling |
| `O-O-O` | Queenside castling |
| `/resign` | Current player resigns |
| `/draw` | Agree to a draw |
| `/quit` | Exit (result: `?-?`) |

### Pawn promotion

When a pawn reaches the last rank, you will be prompted:
```
Promotion! Choose piece (Q, R, B, N):
```
Type one of: `Q` (Queen), `R` (Rook), `B` (Bishop), `N` (Knight).

---

## Board Display

```
     a     b     c     d     e     f     g     h
  +-----+-----+-----+-----+-----+-----+-----+-----+
8 |  ♜  |  ♞  |  ♝  |  ♛  |  ♚  |  ♝  |  ♞  |  ♜  |
  +-----+-----+-----+-----+-----+-----+-----+-----+
7 |  ♟  |  ♟  |  ♟  |  ♟  |  ♟  |  ♟  |  ♟  |  ♟  |
  ...
1 |  ♖  |  ♘  |  ♗  |  ♕  |  ♔  |  ♗  |  ♘  |  ♖  |
  +-----+-----+-----+-----+-----+-----+-----+-----+
```

White pieces: ♔♕♖♗♘♙  
Black pieces: ♚♛♜♝♞♟

---

## Output Format

On game end, the last line of stdout contains:

```
<canonical_position> <result>
```

- **Canonical position:** squares traversed `a1→h1→a2→…→h8`, each encoded as `{w|b}{Q|K|R|B|N|P}`, separated by commas
- **Result:** `1-0` (white wins) · `0-1` (black wins) · `1/2-1/2` (draw) · `?-?` (interrupted)

Example:
```
wR,wN,wB,wQ,wK,wB,,wR,wP,wP,wP,wP,,wP,wP,wP,,..., ?-?
```

---

## Project Structure

```
Chess_game_OOP/
├── square.h / square.cpp        # Coordinate value type (algebraic ↔ index)
├── piece.h  / piece.cpp         # Abstract Piece + Tour, Cavalier, Fou, Dame, Roi, Pion
├── echiquier.h / echiquier.cpp  # 8×8 board, move engine, check & endgame logic
├── jeu.h    / jeu.cpp           # Game loop, turn management, user I/O
├── main.cpp                     # Entry point
├── Makefile                     # Separate compilation
├── REPORT.md                    # Full project report
└── .github/workflows/main.yml   # CI pipeline
```

---

## Class Architecture

```
Jeu
 └── Echiquier
       └── Piece  (abstract)
             ├── Tour     (Rook)
             ├── Cavalier (Knight)
             ├── Fou      (Bishop)
             ├── Dame     (Queen)
             ├── Roi      (King)
             └── Pion     (Pawn)
       └── Square (coordinate helper)
```

---

## Authors

- **Aniya Baghirova** — Base implementation (Levels 1–3)  
- **Teammate (sbayramli06)** — Endgame detection: checkmate, stalemate, memory management (Level 4, branch `my-feature`)
