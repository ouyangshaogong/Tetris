// ConsoleApplication15.cpp : �������̨Ӧ�ó������ڵ㡣
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
    POINT point[4];//��¼��ǰ�����ڱ����е�λ��
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
    RESTART,  //������Ϸ
    STOP,     //��ͣ��Ϸ
    MODIFYLEVEL,  //�޸���Ϸ����
    QUIT        //�˳�
};

HANDLE g_hConsoleOutput; // ����̨������
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

    RecordPosInBackCube(cubeShape, lines, cols, DOWN); //�����ʼλ�õ�point
    FillPosToBackCube(false); //��䷽��
    PrintCurrentCube(); //������
    FillPosToBackCube(true); //��䷽��
    
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
                printf("���뼶��:");
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
	g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨������
	
	static COORD size = {85, 35};
    SetConsoleScreenBufferSize(g_hConsoleOutput, size); // �������û�������С

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
    RecordPosInBackCube(cubeShape, lines, cols, direction); //�ȼ�¼��һ�η�����ͼ�е�λ��
    bool IsGetNextCube = false;
    bool IsGetKey = false;
    if (IsConflict(IsGetNextCube))  //�ж��Ƿ��ͻ
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
        
        RecordPosInBackCube(cubeShape, lines, cols, direction); //������һ�εķ���
        FillPosToBackCube(false); //��䷽��
        PrintCurrentCube(); //������
        EliminateLines();
        if (IsGetKey)
        {
            FillPosToBackCube(true); //����
        }
        
        if (IsGetNextCube) //�Ƿ����»�ȡ����״
        {
            cubeShape = g_cubeShape;
            g_cubeShape = rand() % 7;
            lines = 0;
            cols = CUBE_GROUND_COLS /2 - 2;
        }

    }
    else
    {
        FillPosToBackCube(false); //��䷽��
        PrintCurrentCube(); //������
        FillPosToBackCube(true); //����
    }
}

bool IsConflict(bool &IsGetNextCube)
{
    for(int i = 0; i < CUBE_SIZE; i++)
    {
        if (g_gamePool.CubeBackGround[g_gamePool.point[i].x][g_gamePool.point[i].y]) //����ĸ�λ�ã���һ��λ��Ϊ1�����ͻ
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
    //�Ȱ����Խ��߷�ת
    for(int i = 0; i < CUBE_SIZE; i++)
    {
        for(int j = 0; j < i; j++)
        {
            temp = cube[i][j];
            cube[i][j] = cube[j][i];
            cube[j][i] = temp;
        }
    }

    //��ֱ���߷�ת
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
    //�Ȱ����Խ��߷�ת
    for(int i = 0; i < CUBE_SIZE; i++)
    {
        for(int j = 0; j < i; j++)
        {
            temp = cube[i][j];
            cube[i][j] = cube[j][i];
            cube[j][i] = temp;
        }
    }

    //���·�ת
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
    case 72://��
        CubeShapeRightSwitch(cube);//˳ʱ����ת
        break;
    case 75://��
        cols--;
        break;
    case 77://��
        cols++;
        break;
    case 80://��
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
            
            if (((i == CUBE_GROUND_LINES - 1) && g_gamePool.CubeBackGround[i][j]) //��ӡ�߽�
                || (j == 0 && g_gamePool.CubeBackGround[i][j])
                || ((j == CUBE_GROUND_COLS - 1) && g_gamePool.CubeBackGround[i][j]))
            {
                SetConsoleTextAttribute(g_hConsoleOutput, 0x0006);
				printf("��");
                continue;
            }
            
			if (g_gamePool.CubeBackGround[i][j])       //��ӡ��Ϸ��
			{
                SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_GREEN);
				printf("��");
			}
            else
            {
               // SetConsoleTextAttribute(g_hConsoleOutput, 0x0006);
				printf("%2s", "");
                //printf("��");
            }
            
		}

        PrintNextCube(i);  //��ӡ��һ������
        PrintScoreAndLevel(i); //��ӡ�����ͼ���
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
            printf("��");
        }
    }

    if (i == 21 || i == 22 || i == 23 
        || i == 24 || i == 25 || i == 26)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");
        
        if (i == 22)
        {
            printf("����:r");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else if (i == 23)
        {
            printf("��ͣ:s");
            printf("%2s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else if (i == 24)
        {
            printf("�޸ļ���:m");
            printf("%2s", "");
        }
        else if (i == 25)
        {
            printf("�˳�:q");
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
        printf("��");
    }

    if (i == 27)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");

        printf("��");
        printf("��");
        printf("��");
        printf("��");
        printf("��");
        printf("��");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");
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
            printf("��");
        }
    }
    
    
    if (i == 3 || i == 4 || i == 5 || i == 6)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");

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
				printf("��");
            }
            else
            {
                printf("%2s", "");
            }
            
        }

        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");
    }

    if (i == 7)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");

        printf("��");
        printf("��");
        printf("��");
        printf("��");
        printf("��");
        printf("��");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");
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
            printf("��");
        }
    }

    if (i == 11 || i == 12 || i == 13 
        || i == 14 || i == 15 || i == 16)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");
        
        if (i == 12)
        {
            printf("����:%2d", g_nScore);
            //printf("%1s", "");
            printf("%1s", "");
            printf("%2s", "");
            printf("%2s", "");
        }
        else if (i == 15)
        {
            printf("����:%2d", g_nLevel);
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
        printf("��");
    }

    if (i == 17)
    {
        printf("%2s", "");
        printf("%2s", "");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");

        printf("��");
        printf("��");
        printf("��");
        printf("��");
        printf("��");
        printf("��");
        SetConsoleTextAttribute(g_hConsoleOutput, FOREGROUND_RED);
        printf("��");
    }
}