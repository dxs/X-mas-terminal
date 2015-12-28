#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#ifdef WIN32
	#include <windows.h>
    #define FOND    32//240
    #define SLEEP   200
#else//LINUX
	#include <sys/ioctl.h>
    #define FOND    32//178
    #define SLEEP   2
#endif

#define MAGIC 	0x1B

//ATTRIBUTE
#define OFF 	0
#define BOLD 	1
#define UNDERSC	4
#define BLINK	5
#define REVERSE	7
#define CONCEAL	8

//FOREGROUND COLOR
#define F_BLACK		30
#define F_RED		31
#define F_GREEN		32
#define F_YELLOW	33
#define F_BLUE 		34
#define F_MAGENTA	35
#define F_CYAN 		36
#define F_WHITE 	37

//BACKGROUND COLOR
#define B_BLACK		40
#define B_RED		41
#define B_GREEN		42
#define B_YELLOW 	43
#define B_BLUE 		44
#define B_MAGENTA 	45
#define B_CYAN 		46
#define B_WHITE 	47

struct TSIZE
{
	int width;
	int height;
	int sapin_height;
	int sapin_width;
    int sapin_position;
	int sky_height;
};

static void clear();
static int  tWidth();
static int  tHeight();
static void clearColor();
static void setColor(int foreground, int background, int style);
static void fillSapin(struct TSIZE dat, char *C, char *F, char *B);
static void fillGround(struct TSIZE dat, char *C, char *F, char *B);
static void fillTerminal(struct TSIZE dat,char *C, char *F, char *B);
static void sapin(struct TSIZE*);
static int  ground(int height);
static void OUTPUT(struct TSIZE, char*, char*, char*);
static void OUTPUTMATRIX(struct TSIZE, char*, char*);
static void sapinEtoile(struct TSIZE, char*, char*, char*);
static void fillSnow(struct TSIZE, char*, char*, char*);

int main()
{
	clear();
	struct TSIZE terminal;
	terminal.width = tWidth();
	terminal.height = tHeight();
	//printf("TERMIN\tHEIGHT : %d\tWIDTH : %d\n", terminal.height, terminal.width);	
    terminal.sky_height = ground(terminal.height);
	sapin(&terminal);

    char arrayC[tWidth()][tHeight()];
	char arrayF[tWidth()][tHeight()];
	char arrayB[tWidth()][tHeight()];
    char *C = (char*)arrayC;
    char *F = (char*)arrayF;
    char *B = (char*)arrayB;
    int i = 0, sens = 2;
    for(;i < 100; i++)
    {
        fillTerminal(terminal, C, F, B);
        fillGround(terminal, C, F, B);
        fillSapin(terminal, C, F, B);
        fillSnow(terminal, C, F, B);
   
    //OUTPUTMATRIX(terminal, F, B);
        OUTPUT(terminal, C, F, B);
        
        #ifdef WIN32
        Sleep(SLEEP);
        #else
        sleep(SLEEP);
        #endif
        clearColor();
        clear();
    }
    
	clearColor();
	getchar();
}

static void fillTerminal(struct TSIZE dat, char *C, char *F, char *B)
{
    int i, j;
	for(i = 0; i < dat.height; i++)
		for(j = 0; j < dat.width; j++)
	   {
           *(C + i*dat.width + j) = 0x20;
           *(F + i*dat.width + j) = F_WHITE;
           *(B + i*dat.width + j) = B_BLUE;
       }
}

static void fillGround(struct TSIZE dat, char *C, char *F, char *B)
{
    int i, j;
    for(i = dat.sky_height; i < dat.height; i++)
        for(j = 0; j < dat.width; j++)
        {
            *(C + i*dat.width + j) = 177;
            *(F + i*dat.width + j) = F_CYAN;
            *(B + i*dat.width + j) = B_WHITE;
        }
}

static void fillSnow(struct TSIZE dat, char *C, char *F, char *B)
{
    int skyHeight = dat.sky_height;
    int width = dat.width;
    int random, i, j;
    //sRand(time(NULL));
    for(i = 0; i < skyHeight; i++)
        for(j = 0; j < width; j++)
        {
            random = rand() % 100; //0 to 99
            if(random < 10)
            {
                if(*(B + i*width + j) != F_GREEN && *(B + i*width + j) != B_RED)
                {
                    *(C + i*width + j) = 0x2A;
                    *(F + i*width + j) = F_WHITE;
                }
            }
        }
}

static void sapinEtoile(struct TSIZE dat, char *C, char *F, char *B)
{
    int height = dat.height;
    int width = dat.width;
    int sHeight = dat.sapin_height;
    int position = dat.sapin_position;
    int j = width / 2 + 3;
    
    int start = height - (height-position) - sHeight - 2;
    position = start*width + j;
    
    *(C + position) = 177;
    *(F + position) = F_YELLOW;
    *(B + position) = B_BLACK;
    position -= width;
    *(C + position) = 177;
    *(F + position) = F_YELLOW;
    *(B + position) = B_YELLOW;
    position--;
    *(C + position) = 177;
    *(F + position) = F_YELLOW;
    *(B + position) = B_YELLOW;
    position += 2;
    *(C + position) = 177;
    *(F + position) = F_YELLOW;
    *(B + position) = B_YELLOW;
    position -= width;
    position--;
    *(C + position) = 220;
    *(F + position) = F_YELLOW;
    *(B + position) = B_BLUE; 
}

