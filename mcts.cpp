#include "mcts.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

void printProgressBar(int current, int total, int barWidth = 50) {
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(barWidth * progress);

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(progress * 100.0) << "%";
    std::cout << " (" << current << "/" << total << ")\r";
    std::cout.flush();
}

Point MCTS::getBestMove() {
    MCTSNode root{{-1, -1}, nullptr};

    for (int i = 0; i < simulationLimit; ++i) {
        MCTSNode* selected = select(&root);
        MCTSNode* expanded = expand(selected);
        float result = simulate(expanded);
        backpropagate(expanded, result);

        // std::cout<<"Simulation " << (i + 1) << "/" << simulationLimit
        //          << ", Remaining: " << simulationLimit - i - 1 << std::endl;
        printProgressBar(i + 1, simulationLimit);
    }
    std::cout << std::endl << "Simulation complete!" << std::endl;

    if (root.children.empty()) {
        // 如果没有子节点，返回一个随机有效点
        auto moves = getPossibleMoves(originalCtrl);
        if (!moves.empty()) {
            std::uniform_int_distribution<int> dist(0, moves.size() - 1);
            return moves[dist(rng)];
        }
        return Point{-1, -1}; // 棋盘已满
    }

    MCTSNode* bestNode = nullptr;
    double bestScore = -1.0;
    for (MCTSNode* child : root.children) {
        if (child->visits > 0) {
            double score = static_cast<double>(child->wins) / child->visits;
            if (score > bestScore) {
                bestScore = score;
                bestNode = child;
            }
        }
    }

    return bestNode ? bestNode->move : Point{-1, -1};
}
MCTSNode* MCTS::select(MCTSNode* node) {
    while (!node->children.empty()) {
        calculateUCT(node); // 更新 UCT 值
        node = *std::max_element(
            node->children.begin(), node->children.end(),
            [](MCTSNode* a, MCTSNode* b) { return a->uctValue < b->uctValue; }
            );
    }
    return node;
}

MCTSNode* MCTS::expand(MCTSNode* node) {
    controller tempCtrl(originalCtrl);
    if (node->move.x != -1) {
        tempCtrl.inputNewMove(node->move.x, node->move.y);
    }

    auto moves = getPossibleMoves(tempCtrl);
    for (const auto& move : moves) {  // 现在 moves 已按优先级排序
        bool alreadyExplored = false;
        for (MCTSNode* child : node->children) {
            if (child->move == move) {
                alreadyExplored = true;
                break;
            }
        }
        if (!alreadyExplored) {
            MCTSNode* newNode = new MCTSNode{move, node};
            node->children.push_back(newNode);
            return newNode;
        }
    }
    return node;
}

float MCTS::simulate(MCTSNode* node) {
    controller tempCtrl(originalCtrl);

    if (node->move.x != -1) {
        tempCtrl.inputNewMove(node->move.x, node->move.y);
    }

    bool isMachineTurn = (tempCtrl.moveCnt % 2 == (machineColor == White ? 1 : 0));

    while (!isTerminalState(tempCtrl)) {
        auto moves = getPossibleMoves(tempCtrl);
        if (moves.empty()) return 0.5f;

        // 选择得分最高的点（复用getPossibleMoves的启发式逻辑）
        Point bestMove = moves[0]; // moves已按启发式排序，第一个即为最佳
        tempCtrl.inputNewMove(bestMove.x, bestMove.y);
        isMachineTurn = !isMachineTurn;
    }

    Winner winner = checkWinner(tempCtrl);
    if (winner == YET) return 0.5f;
    return (winner == TheWhite && machineColor == White) ||
                   (winner == TheBlack && machineColor == Black) ? 1.0f : 0.0f;
}


void MCTS::backpropagate(MCTSNode* node, float result) {
    while (node) {
        node->visits++;
        node->wins += result;
        calculateUCT(node);
        node = node->parent;
    }
}

void MCTS::calculateUCT(MCTSNode* node) {
    if (!node->parent || node->visits == 0) return;

    const double exploration = 0.5; // fit gobang , smaller
    node->uctValue = (node->wins / node->visits)
                    + exploration * sqrt(log(node->parent->visits) / node->visits);
}

std::vector<Point> MCTS::getPossibleMoves(const controller& ctrl) const {
    std::vector<std::pair<Point, double>> scoredMoves;
    const int searchRadius = 2;

    Point lastMove(-1, -1);
    if (!ctrl.moveStack.empty()) {
        lastMove = ctrl.moveStack.top();
    }

    for (int x = 0; x < ctrl.boardsize; ++x) {
        for (int y = 0; y < ctrl.boardsize; ++y) {
            if (ctrl.getCrossPointState(x, y) == Empty) {
                Point p(x, y);
                double score = 0.0;

                // 启发式评分：检查落子后能形成的连珠
                controller tempCtrl(ctrl);
                tempCtrl.inputNewMove(x, y);
                int ownConsecutive = countConsecutive(tempCtrl, p, machineColor);
                int opponentConsecutive = countConsecutive(tempCtrl, p, (machineColor == White ? Black : White));

                // 优先级：四连 > 阻断对手四连 > 三连 > 阻断对手三连
                if (ownConsecutive >= 4) score += 100.0;
                else if (opponentConsecutive >= 4) score += 90.0;
                else if (ownConsecutive == 3) score += 50.0;
                else if (opponentConsecutive == 3) score += 40.0;

                // 邻近点加分
                if (lastMove.x != -1 &&
                    abs(x - lastMove.x) <= searchRadius &&
                    abs(y - lastMove.y) <= searchRadius) {
                    score += 10.0;
                }

                scoredMoves.emplace_back(p, score);
            }
        }
    }

    // 按分数从高到低排序
    std::sort(scoredMoves.begin(), scoredMoves.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // 提取排序后的点
    std::vector<Point> moves;
    for (const auto& [point, score] : scoredMoves) {
        moves.push_back(point);
    }

    return moves;
}

int MCTS::countConsecutive(const controller& ctrl, const Point& p, crossPointState color) const {
    int maxConsecutive = 0;
    const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}}; // 水平、垂直、两条对角线
    for (const auto& [dx, dy] : directions) {
        int count = ctrl.countConsecutive(p, dx, dy, color) +
                    ctrl.countConsecutive(p, -dx, -dy, color) - 1; // 减去起点重复计数
        maxConsecutive = std::max(maxConsecutive, count);
    }
    return maxConsecutive;
}

bool MCTS::isTerminalState(const controller& ctrl) const {
    return (checkWinner(ctrl) != YET) || (getPossibleMoves(ctrl).empty());
}

Winner MCTS::checkWinner(const controller& ctrl) const {
    // 复用 controller 的 checkWinner 逻辑
    for (int x = 0; x < ctrl.boardsize; ++x) {
        for (int y = 0; y < ctrl.boardsize; ++y) {
            Point p{x, y};
            auto color = ctrl.getCrossPointState(x, y);
            if (color != Empty && ctrl.checkWinner(color, p) != YET) {
                return (color == White) ? TheWhite : TheBlack;
            }
        }
    }
    return YET;
}
