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

## Requirements

| Tool | Version | Notes |
|---|---|---|
| `g++` | ≥ 7 (C++17) | GCC or MinGW |
| `make` | any | GNU Make |

---

## Build & Run

### 🐧 Linux / macOS

```bash
# 1. Clone the repository
git clone https://github.com/sbayramli06/Chess_game_OOP.git
cd Chess_game_OOP

# 2. Compile
make

# 3. Play interactively
./chess

# 4. Clean build artefacts
make clean
```

### 🪟 Windows

**Option A — MSYS2 (recommended)**

1. Install [MSYS2](https://www.msys2.org/) and open the **MSYS2 MinGW 64-bit** shell.
2. Install the compiler once:
   ```bash
   pacman -S mingw-w64-x86_64-gcc make
   ```
3. Navigate to the project folder and build:
   ```bash
   cd /c/Users/<YourName>/Desktop/Chess_game_OOP
   make
   ./chess
   ```

**Option B — Git Bash (if you already have MinGW/g++ on your PATH)**

```bash
# Open Git Bash in the project folder, then:
make
./chess
```

**Option C — PowerShell / Command Prompt (manual compile, no make)**

```powershell
g++ -std=c++17 -Wall -o chess main.cpp square.cpp piece.cpp echiquier.cpp jeu.cpp
.\chess.exe
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

### Replay a recorded game

```bash
# Linux/macOS
grep -v "^#" game.txt | ./chess

# Windows (Git Bash / MSYS2)
grep -v "^#" game.txt | ./chess
```

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

- **Bayramli Shehana**
- **Farida Orucova**
- **Huseynova Fakhriyya**
- **Baghirova Aniya**
