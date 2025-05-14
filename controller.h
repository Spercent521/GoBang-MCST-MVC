#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <stack>
// #include "mcts.h"

enum crossPointState { Empty , White , Black };
enum Winner { TheBlack, TheWhite, YET };

struct Point {
    int x;
    int y;
    Point(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct MoveResult {
    crossPointState newCrossPointStateSet;
    Winner newWinnerSet;
    bool IsMachineOutput = false;
    // Point machineMove;
};

class controller {
public:
    explicit controller(int boardSizeSet, bool boardModeSelected);
    // 添加拷贝构造函数（深拷贝）
    explicit controller(const controller& other)
        : boardsize(other.boardsize),
        aiNeeded(other.aiNeeded),
        boardState(other.boardState),
        moveStack(other.moveStack),
        moveCnt(other.moveCnt) {}

    friend class MCTS;

    // 输入新落子
    void inputNewMove(int newPosX, int newPosY);

    // 获取最新落子结果
    MoveResult retResultPack();

    // 获取当前格子信息
    crossPointState getCrossPointState(int logicalX,
                                       int logicalY) const{
        return boardState[logicalX][logicalY];
    };

    // controller reset
    void controllerReset();

    // controller retBack
    Point retBackPoint();

    // machine output
    Point machineOutput();
    bool isMachineWin();
private:
    const int boardsize;
    const bool aiNeeded;
    std::vector<std::vector<crossPointState>> boardState;
    std::stack<Point> moveStack;
    int moveCnt = 0;

    // 检查是否产生胜利者
    Winner checkWinner(crossPointState targetColor, const Point& targetPoint) const;

    // 检查坐标是否有效
    bool isValidPosition(int x, int y) const {
        return x >= 0 && x < boardsize && y >= 0 && y < boardsize;
    }

    // 检查特定方向的连续棋子数
    int countConsecutive(const Point& start, int dx, int dy, crossPointState color) const;
};

void WaitForSimulatingThinking();

#endif // CONTROLLER_H
