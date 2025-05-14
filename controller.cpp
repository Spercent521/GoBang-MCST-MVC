#include "controller.h"
#include <stdexcept>
#include <random>
#include <chrono>
#include <thread>
#include "mcts.h"

controller::controller(int boardSizeSet, bool boardModeSelected) :
    boardsize(boardSizeSet > 5 ? boardSizeSet : 15), // 默认最小5x5，推荐15x15
    aiNeeded(boardModeSelected) {
    boardState.resize(boardsize, std::vector<crossPointState>(boardsize, Empty));
}

void controller::inputNewMove(int newPosX, int newPosY) {
    if (!isValidPosition(newPosX, newPosY)) {
        throw std::out_of_range("Invalid board position");
    }

    if (boardState[newPosX][newPosY] != Empty) {
        throw std::logic_error("Position already occupied");
    }

    moveStack.emplace(newPosX, newPosY);
    moveCnt++;
    boardState[newPosX][newPosY] = (moveCnt % 2) ? Black : White;
}

MoveResult controller::retResultPack() {
    if (moveStack.empty()) {
        throw std::logic_error("No moves made yet");
    }

    const Point& lastMove = moveStack.top();
    crossPointState lastMoveState = boardState[lastMove.x][lastMove.y];
    Winner winner = checkWinner(lastMoveState, lastMove);

    return { lastMoveState, winner, aiNeeded};
}

Winner controller::checkWinner(crossPointState targetColor, const Point& targetPoint) const {
    // 检查四个方向：水平、垂直、两条对角线
    constexpr std::pair<int, int> directions[] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

    for (const auto& [dx, dy] : directions) {
        int count = 1 + countConsecutive(targetPoint, dx, dy, targetColor)
        + countConsecutive(targetPoint, -dx, -dy, targetColor);

        if (count >= 5) {
            return targetColor == Black ? TheBlack : TheWhite;
        }
    }

    return YET;
}

int controller::countConsecutive(const Point& start, int dx, int dy, crossPointState color) const {
    int count = 0;
    for (int step = 1; step < 5; ++step) {
        int x = start.x + step * dx;
        int y = start.y + step * dy;

        if (!isValidPosition(x, y) || boardState[x][y] != color) {
            break;
        }
        count++;
    }
    return count;
}

void controller::controllerReset(){
    moveCnt = 0;
    moveStack = std::stack<Point>();

    for(auto &raw:boardState){
        for(auto &pointState:raw){
            pointState=Empty;
        }
    }
}

Point controller::retBackPoint(){
    if(moveStack.empty()){
        return {0,0};
    }

    Point retPoint = moveStack.top();
    moveCnt -= 1;
    moveStack.pop();
    boardState[retPoint.x][retPoint.y]=Empty;

    return retPoint;
}

void WaitForSimulatingThinking(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(200, 400); // 200ms-400ms

    int wait_time_ms = dis(gen);

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
}

Point controller::machineOutput(){
    WaitForSimulatingThinking();
    // interface
    MCTS mcts(*this);
    Point machineFinalResult=mcts.getBestMove();

    // data update
    moveCnt++;
    moveStack.push(machineFinalResult);
    boardState[machineFinalResult.x][machineFinalResult.y]=White;

    return machineFinalResult;
}

bool controller::isMachineWin(){
    // machine always white

    return checkWinner(White,moveStack.top())==TheWhite;
}
