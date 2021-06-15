#include <cstdbool>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
//視窗属性
#define WINDOW_WIDTH  640//視窗寬度
#define WINDOW_HEIGHT  600//視窗長度
using namespace std;

//主迴圈延遲
const int MAIN_LOOP_DELAY = 5;

//位置
SDL_Rect BlockRect; //方塊
SDL_Rect BallRect; //球
SDL_Rect BoardRect; //反彈棒
SDL_Rect Board2Rect;//反彈棒2

//事件
SDL_Event event;

//指针
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

//分數
int score = 0;

//字體
TTF_Font* font;

//分數文字
int scoreFontReslution = 50; //字體解析度
SDL_Color scoreColor = { 255, 255, 255, 0 }; //字體顏色 {red, green, blue ,alpha}
SDL_Rect scoreRect = { 540, 500, 60, 80 }; //字體尺寸 {起始x, 起始y, width, height}

//統計文字
int statisticsFontReslution = 50; //字體解析度
SDL_Color statisticsTextColor = { 255, 255, 150, 0 }; //字體顏色
SDL_Rect statisticsTextRect = { 100, 100, 300, 100 }; //字體尺寸

//統計分數
int statisticsScoreReslution = 50; //字體解析度
SDL_Color statisticsScoreColor = { 255, 255, 255, 0 }; //字體顏色
SDL_Rect statisticsScoreRect = { 175, 250, 150, 200 }; //字體尺寸

SDL_Surface* BallSueface = NULL;
SDL_Surface* BoardSurface = NULL;
SDL_Surface* Board2Surface = NULL;
SDL_Surface* Block = NULL;//方塊
SDL_Surface* breakBlock = NULL;//碎方塊


SDL_Texture* BlockTT;
SDL_Texture* breakBlockTT;
SDL_Texture* BallTT = NULL;
SDL_Texture* BoardTT = NULL;
SDL_Texture* Board2TT = NULL;
//結構體

typedef struct SSurface//方塊結構體
{
    SDL_Texture* Texture;//文理
    SDL_Rect TargetRect;//目標rect
    SDL_Rect SourceRect;//圖片的rect
    int index;//顯示那張圖片
    bool state;//是否有效
}SSurface;
SSurface BlickManager[13][10];//結構體陣列，存放方塊結構體

//參數
const int BlockW = 64;//方塊的寬度
const int BlockH = 32;//方塊的高度

//球移動速度
int BoardSpeed = 3;
int Board2Speed = 3;//反彈棒移動速度
int BallSpeed = 2;//球移動速度

//控制反彈棒的移動方向
bool moveBoardLeft = false;
bool moveBoardRight = false;
bool moveBoard2Left = false;
bool moveBoard2Right = false;

//控制球的移動方向
bool moveRight = true;
bool moveDown = true;

// 暫停狀態
bool pause = true;

//子函數
void init();//初始化函數
int UpdateLogic();//更新邏輯
void randnum();//隨機產生方塊
void draw();
int check_collision();//檢查球與方塊的碰撞
bool checkCollision(SDL_Rect A, SDL_Rect B);//球與反彈棒相撞檢測
bool checkCollision(SDL_Rect A, SDL_Rect C);//球與反彈棒相撞檢測2
double GetDistance(int x1, int y1, int x2, int y2);//兩點距離
bool scorefix = false;//碰撞分數修正
int scorefixpt = 0;//重複記分修正


