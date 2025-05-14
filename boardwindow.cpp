#include "boardwindow.h"
#include <string>
#include <iostream>

// RGB（227，198，148）

void boardWindow::setupBoardsetting(QString &boardQString,
                       QString &modelQString){
    std::string boardsizeString=boardQString.toStdString();
    if(boardsizeString=="9*9棋盘"){
        thisBs.bs=SMALL;
        std::cout<<"case 1 : 9"<<std::endl;
    }else if(boardsizeString=="13*13棋盘"){
        thisBs.bs=STANDARD;
        std::cout<<"case 2 : 13"<<std::endl;
    }else if(boardsizeString=="19*19棋盘"){
        thisBs.bs=LAGER;
        std::cout<<"case 3 : 19"<<std::endl;
    }else{
        thisBs.bs=STANDARD;
        std::cout<<"case default : 13"<<std::endl;
    }

    std::string gameModeString=modelQString.toStdString();
    if(gameModeString=="人人对抗"){
        thisBs.gm=PVP;
        std::cout<<"case 1 : PVP"<<std::endl;
    }else if(gameModeString=="人机对抗"){
        thisBs.gm=PVM;
        std::cout<<"case 2 : PVM"<<std::endl;
    }else{
        thisBs.gm=PVP;
        std::cout<<"case default : PVP"<<std::endl;
    }

    thisBs.autoSetAll();
}

boardWindow::boardWindow(QString &boardQString,
                         QString &modelQString,
                         QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::boardWindow)
{
    ui->setupUi(this);

    setupBoardsetting(boardQString,
                      modelQString);

    uiBasicWindowSet();
    initButtonGrid();

    bool modeSelectedPass = (thisBs.gm == PVM) ? true : false;
    boardController = new controller(thisBs.getBoardSize(),modeSelectedPass);
}

/**
 * @brief boardWindow::initButtonGrid
 *
 * 最后修改20250506
 *        20250507
 */
void boardWindow::initButtonGrid(){

    /** 要重写QButton的paintEvent方法 使之变成圆形按钮 */
    int gridSizeForLoop = thisBs.getBoardSize();
    buttonGrid.resize(thisBs.getBoardSize(),
                      std::vector<CircleButton*>(thisBs.getBoardSize(), nullptr));

    int buttonRadius = thisBs.cellSize / 3;
    int buttonDiameter = buttonRadius * 2;

    for(int i = 0;i<gridSizeForLoop;++i){
        for(int j = 0;j<gridSizeForLoop;++j){
            buttonGrid[i][j]=new CircleButton(i, j,this);
            buttonGrid[i][j]->setFixedSize(buttonDiameter, buttonDiameter);

            // 计算按钮位置（中心对齐交叉点）
            int xPos = thisBs.boardStartPoint.xPos + j * thisBs.cellSize - buttonRadius;
            int yPos = thisBs.boardStartPoint.yPos + i * thisBs.cellSize - buttonRadius;
            buttonGrid[i][j]->move(xPos, yPos);

            // 初始状态设为透明（无棋子）
            buttonGrid[i][j]->setState(CircleButton::Transparent);

            // 点击时直接改变颜色的方案
            // for debugging
            /**
            // 使用捕获i,j的lambda表达式连接信号槽
            connect(buttonGrid[i][j], &QPushButton::clicked, [this, i, j]() {
                // 点击时切换状态
                CircleButton::ButtonState newState;
                switch (buttonGrid[i][j]->state()) {
                    case CircleButton::Black:
                        newState = CircleButton::White;
                        break;
                    case CircleButton::White:
                        newState = CircleButton::Transparent;
                        break;
                    default:
                        newState = CircleButton::Black;
                }
                buttonGrid[i][j]->setState(newState);
            });
            */

            // 贯彻MVC思想方案
            connect(buttonGrid[i][j], &CircleButton::clickedWithLogicalPos,
                    this, &boardWindow::handleStoneClick);
        }
    }
}

/**
 * @brief boardWindow::handleStoneClick
 * @param logicalX
 * @param logicalY
 *
 * ##########################
 * ###controller_interface###
 * ##########################
 */

