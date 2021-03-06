#include "Flappy.h"
#include "ConsoleGraphics.h"
#include "Menu.h"
#include <stdio.h>

enum GAME_STATE
{
	GS_PLAY = 0,
	GS_LOST,
};

enum BIRD_STATE
{
	BS_FLY = 0,
	BS_PUSH,
	BS_LOST
};

struct Wall
{
	int x;
	int up;
	int down;
};

GAME_STATE GameState;
bool       PlayLoop;
uint       TickCnt;

extern HiScoreEntry HiScores[10];

float      Y;
float      YSpeed;
BIRD_STATE BirdState;

int CntAfterPushed = 0;
extern int Score = 0;

int WingsState = 0;

Wall  Walls[WALLS_AMOUNT];

void Draw();
void Tick();
void Init();
void CheckScore();
void CheckWalls();
bool CheckCollision();
void CheckBirdState();
void MoveWalls();
void Controlls();
void PushBird();
void SaveScores();
void LostEnd();

void LostEnd()
{
	PlayLoop = false;
}

void Play()
{
	Init();
	DiscardProcessingTime();

	while(MainLoop() && PlayLoop)
	{
		if(ProcessingTime())
		{
			Tick();
			Draw();
			SwapBuffers();
		}
	}
}

void PlayTick()
{
	Y -= YSpeed;
	YSpeed -= G;
	if(YSpeed < -MAX_SPEED)
		YSpeed = -MAX_SPEED;

	CheckBirdState();
	
	Controlls();

	if(TickCnt % 5 == 0)
	{
		MoveWalls();
		CheckScore();
		TickCnt = 0;
	}

	CheckWalls();
	if(CheckCollision())
	{
		BirdState = BS_LOST;
		GameState = GS_LOST;
		TickCnt = 0;
	}

	if (BirdState == BS_PUSH)
		WingsState++;
	
	SetConsoleCaption(L"<- MATAN BIRD -|- You may consider that you've taken %d integrals! -|", Score);
}

void LostTick()
{
	Y -= YSpeed;
	YSpeed -= G ;
	if(Y >= WORLD_HEIGHT)
		Y = WORLD_HEIGHT - 1;

	if (TickCnt > 50)
		LostEnd();
}

void Controlls()
{
	char key;
	if(KeySinglePressed(&key))
	{
		if(key == ' ') // space
			PushBird();

		if(key == 27) // escape
		{
			PauseMenu();
		}
	}
}

void Tick()
{
	TickCnt++;
	switch (GameState)
	{
	case GS_PLAY:
		PlayTick();
		break;
	case GS_LOST:
		LostTick();
		break;
	}
}

void PushBird()
{
	YSpeed = PUSH_VEL;
	BirdState = BS_PUSH;
 	CntAfterPushed = 0;
}

void GenerateWall(int i)
{
	int gap;  
	
  	gap = WALL_GAP + rand() % WALL_GAP_EPSILON * (rand()%3 - 1); 
	
	Walls[i].x = WORLD_WIDTH;
	Walls[i].up = rand() % (WORLD_HEIGHT - gap) + 1;
	Walls[i].down = WORLD_HEIGHT - gap - Walls[i].up;
}

void InitWalls()
{
	for(int i=0; i<WALLS_AMOUNT; i++)
	{
		GenerateWall(i);
		Walls[i].x += WALL_DIST * i;
	}
}

void MoveWalls()
{
	for(int i=0; i<WALLS_AMOUNT; i++)
	{
		Walls[i].x--;
	}
}

void CheckWalls()
{
	for(int i=0; i<WALLS_AMOUNT; i++)
	{
		if(Walls[i].x < 0)
		{
			GenerateWall(i);
		}
	}
}

void CheckScore()
{
	for(int i=0; i<WALLS_AMOUNT; i++)
		if(Walls[i].x == BIRD_X - 1)
			Score += 1;
}

bool CheckCollision()
{
	if(Y > WORLD_HEIGHT)
	{
		Y = WORLD_HEIGHT-1;
		return true;
	}

	if(Y < 0)
		Y = 0;

	for(int i=0; i<WALLS_AMOUNT; i++)
	{
		if(BIRD_X <= Walls[i].x && Walls[i].x <= BIRD_X + 2)
		{
			if(!(Walls[i].up < Y && Y < WORLD_HEIGHT - Walls[i].down))
				return true;
		}
	}
	return false;
}

void CheckBirdState()
{
	switch (BirdState)
	{
	case BS_FLY:
		break;

	case BS_PUSH:
		CntAfterPushed++;
		if(CntAfterPushed >= PUSHING_TIME)
		{
			BirdState = BS_FLY;
		}
		break;
	}
}

void InitGame()
{
	Y = START_Y;
	YSpeed = 0.0f;
	BirdState = BS_FLY;

	TickCnt = 0;
	Score = 0;
}

void Init()
{
	SetProcessInterval(PROCESS_INTERVAL);
	InitGame();	
	InitWalls();
	GameState = GS_PLAY;

	for (int i = 0; i < 45; i++)
	{
		Y = START_Y;
		PlayTick();
		YSpeed = 0;
	}

	PlayLoop = true;
}

void DrawBird()
{
	switch (BirdState)
	{
	case BS_FLY:
		WritePosition(BIRD_X, Y, L"^v^");
		break;

	case BS_PUSH:
		if (WingsState % 4 < 2)
			WritePosition(BIRD_X, Y, L"vvv");
		else WritePosition(BIRD_X, Y, L"^v^");
		break;

	case BS_LOST:
		if(TickCnt % 6 < 3)
			WritePosition(BIRD_X, Y, L"/v\\");
		break;
	}
}

void DrawWalls()
{
	for(int i=0; i<WALLS_AMOUNT; i++)
	{
		if(Walls[i].x < WORLD_WIDTH-1)
		{
			for(int j=0; j<Walls[i].up; j++)
				WritePosition(Walls[i].x, j, L"I");
			for(int j=0; j<Walls[i].down; j++)
				WritePosition(Walls[i].x, WORLD_HEIGHT - 1 - j, L"I");
		}
	}
}

void Draw()
{
	DrawWalls();
	DrawBird();
}