int main(int argc, char* argv[])
{

    init();//初始化
    srand(time(NULL));
    randnum();
    randnum();
    int scorefixpt = 0;

    while (window)
    {
        while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_QUIT)//退出事件
            {
                SDL_DestroyWindow(window);
                window = NULL;
            }
            else if (event.type == SDL_KEYDOWN)//鍵盤事件
            {
                if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) //暫停
                {

                    pause = !pause;
                }
                if (event.key.keysym.sym == SDLK_LEFT)//左移動
                {
                    moveBoardLeft = true;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)//右移動
                {
                    moveBoardRight = true;
                }

                if (event.key.keysym.sym == SDLK_a)//左移動
                {
                    moveBoard2Left = true;
                }
                else if (event.key.keysym.sym == SDLK_d)//右移動
                {
                    moveBoard2Right = true;
                }

                if (event.key.keysym.sym == SDLK_UP)//反彈棒加速
                {
                    BoardSpeed = 5;
                }
                if (event.key.keysym.sym == SDLK_w)//反彈棒2加速
                {
                    Board2Speed = 5;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_LEFT)//左移動取消
                {
                    moveBoardLeft = false;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)//右移動取消
                {
                    moveBoardRight = false;
                }
                if (event.key.keysym.sym == SDLK_a)//左移動取消
                {
                    moveBoard2Left = false;
                }
                else if (event.key.keysym.sym == SDLK_d)//右移動取消
                {
                    moveBoard2Right = false;
                }
                if (event.key.keysym.sym == SDLK_UP)//加速取消
                {
                    BoardSpeed = 3;
                }
                if (event.key.keysym.sym == SDLK_w)//加速取消
                {
                    Board2Speed = 3;
                }
            }


        }

        if (!pause) {
            if (!UpdateLogic())
            {
                cout << "Game over" << std::endl;
                cout << "Your score is: " << score << std::endl;
                SDL_DestroyWindow(window);
                window = NULL;
            }
        }

        draw();
        SDL_Delay(MAIN_LOOP_DELAY);//加入一點延遲，防止CPU耗盡
    }

    // 統計視窗 //HERE
    window = SDL_CreateWindow("Bouncing Ball => Your score!", 100, 100, 500, 500, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    while (window) {
        while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_QUIT)//退出事件
            {
                SDL_DestroyWindow(window);
                window = NULL;
            }
            else if (event.type == SDL_KEYDOWN)//鍵盤事件
            {
                if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) //暫停
                {
                    SDL_DestroyWindow(window);
                    window = NULL;
                }
            }
        }

        // 繪製統計文字
        SDL_Surface* statisticsTextSurface = TTF_RenderText_Solid(font, "Your score", statisticsTextColor);
        SDL_Texture* statisticsTextTexture = SDL_CreateTextureFromSurface(renderer, statisticsTextSurface);
        SDL_RenderCopy(renderer, statisticsTextTexture, NULL, &statisticsTextRect);
        SDL_FreeSurface(statisticsTextSurface);
        SDL_DestroyTexture(statisticsTextTexture);

        // 繪製統計分數
        SDL_Surface* statisticScoreSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), statisticsScoreColor);
        SDL_Texture* statisticScoreTexture = SDL_CreateTextureFromSurface(renderer, statisticScoreSurface);
        SDL_RenderCopy(renderer, statisticScoreTexture, NULL, &statisticsScoreRect);
        SDL_FreeSurface(statisticScoreSurface);
        SDL_DestroyTexture(statisticScoreTexture);

        SDL_RenderPresent(renderer);//刷新
        SDL_Delay(MAIN_LOOP_DELAY);//加入一點延遲，防止CPU耗盡
    }

    SDL_FreeSurface(Block);
    SDL_FreeSurface(BallSueface);
    SDL_Quit();


    return 0;
}

bool checkCollision(SDL_Rect A, SDL_Rect B)
{
    //矩形的各邊
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    //計算rect A的各邊
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;
    //計算 rect B的各邊
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    if (bottomA <= topB)
    {
        return false;
    }
    if (topA >= bottomB)
    {
        return false;
    }

    if (rightA <= leftB)
    {
        return false;
    }
    if (leftA >= rightB)
    {
        return false;
    }
    
    return true;
}

bool checkCollision2(SDL_Rect A, SDL_Rect C)
{
    //矩形的各邊
    int leftA, leftC;
    int rightA, rightC;
    int topA, topC;
    int bottomA, bottomC;
    //計算rect A的各邊
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;
    //計算 rect C的各邊
    leftC = C.x;
    rightC = C.x + C.w;
    topC = C.y;
    bottomC = C.y + C.h;

    if (bottomA <= topC)
    {
        return false;
    }
    if (topA >= bottomC)
    {
        return false;
    }

    if (rightA <= leftC)
    {
        return false;
    }
    if (leftA >= rightC)
    {
        return false;
    }
    
    return true;
}

