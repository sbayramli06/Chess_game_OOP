# Makefile – Chess OOP project (Level 1)
# Supports separate compilation: each .cpp is compiled independently.

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
TARGET   = chess

# All object files
OBJS = main.o square.o piece.o echiquier.o jeu.o

# ---- Default target --------------------------------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ---- Per-file rules (enable separate compilation) --------------------------
main.o: main.cpp jeu.h echiquier.h piece.h square.h
	$(CXX) $(CXXFLAGS) -c $<

square.o: square.cpp square.h
	$(CXX) $(CXXFLAGS) -c $<

piece.o: piece.cpp piece.h square.h
	$(CXX) $(CXXFLAGS) -c $<

echiquier.o: echiquier.cpp echiquier.h piece.h square.h
	$(CXX) $(CXXFLAGS) -c $<

jeu.o: jeu.cpp jeu.h echiquier.h piece.h square.h
	$(CXX) $(CXXFLAGS) -c $<

# ---- Utility targets -------------------------------------------------------
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