void boardWindow::handleStoneClick(int logicalX, int logicalY) {
    qDebug() << "Stone clicked at:" << logicalX << logicalY;

    // 如果点到已经下过的地方，要 pass ！！！
    if(boardController->getCrossPointState(logicalX,logicalY)!=Empty){
        return;
    }

    /** 可以从这里输出被点击的逻辑坐标 */
    // controller input
    boardController->inputNewMove(logicalX,logicalY);

    // controller output
    MoveResult novelMoveResult =  boardController->retResultPack();

    // enum ButtonState { Black, White, Transparent};
    CircleButton::ButtonState tpBS;
    CircleButton::ButtonState NegTpBs;
    if(novelMoveResult.newCrossPointStateSet==White){
        tpBS=CircleButton::White;
        NegTpBs=CircleButton::Black;
    }else if(novelMoveResult.newCrossPointStateSet==Black){
        tpBS=CircleButton::Black;
        NegTpBs=CircleButton::White;
    }else{
        tpBS=CircleButton::Transparent;
        NegTpBs=CircleButton::Black;
    }

    buttonGrid[logicalX][logicalY]->setState(tpBS);
    qApp->processEvents();  // 强制执行渲染

    if(novelMoveResult.newWinnerSet!=YET){
        QMessageBox msgBox;

        if (QLayout* layout = msgBox.layout()) {
            layout->setSizeConstraint(QLayout::SetFixedSize); // 允许自由调整大小
            layout->setContentsMargins(30, 20, 30, 20);      // 增大内边距（左,上,右,下）
        }

        msgBox.setWindowTitle("GAME_OVER");
        msgBox.setStyleSheet("QLabel { color: #333; font-size: 14px; }"
                             "QPushButton { padding: 5px 10px; }");
        msgBox.setMinimumWidth(200);

        if(novelMoveResult.newWinnerSet==TheWhite){
            msgBox.setText("<b>白棋获胜</b>");
        }

        if(novelMoveResult.newWinnerSet==TheBlack){
            msgBox.setText("<b>黑棋获胜</b>");
        }
        msgBox.exec();
        return;
    }

    if(novelMoveResult.IsMachineOutput){
        qDebug() << "handle ai output";
        // machine output point
        Point machineCalculatePoint = boardController->machineOutput();
        buttonGrid[machineCalculatePoint.x][machineCalculatePoint.y]->setState(NegTpBs);
        if(boardController->isMachineWin()){
            QMessageBox msgBox;
            if (QLayout* layout = msgBox.layout()) {
                layout->setSizeConstraint(QLayout::SetFixedSize); // 允许自由调整大小
                layout->setContentsMargins(30, 20, 30, 20);      // 增大内边距（左,上,右,下）
            }
            msgBox.setWindowTitle("GAME_OVER");
            msgBox.setStyleSheet("QLabel { color: #333; font-size: 14px; }"
                                 "QPushButton { padding: 5px 10px; }");
            msgBox.setMinimumWidth(200);

            msgBox.setText("<b>Machine is the winner</b>");

            msgBox.exec();
            return;
        }
    }
}

