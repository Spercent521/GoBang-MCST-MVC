#ifndef BOARDWINDOW_H
#define BOARDWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>
#include "circleButton.h"
#include "controller.h"
#include "ui_boardwindow.h"
#include "circleButton.h"
#include "controller.h"
#include <QPainter>
#include <QDebug>
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QColor>
#include <QBrush>

namespace Ui {
class boardWindow;
}
QT_END_NAMESPACE

enum boardsize {SMALL,STANDARD,LAGER};

enum gameMode {PVP,PVM};

struct point{
    int xPos;
    int yPos;
};

struct boardsettings {
    // Basic information
    boardsize bs = STANDARD;
    gameMode gm = PVP;

    // Data 1: Window size
    int boardLength; // Window width
    int boardWidth;  // Window height

    // Data 2: Board start & end points (grid corners)
    point boardStartPoint; // Top-left intersection of grid
    point boardEndPoint;   // Bottom-right intersection of grid

    // Data 3: Restart button coordinates and size
    point restartButtonCoordinate;
    int restartButtonLength;
    int restartButtonWidth;

    // Data 4: Undo move button coordinates and size
    point undoMoveButtonCoordinate; // Fixed typo: was undoMovetButtonCoordinate
    int undoMoveButtonLength;
    int undoMoveButtonWidth;

    // New: Cell size for grid drawing and button placement
    int cellSize; // Pixels between adjacent intersections

    // Get board size (unchanged)
    int getBoardSize() {
        if (bs == SMALL) return 9;
        else if (bs == STANDARD) return 13;
        else if (bs == LAGER) return 19;
        else return 13;
    }

    void autoSetAll() {
        int size = getBoardSize();
        int margin;

        // Set cellSize and margin based on board size
        switch (bs) {
        case SMALL: // 9x9
            cellSize = 50; // Larger cells for readability
            margin = 40;   // Compact margin
            break;
        case STANDARD: // 13x13
            cellSize = 45; // Balanced size
            margin = 50;   // Standard margin
            break;
        case LAGER: // 19x19
            cellSize = 40; // Smaller cells for larger board
            margin = 60;   // Larger margin
            break;
        default:
            cellSize = 45;
            margin = 50;
            break;
        }

        // Data 1: Window size
        boardLength = size * cellSize + 2 * margin; // Grid plus margins
        boardWidth = boardLength + 120; // Extra space for buttons below

        // Data 2: Board start and end points (grid intersections)
        boardStartPoint.xPos = margin;
        boardStartPoint.yPos = margin;
        boardEndPoint.xPos = margin + (size - 1) * cellSize; // Last intersection
        boardEndPoint.yPos = margin + (size - 1) * cellSize;

        // Data 3: Restart button
        restartButtonLength = (bs == LAGER) ? 120 : 100;
        restartButtonWidth = (bs == LAGER) ? 50 : 40;
        restartButtonCoordinate.xPos = 20+boardEndPoint.xPos;
        restartButtonCoordinate.yPos = margin+
                                       restartButtonWidth*3;

        // Data 4: Undo button
        undoMoveButtonLength = restartButtonLength;
        undoMoveButtonWidth = restartButtonWidth;
        undoMoveButtonCoordinate.xPos = restartButtonCoordinate.xPos; // Right of restart
        undoMoveButtonCoordinate.yPos = restartButtonCoordinate.yPos+
                                        restartButtonWidth*3;
    }
};

/**
 * @brief The boardWindow class
 * =============================
 * 每一个boardWindow都会负责一盘棋局
 *   1.有一个网格button，对应相应的button，暴露接口给 controller
 *   2.和 controller 对象交互
 *   3.绘制棋盘（按钮，背景等）
 *   4.其他功能交互接口：报告悔棋，报告输赢，报告是否重开
 *
 * input:
 *   全部应该来自 程序控制对象 controller ，输入数据结构为 struct point
 *   即，自身对应的button被点击，报告对应的 point 给 controller 对象
 */
class boardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit boardWindow(QString &boardQString,
                         QString &modelQString,
                         QWidget *parent = nullptr);
    ~boardWindow();

private slots:
    void handleStoneClick(int logicalX, int logicalY);
    void onRestartButtonClicked();
    void onUndoMoveButtonClicked();

signals:
    void stoneClicked(int x, int y);

private:
    Ui::boardWindow *ui;
    void uiBasicWindowSet();

    // controller object
    controller *boardController;

    // 保存棋盘数值信息
    boardsettings thisBs;
    void setupBoardsetting(QString &boardQString,
                           QString &modelQString
                           /* boardsettings &bsetting */);

    // 按钮矩阵 input output 的接口之一
    // 对应每一个格点
    std::vector<std::vector<CircleButton*> > buttonGrid; // CircleButton *buttons[boardSize][boardSize];
    // 初始化每一个格点按钮
    void initButtonGrid();

    // 重新开始对局按钮
    QPushButton *restartButton;

    // 悔棋按钮
    QPushButton *undoMoveButton;

    // 界面设置 绘制棋盘 ui part
    // QWidget *boardWidget;
    void paintEvent(QPaintEvent *event) override;
    void drawChessBoard(QPainter &painter);
};

#endif // BOARDWINDOW_H