int UpdateLogic()
{
    //更新圖片，如果index=2,代表碰撞，更換碎方塊
    int x, y;
    for (x = 5; x < 13; x++)
    {
        for (y = 0; y < 10; y++)
        {
            if (BlickManager[x][y].state == true)
            {
                if (BlickManager[x][y].index == 2)
                {
                    BlickManager[x][y].Texture = breakBlockTT;
                    BlickManager[x][y].SourceRect.x = 0;
                    BlickManager[x][y].SourceRect.y = 0;
                    BlickManager[x][y].index += 1;
                }
                else if (BlickManager[x][y].index == 3)
                {
                    BlickManager[x][y].SourceRect.x = BlockW;
                    BlickManager[x][y].SourceRect.y = 0;
                    BlickManager[x][y].index += 1;
                }
                else if (BlickManager[x][y].index == 4)
                {
                    BlickManager[x][y].SourceRect.x = BlockW * 2;
                    BlickManager[x][y].SourceRect.y = 0;
                    BlickManager[x][y].index += 1;
                }
                else if (BlickManager[x][y].index == 5)
                {
                    BlickManager[x][y].SourceRect.x = BlockW * 3;
                    BlickManager[x][y].SourceRect.y = 0;
                    BlickManager[x][y].index += 1;
                }
                else if (BlickManager[x][y].index == 6)
                {
                    BlickManager[x][y].SourceRect.x = BlockW * 4;
                    BlickManager[x][y].SourceRect.y = 0;
                    BlickManager[x][y].index = 1;
                    BlickManager[x][y].state = false;
                }
            }
        }
    }

    //球的運動軌跡控制
    if (true == moveRight)
    {
        BallRect.x = BallRect.x + BallSpeed;
        if (BallRect.x + BallRect.w >= WINDOW_WIDTH)
        {
            moveRight = false;
            //printf("trun left\n");
            scorefixpt = 0;
            scorefix = false;
        }
        else if (1 == check_collision())
        {
            moveRight = false;
        }
    }
    else
    {
        BallRect.x = BallRect.x - BallSpeed;
        if (BallRect.x <= 0)
        {
            moveRight = true;
            //printf("trun right\n");
            scorefixpt = 0;
            scorefix = false;
        }
        else if (2 == check_collision())
        {
            moveRight = true;
        }
    }

    if (true == moveDown)
    {
        BallRect.y = BallRect.y + BallSpeed;
        if (BallRect.y + BallRect.h >= WINDOW_HEIGHT)
        {
            SDL_Quit();
            return 0;
        }
        if (checkCollision(BallRect, BoardRect))
        {
            moveDown = false;
            //printf("trun up\n");
            scorefix = false;
            scorefixpt = 0;
        }
        else if (3 == check_collision())
        {
            moveDown = false;
        }
    }
    else
    {
        BallRect.y = BallRect.y - BallSpeed;
        if (BallRect.y <= 0) {
            SDL_Quit();
            return 0;
        }
        if (checkCollision(BallRect, Board2Rect))
        {
            moveDown = true;
            //printf("trun down\n");
            scorefix = false;
            scorefixpt = 0;
        }
        else if (4 == check_collision())
        {
            moveDown = true;
        }
    }

    //反彈棒的運動軌跡控制
    if (moveBoardLeft == true)//反彈棒左移
    {
        if (BoardRect.x > 0)
        {
            BoardRect.x = BoardRect.x - BoardSpeed;
        }
    }
    if (moveBoardRight == true)//反彈棒右移
    {
        if (BoardRect.x + BoardRect.w < WINDOW_WIDTH)
        {
            BoardRect.x = BoardRect.x + BoardSpeed;
        }
    }

    if (moveBoard2Left == true)//反彈棒左移
    {
        if (Board2Rect.x > 0)
        {
            Board2Rect.x = Board2Rect.x - Board2Speed;
        }
    }
    if (moveBoard2Right == true)//反彈棒右移
    {
        if (Board2Rect.x + Board2Rect.w < WINDOW_WIDTH)
        {
            Board2Rect.x = Board2Rect.x + Board2Speed;
        }
    }
    return 1;
}