static void fillSapin(struct TSIZE dat, char *C, char *F, char *B)
{
    int height = dat.height;
    int width = dat.width;
    int sHeight = dat.sapin_height;
    int position = dat.sapin_position;
    int basTronc;
    
    //TRONC
    int i, j = width / 2 + 3, start;
    start = height - (height-position) - sHeight;
    for(i = 0; i < sHeight; i++)
    {
        position = (start + i)*width + j;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
    }
    
    //pour pair impair
    if(sHeight % 2 == 0)
    {
        basTronc = 2;
        position++;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
        position -= 2;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
        position -= width;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
        position += 2;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
    }
    else
    {
        basTronc = 1;
        position++;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
        position -= 2;
        *(C + position) = 177;
        *(F + position) = F_YELLOW;
        *(B + position) = B_RED;
    }
        
    sapinEtoile(dat, C, F, B);    
    
    position = dat.sapin_position;
    start = height - (height-position) - sHeight - 2;
    position = start*width + j;
    //printf("SAPIN\tPos : %d\tstart : %d\n", position, start);
    *(C + position) = 177;
    *(F + position) = F_YELLOW;
    *(B + position) = B_YELLOW;
    start++;
    position = start*width + j;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    position--;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    position += 2;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    start++;
    position = start*width + j;
    position--;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    position--;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    position += 3;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    position++;
    *(C + position) = 177;
    *(F + position) = F_GREEN;
    *(B + position) = F_GREEN;
    
    
    //FEUILLES DROITE
    int posV = start+1, posH = j+1, borneF = 6, borneA = 4, max, zoomH = 2;
    for(; posV < start+sHeight-basTronc; posV++)
    {
        max = posH + borneA;
        for(i = 0; posH < max; posH++)
        {
                position = posV*width + posH;
                *(C + position) = 177;
                *(F + position) = F_GREEN;
                *(B + position) = F_GREEN;
        }
        posH = j+1;
        if(borneA == borneF)
        {
            borneF += zoomH;
            borneA -= zoomH;
        }
        else
        {
            borneA += zoomH;
        }
    }
    
    //FEUILLEGAUCHE
    posV = start+1, posH = j-1, borneF = -6, borneA = -4, max, zoomH = -2;
    for(; posV < start+sHeight-basTronc; posV++)
    {
        max = posH + borneA;
        for(i = 0; posH > max; posH--)
        {
                position = posV*width + posH;
                *(C + position) = 177;
                *(F + position) = F_GREEN;
                *(B + position) = F_GREEN;
        }
        posH = j-1;
        if(borneA == borneF)
        {
            borneF += zoomH;
            borneA -= zoomH;
        }
        else
        {
            borneA += zoomH;
        }
    }
    
}

static void OUTPUTMATRIX(struct TSIZE t, char *F, char *B)
{
    int i, j;
    for(i = 0; i < t.height; i++)
        for(j = 0; j < t.width; j++)
            printf("%c", *(B + i*t.width + j));
}

static void OUTPUT(struct TSIZE t, char* C, char *F, char *B)
{
    int i, j;
    for(i = 0; i < t.height; i++)
        for(j = 0; j < t.width; j++)
        {
            clearColor();
            setColor(*(F + (i*t.width) + j), *(B + (i*t.width) + j), OFF);
            printf("%c", *(C + (i*t.width) + j));
        }
        clearColor();
}

static void sapin(struct TSIZE *t)
{
    int height, width, position;
    height = t->height * 2 / 3;
    width = height - 3;
    width = height / 2 + height % 2;
    
    position = t->sky_height + ((t->height - t->sky_height) / 2);
    
    t->sapin_height = height;
    t->sapin_width  = width;
    t->sapin_position = position;
    //printf("SAPIN\tHEIGHT : %d\tWIDTH : %d\tPOSITION : %d\n", height, width, position);
}

static void sapin_OLD(struct TSIZE *t)
{
	t->sapin_height = t->height * 2 / 3;
	int tmp = 1, i;
	
	for(i = 1; tmp < t->sapin_height; i++)
	{
		tmp = i*(i+1);
		tmp /= 2;
	}
	tmp = (i-1)*(i);
	tmp /= 2; 
	t->sapin_width = tmp;
	printf("SAPIN\tHEIGHT : %d\t WIDTH : %d\n", t->sapin_height, tmp);
}

static int ground(int height)
{
    return height * 2 / 3;
}

static void setColor(int foreground, int background, int style)
{
	printf("%c[%d;%d;%dm", MAGIC, style, foreground, background);
}

static int tWidth()
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
#endif
}

static int tHeight()
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
#endif
}

static void clearColor()
{
	printf("%c[%dm", 0x1B, 0);
}

static void clear()
{
	printf("%s", "\e[1;1H\e[2J");
}
