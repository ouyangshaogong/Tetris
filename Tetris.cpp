// ConsoleApplication15.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define CUBE_GROUND_COLS 30
#define CUBE_GROUND_LINES 30
#define CUBE_SIZE 4
#define CUBE_COUNT 7

#define DOWN 80
#define UP 72
#define LEFT 75
#define RIGHT 77

#define ERROR_KEY 224

struct GamePool
{
    int CubeBackGround[CUBE_GROUND_LINES][CUBE_GROUND_COLS];
    POINT point[4];//记录当前方块在背景中的位置
    int direction;
};


int g_Cube[][CUBE_SIZE][CUBE_SIZE] = {
    {
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    
    {
        {1, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    },
    { 
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
  
};

enum GameState
{
    RUNNING = 0,
    RESTART,  //重启游戏
    STOP,     //暂停游戏
    MODIFYLEVEL,  //修改游戏级别
    QUIT        //退出
};

HANDLE g_hConsoleOutput; // 控制台输出句柄
GamePool g_gamePool;
int g_cubeShape = 0;
int g_nScore = 0;
int g_nLevel = 4;
int g_nSpeed = 1000;
GameState g_gameState = STOP;

void InitGame();
void InitGamePool();
void StartGame();
void FillAndPrintCube(int &cubeShape, int &lines, int &cols, int &direction);
void RecordPosInBackCube(int cubeShape, int lines, int cols, int direction);
void FillPosToBackCube(bool IsClearZero);
void PrintCurrentCube();
bool CalcNextCubeShape(int key, int cube[CUBE_SIZE][CUBE_SIZE], int &lines, int &cols);
bool IsConflict(bool &IsGetNextCube);
void CubeShapeLeftSwitch(int cube[CUBE_SIZE][CUBE_SIZE]);
void CubeShapeRightSwitch(int cube[CUBE_SIZE][CUBE_SIZE]);
void ClearBackGround();
void MoveCubeDown(int cube[][CUBE_SIZE]);
void EndGame();
void PrintNextCube(int i);
void PrintScoreAndLevel(int i);
void MoveDownLines(int k);
void EliminateLines();
void CalcDownSpeedLevel(int nScore);
void PrintKeyOptions(int i);

int main()
{
    InitGame();
    
    while(1)
    {
        if (g_gameState == RUNNING)
        {
            StartGame();
        }
        else if (g_gameState == RESTART)
        {
            InitGamePool();
            StartGame();
        }
        else if (g_gameState == QUIT)
        {
            break;
        }
    }
    

    EndGame();

    return 0;
}

void StartGame()
{ 
    g_gameState = RUNNING;
    g_cubeShape = rand() % 7;
    int cubeShape = g_cubeShape;
    g_cubeShape = rand() % 7;
    int lines = 0;
    int cols = CUBE_GROUND_COLS / 2 - 2;
    int direction = 0;

    RecordPosInBackCube(cubeShape, lines, cols, DOWN); //计算初始位置的point
    FillPosToBackCube(false); //填充方块
    PrintCurrentCube(); //画界面
    FillPosToBackCube(true); //填充方块
    
    while(1)
    {
        if (_kbhit())
        {
            direction = _getch();
            if (direction == ERROR_KEY)
            {
                continue;
            }

            if (direction == 'r')
            {
                g_gameState = RESTART;
                break;
            }

            if (direction == 's')
            {
                getchar();
            }
            
            if (direction == 'm')
            {
                int level = 0;
                printf("输入级别:");
                scanf("%d", &level);

                if (level == 4)
                {
                    g_nLevel = 4;
                    g_nSpeed = 1000;
                }
                else if (level == 3)
                {
                    g_nLevel = 3;
                    g_nSpeed = 100;
                }
                else if (level == 2)
                {
                    g_nLevel = 2;
                    g_nSpeed = 10;
                }
                else if (level == 1)
                {
                    g_nLevel = 1;
                    g_nSpeed = 1;
                }
                else
                {
                    g_nLevel = 4;
                    g_nSpeed = 1000;
                }
                
                COORD c;
                CONSOLE_SCREEN_BUFFER_INFO pBuffer;
                GetConsoleScreenBufferInfo(g_hConsoleOutput, &pBuffer);
                

                c.X=pBuffer.dwCursorPosition.X + 10;
                c.Y=pBuffer.dwCursorPosition.Y - 1;
                SetConsoleCursorPosition(g_hConsoleOutput, c);
                printf("\b\b\b\b\b\b\b\b\b\b");
                printf("          ");
            }

            if (direction == 'q')
            {
                g_gameState = QUIT;
                break;
            }
            
            CalcNextCubeShape(direction, g_Cube[cubeShape], lines, cols); 
        } 
        else 
        {
            
            Sleep(g_nSpeed);
            direction = DOWN;
            lines++;
        }
    
        FillAndPrintCube(cubeShape, lines, cols, direction);
    }
}

void InitGame()
{
	system("mode con cols=85 lines=35");
	g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
	
	static COORD size = {85, 35};
    SetConsoleScreenBufferSize(g_hConsoleOutput, size); // 重新设置缓冲区大小

    srand((unsigned)time(NULL));
    
    InitGamePool();

    g_gameState = RUNNING;
}

void InitGamePool()
{
    for (int i = 0; i < CUBE_GROUND_LINES; i++)
    {
        for (int j = 0; j < CUBE_GROUND_COLS; j++)
        {
            g_gamePool.CubeBackGround[i][j] = 0;
        }
    }

    for(i = 0; i < CUBE_GROUND_LINES; i++)
    {
        g_gamePool.CubeBackGround[i][0] = 1;
    }
    
    for(i = 0; i < CUBE_GROUND_COLS; i++)
    {
        g_gamePool.CubeBackGround[CUBE_GROUND_LINES - 1][i] = 1;
    }

    for(i = 0; i < CUBE_GROUND_LINES; i++)
    {
        g_gamePool.CubeBackGround[i][CUBE_GROUND_COLS - 1] = 1;
    }


}


void EndGame()
{
	CloseHandle(g_hConsoleOutput);
}

void FillAndPrintCube(int &cubeShape, int &lines, int &cols, int &direction)
{
    RecordPosInBackCube(cubeShape, lines, cols, direction); //先记录下一次方块在图中的位置
    bool IsGetNextCube = false;
    bool IsGetKey = false;
    if (IsConflict(IsGetNextCube))  //判断是否冲突
    {
        if (direction == DOWN)
        {
            lines -= 1;
        }
        
        if (direction == UP)
        {
            IsGetKey = true;
            CubeShapeLeftSwitch(g_Cube[cubeShape]);
        } 

        else if (direction == LEFT)
        {
            cols += 1;
            IsGetKey = true;
        }
        else if (direction == RIGHT)
        {
            cols -= 1;
            IsGetKey = true;
        }
        
        RecordPosInBackCube(cubeShape, lines, cols, direction); //计算上一次的方块
        FillPosToBackCube(false); //填充方块
        PrintCurrentCube(); //画界面
        EliminateLines();
        if (IsGetKey)
        {
            FillPosToBackCube(true); //清零
        }
        
        if (IsGetNextCube) //是否重新获取新形状
        {
            cubeShape = g_cubeShape;
            g_cubeShape = rand() % 7;
            lines = 0;
            cols = CUBE_GROUND_COLS /2 - 2;
        }

    }
    else
    {
        FillPosToBackCube(false); //填充方块
        PrintCurrentCube(); //画界面
        FillPosToBackCube(true); //清零
    }
}

bool IsConflict(bool &IsGetNextCube)
{
    for(int i = 0; i < CUBE_SIZE; i++)
    {
        if (g_gamePool.CubeBackGround[g_gamePool.point[i].x][g_gamePool.point[i].y]) //如果四个位置，有一个位置为1，则冲突
        {
            if (g_gamePool.direction == DOWN)
                IsGetNextCube = true;
            return true;
        }
    }

    return false;
}

void CalcDownSpeedLevel(int nScore)
{
    if (nScore <= 30)
    {
        g_nSpeed = 1000;
        g_nLevel = 4;
    }
    else if (nScore > 30 && nScore <= 100)
    {
        g_nSpeed = 100;
        g_nLevel = 3;
    }
    else if (nScore > 100 && nScore <= 300)
    {
        g_nSpeed = 10;
        g_nLevel = 2;
    }
    else if (nScore > 300)
    {
        g_nSpeed = 1;
        g_nLevel = 1;
    }
}

void EliminateLines()
{
    bool isAllFull = true;
    for (int i = CUBE_GROUND_LINES - 2; i >= 1; i--)
    {
        for (int j = 0; j < CUBE_GROUND_COLS - 1; j++)
        {
            if (!g_gamePool.CubeBackGround[i][j])
            {
                isAllFull = false;
            }
        }

        if (isAllFull)
        {
            g_nScore += 10;
            CalcDownSpeedLevel(g_nScore);
            MoveDownLines(i);
            i++;
        }
    }
}

void MoveDownLines(int k)
{
    for (int i = k; i > 0; i--)
    {
        for (int j = 0; j < CUBE_GROUND_COLS - 1; j++)
        {
            g_gamePool.CubeBackGround[i][j] = g_gamePool.CubeBackGround[i - 1][j];
        }
    }
    
    for (int j = 1; j < CUBE_GROUND_COLS - 1; j++)
    {
        g_gamePool.CubeBackGround[0][j] = 0;
    }
    
}

void RecordPosInBackCube(int cubeShape, int lines, int cols, int direction)
{
    int p = 0;
    for(int i = 0; i< CUBE_SIZE; i++)
    {
        for(int j = 0; j < CUBE_SIZE; j++)
        {
            if (g_Cube[cubeShape][i][j])
            {
                g_gamePool.point[p].x = lines + i;
                g_gamePool.point[p].y = cols + j;
                
                p++;
            }    
        }
    }

    g_gamePool.direction = direction;
}

void FillPosToBackCube(bool IsClearZero)
{
    for(int i = 0; i< CUBE_SIZE; i++)
    {
        if (IsClearZero)
        {
            g_gamePool.CubeBackGround[g_gamePool.point[i].x][g_gamePool.point[i].y] = 0;
        }
        else
        {
            g_gamePool.CubeBackGround[g_gamePool.point[i].x][g_gamePool.point[i].y] = 1;
        }  
    }
}

void CubeShapeRightSwitch(int cube[CUBE_SIZE][CUBE_SIZE])
{
    int temp = 0;
    //先按主对角线反转
    for(int i = 0; i < CUBE_SIZE; i++)
    {
        for(int j = 0; j < i; j++)
        {
            temp = cube[i][j];
            cube[i][j] = cube[j][i];
            cube[j][i] = temp;
        }
    }

    //垂直中线反转
    for(i = 0; i < CUBE_SIZE; i++)
    {
        for(int j = 0; j < CUBE_SIZE/2; j++)
        {
            temp = cube[i][j];
            cube[i][j] = cube[i][CUBE_SIZE - 1 - j];
            cube[i][CUBE_SIZE - 1 - j] = temp;
        }
    }
}

void CubeShapeLeftSwitch(int cube[CUBE_SIZE][CUBE_SIZE])
{
    int temp = 0;
    //先按主对角线反转
    for(int i = 0; i < CUBE_SIZE; i++)
    {
        for(int j = 0; j < i; j++)
        {
            temp = cube[i][j];
            cube[i][j] = cube[j][i];
            cube[j][i] = temp;
        }
    }

    //上下反转
    for(i = 0; i < CUBE_SIZE/2; i++)
    {
        for(int j = 0; j < CUBE_SIZE; j++)
        {
            temp = cube[i][j];
            cube[i][j] = cube[CUBE_SIZE - 1 - i][j];
            cube[CUBE_SIZE - 1 - i][j] = temp;
        }
    }
}

bool CalcNextCubeShape(int key, int cube[CUBE_SIZE][CUBE_SIZE], int &lines, int &cols)
{
    switch (key)
    {
    case 72://上
        CubeShapeRightSwitch(cube);//顺时针旋转
        break;
    case 75://左
        cols--;
        break;
    case 77://右
        cols++;
        break;
    case 80://下
        lines++;
        break;
    default:
        return false;
    }

    return true;
}

void PrintCurrentCube()
{   
    static COORD pos = {0, 0};
    SetConsoleCursorPosition(g_hConsoleOutput, pos);  
    for(int i = 0; i < CUBE_GROUND_LINES; i++)
	{
		for(int j = 0; j < CUBE_GROUND_COLS; j++)
		{
            
            if (((i == CUBE_GROUND_LINES - 1) && g_gamePool.CubeBackGround[i][j]) //打印边界
                || (j == 0 && g_gamePool.CubeBackGround[i][j])
                || ((j == CUBE_GROUND_COLS - 1) && g_gamePool.CubeBackGround[i][j]))
            {
                SetConsoleTextAttribute(g_hConsoleOutput, 0x0006);
				printf("■");
                continue;
            }
            
			if (g_gamePool.CubeBackGround[i][j])       //打印游戏池
			{
                SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN);
				printf("■");
			}
            else
            {
               // SetConsoleTextAttribute(g_hConsoleOutput, 0x0006);
				printf("%2s", "");
                //printf("■");
            }
            
		}

        PrintNextCube(i);  //打印下一个方块
        PrintScoreAndLevel(i); //打印分数和级别
        PrintKeyOptions(i);
		printf("\n");
	}
        
}

void PrintKeyOptions(int i)
{
    if (i == 20)
    {
        printf("%2s", "");
        printf("%2s", "");

        for (int k = 0; k < 8; k++)
        {
            SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
            printf("■");
        }
    }

    if (i == 21 || i == 22 || i == 23 
        || i == 24 || i == 25 || i == 26)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
        
        if (i == 22)
        {
            printf("重启:r");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else if (i == 23)
        {
            printf("暂停:s");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else if (i == 24)
        {
            printf("修改级别:m");
            printf("%2s", "");
        }
        else if (i == 25)
        {
            printf("退出:q");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else
        {
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        
        
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
    }

    if (i == 27)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");

        printf("■");
        printf("■");
        printf("■");
        printf("■");
        printf("■");
        printf("■");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
    }
}

void PrintNextCube(int i)
{
    if (i == 2)
    {
        printf("%2s", "");
        printf("%2s", "");

        for (int k = 0; k < 8; k++)
        {
            SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
            printf("■");
        }
    }
    
    
    if (i == 3 || i == 4 || i == 5 || i == 6)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");

        printf("%2s", "");

        for (int k = 0; k < 4; k++)
        {
            if (i == 3 )
            {
                printf("%2s", "");
                printf("%2s", "");
                printf("%2s", "");
                printf("%2s", "");
                break;
            }
            
            if (g_Cube[g_cubeShape][i - 4][k])
            {
                SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN);
				printf("■");
            }
            else
            {
                printf("%2s", "");
            }
            
        }

        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
    }

    if (i == 7)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");

        printf("■");
        printf("■");
        printf("■");
        printf("■");
        printf("■");
        printf("■");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
    }
}

void PrintScoreAndLevel(int i)
{
    if (i == 10)
    {
        printf("%2s", "");
        printf("%2s", "");

        for (int k = 0; k < 8; k++)
        {
            SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
            printf("■");
        }
    }

    if (i == 11 || i == 12 || i == 13 
        || i == 14 || i == 15 || i == 16)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
        
        if (i == 12)
        {
            printf("分数:%2d", g_nScore);
            //printf("%1s", "");
            printf("%1s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else if (i == 15)
        {
            printf("级别:%2d", g_nLevel);
            printf("%1s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else
        {
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
    }

    if (i == 17)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");

        printf("■");
        printf("■");
        printf("■");
        printf("■");
        printf("■");
        printf("■");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("■");
    }
}