// 用来设置棋盘窗口的ui
void boardWindow::uiBasicWindowSet(){
    // 设置窗口标题为对局开始时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = "board begin at "+currentDateTime.toString("yyyy-MM-dd HH:mm:ss");
    this->setWindowTitle(currentTimeString);

    // 改变窗口大小
    this->resize(thisBs.boardWidth,thisBs.boardLength);

    // 设置 悔棋 和 重开 button
    restartButton = new QPushButton("Restart", this);
    restartButton->setGeometry(thisBs.restartButtonCoordinate.xPos,
                               thisBs.restartButtonCoordinate.yPos,
                               thisBs.restartButtonLength,
                               thisBs.restartButtonWidth);

    undoMoveButton = new QPushButton("Undo Move", this);
    undoMoveButton->setGeometry(thisBs.undoMoveButtonCoordinate.xPos,
                               thisBs.undoMoveButtonCoordinate.yPos,
                               thisBs.undoMoveButtonLength,
                               thisBs.undoMoveButtonWidth);

    // 连接信号槽
    connect(restartButton, &QPushButton::clicked,
            this, &boardWindow::onRestartButtonClicked);
    connect(undoMoveButton, &QPushButton::clicked, this,
            &boardWindow::onUndoMoveButtonClicked);

    // 美化按钮
    // 使用 CSS 样式表
    QString buttonStyle = "QPushButton { "
                          "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                          "stop:0 #d2b48c, stop:1 #8b4513); "  // 木质渐变背景
                          "color: #f5f5dc; "                   // 米色文字
                          "font: bold 14px 'Microsoft YaHei'; "// 使用雅黑字体
                          "border: 2px solid #5d4037; "       // 深棕色边框
                          "border-radius: 8px; "              // 圆角
                          "padding: 8px 15px; "               // 内边距
                          "min-width: 100px; "                // 最小宽度
                          "min-height: 35px; "                // 最小高度
                          "}"
                          "QPushButton:hover { "
                          "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                          "stop:0 #c19a6b, stop:1 #6e3b0e); " // 悬停颜色
                          "border: 2px solid #4e342e; "      // 悬停边框
                          "}"
                          "QPushButton:pressed { "
                          "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                          "stop:0 #a67c52, stop:1 #5d2906); "// 按下颜色
                          "padding: 7px 14px 9px 16px; "     // 按下效果
                          "}";

    restartButton->setStyleSheet(buttonStyle);
    undoMoveButton->setStyleSheet(buttonStyle);
}

// 启动背景绘制
void boardWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    drawChessBoard(painter);  // 绘制棋盘
}

// 负责背景具体的绘制
void boardWindow::drawChessBoard(QPainter &painter) {
    painter.fillRect(rect(), QColor(227,198,148)); // 背景色

    // 绘制棋盘网格
    painter.setPen(QPen(Qt::black, 2));
    int size = thisBs.getBoardSize();

    for (int i = 0; i < size; ++i) {
        // 横线
        painter.drawLine(
            thisBs.boardStartPoint.xPos,
            thisBs.boardStartPoint.yPos + i * thisBs.cellSize,
            thisBs.boardEndPoint.xPos,
            thisBs.boardStartPoint.yPos + i * thisBs.cellSize
            );
        // 竖线
        painter.drawLine(
            thisBs.boardStartPoint.xPos + i * thisBs.cellSize,
            thisBs.boardStartPoint.yPos,
            thisBs.boardStartPoint.xPos + i * thisBs.cellSize,
            thisBs.boardEndPoint.yPos
            );
    }
}

boardWindow::~boardWindow()
{
    /** 双重释放风险：ui 是通过 ui->setupUi(this); 设置的，
     *  Qt 的 Ui::boardWindow 通常管理自己的对象。
     *  delete ui; 可能导致双重释放，因为 Qt 的父子对象关系会在 QMainWindow 析构时自动删除子对象。
     */
    // delete ui;
    // delete restartButton;
    // delete undoMoveButton;
    // delete boardWidget;
    delete boardController;

    // 巧妙的释放
    // 并不巧妙
    /** buttonGrid 的释放：buttonGrid 中的 CircleButton 指针由 initButtonGrid 分配，且其父对象是 this（boardWindow）。
     *  Qt 的父子关系会自动删除这些按钮，因此手动 delete button;
     *  可能导致双重释放。
     */
    // for (auto &row : buttonGrid) {
    //     for (auto &button : row) {
    //         delete button;
    //     }
    // }
}

/**
 * @brief boardWindow::onRestartButtonClicked
 * 实现重新开始行为
 */
void boardWindow::onRestartButtonClicked(){
    qDebug() << "reStartButton was clicked";
    // controller reset
    boardController->controllerReset();

    // window&board reset
    for(auto &raw : buttonGrid){
        for(auto &button:raw){
            if(button->state()!=CircleButton::Transparent){
                button->setState(CircleButton::Transparent);
            }
        }
    }
}

/**
 * @brief boardWindow::onUndoMoveButtonClicked
 * 实现悔棋行为
 */
void boardWindow::onUndoMoveButtonClicked(){
    qDebug() << "undo Button was clicked";

    // controller retback
    Point retBackPoint = boardController->retBackPoint();

    // window&board retback
    buttonGrid[retBackPoint.x][retBackPoint.y]->setState(CircleButton::Transparent);
}
