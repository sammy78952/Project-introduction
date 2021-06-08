#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
//視窗属性
#define WINDOW_WIDTH  640//視窗寬度
#define WINDOW_HEIGHT  600//視窗長度


//位置
SDL_Rect BlockRect; //方塊
SDL_Rect BallRect; //球
SDL_Rect BoardRect; //反彈棒
SDL_Rect Board2Rect;

//事件
SDL_Event event;

//指针
SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

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
int BallSpeed = 2;//小球移動速度

//控制反彈棒的移動方向
bool moveBoardLeft = false;
bool moveBoardRight = false;
bool moveBoard2Left = false;
bool moveBoard2Right = false;
//控制球的移動方向
bool moveRight = true;
bool moveDown = true;
//
int index;
//子函數
void init(void);//初始化函數
int UpdateLogic(void);//更新邏輯
void randnum();
void draw(void);
int check_collision();//檢查球與方塊的碰撞
bool checkCollision(SDL_Rect A, SDL_Rect B);
bool checkCollision(SDL_Rect A, SDL_Rect C);//球與反彈棒相撞檢測
double GetDistance(int x1, int y1, int x2, int y2);//兩點距離



int main(int argc, char* argv[])
{
    init();//初始化
    bool quit = false;
    while (quit == false)
    {
        while (SDL_PollEvent(&event))
        {

            if (event.type == SDL_QUIT)//退出事件
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)//鍵盤事件
            {
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

                if (event.key.keysym.sym == SDLK_UP)//加速
                {
                    BoardSpeed = 5;
                }
                if (event.key.keysym.sym == SDLK_w)
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
                if (event.key.keysym.sym == SDLK_w)
                {
                    Board2Speed = 3;
                }
            }

            randnum();
        }


        if (UpdateLogic() == 1) {
            UpdateLogic();//更新邏輯

        }
        else {
            
            quit = true;
        }

        SDL_RenderClear(renderer);
        draw();
        SDL_RenderPresent(renderer);//刷新
        SDL_Delay(10);//釋放CPU
    }
    SDL_FreeSurface(Block);
    SDL_FreeSurface(BallSueface);
    SDL_DestroyWindow(window);
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

int UpdateLogic(void)
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
        if ((BallRect.x + BallRect.w >= WINDOW_WIDTH) || (1 == check_collision()))
        {
            moveRight = false;
            //printf("trun left\n");
        }
    }
    else
    {
        BallRect.x = BallRect.x - BallSpeed;
        if ((BallRect.x <= 0) || (2 == check_collision()))
        {
            moveRight = true;
            //printf("trun right\n");
        }
    }

    if (true == moveDown)
    {
        BallRect.y = BallRect.y + BallSpeed;
        if (BallRect.y + BallRect.h >= WINDOW_HEIGHT) {
            
            printf("Game over");
            SDL_Quit();
            return 0;
        }
        if (checkCollision(BallRect, BoardRect) || (3 == check_collision()))
        {
            moveDown = false;
            //printf("trun up\n");
        }
    }
    else
    {
        BallRect.y = BallRect.y - BallSpeed;
        if (BallRect.y <= 0) {
           
            printf("Game over");
            SDL_Quit();
            return 0;
        }
        if (checkCollision(BallRect, Board2Rect) || 4 == check_collision())
        {
            moveDown = true;
            //printf("trun down\n");
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
void randnum() {
    int x, y;
    int max = 9, min = 0;

    x = rand() % (max - min + 1) + min;
    y = rand() % (max - min + 1) + min;

    BlickManager[x][y].index = 1;



}

void draw(void)
{
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
}

void init(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Bouncing Ball", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);

    //載入圖片
    BallSueface = IMG_Load("C:/Project_image/ballgrey.bmp");//球

    BoardSurface = IMG_Load("C:/Project_image/Board.bmp");//反彈棒

    Board2Surface = IMG_Load("C:/Project_image/Boardred.bmp");//反彈棒2

    Block = IMG_Load("C:/Project_image/element_red_rectangle.bmp");//方塊

    breakBlock = IMG_Load("C:/Project_image/element_red_rectangle_break.bmp");//碎方塊

    BallTT = SDL_CreateTextureFromSurface(renderer, BallSueface);

    BoardTT = SDL_CreateTextureFromSurface(renderer, BoardSurface);

    Board2TT = SDL_CreateTextureFromSurface(renderer, Board2Surface);

    BlockTT = SDL_CreateTextureFromSurface(renderer, Block);

    breakBlockTT = SDL_CreateTextureFromSurface(renderer, breakBlock);

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

            BlickManager[i][j].state = true;
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
                        //printf("touch left\n");
                        return 1;
                    }
                    else if (cx > x && y >= BlickManager[i][j].TargetRect.y && y <= BlickManager[i][j].TargetRect.y + BlockH)//碰到右邊
                    {
                        //printf("touch right\n");
                        return 2;
                    }
                    else if (cy < y && x >= BlickManager[i][j].TargetRect.x && x <= BlickManager[i][j].TargetRect.x + BlockW)//碰到上邊
                    {
                        //printf("touch top\n");
                        return 3;
                    }
                    else if (cy > y && x >= BlickManager[i][j].TargetRect.x && x <= BlickManager[i][j].TargetRect.x + BlockW)//碰到下邊
                    {
                        //printf("touch button\n");
                        return 4;
                    }
                    else
                    {
                        //printf("x:%d...y:%d\n",x,y);
                    }
                }
            }
        }
    }

    return 0;
}