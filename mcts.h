#ifndef MCTS_H
#define MCTS_H

#include <cmath>
#include <vector>
#include <random>
#include "controller.h"

struct MCTSNode {
    Point move;
    MCTSNode* parent;
    std::vector<MCTSNode*> children;
    int wins = 0;
    int visits = 0;
    double uctValue = 0;

    ~MCTSNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

class MCTS {
private:
    const controller& originalCtrl;     // originalCtrl
    int simulationLimit;                // default : 1000
    crossPointState machineColor;       // default : white

    std::mt19937 rng;

public:
    explicit MCTS(const controller& gameCtrl, int simLimit = 1000)
        : originalCtrl(gameCtrl),
        simulationLimit(simLimit),
        machineColor(White),
        rng(std::random_device{}()) {}

    Point getBestMove();

private:
    MCTSNode* select(MCTSNode* node);
    MCTSNode* expand(MCTSNode* node);
    float simulate(MCTSNode* node);
    void backpropagate(MCTSNode* node, float result);
    void calculateUCT(MCTSNode* node);
    int countConsecutive(const controller& ctrl, const Point& p, crossPointState color) const;

    // 辅助函数（使用 controller 现有功能）
    std::vector<Point> getPossibleMoves(const controller& ctrl) const;
    bool isTerminalState(const controller& ctrl) const;
    Winner checkWinner(const controller& ctrl) const;
};
#endif // MCTS_H
