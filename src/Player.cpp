#include "main.hpp"

std::pair<int, int> Player::CPUTurn() {
    GameState currentState = Game::instance().getCurrentState();

    int bestScore = -1e9;
    std::pair<int, int> bestMove = {-1, -1};

    int id = getID();
    int rows = Game::instance().rows;
    int cols = Game::instance().cols;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const Cell& cell = currentState.Board[i][j];

            if (cell.p != 0 && cell.p != id) continue; // Can't play on enemy

            // Fast skip if cell is empty and has no value
            if (cell.p == 0 && cell.l == 0) {
                if (bestScore < 1) {
                    bestScore = 1;
                    bestMove = {i, j};
                }
                continue;
            }

            GameState simulatedState = currentState;
            int winner;
            std::tie(simulatedState, winner) = simulateTurn(currentState, i, j);

            if (winner < 0) continue; // Invalid move
            if (winner == id) return {i, j}; // Immediate win

            // Evaluate heuristic score
            int gain = 0, flipped = 0, risk = 0, potential = 0;

            for (int x = 0; x < rows; ++x) {
                for (int y = 0; y < cols; ++y) {
                    const Cell& c = simulatedState.Board[x][y];
                    if (c.p == id) {
                        gain++;
                        // Explosion potential
                        if (c.l + 1 >= (int)c.neighbors.size())
                            potential++;

                        // Risk = enemy adjacent to high-load cell
                        for (Cell* n : c.neighbors) {
                            if (n->p != 0 && n->p != id && c.l + 1 >= (int)c.neighbors.size())
                                risk++;
                        }
                    } else if (c.p != 0) {
                        flipped++; // Enemy cells still present
                    }
                }
            }

            int score = gain * 10 - risk * 5 + flipped * 8 + potential * 6;

            if (score > bestScore) {
                bestScore = score;
                bestMove = {i, j};
            }
        }
    }

    return bestMove;
}


std::pair<GameState, int> Player::simulateTurn(GameState state, int x, int y) {
    state.linkNeighbours(); // Ensure neighbors are linked
    int id = getID();
    int rows = state.Board.size();
    int cols = state.Board[0].size();

    if (x < 0 || x >= rows || y < 0 || y >= cols) {
        return {state, -1}; // Invalid move
    }

    Cell& targetCell = state.Board[x][y];
    if (targetCell.p != 0 && targetCell.p != id) {
        return {state, -1}; // Can't play on enemy cell
    }

    std::queue<Cell*> queue;

    // Make the move
    targetCell.p = id;
    targetCell.l += 1;

    if (targetCell.l >= static_cast<int>(targetCell.neighbors.size())) {
        queue.push(&targetCell);
    }

    // Explosion simulation
    while (!queue.empty()) {
        Cell* cell = queue.front();
        queue.pop();

        int threshold = static_cast<int>(cell->neighbors.size());
        if (cell->l < threshold) continue;

        // Explosion
        cell->l = 0;
        cell->p = 0;

        for (Cell* neighbor : cell->neighbors) {
            neighbor->l += 1;
            neighbor->p = id;

            if (neighbor->l >= static_cast<int>(neighbor->neighbors.size())) {
                queue.push(neighbor);
            }
        }
    }

    // Early win check
    bool won = true;
    for (const auto& row : state.Board) {
        for (const auto& cell : row) {
            if (cell.p != 0 && cell.p != id) {
                won = false;
                break;
            }
        }
        if (!won) break;
    }


    return {state, won ? id : 0};
}