//隨機產生方塊
void randnum(void) {
    int x, y;
    
    x = (rand() % 8) + 5;//x[5,13]
    y = (rand() % 10);//y[0,10]
       
    //printf("x=%d y=%d \n", x, y);
    if (BlockRect.x == WINDOW_WIDTH / 2 - (BallSueface->w / 2) && BlockRect.y == WINDOW_HEIGHT / 10 * 9 - BallSueface->h)
    {     
        BlickManager[x][y].state = false;
        BlickManager[x][y].index = 2;
        //printf("ERROR");
    }
    else
    {     
        BlickManager[x][y].Texture = BlockTT;

        BlickManager[x][y].SourceRect = BlockRect;

        BlickManager[x][y].TargetRect.x = BlockW * y;
        BlickManager[x][y].TargetRect.y = BlockH * x;
        BlickManager[x][y].TargetRect.w = BlockRect.w;
        BlickManager[x][y].TargetRect.h = BlockRect.h;
        BlickManager[x][y].state = true;
        BlickManager[x][y].index = 1;      
    }
    if (y % 2 == 1)
    {
        x = (rand() % 8) + 5;
        y = (rand() % 10);

        BlickManager[x][y].Texture = BlockTT;

        BlickManager[x][y].SourceRect = BlockRect;

        BlickManager[x][y].TargetRect.x = BlockW * y;
        BlickManager[x][y].TargetRect.y = BlockH * x;
        BlickManager[x][y].TargetRect.w = BlockRect.w;
        BlickManager[x][y].TargetRect.h = BlockRect.h;
        BlickManager[x][y].state = true;
        BlickManager[x][y].index = 1;
        //printf("random x=%d y=%d \n", x, y);
        //printf("random!!!");
    }
}

void draw(void)
{
    SDL_RenderClear(renderer);
    int i, j;
    for (i = 5; i < 13; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (BlickManager[i][j].state == true)
            {
                SDL_RenderCopy(renderer, BlickManager[i][j].Texture, &BlickManager[i][j].SourceRect, &BlickManager[i][j].TargetRect);
            }
        }
    }

    SDL_RenderCopy(renderer, BallTT, NULL, &BallRect);
    SDL_RenderCopy(renderer, BoardTT, NULL, &BoardRect);
    SDL_RenderCopy(renderer, Board2TT, NULL, &Board2Rect);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), scoreColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(renderer);//刷新
}

void init()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    window = SDL_CreateWindow("Bouncing Ball", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    //SDL_RenderClear(renderer);

    //載入圖片
    BallSueface = IMG_Load("Resources/images/ballgrey.bmp");//球

    BoardSurface = IMG_Load("Resources/images/Board.bmp");//反彈棒

    Board2Surface = IMG_Load("Resources/images/Boardred.bmp");//反彈棒2

    Block = IMG_Load("Resources/images/element_red_rectangle.bmp");//方塊

    breakBlock = IMG_Load("Resources/images/element_red_rectangle_break.bmp");//碎方塊

    BallTT = SDL_CreateTextureFromSurface(renderer, BallSueface);

    BoardTT = SDL_CreateTextureFromSurface(renderer, BoardSurface);

    Board2TT = SDL_CreateTextureFromSurface(renderer, Board2Surface);

    BlockTT = SDL_CreateTextureFromSurface(renderer, Block);

    breakBlockTT = SDL_CreateTextureFromSurface(renderer, breakBlock);

    //載入文字
    font = TTF_OpenFont("Resources/ttf/OpenSans-Bold.ttf", scoreFontReslution);//字體

    //初始位置
    BallRect.x = WINDOW_WIDTH / 2 - (BallSueface->w / 2);
    BallRect.y = WINDOW_HEIGHT / 10 * 9 - BallSueface->h;
    BallRect.w = BallSueface->w;
    BallRect.h = BallSueface->h;

    BoardRect.x = WINDOW_WIDTH / 2 - (BoardSurface->w / 2);
    BoardRect.y = WINDOW_HEIGHT / 10 * 9;
    BoardRect.w = BoardSurface->w;
    BoardRect.h = BoardSurface->h;

    Board2Rect.x = WINDOW_WIDTH / 2 - (BoardSurface->w / 2);
    Board2Rect.y = WINDOW_HEIGHT / 10 * 1 / 2;
    Board2Rect.w = BoardSurface->w;
    Board2Rect.h = BoardSurface->h;

    BlockRect.x = 0;
    BlockRect.y = 0;
    BlockRect.w = Block->w;
    BlockRect.h = Block->h;

    //賦予各個方塊值
    int i, j;
    for (i = 5; i < 13; i++)
    {
        for (j = 0; j < 10; j++)
        {
            BlickManager[i][j].Texture = BlockTT;

            BlickManager[i][j].SourceRect = BlockRect;

            BlickManager[i][j].TargetRect.x = BlockW * j;
            BlickManager[i][j].TargetRect.y = BlockH * i;
            BlickManager[i][j].TargetRect.w = BlockRect.w;
            BlickManager[i][j].TargetRect.h = BlockRect.h;

            BlickManager[i][j].state = false;
            BlickManager[i][j].index = 2;
        }
    }
}

