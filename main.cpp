#include "jeu.h"
#include <iostream>
using namespace std;

/**
 * @brief Entry point.
 *
 * Initialises a Jeu object and runs the game loop until the user quits,
 * resigns, or agrees to a draw.
 */
int main() {
    Jeu monjeu;
    bool stop = false;

    monjeu.affiche();

    do {
        stop = monjeu.coup();
    } while (!stop);

    return 0;
}