double GetDistance(int x1, int y1, int x2, int y2)
{
    double distance = sqrt((double)(pow(x2 - x1, 2) + pow(y2 - y1, 2)));
    return distance;
}

int check_collision()
{
    int x = 0;// 矩形上的最近點x軸坐標
    int y = 0;//矩形上的最近點y軸坐標

    int cx = BallRect.x + BallRect.w / 2;//圓心的x軸坐標
    int cy = BallRect.y + BallRect.h / 2;//圓心的y軸坐標
    int cr = BallRect.w / 2;//圓半徑

    int i;
    int j;
    for (i = 5; i < 13; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (BlickManager[i][j].state == true)//如果state為真，判斷是否碰撞
            {
                //尋找最近的X位置
                if (cx < BlickManager[i][j].TargetRect.x)
                {
                    x = BlickManager[i][j].TargetRect.x;
                }
                else if (cx > BlickManager[i][j].TargetRect.x + BlickManager[i][j].TargetRect.w)
                {
                    x = BlickManager[i][j].TargetRect.x + BlickManager[i][j].TargetRect.w;
                }
                else
                {
                    x = cx;
                }

                //尋找最近的Y位置
                if (cy < BlickManager[i][j].TargetRect.y)
                {
                    y = BlickManager[i][j].TargetRect.y;
                }
                else if (cy > BlickManager[i][j].TargetRect.y + BlickManager[i][j].TargetRect.h)
                {
                    y = BlickManager[i][j].TargetRect.y + BlickManager[i][j].TargetRect.h;
                }
                else
                {
                    y = cy;
                }

                if (GetDistance(cx, cy, x, y) <= cr)//當兩點距離小於半徑，發生碰撞
                {
                    //BlickManager[i][j].state = false;
                    BlickManager[i][j].index = 2;//把方塊設置成碎方塊    

                    if (cx < x && y >= BlickManager[i][j].TargetRect.y && y <= BlickManager[i][j].TargetRect.y + BlockH)//碰到左邊
                    {
                        //printf("left test");
                        if ((scorefixpt != 1) || (scorefix == false))//避免重複碰撞
                        {
                            randnum();
                            scorefix = true;
                            scorefixpt = 1;
                            ++score;//加分
                            //printf("touch left\n");
                        }
                        return 1;
                    }
                    else if (cx > x && y >= BlickManager[i][j].TargetRect.y && y <= BlickManager[i][j].TargetRect.y + BlockH)//碰到右邊
                    {

                        //printf("right test");
                        if (scorefixpt != 2 || (scorefix == false))//避免重複碰撞
                        {
                            randnum();
                            scorefix = true;
                            scorefixpt = 2;
                            ++score;//加分
                            //printf("touch right\n");
                        }
                        return 2;
                    }
                    else if (cy < y && x >= BlickManager[i][j].TargetRect.x && x <= BlickManager[i][j].TargetRect.x + BlockW)//碰到上邊
                    {
                        //printf("top test");
                        if (scorefixpt != 3)//避免重複碰撞
                        {
                            randnum();
                            scorefixpt = 3;
                            ++score;//加分
                            //printf("touch top\n");

                        }
                        return 3;
                    }
                    else if (cy > y && x >= BlickManager[i][j].TargetRect.x && x <= BlickManager[i][j].TargetRect.x + BlockW)//碰到下邊
                    {

                        //printf("bottom test");
                        if (scorefixpt != 4 )//避免重複碰撞
                        {
                            randnum();
                            scorefixpt = 4;
                            ++score;//加分
                            //printf("touch button\n");
                        }
                        return 4;
                    }
                    else
                    {
                        //printf("x = %d, y = %d\n",x,y);
                    }
                }
            }
        }
    }
    return 0;
}

