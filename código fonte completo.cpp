#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include <math.h>


bool executando = true; // essa variável controla o game loop

using namespace std;


const int screen_width = 800;
const int screen_height = 448;

SDL_Event evento;

// use essa função pra converter int pra string
template <typename T>
string ToString(T val)
{
    stringstream stream;
    stream << val;
    return stream.str();
}


// essa classe não precisa de dependência
class Vector2
{
	private:
	int X, Y;

	public:

	Vector2(int X, int Y)
	{
		this->X = X;
		this->Y = Y;
	}

	~Vector2(void)
	{

	}

	int getX()
	{
		return X;
	}
	int getY()
	{
		return Y;
	}
	int setX(int X)
	{
		this->X = X;
	}
	int setY(int Y)
	{
		this->Y = Y;
	}
};

// essa classe não precisa de dependência
class MenuOption
{
private:
    string sText;
    int iXPos, iYPos;
public:
    MenuOption(string sText, int iXPos, int iYPos)
    {
        this->sText = sText;
        this->iXPos = iXPos;
        this->iYPos = iYPos;
    }


    ~MenuOption(void)
    {

    }

    string getText()
    {
        return sText;
    }

    void setText(string sText)
    {
        this->sText = sText;
    }
    int getXPos()
    {
        return iXPos;
    }

    int getYPos()
    {
        return iYPos;
    }
};



// essa classe precisa do SDL2
class CIMG
{
private:
    SDL_Texture* tIMG;
    SDL_Rect rRect;
public:
    CIMG(void) {}

	CIMG(string fileName, SDL_Renderer* rR)
    {
        setIMG(fileName, rR);
    }
    ~CIMG(void)
    {
    SDL_DestroyTexture(tIMG);
    }
    void Draw(SDL_Renderer* rR, int iXOffset, int iYOffset)
    {
        rRect.x = iXOffset;
        rRect.y = iYOffset;
        SDL_RenderCopy(rR, tIMG, NULL, &rRect);
    }

    void Draw(SDL_Renderer* rR, int iXOffset, int iYOffset, bool bRotate)
    {
        rRect.x = iXOffset;
        rRect.y = iYOffset;
        if(!bRotate)
        {
            SDL_RenderCopy(rR, tIMG, NULL, &rRect);
        }
        else
        {
            SDL_RenderCopyEx(rR, tIMG, NULL, &rRect, 180.0, NULL, SDL_FLIP_VERTICAL);
        }
    }

    void DrawVert(SDL_Renderer* rR, int iXOffset, int iYOffset)
    {
        rRect.x = iXOffset;
        rRect.y = iYOffset;
        SDL_RenderCopyEx(rR, tIMG, NULL, &rRect, 180.0, NULL, SDL_FLIP_HORIZONTAL);
    }

    void Draw(SDL_Renderer* rR, SDL_Rect rCrop, SDL_Rect rRect)
    {
        SDL_RenderCopy(rR, tIMG, &rCrop, &rRect);
    }

    void setIMG(string fileName, SDL_Renderer* rR)
    {
        fileName = "files/images/" + fileName + ".bmp";
        SDL_Surface* loadedSurface = SDL_LoadBMP(fileName.c_str());
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));
        tIMG = SDL_CreateTextureFromSurface(rR, loadedSurface);
        int iWidth, iHeight;
        SDL_QueryTexture(tIMG, NULL, NULL, &iWidth, &iHeight);
        rRect.x  = 0;
        rRect.y = 0;
        rRect.w = iWidth;
        rRect.h = iHeight;
        SDL_FreeSurface(loadedSurface);
    }

    /* ----- get & set ----- */
    SDL_Texture* getIMG()
    {
        return tIMG;
    }

    SDL_Rect getRect()
    {
        return rRect;
    }
};


// essa classe precisa de CIMG

class Text
{
private:
    CIMG* FONT;
    SDL_Rect rCrop;
    SDL_Rect rRect;
    int fontSize;
    int extraLeft, nextExtraLeft;
public:
    Text(void)
    {
        rCrop.x = 0;
        rCrop.y = 0;
        rCrop.w = 8;
        rCrop.h = 8;
        rRect.x = 0;
        rRect.y = 0;
        rRect.w = 16;
        rRect.h = 16;
        this->fontSize = 16;
        this->extraLeft = 0;
        this->nextExtraLeft = 0;
    }

    ~Text(void)
    {
        delete FONT;
    }

    void checkExtra(int iChar)
    {
        switch(iChar)
        {
            case 44: case 46: case 58: case 59: nextExtraLeft += 4 * fontSize / rRect.w; break;
        }
    }

    int getPos(int iChar)
    {
        if(iChar >= 43 && iChar < 91)
        {
            checkExtra(iChar);
            return (iChar - 43) * rCrop.w + rCrop.w;
        }
        if(iChar >= 118 && iChar < 123)
        { // v w x y z
            return (iChar - 70) * rCrop.w + rCrop.w;
        }
        return 0;
    }

    void Draw(SDL_Renderer* rR, string sText, int X, int Y, int fontSize = 16)
    {
        this->fontSize = fontSize;
        this->extraLeft = 0;
        this->nextExtraLeft = 0;
        for(unsigned int i = 0; i < sText.size(); i++)
        {
            rCrop.x = getPos(sText.at(i));
            rRect.x = X + fontSize * i - extraLeft;
            rRect.y = Y;
            rRect.w = fontSize;
            rRect.h = fontSize;
            FONT->Draw(rR, rCrop, rRect);
            extraLeft += nextExtraLeft;
            nextExtraLeft = 0;
        }
    }

    void Draw(SDL_Renderer* rR, string sText, int X, int Y, int fontSize, int iR, int iG, int iB)
    {
        this->fontSize = fontSize;
        this->extraLeft = 0;
        this->nextExtraLeft = 0;
        for(unsigned int i = 0; i < sText.size(); i++)
        {
            SDL_SetTextureColorMod(FONT->getIMG(), iR, iG, iB);
            rCrop.x = getPos(sText.at(i));
            rRect.x = X + fontSize * i - extraLeft;
            rRect.y = Y;
            rRect.w = fontSize;
            rRect.h = fontSize;
            FONT->Draw(rR, rCrop, rRect);
            extraLeft += nextExtraLeft;
            nextExtraLeft = 0;
            SDL_SetTextureColorMod(FONT->getIMG(), 255, 255, 255);
        }
    }

    void DrawCenterX(SDL_Renderer* rR, string sText, int Y, int fontSize = 16, int iR = 255, int iG = 255, int iB = 255)
    {
        int X = screen_width / 2 - getTextWidth(sText, fontSize) / 2;
        Draw(rR, sText, X, Y, fontSize, iR, iG, iB);
    }


    void Draw(SDL_Renderer* rR, string sText, int X, int Y, int iWidth, int iHeight)
    {
        for(unsigned int i = 0; i < sText.size(); i++)
        {
            rCrop.x = getPos(sText.at(i));
            rRect.x = X + iWidth * i - extraLeft;
            rRect.y = Y;
            rRect.w = iWidth;
            rRect.h = iHeight;
            FONT->Draw(rR, rCrop, rRect);

        }
    }

    void DrawWS(SDL_Renderer* rR, string sText, int X, int Y,int iR, int iG, int iB, int fontSize = 16)
    {
        this->fontSize = fontSize;
        this->extraLeft = 0;
        this->nextExtraLeft = 0;
        for(unsigned int i = 0; i < sText.size(); i++)
        {
            SDL_SetTextureColorMod(FONT->getIMG(), 0, 0, 0);
            rCrop.x = getPos(sText.at(i));
            rRect.x = X + fontSize * i - extraLeft - 1;
            rRect.y = Y + 1;
            rRect.w = fontSize;
            rRect.h = fontSize;
            FONT->Draw(rR, rCrop, rRect);
            SDL_SetTextureColorMod(FONT->getIMG(), 255, 255, 255);
            rRect.x = X + fontSize * i - extraLeft + 1;
            rRect.y = Y - 1;
            FONT->Draw(rR, rCrop, rRect);
            extraLeft += nextExtraLeft;
            nextExtraLeft = 0;
        }
    }

    int getTextWidth(string sText, int fontSize = 16)
    {
        int iOutput = sText.size() * fontSize;
        nextExtraLeft = 0;
        for(unsigned int i = 0; i < sText.size(); i++)
        {
            checkExtra(sText.at(i));
        }
        iOutput -= nextExtraLeft;
        return iOutput;
    }


    // ----- SET FONT IMG
    void setFont(SDL_Renderer* rR, string fileName)
    {
        FONT = new CIMG(fileName, rR);
    }
};


// essa classe precisa de vector, CIMG e SDL2

class Sprite
{
private:
    vector<CIMG*> tSprite;
    vector<unsigned int> iDelay;
    bool bRotate;
    int iCurrentFrame;
    int iStartFrame;
    int iEndFrame;
    unsigned long lTimePassed;
public:
    Sprite(void) {}

    Sprite(SDL_Renderer* rR, vector<string> sSprite, vector<unsigned int> iDelay, bool bRotate)
    {
        this->iDelay = iDelay;
        this->bRotate = bRotate;
        this->iCurrentFrame = 0;
        this->iStartFrame = 0;
        this->iEndFrame = sSprite.size() - 1;
        for(int i = 0; i < this->iEndFrame + 1; i++)
        {
            this->tSprite.push_back(new CIMG(sSprite[i], rR));
        }
        this->lTimePassed = 0;
    }


    ~Sprite(void)
    {
        for(vector<CIMG*>::iterator i = tSprite.begin(); i != tSprite.end(); i++)
        {
            delete (*i);
        }
    }

    void Update()
    {
        if(SDL_GetTicks() - iDelay[iCurrentFrame] > lTimePassed)
        {
            lTimePassed = SDL_GetTicks();
            if(iCurrentFrame == iEndFrame)
            {
                iCurrentFrame = 0;
            }
            else
            {
                ++iCurrentFrame;
            }
        }
    }

    /* ----- get & set ----- */
    CIMG* getTexture()
    {
        return tSprite[iCurrentFrame];
    }
    CIMG* getTexture(int iID)
    {
        return tSprite[iID];
    }
};


class Menu
{
public:
    Menu(void);
    ~Menu(void);
    vector<MenuOption*> lMO;
    // ----- ID aktywnego buttona
    int activeMenuOption;
    int numOfMenuOptions;
    virtual void Update();
    virtual void Draw(SDL_Renderer* rR);
    // ----- 0 = TOP, 1 = RIGHT, 2 = BOTTOM, 3 = LEFT
    virtual void updateActiveButton(int iDir);
};

class AboutMenu : public Menu
{
private:
    unsigned int iTime;
    int cR, cG, cB, nR, nG, nB;
    int colorStepID, iColorID;
    // ----- true = RIGHT, false = LEFT
    bool moveDirection;
    int iNumOfUnits;
public:
    AboutMenu(void);
    ~AboutMenu(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void enter();
    void launch();
    void reset();
    void nextColor();
    int getColorStep(int iOld, int iNew);
    void setBackgroundColor(SDL_Renderer* rR);
    void updateTime();
};



class Block
{
private :
    Sprite* sSprite;
    int iBlockID;
    bool bCollision;
    bool bDeath;
    bool bUse;
    bool bVisible;
public:
    Block(void);
    Block(int iBlockID, Sprite* sSprite, bool bCollision, bool bDeath, bool bUse, bool bVisible);
    ~Block(void);
    void Draw(SDL_Renderer* rR, int iOffsetX, int iOffsetY);
    /* ----- get & set ----- */
    int getBlockID();
    void setBlockID(int iID);
    Sprite* getSprite();
    bool getCollision();
    bool getDeath();
    bool getUse();
    bool getVisible();
};


class BlockDebris
{
private:
    // ----- 0 = Animation, -1 = Delete
    int debrisState;
    Vector2* vPositionL;
    Vector2* vPositionR;
    Vector2* vPositionL2;
    Vector2* vPositionR2;
    int iFrameID;
    float fSpeedX, fSpeedY;
    bool bRotate;
public:
    BlockDebris(int iXPos, int iYPos);
    ~BlockDebris(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    int getDebrisState();
};

class Bubble
{
private:
    int iXPos, iYPos;
    bool bDestoy; // -- DELETE BUBBLE
    int iBlockID;
public:
    Bubble(int iXPos, int iYPos);
    ~Bubble(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    int getBlockID();
    bool getDestroy();
};


class MainMenu : public Menu
{
private:
    bool selectWorld;
    int activeWorldID, activeSecondWorldID;
    SDL_Rect rSelectWorld;
public:
    MainMenu(void);
    ~MainMenu(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void enter();
    void escape();
    void updateActiveButton(int iDir);
};

class LoadingMenu : public Menu
{
private:
    unsigned int iTime;
public:
    LoadingMenu(void);
    ~LoadingMenu(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void updateTime();
    bool loadingType; // -- true = LOADING, false = GAME OVER
};
class OptionsMenu : public Menu
{
private:
    SDL_Rect rRect;
    SDL_Rect rSetKeyRect;
    SDL_Rect rVolumeBG;
    SDL_Rect rVolume;
    bool inSetKey, resetSetKey;
    bool escapeToMainMenu;
public:
    OptionsMenu(void);
    ~OptionsMenu(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void enter();
    void escape();
    void setKey(int keyID);
    void updateActiveButton(int iDir);
    void updateVolumeRect();
    void setEscapeToMainMenu(bool escapeToMainMenu);
};
class PauseMenu : public Menu
{
private:
    SDL_Rect rPause;
public:
    PauseMenu(void);
    ~PauseMenu(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void enter();
    void escape();
    void updateActiveButton(int iDir);
};
class MenuManager
{
private:
    CIMG* activeOption;
    MainMenu* oMainMenu;
    LoadingMenu* oLoadingMenu;
    AboutMenu* oAboutMenu;
    OptionsMenu* oOptionsMenu;
    PauseMenu* oPauseMenu;
    //Console* oConsole;
    //LevelEditor* oLE;
public:
    MenuManager(void);
    ~MenuManager(void);
    enum gameState
	{
        eMainMenu,
        eGameLoading,
        eGame,
        eAbout,
        eOptions,
        ePasue,
        //eLevelEditor,
    };
    gameState currentGameState;
    void Update();
    void Draw(SDL_Renderer* rR);
    void setBackgroundColor(SDL_Renderer* rR);
    void enter();
    void escape();
    void setKey(int keyID);
    void keyPressed(int iDir);
    void resetActiveOptionID(gameState ID);
    int getViewID();
    void setViewID(gameState viewID);
    CIMG* getActiveOption();
    void setActiveOption(SDL_Renderer* rR);
    LoadingMenu* getLoadingMenu();
    AboutMenu* getAboutMenu();
    //Console* getConsole();
    //LevelEditor* getLE();
    OptionsMenu* getOptions();
};



class Music
{
private:
    vector<Mix_Music*> vMusic;
    vector<Mix_Chunk*> vChunk;
    int iVolume;
public:
    Music(void);
    ~Music(void);
    bool musicStopped;
    enum eMusic
	{
        mNOTHING,
        mOVERWORLD,
        mOVERWORLDFAST,
        mUNDERWORLD,
        mUNDERWORLDFAST,
        mUNDERWATER,
        mUNDERWATERFAST,
        mCASTLE,
        mCASTLEFAST,
        mLOWTIME,
        mSTAR,
        mSTARFAST,
        mSCORERING,
    };

    eMusic currentMusic;
    enum eChunk
	{
        cCOIN,
        cBLOCKBREAK,
        cBLOCKHIT,
        cBOOM,
        cBOWSERFALL,
        cBRIDGEBREAK,
        cBULLETBILL,
        cDEATH,
        cFIRE,
        cFIREBALL,
        cGAMEOVER,
        cINTERMISSION,
        cJUMP,
        cJUMPBIG,
        cLEVELEND,
        cLOWTIME,
        cMUSHROOMAPPER,
        cMUSHROOMMEAT,
        cONEUP,
        cPASUE,
        cPIPE,
        cRAINBOOM,
        cSHOT,
        cSHRINK,
        cSTOMP,
        cSWIM,
        cVINE,
        cCASTLEEND,
        cPRINCESSMUSIC,
    };
    void changeMusic(bool musicByLevel, bool forceChange);
    void PlayMusic();
    void PlayMusic(eMusic musicID);
    void StopMusic();
    void PauseMusic();
    void PlayChunk(eChunk chunkID);
    Mix_Music* loadMusic(string fileName);
    Mix_Chunk* loadChunk(string fileName);
    // -- get & set
    int getVolume();
    void setVolume(int iVolume);
};

class CCFG
{
private:
    static MenuManager* oMM;
    static Text* oText;
    static CIMG* tSMBLOGO;
    static Music* oMusic;
public:
    CCFG(void);
    ~CCFG(void);
    static int GAME_WIDTH, GAME_HEIGHT;
    static bool keySpace;
    static int keyIDA, keyIDS, keyIDD, keyIDSpace, keyIDShift;
    static string getKeyString(int keyID);
    static CIMG* getSMBLOGO();
    static Text* getText();
    static MenuManager* getMM();
    static Music* getMusic();
    static bool canMoveBackward;
};


class Coin
{
private:
    int iXPos, iYPos;
    int iLEFT;
    int iSpriteID;
    int iStepID;
    bool bTOP;
    bool bDelete;
public:
    Coin(int iXPos, int iYPos);
    ~Coin(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    int getXPos();
    int getYPos();
    bool getDelete();
};


class Console
{
private:
    SDL_Rect rRect;
    vector<string> vPrint;
    unsigned int iMax;
public:
    Console(void);
    ~Console(void);
    void Draw(SDL_Renderer* rR);
    void print(int);
    void print(string sPrint);
};


class MapLevel
{
private:
    int iBlockID;
    bool spawnMushroom;
    bool powerUP; // -- true = powerUP, false = 1UP
    bool spawnStar;
    int iNumOfUse;
    // ----- Animation -----
    bool blockAnimation;
    int iYPos;
    bool bYDirection;   // ----- true TOP, false BOTTOM
    // ----- Animation -----
public:
    MapLevel(void);
    MapLevel(int iBlockID);
    ~MapLevel(void);
    void startBlockAnimation();
    int updateYPos();
    /* ----- get & set ----- */
    int getBlockID();
    void setBlockID(int iBlockID);
    int getYPos();
    int getNumOfUse();
    void setNumOfUse(int iNumOfUse);
    bool getSpawnMushroom();
    void setSpawnMushroom(bool spawnMushroom);
    bool getPowerUP();
    void setPowerUP(bool powerUP);
    bool getSpawnStar();
    void setSpawnStar(bool spawnStar);
};


class Platform
{
private:
    // -- 0 = BOT, 1 = TOP, 2 = TOP & BOT, 3 = RIGHT & LEFT, 4 = BONUS, 5 = FALLING, 6 = SEESAW, 7 = FALLING SEESAW
    int iType;
    int iXStart, iXEnd, iYStart, iYEnd;
    float fXPos, fYPos;
    // -- true = RIGHT | TOP, false = LEFT | BOT
    bool direction;
    int iSize;
    bool ON; // -- iType = 4, BONUS, OFF & ON
    int seesawPlatformID;
public:
    Platform(int iSize, int iType, int iXStart, int iXEnd, int iYStart, int iYEnd, float fXPos, float fYPos, bool direction, int seesawPlatformID = 0);
    ~Platform(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void moveY();
    void moveYReverse();
    int getXPos();
    int getYPos();
    int getSize();
    float getMoveX();
    int getMoveY();
    int getTypeID();
    void setTypeID(int iType);
    void turnON();
};

class LevelText
{
private:
    int iXPos, iYPos;
    string sText;
public:
    LevelText(int iXPos, int iYPos, string sText);
    ~LevelText(void);
    int getXPos();
    int getYPos();
    string getText();
    void setText(string sText);
};

class Event
{
private:
    bool bState; // true = OLD, false = NEW
    unsigned int stepID;
public:
    Event(void);
    ~Event(void);
    enum animationType
	{
        eTOP,
        eRIGHT,
        eRIGHTEND,
        eBOT,
        eLEFT,
        eBOTRIGHTEND,
        eENDBOT1,
        eENDBOT2,
        eENDPOINTS,
        eDEATHNOTHING,
        eDEATHTOP,
        eDEATHBOT,
        eNOTHING,
        ePLAYPIPERIGHT,
        ePLAYPIPETOP,
        eLOADINGMENU,
        eGAMEOVER,
        eBOSSEND1,
        eBOSSEND2,
        eBOSSEND3,
        eBOSSEND4,
        eBOTRIGHTBOSS,
        eBOSSTEXT1,
        eBOSSTEXT2,
        eENDGAMEBOSSTEXT1,
        eENDGAMEBOSSTEXT2,
        eMARIOSPRITE1,
        eVINE1,
        eVINE2,
        eVINESPAWN,
    };
    vector<animationType> vOLDDir;
    vector<int> vOLDLength;
    vector<animationType> vNEWDir;
    vector<int> vNEWLength;
    vector<int> reDrawX;
    vector<int> reDrawY;
    enum eventType
	{
        eNormal,
        eEnd,
        eBossEnd,
    };
    eventType eventTypeID;
    void Normal();
    void end();
    int iSpeed;
    void resetData();
    int newLevelType;
    int newMapXPos;
    int newPlayerXPos;
    int newPlayerYPos;
    bool newMoveMap;
    unsigned int iTime;
    int iDelay;
    int newCurrentLevel;
    bool inEvent;
    bool newUnderWater;
    bool endGame;
    // ----- Methods
    void Draw(SDL_Renderer* rR);
    void Animation();
    void newLevel();
    void resetRedraw();
};

class Player
{
private:
    vector<Sprite*> sMario;
    int iSpriteID;
    unsigned int iMoveAnimationTime;
    CIMG* tMarioLVLUP;
    float fXPos, fYPos;
    int iNumOfLives;
    bool unKillAble;
    bool starEffect;
    int unKillAbleTimeFrameID;
    int unKillAbleFrameID;
    bool inLevelDownAnimation;
    int inLevelDownAnimationFrameID;
    unsigned int iScore;
    unsigned int iCoins;
    int iComboPoints, iFrameID;
    // ----- LVL UP
    int powerLVL;
    // -- LEVEL CHANGE ANIMATION
    bool inLevelAnimation;
    bool inLevelAnimationType; // -- true = UP, false = DOWN
    int inLevelAnimationFrameID;
    // ----- LVL UP
    // ----- MOVE
    bool moveDirection; // true = LEFT, false = RIGHT
    bool bMove;
    bool changeMoveDirection;
    bool newMoveDirection;
    static const int maxMove = 4;
    int currentMaxMove;
    int moveSpeed;
    unsigned int iTimePassed;
    bool bSquat;
    int onPlatformID;
    // ----- MOVE
    // ----- JUMP
    int jumpState;
    float startJumpSpeed;
    float currentJumpSpeed;
    float jumpDistance;
    float currentJumpDistance;
    float currentFallingSpeed;
    bool springJump;
    // ----- JUMP
    // ----- BUBBLE
    unsigned int nextBubbleTime;
    int nextFallFrameID;
    const static int iSmallX = 24, iSmallY = 32;
    const static int iBigX = 32, iBigY = 64;
    int nextFireBallFrameID;
    // ----- Method
    void movePlayer();
    bool checkCollisionBot(int nX, int nY);
    bool checkCollisionCenter(int nX, int nY);
    Vector2* getBlockLB(float nX, float nY);
    Vector2* getBlockRB(float nX, float nY);
    Vector2* getBlockLC(float nX, float nY);
    Vector2* getBlockRC(float nX, float nY);
    Vector2* getBlockLT(float nX, float nY);
    Vector2* getBlockRT(float nX, float nY);
public:
    Player(SDL_Renderer* rR, float fXPos, float fYPos);
    ~Player(void);
    void Draw(SDL_Renderer* rR);
    void Update();
    void playerPhysics();
    void updateXPos(int iN);
    void updateYPos(int iN);
    void powerUPAnimation();
    // ----- MOVE
    void moveAnimation();
    void swimingAnimation();
    void startMove();
    void resetMove();
    void stopMove();
    void setMoveDirection(bool moveDirection);
    bool getChangeMoveDirection();
    void setChangeMoveDirection();
    void startRun();
    void resetRun();
    void createFireBall();
    // ----- MOVE
    // ----- JUMP
    void jump();
    void startJump(int iH);
    void resetJump();
    // ----- JUMP
    void setMarioSpriteID(int iID);
    int getMarioSpriteID();
    int getHitBoxX();
    int getHitBoxY();
    // ----- get & set -----
    bool getInLevelAnimation();
    void setInLevelAnimation(bool inLevelAnimation);
    int getXPos();
    void setXPos(float fXPos);
    int getYPos();
    void setYPos(float fYPos);
    int getPowerLVL();
    void setPowerLVL(int powerLVL);
    void resetPowerLVL();
    bool getUnkillAble();
    int getNumOfLives();
    void setNumOfLives(int iNumOfLives);
    bool getStarEffect();
    void setStarEffect(bool starEffect);
    int getMoveSpeed();
    bool getMove();
    bool getMoveDirection();
    void setNextFallFrameID(int nextFallFrameID);
    void setCurrentJumpSpeed(float currentJumpSpeed);
    void setMoveSpeed(int moveSpeed);
    int getJumpState();
    bool getSquat();
    void setSquat(bool bSquat);
    CIMG* getMarioLVLUP();
    Sprite* getMarioSprite();
    void addCoin();
    unsigned int getScore();
    void setScore(unsigned int iScore);
    void addComboPoints();
    int getComboPoints();
    unsigned int getCoins();
    void setCoins(unsigned int iCoins);
    void setSpringJump(bool springJump);
};
class Minion
{
public:
    Minion(void);
    ~Minion(void);
    int minionState;
    bool killOtherUnits;
    int iBlockID;
    float fXPos, fYPos;
    int iHitBoxX, iHitBoxY;
    bool minionSpawned;
    bool collisionOnlyWithPlayer;
    int deadTime;
    bool onAnotherMinion;
    // ----- true = LEFT, false = RIGHT
    bool moveDirection;
    int moveSpeed;
    int jumpState;
    float startJumpSpeed;
    float currentJumpSpeed;
    float jumpDistance;
    float currentJumpDistance;
    float currentFallingSpeed;
    // ---------- Methods
    virtual void Update();
    virtual void Draw(SDL_Renderer* rR, CIMG* iIMG);
    virtual void updateYPos(int iN);
    virtual void updateXPos();
    virtual bool updateMinion();
    virtual void minionPhysics();
    virtual void collisionEffect();
    virtual void minionDeathAnimation();
    void physicsState1();
    void physicsState2();
    void Spawn();
    void startJump(int iH);
    void resetJump();
    // ----- COLLISON
    virtual void collisionWithPlayer(bool TOP);
    virtual void points(int iPoints);
    virtual void collisionWithAnotherUnit(); // -- PLAYERFIREBALL
    virtual void lockMinion();
    // ----- get & set -----
    int getBloockID();
    void setBlockID(int iBlockID);
    int getMinionState();
    virtual void setMinionState(int minionState);
    virtual bool getPowerUP();
    int getXPos();
    int getYPos();
    void setYPos(int iYPos);
};
class Beetle : public Minion
{
private:
public:
    Beetle(int iXPos, int iYPos, bool moveDirection);
    ~Beetle(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void collisionWithPlayer(bool TOP);
    void collisionEffect();
    void setMinionState(int minionState);
};
class Bowser : public Minion
{
private:
    // -- minionState, 0 = Normal, 1 = Shot, -1 = Dead
    int nextJumpFrameID;
    bool spawnHammer;
    int nextHammerFrameID, numOfHammers;
    int iMinBlockID, iMaxBlockID;
    bool moveDir;
    int nextFireFrameID;
    int iYFireStart;
    int iFireID;
public:
    Bowser(float fXPos, float fYPos, bool spawnHammer = false);
    ~Bowser(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void createFire();
};
class BulletBill : public Minion
{
private:
public:
    BulletBill(int iXPos, int iYPos, bool moveDirection, int minionState);
    ~BulletBill(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class BulletBillSpawner : public Minion
{
private:
    int nextBulletBillFrameID;
public:
    BulletBillSpawner(int iXPos, int iYPos, int minionState);
    ~BulletBillSpawner(void);
    void Update();
    void minionPhysics();
};
class Cheep : public Minion
{
private:
    bool moveY, moveYDIR;
public:
    Cheep(int iXPos, int iYPos, int minionType, int moveSpeed, bool moveDiretion = false);
    ~Cheep(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class CheepSpawner : public Minion
{
private:
    int iXEnd;
    unsigned int iSpawnTime;
public:
    CheepSpawner(int iXPos, int iXEnd);
    ~CheepSpawner(void);
    void Update();
    void minionPhysics();
    void spawnCheep();
    void nextCheep();
};
class Fire : public Minion
{
private:
    // -- minionState, 0 = Alive, -1 = Dead
    int toYPos;
public:
    Fire(float fXPos, float fYPos, int toYPos);
    ~Fire(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void collisionWithPlayer(bool TOP);
    void minionPhysics();
};
class FireBall : public Minion
{
private:
    int radius; // -- r
    double angle;
    int iCenterX, iCenterY;
    double slice;
    int sliceID;
public:
    FireBall(int iXPos, int iYPos, int radius, int nSliceID, bool moveDirection);
    ~FireBall(void);
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void Update();
    void updateXPos();
    void collisionWithPlayer(bool TOP);
    void minionPhysics();
};
class Flower : public Minion
{
private:
    bool inSpawnState;
    int inSpawnY;
    int iX, iY; // inSpawnState draw Block
public:
    Flower(int iXPos, int iYPos, int iX, int iY);
    ~Flower(void);
    void Update();
    bool updateMinion();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void collisionWithPlayer(bool TOP);
    void setMinionState(int minionState);
};
class Goombas : public Minion
{
private:
    // ----- MinionState, 0 = Alive, 1 = Dead, -1 = Destroy
public:
    Goombas(int iX, int iY, int iBlockID, bool moveDirection);
    ~Goombas(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void collisionWithPlayer(bool TOP);
    void setMinionState(int minionState);
};
class Hammer : public Minion
{
private:
public:
    Hammer(int iXPos, int iYPos, bool moveDirection);
    ~Hammer(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class HammerBro : public Minion
{
private:
    bool moveDIR;
    float moveDistance;
    int nextJumpFrameID;
    bool newY; // true = UP, false = DOWN
    int hammerID;
    int nextHammerFrameID;
public:
    HammerBro(int iXPos, int iYPos);
    ~HammerBro(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void collisionEffect();
};
class Koppa : public Minion
{
private:
    // ----- MinionState, 0 & 3 = Alive with wings, 1 = Alive, 2 = Dead, 4 = Dead animation, -1 = Destroy
    int iDistance;
    bool flyDirection; // -- minionState 3
public:
    Koppa(int iX, int iY, int minionState, bool moveDirection, int iBlockID);
    ~Koppa(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void updateXPos();
    void collisionWithPlayer(bool TOP);
    void collisionEffect();
    void setMinionState(int minionState);
    void setMinion();
};
class Lakito : public Minion
{
private:
    int iMaxXPos;
    bool end;
    bool followPlayer;
    int nextSpikeyFrameID;
public:
    Lakito(int iXPos, int iYPos, int iMaxXPos);
    ~Lakito(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class Mushroom : public Minion
{
private:
    bool inSpawnState;
    int inSpawnY;
    bool powerUP;
    int iX, iY; // inSpawnState draw Block
public:
    Mushroom(int iXPos, int iYPos, bool powerUP, int iX, int iY);
    ~Mushroom(void);
    void Update();
    bool updateMinion();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void collisionWithPlayer(bool TOP);
    void setMinionState(int minionState);
};
class Plant : public Minion
{
private:
    bool bWait;
    unsigned int iTime;
    bool lockPlant;
    int iLength;
    int X, Y;
public:
    Plant(int fXPos, int fYPos, int iBlockID);
    ~Plant(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void collisionEffect();
    void lockMinion();
    void setMinionState(int minionState);
};
class PlayerFireBall : public Minion
{
private:
    bool bDestroy;
    int destroyFrameID;
public:
    PlayerFireBall(int X, int Y, bool moveDirection);
    ~PlayerFireBall(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void updateXPos();
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void collisionWithAnotherUnit();
    void collisionEffect();
    void setMinionState(int minionState);
};
class Spikey : public Minion
{
private:
public:
    Spikey(int iXPos, int iYPos);
    ~Spikey(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class Spring : public Minion
{
private:
    bool inAnimation;
    int nextFrameID;
    bool extraJump;
public:
    Spring(int iXPos, int iYPos);
    ~Spring(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void setMinionState(int minionState);
};
class Squid : public Minion
{
private:
    int moveXDistance, moveYDistance;
public:
    Squid(int iXPos, int iYPos);
    ~Squid(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void changeBlockID();
};
class Star : public Minion
{
private:
    bool inSpawnState;
    int inSpawnY;
    int iX, iY; // inSpawnState draw Block
public:
    Star(int iXPos, int iYPos, int iX, int iY);
    ~Star(void);
    void Update();
    bool updateMinion();
    void minionPhysics();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void collisionWithPlayer(bool TOP);
    void setMinionState(int minionState);
};
class Toad : public Minion
{
private:
public:
    Toad(int iXPos, int iYPos, bool peach);
    ~Toad(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
    void setMinionState(int minionState);
};
class UpFire : public Minion
{
private:
    // moveDirection true = TOP, false = BOT
    int iYJump;
    int nextJumpFrameID;
public:
    UpFire(int iXPos, int iYJump);
    ~UpFire(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class Vine : public Minion
{
private:
    // -- minionState = 0 VINE START, minionState = 1 VINE END
    int iX, iY;
public:
    Vine(int iXPos, int iYPos, int minionState, int iBlockID);
    ~Vine(void);
    void Update();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void minionPhysics();
    void collisionWithPlayer(bool TOP);
};
class Points
{
private:
    int iXPos, iYPos, iYLEFT;
    string sText;
    int iW, iH;
    bool bDelete;
public:
    Points(int iXPos, int iYPos, string sText);
    Points(int iXPos, int iYPos, string sText, int iW, int iH);
    ~Points(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    bool getDelete();
};


class Pipe
{
private:
    // ----- 0 = VERTICAL, 1 = HORIZONTAL -> VERTICAL, 2 = VERTICAL -> VERTICAL
    int iType;
    // ----- X, Y LEFT Block Position
    int iLX, iLY;
    // ----- X, Y RIGHT Block Position
    int iRX, iRY;
    int newPlayerPosX, newPlayerPosY;
    // ----- MAP LVL ID
    int newCurrentLevel;
    int newLevelType;
    bool newMoveMap;
    bool newUnderWater;
    int iDelay;
    int iSpeed;
public:
    Pipe(int iType, int iLX, int iLY, int iRX, int iRY, int newPlayerPosX, int newPlayerPosY, int newCurrentLevel, int newLevelType, bool newMoveMap, int iDelay, int iSpeed, bool underWater);
    ~Pipe(void);
    void checkUse();
    void setEvent();
};
class Flag
{
private:
public:
    Flag(int iXPos, int iYPos);
    ~Flag(void);
    int iXPos, iYPos;
    int iBlockID;
    int iYTextPos;
    int iPoints;
    int castleFlagExtraXPos;
    int castleFlagY;
    void Update();
    void UpdateCastleFlag();
    void Draw(SDL_Renderer* rR, CIMG* iIMG);
    void DrawCastleFlag(SDL_Renderer* rR, CIMG* iIMG);
};
class Map
{
private:
    float fXPos, fYPos;
    vector<Block*> vBlock;
    int iBlockSize; // Size of vBlock
    vector<Block*> vMinion;
    int iMinionSize; // Size of vBlock
    vector<vector<MapLevel*> > lMap;
    int iMapWidth, iMapHeight;
    vector<BlockDebris*> lBlockDebris;
    vector<Platform*> vPlatform;
    vector<LevelText*> vLevelText;
    vector<Bubble*> lBubble;
    int currentLevelID;
    int iLevelType; // 0, 1, 2;
    bool underWater;
    int iSpawnPointID;
    bool bMoveMap;
    int iFrameID;
    int iMapTime;
    bool inEvent;
    Event* oEvent;
    // ----- PLAYER -----
    Player* oPlayer;
    // ----- MINIONS -----
    vector<vector<Minion*> > lMinion;
    int iMinionListSize;
    int getListID(int nXPos);
    int getNumOfMinions(); // ----- Ilosc minionow w grze.
    // ----- MINIONS -----
    // ----- POINTS & COIN -----
    vector<Coin*> lCoin;
    vector<Points*> lPoints;
    // ----- POINTS & COIN -----
    // ----- PIPEEVENTS -----
    vector<Pipe*> lPipe;
    // ----- ENDEVENTS
    Flag* oFlag;
    bool drawLines;
    // ---------- Methods ----------
    int getStartBlock();
    int getEndBlock();
    // ----- Load -----
    void loadGameData(SDL_Renderer* rR);
    void createMap();
    void checkSpawnPoint();
    int getNumOfSpawnPoints();
    int getSpawnPointXPos(int iID);
    int getSpawnPointYPos(int iID);
    void loadLVL_1_1();
    void loadLVL_1_2();
    void loadLVL_1_3();
    void loadLVL_1_4();
    void loadLVL_2_1();
    void loadLVL_2_2();
    void loadLVL_2_3();
    void loadLVL_2_4();
    void loadLVL_3_1();
    void loadLVL_3_2();
    void loadLVL_3_3();
    void loadLVL_3_4();
    void loadLVL_4_1();
    void loadLVL_4_2();
    void loadLVL_4_3();
    void loadLVL_4_4();
    void loadLVL_5_1();
    void loadLVL_5_2();
    void loadLVL_5_3();
    void loadLVL_5_4();
    void loadLVL_6_1();
    void loadLVL_6_2();
    void loadLVL_6_3();
    void loadLVL_6_4();
    void loadLVL_7_1();
    void loadLVL_7_2();
    void loadLVL_7_3();
    void loadLVL_7_4();
    void loadLVL_8_1();
    void loadLVL_8_2();
    void loadLVL_8_3();
    void loadLVL_8_4();
    void loadMinionsLVL_1_1();
    void loadMinionsLVL_1_2();
    void loadMinionsLVL_1_3();
    void loadMinionsLVL_1_4();
    void loadMinionsLVL_2_1();
    void loadMinionsLVL_2_2();
    void loadMinionsLVL_2_3();
    void loadMinionsLVL_2_4();
    void loadMinionsLVL_3_1();
    void loadMinionsLVL_3_2();
    void loadMinionsLVL_3_3();
    void loadMinionsLVL_3_4();
    void loadMinionsLVL_4_1();
    void loadMinionsLVL_4_2();
    void loadMinionsLVL_4_3();
    void loadMinionsLVL_4_4();
    void loadMinionsLVL_5_1();
    void loadMinionsLVL_5_2();
    void loadMinionsLVL_5_3();
    void loadMinionsLVL_5_4();
    void loadMinionsLVL_6_1();
    void loadMinionsLVL_6_2();
    void loadMinionsLVL_6_3();
    void loadMinionsLVL_6_4();
    void loadMinionsLVL_7_1();
    void loadMinionsLVL_7_2();
    void loadMinionsLVL_7_3();
    void loadMinionsLVL_7_4();
    void loadMinionsLVL_8_1();
    void loadMinionsLVL_8_2();
    void loadMinionsLVL_8_3();
    void loadMinionsLVL_8_4();
    void clearPipeEvents();
    void loadPipeEventsLVL_1_1();
    void loadPipeEventsLVL_1_2();
    void loadPipeEventsLVL_1_3();
    void loadPipeEventsLVL_2_1();
    void loadPipeEventsLVL_2_2();
    void loadPipeEventsLVL_3_1();
    void loadPipeEventsLVL_4_1();
    void loadPipeEventsLVL_4_2();
    void loadPipeEventsLVL_5_1();
    void loadPipeEventsLVL_5_2();
    void loadPipeEventsLVL_6_2();
    void loadPipeEventsLVL_7_1();
    void loadPipeEventsLVL_7_2();
    void loadPipeEventsLVL_8_1();
    void loadPipeEventsLVL_8_2();
    void loadPipeEventsLVL_8_4();
    void clearLevelText();
    void pipeUse();
    void EndUse();
    void EndBoss();
    void EndBonus();
    void TPUse();
    void TPUse2();
    void TPUse3();
    bool bTP; // -- TP LOOP
    void spawnVine(int nX, int nY, int iBlockID);
    void clearMap();
    void clearMinions();
public:
    Map(void);
    Map(SDL_Renderer* rR);
    ~Map(void);
    void Update();
    void UpdatePlayer();
    void UpdateMinions();
    void UpdateMinionsCollisions();
    void UpdateBlocks();
    void UpdateMinionBlokcs();
    void Draw(SDL_Renderer* rR);
    void DrawMap(SDL_Renderer* rR);
    void DrawMinions(SDL_Renderer* rR);
    void DrawGameLayout(SDL_Renderer* rR);
    void DrawLines(SDL_Renderer* rR);
    void moveMap(int iX, int iY);
    void setSpawnPoint();
    bool blockUse(int nX, int nY, int iBlockID, int POS);
    void addPoints(int X, int Y, string sText, int iW, int iH);
    void addGoombas(int iX, int iY, bool moveDirection);
    void addKoppa(int iX, int iY, int minionState, bool moveDirection);
    void addBeetle(int X, int Y, bool moveDirection);
    void addPlant(int iX, int iY);
    void addToad(int X, int Y, bool peach);
    void addSquid(int X, int Y);
    void addCheep(int X, int Y, int minionType, int moveSpeed, bool moveDirection = false);
    void addCheepSpawner(int X, int XEnd);
    void addHammerBro(int X, int Y);
    void addHammer(int X, int Y, bool moveDirection);
    void addLakito(int X, int Y, int iMaxXPos);
    void addSpikey(int X, int Y);
    void addPlayerFireBall(int X, int Y, bool moveDirection);
    void addUpFire(int X, int iYEnd);
    void addSpring(int X, int Y);
    void addBowser(int X, int Y, bool spawnHammer = false);
    void addFire(float fX, float fY, int toYPos);
    void addFireBall(int X, int Y, int iWidth, int iSliceID, bool LEFT);
    void addVine(int X, int Y, int minionState, int iBlockID);
    void addText(int X, int Y, string sText);
    void addBubble(int X, int Y);
    void addBulletBillSpawner(int X, int Y, int minionState);
    void addBulletBill(int X, int Y, bool moveDirection, int minionState);
    void lockMinions();
    void clearPlatforms();
    void clearBubbles();
    // ********** COLLISION
    Vector2* getBlockID(int nX, int nY);
    int getBlockIDX(int nX);
    int getBlockIDY(int nY);
    // ----- LEFT
    bool checkCollisionLB(int nX, int nY, int nHitBoxY, bool checkVisible);
    bool checkCollisionLT(int nX, int nY, bool checkVisible);
    // ----- CENTER
    bool checkCollisionLC(int nX, int nY, int nHitBoxY, bool checkVisible);
    bool checkCollisionRC(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible);
    // ----- RIGHT
    bool checkCollisionRB(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible);
    bool checkCollisionRT(int nX, int nY, int nHitBoxX, bool checkVisible);
    int checkCollisionWithPlatform(int nX, int nY, int iHitBoxX, int iHitBoxY);
    bool checkCollision(Vector2* nV, bool checkVisible);
    void checkCollisionOnTopOfTheBlock(int nX, int nY);
    // ********** COLLISION
    void playerDeath(bool animation, bool instantDeath);
    // ----- LOAD
    void resetGameData();
    void loadLVL();
    void setBackgroundColor(SDL_Renderer* rR);
    string getLevelName();
    void startLevelAnimation();
    void structBush(int X, int Y, int iSze);
    void structGrass(int X, int Y, int iSize);
    void structCloud(int X, int Y, int iSize);
    void structGND(int X, int Y, int iWidth, int iHeight);
    void structGND2(int X, int Y, int iSize, bool bDir); // true = LEFT, false = RIGHT
    void structGND2(int X, int Y, int iWidth, int iHeight);
    void structUW1(int X, int Y, int iWidth, int iHeight);
    void structBonus(int X, int Y, int iWidth);
    void structBonusEnd(int X);
    void structPipe(int X, int Y, int iHeight);
    void structPipeVertical(int X, int Y, int iHeight);
    void structPipeHorizontal(int X, int Y, int iWidth);
    void structCoins(int X, int Y, int iWidth, int iHeight);
    void structBrick(int X, int Y, int iWidth, int iHeight);
    void struckBlockQ(int X, int Y, int iWidth);
    void struckBlockQ2(int X, int Y, int iWidth);
    void structEnd(int X, int Y, int iHeight);
    void structCastleSmall(int X, int Y);
    void structCastleBig(int X, int Y);
    void structCastleWall(int X, int Y, int iWidth, int iHeight);
    void structT(int X, int Y, int iWidth, int iHeight);
    void structTMush(int X, int Y, int iWidth, int iHeight);
    void structWater(int X, int Y, int iWidth, int iHeight);
    void structLava(int X, int Y, int iWidth, int iHeight);
    void structBridge(int X, int Y, int iWidth);
    void structTree(int X, int Y, int iHeigth, bool BIG);
    void structFence(int X, int Y, int iWidth);
    void structBridge2(int X, int Y, int iWidth);
    void structSeeSaw(int X, int Y, int iWidth);
    void structPlatformLine(int X);
    void structBulletBill(int X, int Y, int iHieght);
    void setBlockID(int X, int Y, int iBlockID);
    // ----- get & set -----
    float getXPos();
    void setXPos(float iYPos);
    float getYPos();
    void setYPos(float iYPos);
    int getLevelType();
    void setLevelType(int iLevelType);
    int getCurrentLevelID();
    void setCurrentLevelID(int currentLevelID);
    bool getUnderWater();
    void setUnderWater(bool underWater);
    int getMapTime();
    void setMapTime(int iMapTime);
    bool getDrawLines();
    void setDrawLines(bool drawLines);
    void setSpawnPointID(int iSpawnPointID);
    int getMapWidth();
    Block* getBlock(int iID);
    Block* getMinionBlock(int iID);
    MapLevel* getMapBlock(int iX, int iY);
    Player* getPlayer();
    Platform* getPlatform(int iID);
    Flag* getFlag();
    bool getMoveMap();
    void setMoveMap(bool bMoveMap);
    Event* getEvent();
    bool getInEvent();
    void setInEvent(bool inEvent);
};


class CCore
{
private:
    SDL_Window* janela;
    SDL_Renderer* rR;
    // ----- FPS -----
    long frameTime;
    static const int MIN_FRAME_TIME = 16;
    unsigned long lFPSTime;
    int iNumOfFPS, iFPS;
    // ----- FPS -----
    // ----- INPUT
    static bool movePressed, keyMenuPressed, keyS, keyW, keyA, keyD, keyShift;
    static bool keyAPressed, keyDPressed;
    // ----- true = RIGHT, false = LEFT
    bool firstDir;
    // ----- INPUT
    static Map* oMap;
    // ----- Methods
    void Input()
    {
        switch(CCFG::getMM()->getViewID())
        {
            case 2: case 7:
                if(!oMap->getInEvent())
                {
                    InputPlayer();
                }
                else
                {
                    resetMove();
                }
                break;
            default: InputMenu(); break;
        }
    }

    void InputPlayer();
    void InputMenu();
public:

    CCore(void)
    {
        this->iFPS = 0;
        this->iNumOfFPS = 0;
        this->lFPSTime = 0;

        janela = SDL_CreateWindow("Super Mario Bros c++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);

        if(janela == NULL)
        {
            executando = false;
        }

        rR = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);
        // ----- ICO
        SDL_Surface* loadedSurface = SDL_LoadBMP("files/images/ico.bmp");
        SDL_SetColorKey(loadedSurface, SDL_TRUE, 0xff00ff);
        SDL_SetWindowIcon(janela, loadedSurface);
        SDL_FreeSurface(loadedSurface);

        // ----- ICO
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        oMap = new Map(rR);
        CCFG::getMM()->setActiveOption(rR);
        CCFG::getSMBLOGO()->setIMG("super_mario_bros", rR);
        CCFG::getMusic()->PlayMusic();
        this->keyMenuPressed = false;
        this->movePressed = false;
        this->keyS = this->keyW = false;
        this->keyA = this->keyD = false;
        this->keyShift = false;
        this->keyAPressed = false;
        this->keyDPressed = false;
        this->firstDir = false;
        this->mouseX = 0;
        this->mouseY = 0;
        CCFG::keyIDA = SDLK_a;
        CCFG::keyIDS = SDLK_s;
        CCFG::keyIDD = SDLK_d;
        CCFG::keyIDSpace = SDLK_SPACE;
        CCFG::keyIDShift = SDLK_LSHIFT;
    }


    ~CCore(void)
    {
        delete oMap;
        SDL_DestroyRenderer(rR);
        SDL_DestroyWindow(janela);
    }


    void mainLoop();
    void Update();
    void Draw();
    void resetMove();
    static void resetKeys();
    static bool mouseLeftPressed, mouseRightPressed;
    static int mouseX, mouseY;
    /* ----- get & set ----- */
    static Map* getMap();
};


class LevelEditor
{
private:
    SDL_Rect rBlock;
    SDL_Rect rDrag;
    int currentBlockID;
public:
    LevelEditor(void);
    ~LevelEditor(void);
    void Update();
    void Draw(SDL_Renderer* rR);
    void mouseWheel(int Y);
    void editMap();
    void drawStruct(SDL_Renderer* rR);
};
class MusicManager
{
private:
    vector<Mix_Music*> vMusic;
    vector<Mix_Chunk*> vChunk;
public:
    MusicManager(void);
    ~MusicManager(void);
    Mix_Music* loadMusic(string fileName);
    void PlayMusic();
};

AboutMenu::AboutMenu(void)
{
    lMO.push_back(new MenuOption("MAIN MENU", 150, 340));
    this->numOfMenuOptions = lMO.size();
    this->cR = 93;
    this->cG = 148;
    this->cB = 252;
    this->nR = 0;
    this->nG = 0;
    this->nB = 0;
    this->colorStepID = 1;
    this->iColorID = 0;
    this->moveDirection = true;
    this->iNumOfUnits = 0;
    srand((unsigned)time(NULL));
}
AboutMenu::~AboutMenu(void)
{
}

void AboutMenu::Update() {
    if(SDL_GetTicks() >= iTime + 35) {
        this->cR = getColorStep(cR, nR);
        this->cG = getColorStep(cG, nG);
        this->cB = getColorStep(cB, nB);
        if (colorStepID >= 15 || (cR == nR && cG == nG && cB == nB)) {
            nextColor();
            colorStepID = 1;
        } else {
            ++colorStepID;
        }
        CCore::getMap()->setLevelType(rand()%4);
        if(rand()%10 < 6) {
            CCore::getMap()->addGoombas(-(int)CCore::getMap()->getXPos() + rand() % (screen_width + 128), -32, rand()%2 == 0);
            CCore::getMap()->addGoombas(-(int)CCore::getMap()->getXPos() + rand() % (screen_width + 128), -32, rand()%2 == 0);
        } else if(rand()%10 < 8) {
            CCore::getMap()->addKoppa(-(int)CCore::getMap()->getXPos() + rand() % (screen_width + 128), -32, 0, rand()%2 == 0);
            CCore::getMap()->addKoppa(-(int)CCore::getMap()->getXPos() + rand() % (screen_width + 128), -32, 0, rand()%2 == 0);
        } else if(rand()%6 < 4) {
            CCore::getMap()->addFire(-CCore::getMap()->getXPos() + screen_width + 128, screen_height - 16.0f - rand()%16*32, screen_height - 16 - rand()%16*32);
        } else if(rand()%6 < 4) {
            CCore::getMap()->addBulletBill((int)(-CCore::getMap()->getXPos() + screen_width + 128), screen_height - 16 - rand()%16*32, true, 1);
        } else {
            CCore::getMap()->addFireBall(-(int)CCore::getMap()->getXPos() + rand() % (screen_width + 128) + 8, screen_height - 16 - rand()%16 * 32, rand()%8 + 4 + 8, rand()%360, rand()%2 == 0);
        }
        //iNumOfUnits += 2;
        iTime = SDL_GetTicks();
    }
    if(moveDirection && screen_width - CCore::getMap()->getXPos() >= (CCore::getMap()->getMapWidth() - 20) * 32) {
        moveDirection = !moveDirection;
    } else if(!moveDirection && -CCore::getMap()->getXPos() <= 0) {
        moveDirection = !moveDirection;
    }
    CCore::getMap()->setXPos(CCore::getMap()->getXPos() + 4 * (moveDirection ? -1 : 1));
    //CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + 4 * (moveDirection ? -1 : 1));
}
void AboutMenu::Draw(SDL_Renderer* rR)
{
    CCFG::getText()->DrawWS(rR, "MARIO V 1.03 - C++ AND SDL2", 150, 128, 0, 0, 0);
    CCFG::getText()->DrawWS(rR, "AUTOR: LUKASZ JAKOWSKI", 150, 146, 0, 0, 0);
    CCFG::getText()->DrawWS(rR, "INFORMATYKA INZ 2012-2016", 150, 188, 0, 0, 0);
    CCFG::getText()->DrawWS(rR, "UNIWERSYTET SLASKI W KATOWICACH", 150, 206, 0, 0, 0);
    CCFG::getText()->DrawWS(rR, "MAJ 2014", 150, 224, 0, 0, 0);
    CCFG::getText()->DrawWS(rR, "WWW.LUKASZJAKOWSKI.PL", 150, 264, 0, 0, 0);
    //CCFG::getText()->DrawWS(rR, to_string(iNumOfUnits), 5, screen_height - 21, 0, 0, 0);
    for(unsigned int i = 0; i < lMO.size(); i++) {
        CCFG::getText()->DrawWS(rR, lMO[i]->getText(), lMO[i]->getXPos(), lMO[i]->getYPos(), 0, 0, 0);
    }
    CCFG::getMM()->getActiveOption()->Draw(rR, lMO[activeMenuOption]->getXPos() - 32, lMO[activeMenuOption]->getYPos());
}

void AboutMenu::enter() {
    CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->eMainMenu);
    CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
    reset();
    CCFG::getMusic()->StopMusic();
}

void AboutMenu::launch() {
    this->cR = 93;
    this->cG = 148;
    this->cB = 252;
}
void AboutMenu::reset() {
    CCore::getMap()->setXPos(0);
    CCore::getMap()->loadLVL();
}

// fica mudando de cor no menu
void AboutMenu::nextColor()
{
    int iN = iColorID;
    while(iN == iColorID)
    {
        iColorID = rand() % 16;
    }
    ++iColorID;

    switch (iColorID)
    {
        case 0:  nR = 73;   nG = 133; nB = 203; break;
        case 1:  nR = 197;  nG = 197; nB = 223; break;
        case 2:  nR = 27;   nG = 60;  nB = 173; break;
        case 3:  nR = 6;    nG = 21;  nB = 86;  break;
        case 4:  nR = 183;  nG = 85;  nB = 76;  break;
        case 5:  nR = 110;  nG = 58;  nB = 70;  break;
        case 6:  nR = 55;   nG = 19;  nB = 63;  break;
        case 7:  nR = 115;  nG = 53;  nB = 126; break;
        case 8:  nR = 227;  nG = 200; nB = 0;   break;
        case 9:  nR = 255;  nG = 180; nB = 2;   break;
        case 10: nR = 231;  nG = 51;  nB = 24;  break;
        case 11: nR = 255;  nG = 180; nB = 2;   break;
        case 12: nR = 4;    nG = 2;   nB = 15;  break;
        case 13: nR = 135;  nG = 178; nB = 168; break;
        case 14: nR = 64;   nG = 43;  nB = 24;  break;
        case 15: nR = rand() % 255;   nG = rand() % 255; nB = rand() % 255; break;
    }
}

int AboutMenu::getColorStep(int iOld, int iNew)
{
    return iOld + (iOld > iNew ? (iNew - iOld) * colorStepID / 30 : (iNew - iOld) * colorStepID / 30);
}

void AboutMenu::setBackgroundColor(SDL_Renderer* rR) {
    SDL_SetRenderDrawColor(rR, cR, cG, cB, 255);
}
void AboutMenu::updateTime()
{
    this->iTime = SDL_GetTicks();
}

Beetle::Beetle(int iXPos, int iYPos, bool moveDirection) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos;
    this->minionState = 0;
    this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 53 : CCore::getMap()->getLevelType() == 1 ? 55 : 57;
    this->moveDirection = moveDirection;
    this->moveSpeed = 1;
}
Beetle::~Beetle(void) {
}

void Beetle::Update() {
    if(minionState == -2) {
        Minion::minionDeathAnimation();
    } else {
        updateXPos();
    }
}
void Beetle::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, !moveDirection);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos);
    }
}

void Beetle::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect()) {
        setMinionState(-2);
    } else if(TOP) {
        if(minionState == 0) {
            minionState = 1;
            ++iBlockID;
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->startJump(1);
            CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
            points(100);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
            moveSpeed = 0;
            killOtherUnits = true;
        } else {
            if(moveSpeed > 0) {
                moveSpeed = 0;
            } else {
                if((fXPos + iHitBoxX) / 2 < (CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()) / 2) {
                    moveDirection = true;
                } else {
                    moveDirection = false;
                }
                moveSpeed = 6;
            }
            CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->startJump(1);
            points(100);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
        }
    } else {
        if(minionState == 1) {
            if(moveSpeed == 0) {
                //moveDirection = !CCore::getMap()->getPlayer()->getMoveDirection();
                moveDirection = (fXPos + iHitBoxX/2 < CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2);
                if(moveDirection) fXPos -= CCore::getMap()->getPlayer()->getMoveSpeed() + 1;
                else fXPos += CCore::getMap()->getPlayer()->getMoveSpeed() + 1;
                moveSpeed = 6;
                CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
            } else {
                CCore::getMap()->playerDeath(true, false);
            }
        } else {
            CCore::getMap()->playerDeath(true, false);
        }
    }
}
void Beetle::collisionEffect() {
    if(minionState == 0) moveDirection = !moveDirection;
}
void Beetle::setMinionState(int minionState) {
    if(minionState != -2 || CCore::getMap()->getPlayer()->getStarEffect()) {
        Minion::setMinionState(minionState);
    } else {
        moveDirection = !moveDirection;
    }
}

Block::Block(void) { }
Block::Block(int iBlockID, Sprite* sSprite, bool bCollision, bool bDeath, bool bUse, bool bVisible) {
    this->iBlockID = iBlockID;
    this->sSprite = sSprite;
    this->bCollision = bCollision;
    this->bDeath = bDeath;
    this->bUse = bUse;
    this->bVisible = bVisible;
}
Block::~Block(void) {
    delete sSprite;
}

void Block::Draw(SDL_Renderer* rR, int iOffsetX, int iOffsetY) {
    sSprite->getTexture()->Draw(rR, iOffsetX, iOffsetY);
}

int Block::getBlockID() {
    return iBlockID;
}
void Block::setBlockID(int iBlockID) {
    this->iBlockID = iBlockID;
}
Sprite* Block::getSprite() {
    return sSprite;
}
bool Block::getCollision() {
    return bCollision;
}
bool Block::getDeath() {
    return bDeath;
}
bool Block::getUse() {
    return bUse;
}
bool Block::getVisible() {
    return bVisible;
}


BlockDebris::BlockDebris(int iXPos, int iYPos) {
    this->debrisState = 0;
    this->bRotate = false;
    this->vPositionL = new Vector2(iXPos, iYPos);
    this->vPositionR = new Vector2(iXPos + 16, iYPos);
    this->vPositionL2 = new Vector2(iXPos, iYPos + 16);
    this->vPositionR2 = new Vector2(iXPos + 16, iYPos + 16);
    this->fSpeedX = 2.15f;
    this->fSpeedY = 1;
}
BlockDebris::~BlockDebris(void) {
    delete vPositionL;
    delete vPositionR;
    delete vPositionL2;
    delete vPositionR2;
}

void BlockDebris::Update() {
    ++iFrameID;
    if(iFrameID > 4) {
        bRotate = !bRotate;
        iFrameID = 0;
    }
    vPositionL->setX((int)(vPositionL->getX() - fSpeedX));
    vPositionL->setY((int)(vPositionL->getY() + (fSpeedY - 3.0f) + (fSpeedY < 3 ? fSpeedY < 2.5f ? -3 : -1.5f : fSpeedY < 3.5f ? 1.5f : 3)));
    vPositionR->setX((int)(vPositionR->getX() + fSpeedX));
    vPositionR->setY((int)(vPositionR->getY() + (fSpeedY - 3.0f) + (fSpeedY < 3 ? fSpeedY < 2.5f ? -3 : -1.5f : fSpeedY < 3.5f ? 1.5f : 3)));
    vPositionL2->setX((int)(vPositionL2->getX() - (fSpeedX - 1.1f)));
    vPositionL2->setY((int)(vPositionL2->getY() + (fSpeedY - 1.5f) + (fSpeedY < 1.5 ? fSpeedY < 1.3f ? -3 : -1 : fSpeedY < 1.8f ? 1 : 3)));
    vPositionR2->setX((int)(vPositionR2->getX() + (fSpeedX - 1.1f)));
    vPositionR2->setY((int)(vPositionR2->getY() + (fSpeedY - 1.5f) + (fSpeedY < 1.5 ? fSpeedY < 1.3f ? -3 : -1 : fSpeedY < 1.8f ? 1 : 3)));
    fSpeedY *= 1.09f;
    fSpeedX *= 1.005f;
    if (vPositionL->getY() >= screen_height) {
        debrisState = -1;
    }
}
void BlockDebris::Draw(SDL_Renderer* rR) {
    CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 64 : CCore::getMap()->getLevelType() == 1 ? 65 : 66)->getSprite()->getTexture()->Draw(rR, vPositionL->getX() + (int)CCore::getMap()->getXPos(), vPositionL->getY(), bRotate);
    CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 64 : CCore::getMap()->getLevelType() == 1 ? 65 : 66)->getSprite()->getTexture()->Draw(rR, vPositionR->getX() + (int)CCore::getMap()->getXPos(), vPositionR->getY(), bRotate);
    CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 64 : CCore::getMap()->getLevelType() == 1 ? 65 : 66)->getSprite()->getTexture()->Draw(rR, vPositionL2->getX() + (int)CCore::getMap()->getXPos(), vPositionL2->getY(), bRotate);
    CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 64 : CCore::getMap()->getLevelType() == 1 ? 65 : 66)->getSprite()->getTexture()->Draw(rR, vPositionR2->getX() + (int)CCore::getMap()->getXPos(), vPositionR2->getY(), bRotate);
}

int BlockDebris::getDebrisState() {
    return debrisState;
}

Bowser::Bowser(float fXPos, float fYPos, bool spawnHammer) {
    this->fXPos = fXPos;
    this->fYPos = fYPos;
    this->iHitBoxX = this->iHitBoxY = 64;
    this->iBlockID = 20;
    this->moveDirection = true;
    this->nextJumpFrameID = 128;
    this->nextFireFrameID = 1;
    this->minionSpawned = true;
    this->iMinBlockID = (int)fXPos/32 - 4;
    this->iMaxBlockID = (int)fXPos/32 + 4;
    this->collisionOnlyWithPlayer = true;
    this->moveDir = true;
    this->iYFireStart = (int)fYPos - 32;
    this->iFireID = 0;
    srand((unsigned)time(NULL));
    this->nextHammerFrameID = 128;
    this->numOfHammers = 3 + rand()%6;
    this->spawnHammer = spawnHammer;
}
Bowser::~Bowser(void) {
}

void Bowser::Update() {
    moveDirection = CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() < fXPos + iHitBoxX;
    if(!CCore::getMap()->getInEvent()) {
        if(nextJumpFrameID < 1 && jumpState == 0) {
            startJump(2);
            nextJumpFrameID = 110 + rand() % 100;
        } else if(jumpState == 0) {
            --nextJumpFrameID;
        }
        if(moveDirection && CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos())) > CCore::getMap()->getMapWidth()/2) {
            if(nextFireFrameID < 1) {
                createFire();
                nextFireFrameID = 150 + rand() % 115;
            } else if(nextFireFrameID < 30) {
                --nextFireFrameID;
                iBlockID = 21;
            } else {
                --nextFireFrameID;
                iBlockID = 20;
            }
        }
        if(!moveDirection) {
            moveDir = false;
        } else if(rand() % 275 == 0) {
            moveDir = !moveDir;
        }
        if(moveDir) {
            if(CCore::getMap()->getBlockIDX((int)fXPos) <= iMinBlockID) {
                moveDir = false;
            } else {
                fXPos -= (rand()%4 == 0 ? 0.5f : 1);
            }
        } else {
            if(CCore::getMap()->getBlockIDX((int)fXPos) >= iMaxBlockID) {
                moveDir = true;
            } else {
                fXPos += (rand()%4 == 0 ? 0.5f : 1);
            }
        }
        if(spawnHammer && moveDirection) {
            if(nextHammerFrameID <= 0) {
                if(numOfHammers > 0) {
                    nextHammerFrameID = 8;
                    --numOfHammers;
                    CCore::getMap()->addHammer((int)(fXPos + iHitBoxX/4), (int)(fYPos - 18), !moveDirection);
                } else {
                    nextHammerFrameID = 45 + rand()%55;
                    numOfHammers = 2 + rand()%9;
                }
            } else {
                --nextHammerFrameID;
            }
        }
    }
}
void Bowser::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos, !moveDirection);
    if(spawnHammer) {
        if(nextHammerFrameID <= 24 && moveDirection) {
            CCore::getMap()->getMinionBlock(61)->getSprite()->getTexture()->Draw(rR, (int)(fXPos + CCore::getMap()->getXPos() + 4), (int)(fYPos - 20), moveDirection);
        }
    }
}
void Bowser::minionPhysics() {
    if (jumpState == 1) {
        if (jumpDistance <= currentJumpDistance + 16) {
            fYPos -= 1;
            currentJumpDistance += 1;
        } else {
            fYPos -= 2;
                currentJumpDistance += 2;
        }
        if (jumpDistance <= currentJumpDistance) {
            jumpState = 2;
            currentJumpDistance = 0;
        }
    } else {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true)) {
            if(currentJumpDistance < 16) {
                currentJumpDistance += 1;
                updateYPos(1);
            } else {
                updateYPos(2);
            }
        } else {
            jumpState = 0;
        }
    }
}
void Bowser::createFire() {
    CCore::getMap()->addFire(fXPos - 40, fYPos + 16, iYFireStart + 16 * (rand()%4 + 1) + 6);
    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cFIRE);
    ++iFireID;
    if(iFireID > 2) {
        nextFireFrameID += 88 + rand() % 110;
        iFireID = 0;
    }
}

void Bowser::collisionWithPlayer(bool TOP) {
    CCore::getMap()->playerDeath(true, false);
}

Bubble::Bubble(int iXPos, int iYPos) {
    this->iXPos = iXPos;
    this->iYPos = iYPos;
    this->bDestoy = false;
    this->iBlockID = 96;
}
Bubble::~Bubble(void) {
}

void Bubble::Update() {
    if(iYPos < screen_height - 12*32 - 8) {
        bDestoy = true;
    } else {
        iYPos -= 1;
    }
}
void Bubble::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    iIMG->Draw(rR,(int)(iXPos + CCore::getMap()->getXPos()), iYPos);
}

int Bubble::getBlockID() {
    return iBlockID;
}
bool Bubble::getDestroy() {
    return bDestoy;
}

BulletBill::BulletBill(int iXPos, int iYPos, bool moveDirection, int minionState) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos - 2;
    this->moveDirection = CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/32 < fXPos + iHitBoxX/32;
    this->moveSpeed = 4;
    this->iHitBoxY = 30;
    this->collisionOnlyWithPlayer = true;
    this->iBlockID = CCore::getMap()->getCurrentLevelID() == 22 ? 60 : 59;
    this->minionSpawned = true;
    this->minionState = minionState;
    this->currentJumpSpeed = (float)CCore::getMap()->getBlockIDX((int)(fXPos + 16));
    this->currentFallingSpeed = (float)CCore::getMap()->getBlockIDY((int)(fYPos));
    this->currentJumpDistance = (float)(CCore::getMap()->getCurrentLevelID() == 17 ? 73*32 : 0); // -- MIN X
    this->jumpState = CCore::getMap()->getCurrentLevelID() == 17 ? 303*32 : CCore::getMap()->getMapWidth()*32; // -- MAX X
    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBULLETBILL);
}
BulletBill::~BulletBill(void) {
}

void BulletBill::Update() {
    if(minionState != -2) {
        fXPos += moveDirection ? -moveSpeed : moveSpeed;
        if(fXPos + iHitBoxX < currentJumpDistance || fXPos > jumpState) {
            minionState = -1;
        }
    } else {
        Minion::minionDeathAnimation();
    }
}
void BulletBill::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2, moveDirection);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos);
    }
    if(minionState == 0) CCore::getMap()->getBlock(145)->getSprite()->getTexture()->Draw(rR,(int)(currentJumpSpeed*32 + CCore::getMap()->getXPos()), (int)(screen_height - 16 - currentFallingSpeed*32) + 32);
}
void BulletBill::minionPhysics() { }

void BulletBill::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect() || TOP) {
        setMinionState(-2);
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}

BulletBillSpawner::BulletBillSpawner(int iXPos, int iYPos, int minionState) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos;
    this->moveSpeed = 0;
    this->collisionOnlyWithPlayer = true;
    this->minionState = minionState;
    this->iBlockID = 0;
    this->minionSpawned = minionState != 0;
    srand((unsigned)time(NULL));
    this->nextBulletBillFrameID = 12;
}
BulletBillSpawner::~BulletBillSpawner(void) {
}

void BulletBillSpawner::Update() {
    if(nextBulletBillFrameID <= 0) {
        if(minionState == 0) {
            if(fXPos > -CCore::getMap()->getXPos() - 64 && fXPos < -CCore::getMap()->getXPos() + screen_width + 64 + iHitBoxX) {
                if(!(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 > fXPos - 96 && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 < fXPos + 96)) {
                    CCore::getMap()->addBulletBill((int)fXPos, (int)fYPos - 14, true, minionState);
                    nextBulletBillFrameID = 145 + rand()%145;
                }
            }
        } else {
            CCore::getMap()->addBulletBill((int)(-CCore::getMap()->getXPos() + screen_width + iHitBoxX * 2), (int)fYPos - rand()%9*32 - 16, true, minionState);
            nextBulletBillFrameID = 80 + rand()%96;
        }
    } else {
        if(!(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 > fXPos - 96 && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 < fXPos + 96)) {
            --nextBulletBillFrameID;
        }
    }
}
void BulletBillSpawner::minionPhysics() { }


CCFG::CCFG(void) { }
CCFG::~CCFG(void) {
    delete oText;
    delete oMM;
    delete tSMBLOGO;
}


Text* CCFG::oText = new Text();
CIMG* CCFG::tSMBLOGO = new CIMG();
MenuManager* CCFG::oMM = new MenuManager();
Music* CCFG::oMusic = new Music();
bool CCFG::keySpace = false;
int CCFG::keyIDA = 0;
int CCFG::keyIDD = 0;
int CCFG::keyIDS = 0;
int CCFG::keyIDSpace = 0;
int CCFG::keyIDShift = 0;
bool CCFG::canMoveBackward = true;

Text* CCFG::getText() {
    return oText;
}
MenuManager* CCFG::getMM() {
    return oMM;
}
Music* CCFG::getMusic() {
    return oMusic;
}
CIMG* CCFG::getSMBLOGO() {
    return tSMBLOGO;
}
string CCFG::getKeyString(int keyID) {
    if(keyID >= 97 && keyID <= 122) {
        return string(1, '0' + (keyID - 32) - 48);
    }
    if(keyID >= 48 && keyID <= 57) {
        return string(1, '0' + (keyID - 32) - 48);
    }
    switch(keyID) {
        case SDLK_ESCAPE:
            return "ESCAPE";
        case SDLK_SPACE:
            return "SPACE";
        case SDLK_LSHIFT:
            return "LSHIFT";
        case SDLK_RSHIFT:
            return "RSHIFT";
        case SDLK_UP:
            return "UP";
        case SDLK_DOWN:
            return "DOWN";
        case SDLK_RIGHT:
            return "RIGHT";
        case SDLK_LEFT:
            return "LEFT";
        case SDLK_LCTRL:
            return "LCTRL";
        case SDLK_RCTRL:
            return "RCTRL";
    }
    return "NONE";
}

Cheep::Cheep(int iXPos, int iYPos, int minionType, int moveSpeed, bool moveDirection) {

    srand((unsigned)time(NULL));
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos + rand() % 16;
    this->moveY = false;
    this->moveDirection = false;
    this->iHitBoxX = 30;
    this->iHitBoxY = 28;
    if(minionType == 1) {
        this->minionState = 1;
        this->moveSpeed = 2;
        this->iBlockID = 31;
    } else if(minionType == 0) {
        this->minionState = 0;
        this->moveSpeed = 1;
        this->iBlockID = 30;
    } else {
        this->minionState = minionType;
        this->moveSpeed = moveSpeed;
        this->iBlockID = 31;
        this->moveDirection = moveDirection;
        this->iHitBoxY = 20;
        this->jumpDistance = 11*32.0f + rand()%80;
        moveYDIR = true;
    }
    this->collisionOnlyWithPlayer = true;
}
Cheep::~Cheep(void) {
}

void Cheep::Update() {
    if(minionState == 0 || minionState == 1) {
        if(fXPos + iHitBoxX < 0) {
            minionState = -1;
        } else {
            fXPos -= moveSpeed / 2.0f;
        }
        if(moveY) {
            if(moveYDIR) {
                if(jumpDistance > 0) {
                    if(fYPos > screen_height - 12*32 + 8) {
                        fYPos -= 0.25f;
                    }
                    --jumpDistance;
                } else {
                    jumpDistance = 80 * 4.0f + (rand()%32)*4;
                    moveYDIR = !moveYDIR;
                }
            } else {
                if(jumpDistance > 0) {
                    fYPos += 0.25f;
                    --jumpDistance;
                } else {
                    jumpDistance = 80 * 4.0f + (rand()%32)*4;
                    moveYDIR = !moveYDIR;
                }
            }
        } else {
            moveY = rand()%218 == 0;
            if(moveY) {
                jumpDistance = (float)(rand()%96);
                moveYDIR = rand()%2 == 0;
            }
        }
    } else { // -- CHEEP SPAWNED
        if(fXPos + iHitBoxX < 0) {
            minionState = -1;
        } else {
            fXPos += moveDirection ? moveSpeed / 4.0f : -moveSpeed / 4.0f;
        }
        if(moveYDIR) {
            if(jumpDistance > 10*32) {
                fYPos -= 8;
                jumpDistance -= 8;
            } else if(jumpDistance > 9*32) {
                fYPos -= 7.5f;
                jumpDistance -= 7.5f;
            } else if(jumpDistance > 8*32) {
                fYPos -= 7;
                jumpDistance -= 7;
            } else if(jumpDistance > 6*32) {
                fYPos -= 6.5f;
                jumpDistance -= 6.5f;
            } else if(jumpDistance > 5*32) {
                fYPos -= 5.5f;
                jumpDistance -= 5.5f;
            } else if(jumpDistance > 4*32) {
                fYPos -= 4.5f;
                jumpDistance -= 4.5f;
            } else if(jumpDistance > 2*32) {
                fYPos -= 3.5f;
                jumpDistance -= 3.5f;
            } else if(jumpDistance > 0) {
                fYPos -= 2.5f;
                jumpDistance -= 2.5f;
            } else {
                moveYDIR = !moveYDIR;
                this->jumpDistance = 13*32;
            }
        } else {
            if(jumpDistance > 11*32) {
                fYPos += 2.5f;
                jumpDistance -= 2.5f;
            } else if(jumpDistance > 9*32) {
                fYPos += 3.5f;
                jumpDistance -= 3.5f;
            } else if(jumpDistance > 8*32) {
                fYPos += 4.5f;
                jumpDistance -= 4.5f;
            } else if(jumpDistance > 7*32) {
                fYPos += 5.5f;
                jumpDistance -= 5.5f;
            } else if(jumpDistance > 6*32) {
                fYPos += 6.5f;
                jumpDistance -= 6.5f;
            } else if(jumpDistance > 4*32) {
                fYPos += 7;
                jumpDistance -= 7;
            } else if(jumpDistance > 3*32) {
                fYPos += 7.5f;
                jumpDistance -= 7.5f;
            } else {
                fYPos += 8;
                jumpDistance -= 8;
            }
            if(fYPos > screen_height) {
                minionState = -1;
            }
        }
    }
}
void Cheep::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState == -2) {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
    } else {
        iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos, moveDirection);
    }
}

void Cheep::minionPhysics() { }
void Cheep::collisionWithPlayer(bool TOP) {
    if(minionState > 1 && TOP) {
        setMinionState(-2);
        this->minionState = -2;
        moveYDIR = false;
        this->jumpDistance = screen_height - fYPos;
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}

CheepSpawner::CheepSpawner(int iXPos, int iXEnd) {
    this->fXPos = (float)iXPos;
    this->fYPos = 0;
    this->iXEnd = iXEnd;
    this->collisionOnlyWithPlayer = true;
    this->iHitBoxX = this->iHitBoxY = 1;
    this->minionSpawned = true;
    nextCheep();
    this->iBlockID = 0;
    srand((unsigned)time(NULL));
}
CheepSpawner::~CheepSpawner(void) {
}


void CheepSpawner::Update() {
    if(iSpawnTime < SDL_GetTicks()) {
        spawnCheep();
        nextCheep();
    }
}
void CheepSpawner::minionPhysics() { }


void CheepSpawner::spawnCheep() {
    if(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() > fXPos && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() < iXEnd) {
        CCore::getMap()->addCheep((int)(-CCore::getMap()->getXPos() + 64 + rand()%(screen_width - 128)), screen_height - 4, 2, 2 + rand() % 32, !(rand()%4 == 0));
    } else {
        nextCheep();
    }
}
void CheepSpawner::nextCheep() {
    iSpawnTime = SDL_GetTicks() + 675 + rand()%1025;
}


Coin::Coin(int iXPos, int iYPos) {
    this->iXPos = iXPos;
    this->iYPos = iYPos;
    this->iSpriteID = 0;
    this->iStepID = 0;
    this->iLEFT = 80;
    this->bTOP = true;
    this->bDelete = false;
}
Coin::~Coin(void) {
}

void Coin::Update()
{
    if(iLEFT > 0)
    {
        iLEFT -= 5;
        iYPos = iYPos + (bTOP ? - 5 : 5);
        ++iStepID;
        if(iStepID > 2)
        {
            iStepID = 0;
            ++iSpriteID;
            if(iSpriteID > 3)
            {
                iSpriteID = 0;
            }
        }
    }
    else if(bTOP)
    {
        bTOP = false;
        iLEFT = 80;
    }
    else
    {
        bDelete = true;
    }
}
void Coin::Draw(SDL_Renderer* rR)
{
    CCore::getMap()->getBlock(50)->getSprite()->getTexture(iSpriteID)->Draw(rR, iXPos + (int)CCore::getMap()->getXPos(), iYPos);
}

int Coin::getXPos()
{
    return iXPos;
}
int Coin::getYPos()
{
    return iYPos;
}

bool Coin::getDelete()
{
    return bDelete;
}

Console::Console(void)
{
    rRect.x = 5;
    rRect.y = 5;
    rRect.w = 175;
    rRect.h = 105;
    this->iMax = (rRect.h - rRect.y)/12;
}
Console::~Console(void)
{
}

void Console::Draw(SDL_Renderer* rR)
{
    if(vPrint.size() > 0)
    {
        SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rR, 4, 4, 4, 128);
        SDL_RenderFillRect(rR, &rRect);
        SDL_SetRenderDrawColor(rR, 255, 255, 255, 128);
        SDL_RenderDrawRect(rR, &rRect);
        SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
        for(int i = vPrint.size() - 1, j = 0; i >= 0; i--, j++)
        {
            CCFG::getText()->Draw(rR, vPrint[i], rRect.x + rRect.w - 5 - CCFG::getText()->getTextWidth(vPrint[i], 8), rRect.y + rRect.h - 5 - 8*(j+1) - 4*j, 8);
        }
        SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE);
    }
}

void Console::print(int iPrint)
{
    print(ToString(iPrint));
}
void Console::print(string sPrint)
{
    string nPrint;
    for(unsigned int i = 0; i < sPrint.size(); i++)
    {
        if(sPrint[i] >= 97 && sPrint[i] < 123)
        {
            nPrint += sPrint[i] - 32;
        }
        else
        {
            nPrint += sPrint[i];
        }
    }
    if(vPrint.size() >= iMax)
    {
        vPrint.erase(vPrint.begin());
    }
    vPrint.push_back(nPrint);
}

Map* CCore::oMap = new Map();
bool CCore::mouseLeftPressed = false;
bool CCore::mouseRightPressed = false;
int CCore::mouseX = 0;
int CCore::mouseY = 0;
bool CCore::movePressed = false;
bool CCore::keyMenuPressed = false;
bool CCore::keyS = false;
bool CCore::keyW = false;
bool CCore::keyA = false;
bool CCore::keyD = false;
bool CCore::keyShift = false;
bool CCore::keyAPressed = false;
bool CCore::keyDPressed = false;




void CCore::mainLoop()
{
    lFPSTime = SDL_GetTicks();

    // game loop
    while(executando && evento.type != SDL_QUIT)
    {
        frameTime = SDL_GetTicks();
        SDL_PollEvent(&evento);
        SDL_RenderClear(rR);
        CCFG::getMM()->setBackgroundColor(rR);
        SDL_RenderFillRect(rR, NULL);
        Input();
        Update();
        Draw();
        /*CCFG::getText()->Draw(rR, "FPS:" + to_string(iNumOfFPS), screen_width - CCFG::getText()->getTextWidth("FPS:" + to_string(iNumOfFPS), 8) - 8, 5, 8);
        if(SDL_GetTicks() - 1000 >= lFPSTime) {
            lFPSTime = SDL_GetTicks();
            iNumOfFPS = iFPS;
            iFPS = 0;
        }
        ++iFPS;*/
        SDL_RenderPresent(rR);
        if(SDL_GetTicks() - frameTime < MIN_FRAME_TIME)
        {
            SDL_Delay(MIN_FRAME_TIME - (SDL_GetTicks () - frameTime));
        }
    }
}

void CCore::InputMenu()
{
    if(evento.type == SDL_KEYDOWN)
    {
        CCFG::getMM()->setKey(evento.key.keysym.sym);
        switch(evento.key.keysym.sym)
        {
            case SDLK_s: case SDLK_DOWN:
                if(!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(2);
                    keyMenuPressed = true;
                }
                break;
            case SDLK_w: case SDLK_UP:
                if(!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(0);
                    keyMenuPressed = true;
                }
                break;
            case SDLK_KP_ENTER: case SDLK_RETURN:
                if(!keyMenuPressed) {
                    CCFG::getMM()->enter();
                    keyMenuPressed = true;
                }
                break;
            case SDLK_ESCAPE:
                if(!keyMenuPressed) {
                    CCFG::getMM()->escape();
                    keyMenuPressed = true;
                }
                break;
            case SDLK_LEFT: case SDLK_d:
                if(!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(3);
                    keyMenuPressed = true;
                }
                break;
            case SDLK_RIGHT: case SDLK_a:
                if(!keyMenuPressed) {
                    CCFG::getMM()->keyPressed(1);
                    keyMenuPressed = true;
                }
                break;
        }
    }
    if(evento.type == SDL_KEYUP)
    {
        switch(evento.key.keysym.sym)
        {
            case SDLK_s: case SDLK_DOWN: case SDLK_w: case SDLK_UP: case SDLK_KP_ENTER: case SDLK_RETURN: case SDLK_ESCAPE: case SDLK_a: case SDLK_RIGHT: case SDLK_LEFT: case SDLK_d:
                keyMenuPressed = false;
                break;
            default:
                break;
        }
    }
}
void CCore::InputPlayer()
{
    if(evento.type == SDL_WINDOWEVENT)
    {
        switch(evento.window.event)
        {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
                CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
                CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
                CCFG::getMusic()->PauseMusic();
                break;
        }
    }
    if(evento.type == SDL_KEYUP)
    {
        if(evento.key.keysym.sym == CCFG::keyIDD)
        {
                if(firstDir)
                {
                    firstDir = false;
                }
                keyDPressed = false;
            }
            if(evento.key.keysym.sym == CCFG::keyIDS)
            {
                oMap->getPlayer()->setSquat(false);
                keyS = false;
            }
            if(evento.key.keysym.sym == CCFG::keyIDA)
            {
                if(!firstDir)
                {
                    firstDir = true;
                }
                keyAPressed = false;
            }
            if(evento.key.keysym.sym == CCFG::keyIDSpace)
            {
                CCFG::keySpace = false;
            }
            if(evento.key.keysym.sym == CCFG::keyIDShift)
            {
                if(keyShift)
                {
                    oMap->getPlayer()->resetRun();
                    keyShift = false;
                }
            }
        switch(evento.key.keysym.sym)
        {
            case SDLK_KP_ENTER: case SDLK_RETURN: case SDLK_ESCAPE:
                keyMenuPressed = false;
                break;
        }
    }
    if(evento.type == SDL_KEYDOWN)
    {
        if(evento.key.keysym.sym == CCFG::keyIDD)
        {
            keyDPressed = true;
            if(!keyAPressed)
            {
                firstDir = true;
            }
        }
        if(evento.key.keysym.sym == CCFG::keyIDS)
        {
            if(!keyS)
            {
                keyS = true;
                if(!oMap->getUnderWater() && !oMap->getPlayer()->getInLevelAnimation()) oMap->getPlayer()->setSquat(true);
            }
        }
        if(evento.key.keysym.sym == CCFG::keyIDA)
        {
            keyAPressed = true;
            if(!keyDPressed)
            {
                firstDir = false;
            }
        }
        if(evento.key.keysym.sym == CCFG::keyIDSpace)
        {
            if(!CCFG::keySpace)
            {
                oMap->getPlayer()->jump();
                CCFG::keySpace = true;
            }
        }
        if(evento.key.keysym.sym == CCFG::keyIDShift)
        {
            if(!keyShift)
            {
                oMap->getPlayer()->startRun();
                keyShift = true;
            }
        }
        switch(evento.key.keysym.sym)
        {
            case SDLK_KP_ENTER: case SDLK_RETURN:
                if(!keyMenuPressed)
                {
                    CCFG::getMM()->enter();
                    keyMenuPressed = true;
                }
            case SDLK_ESCAPE:
                if(!keyMenuPressed && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame)
                {
                    CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->ePasue);
                    CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPASUE);
                    CCFG::getMusic()->PauseMusic();
                    keyMenuPressed = true;
                }
                break;
        }
    }
    if(keyAPressed)
    {
        if(!oMap->getPlayer()->getMove() && firstDir == false && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat())
        {
            oMap->getPlayer()->startMove();
            oMap->getPlayer()->setMoveDirection(false);
        }
    else if(!keyDPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection())
        {
            oMap->getPlayer()->setChangeMoveDirection();
        }
    }
    if(keyDPressed)
    {
        if(!oMap->getPlayer()->getMove() && firstDir == true && !oMap->getPlayer()->getChangeMoveDirection() && !oMap->getPlayer()->getSquat())
        {
            oMap->getPlayer()->startMove();
            oMap->getPlayer()->setMoveDirection(true);
        }
    else if(!keyAPressed && oMap->getPlayer()->getMoveSpeed() > 0 && firstDir != oMap->getPlayer()->getMoveDirection())
        {
            oMap->getPlayer()->setChangeMoveDirection();
        }
    }
    if(oMap->getPlayer()->getMove() && !keyAPressed && !keyDPressed)
    {
        oMap->getPlayer()->resetMove();
    }
}


void CCore::resetKeys()
{
    movePressed = keyMenuPressed = keyS = keyW = keyA = keyD = CCFG::keySpace = keyShift = keyAPressed = keyDPressed = false;
}
void CCore::Update()
{
    CCFG::getMM()->Update();
}
void CCore::Draw()
{
    CCFG::getMM()->Draw(rR);
}

void CCore::resetMove()
{
    this->keyAPressed = this->keyDPressed = false;
}
Map* CCore::getMap()
{
    return oMap;
}

Event::Event(void)
{
    this->iDelay = 0;
    this->newUnderWater = false;
    this->endGame = false;
    this->iTime = 0;
    this->bState = true;
    this->stepID = 0;
}
Event::~Event(void)
{
}

void Event::Draw(SDL_Renderer* rR)
{
    for(unsigned int i = 0; i < reDrawX.size(); i++)
    {
        if(reDrawX[i] < CCore::getMap()->getMapWidth())
            CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(reDrawX[i], reDrawY[i])->getBlockID())->Draw(rR, 32 * reDrawX[i] + (int)CCore::getMap()->getXPos(), screen_height - 32 * reDrawY[i] - 16);
    }
}

void Event::Animation()
{
    switch(eventTypeID)
    {
        case eNormal:
        {
            Normal(); break;
        }
        case eEnd:
        {
            Normal(); end(); break;
        }
        case eBossEnd:
        {
            Normal(); break;
        }
        default: Normal(); break;
    }
}
void Event::Normal() {
    if(bState) {
        if(vOLDDir.size() > stepID) {
            if(vOLDLength[stepID] > 0) {
                switch(vOLDDir[stepID]) {
                    case eTOP: // TOP
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        break;
                    case eBOT:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        break;
                    case eRIGHT:
                        CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->moveAnimation();
                        CCore::getMap()->getPlayer()->setMoveDirection(true);
                        break;
                    case eRIGHTEND:
                        CCore::getMap()->setXPos((float)CCore::getMap()->getXPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->moveAnimation();
                        CCore::getMap()->getPlayer()->setMoveDirection(true);
                        break;
                    case eLEFT:
                        CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->moveAnimation();
                        CCore::getMap()->getPlayer()->setMoveDirection(false);
                        break;
                    case eBOTRIGHTEND: // BOT & RIGHT
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
                        CCore::getMap()->setXPos((float)CCore::getMap()->getXPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->moveAnimation();
                        break;
                    case eENDBOT1:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(10);
                        break;
                    case eENDBOT2:
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMoveDirection(false);
                        break;
                    case eENDPOINTS:
                        if(CCore::getMap()->getMapTime() > 0) {
                            CCore::getMap()->setMapTime(CCore::getMap()->getMapTime() - 1);
                            CCore::getMap()->getPlayer()->setScore(CCore::getMap()->getPlayer()->getScore() + 50);
                            if(CCFG::getMusic()->musicStopped) {
                                CCFG::getMusic()->PlayMusic(CCFG::getMusic()->mSCORERING);
                            }
                        } else {
                            vOLDLength[stepID] = 0;
                            CCFG::getMusic()->StopMusic();
                        }
                        CCore::getMap()->getFlag()->UpdateCastleFlag();
                        break;
                    case eDEATHNOTHING:
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(0);
                        break;
                    case eDEATHTOP: // DEATH TOP
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(0);
                        break;
                    case eDEATHBOT: // DEATH BOT
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(0);
                        break;
                    case eNOTHING: // NOTHING YAY
                        vOLDLength[stepID] -= 1;
                        break;
                    case ePLAYPIPERIGHT:
                        CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + iSpeed);
                        vOLDLength[stepID] -= 1;
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPIPE);
                        break;
                    case eLOADINGMENU:
                        vOLDLength[stepID] -= 1;
                        if(vOLDLength[stepID] < 2) {
                            CCore::getMap()->setInEvent(false);
                            inEvent = false;
                            CCore::getMap()->getPlayer()->stopMove();
                            CCFG::getMM()->getLoadingMenu()->loadingType = true;
                            CCFG::getMM()->getLoadingMenu()->updateTime();
                            CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
                        }
                        break;
                    case eGAMEOVER:
                        vOLDLength[stepID] -= 1;
                        if(vOLDLength[stepID] < 2) {
                            CCore::getMap()->setInEvent(false);
                            inEvent = false;
                            CCore::getMap()->getPlayer()->stopMove();
                            CCFG::getMM()->getLoadingMenu()->loadingType = false;
                            CCFG::getMM()->getLoadingMenu()->updateTime();
                            CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
                            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cGAMEOVER);
                        }
                        break;
                    case eBOSSEND1:
                        for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
                            if(CCore::getMap()->getMapBlock(i, 6)->getBlockID() == 82) {
                                CCore::getMap()->getMapBlock(i, 6)->setBlockID(0);
                                break;
                            }
                        }
                        //CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos()) + vOLDLength[stepID] - 1), 6)->setBlockID(0);
                        CCore::getMap()->clearPlatforms();
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBRIDGEBREAK);
                        vOLDLength[stepID] = 0;
                        CCore::getMap()->getPlayer()->setMoveDirection(false);
                        break;
                    case eBOSSEND2:
                        //CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos())) - 1, 5)->setBlockID(0);
                        //CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos())) - 1, 4)->setBlockID(0);
                        for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
                            if(CCore::getMap()->getMapBlock(i, 5)->getBlockID() == 79) {
                                CCore::getMap()->getMapBlock(i, 5)->setBlockID(0);
                                break;
                            }
                        }
                        for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
                            if(CCore::getMap()->getMapBlock(i, 4)->getBlockID() == 76) {
                                CCore::getMap()->getMapBlock(i, 4)->setBlockID(0);
                                break;
                            }
                        }
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBRIDGEBREAK);
                        vOLDLength[stepID] = 0;
                        break;
                    case eBOSSEND3:
                        for(int i = CCore::getMap()->getMapWidth() - 1; i > 0; i--) {
                            if(CCore::getMap()->getMapBlock(i, 4)->getBlockID() == 76) {
                                CCore::getMap()->getMapBlock(i, 4)->setBlockID(0);
                                break;
                            }
                        }
                        //CCore::getMap()->getMapBlock(CCore::getMap()->getBlockIDX((int)(CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - CCore::getMap()->getXPos())) - vOLDLength[stepID], 4)->setBlockID(0);
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBRIDGEBREAK);
                        CCore::getMap()->getPlayer()->setMoveDirection(true);
                        vOLDLength[stepID] = 0;
                        break;
                    case eBOSSEND4:
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBOWSERFALL);
                        vOLDLength[stepID] = 0;
                        break;
                    case eBOTRIGHTBOSS: // BOT & RIGHT
                        CCore::getMap()->getPlayer()->moveAnimation();
                        CCore::getMap()->getPlayer()->playerPhysics();
                        CCore::getMap()->setXPos((float)CCore::getMap()->getXPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        break;
                    case eBOSSTEXT1:
                        CCore::getMap()->addText(vOLDLength[stepID], screen_height - 16 - 9*32, "THANK YOU MARIOz");
                        vOLDLength[stepID] = 0;
                        break;
                    case eBOSSTEXT2:
                        CCore::getMap()->addText(vOLDLength[stepID] + 16, screen_height - 16 - 7*32, "BUT OUR PRINCESS IS IN");
                        CCore::getMap()->addText(vOLDLength[stepID] + 16, screen_height - 16 - 6*32, "ANOTHER CASTLEz");
                        vOLDLength[stepID] = 0;
                        break;
                    case eENDGAMEBOSSTEXT1:
                        CCore::getMap()->addText(vOLDLength[stepID], screen_height - 16 - 9*32, "THANK YOU MARIOz");
                        vOLDLength[stepID] = 0;
                        break;
                    case eENDGAMEBOSSTEXT2:
                        CCore::getMap()->addText(vOLDLength[stepID] + 16, screen_height - 16 - 7*32, "YOUR QUEST IS OVER.");
                        vOLDLength[stepID] = 0;
                        break;
                    case eMARIOSPRITE1:
                        CCore::getMap()->getPlayer()->setMarioSpriteID(1);
                        vOLDLength[stepID] = 0;
                        break;
                    case eVINE1:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(10);
                        break;
                    case eVINE2:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vOLDLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(11);
                        break;
                    case eVINESPAWN:
                        CCore::getMap()->addVine(vOLDLength[stepID], 0, 1, newLevelType == 0 || newLevelType == 4 ? 34 : 36);
                        vOLDLength[stepID] = 0;
                        break;
                }
            } else {
                ++stepID;
                iTime = SDL_GetTicks();
            }
        } else {
            if(!endGame) {
                if(SDL_GetTicks() >= iTime + iDelay) {
                    bState = false;
                    stepID = 0;
                    newLevel();
                    CCore::getMap()->getPlayer()->stopMove();
                    if(inEvent) {
                        CCFG::getMM()->getLoadingMenu()->updateTime();
                        CCFG::getMM()->getLoadingMenu()->loadingType = true;
                        CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
                        CCore::getMap()->startLevelAnimation();
                    }
                    CCFG::keySpace = false;
                }
            } else {
                CCore::getMap()->resetGameData();
                CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
                CCore::getMap()->setInEvent(false);
                CCore::getMap()->getPlayer()->stopMove();
                inEvent = false;
                CCFG::keySpace = false;
                endGame = false;
                stepID = 0;
            }
        }
    } else {
        if(vNEWDir.size() > stepID) {
            if(vNEWLength[stepID] > 0) {
                switch(vNEWDir[stepID]) {
                    case eTOP: // TOP
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vNEWLength[stepID] -= iSpeed;
                        break;
                    case eBOT:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() + iSpeed);
                        vNEWLength[stepID] -= iSpeed;
                        break;
                    case eRIGHT:
                        CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + iSpeed);
                        vNEWLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->moveAnimation();
                        break;
                    case eLEFT:
                        CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() - iSpeed);
                        vNEWLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->moveAnimation();
                        break;
                    case ePLAYPIPETOP:
                        vNEWLength[stepID] -= 1;
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPIPE);
                        break;
                    case eNOTHING: // NOTHING YAY
                        vNEWLength[stepID] -= 1;
                        break;
                    case eVINE1:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vNEWLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(10);
                        break;
                    case eVINE2:
                        CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - iSpeed);
                        vNEWLength[stepID] -= iSpeed;
                        CCore::getMap()->getPlayer()->setMarioSpriteID(11);
                        break;
                }
            } else {
                ++stepID;
            }
        } else {
            CCore::getMap()->setInEvent(false);
            CCore::getMap()->getPlayer()->stopMove();
            CCFG::getMusic()->changeMusic(true, true);
            inEvent = false;
            CCFG::keySpace = false;
            CCore::resetKeys();
        }
    }
}
void Event::end()
{
    if(CCore::getMap()->getFlag() != NULL && CCore::getMap()->getFlag()->iYPos < screen_height - 16 - 3*32 - 4)
    {
        CCore::getMap()->getFlag()->Update();
    }
}
void Event::newLevel() {
    CCore::getMap()->setXPos((float)newMapXPos);
    CCore::getMap()->getPlayer()->setXPos((float)newPlayerXPos);
    CCore::getMap()->getPlayer()->setYPos((float)newPlayerYPos);
    CCore::getMap()->setMoveMap(newMoveMap);
    if(CCore::getMap()->getCurrentLevelID() != newCurrentLevel) {
        CCFG::getMM()->getLoadingMenu()->updateTime();
        CCFG::getMM()->getLoadingMenu()->loadingType = true;
        CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
        CCore::getMap()->getPlayer()->setCoins(0);
    }
    CCore::getMap()->setCurrentLevelID(newCurrentLevel);
    CCore::getMap()->setLevelType(newLevelType);
    if(newUnderWater) {
        CCore::getMap()->getPlayer()->resetRun();
    }
    CCore::getMap()->setUnderWater(newUnderWater);
    CCore::getMap()->lockMinions();
}

void Event::resetData() {
    vNEWDir.clear();
    vNEWLength.clear();
    vOLDDir.clear();
    vOLDLength.clear();
    resetRedraw();
    this->eventTypeID = eNormal;
    this->bState = true;
    this->stepID = 0;
    this->inEvent = false;
    this->endGame = false;
    this->newUnderWater = false;
}
void Event::resetRedraw() {
    reDrawX.clear();
    reDrawY.clear();
}

Fire::Fire(float fXPos, float fYPos, int toYPos) {
    this->fXPos = fXPos;
    this->fYPos = fYPos;
    this->toYPos = toYPos;
    this->collisionOnlyWithPlayer = true;
    this->moveDirection = true;
    this->iBlockID = 22;
    this->iHitBoxX = 48;
    this->iHitBoxY = 10;
    this->minionSpawned = true;
}
Fire::~Fire(void) {
}

void Fire::Update() {
    fXPos -= 2;
    if(fXPos + iHitBoxX < 0) {
        minionState = -1;
    }
    if(toYPos > fYPos) {
        ++fYPos;
    } else if(toYPos < fYPos) {
        --fYPos;
    }
}
void Fire::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    iIMG->Draw(rR, (int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos - 4);
}
void Fire::minionPhysics() {
}

void Fire::collisionWithPlayer(bool TOP) {
    CCore::getMap()->playerDeath(true, false);
}

FireBall::FireBall(int iXPos, int iYPos, int radius, int nSliceID, bool moveDirection) {
    this->iCenterX = iXPos;
    this->iCenterY = iYPos;
    this->iHitBoxX = 16;
    this->iHitBoxY = 16;
    this->radius = radius;
    this->moveDirection = moveDirection;
    this->collisionOnlyWithPlayer = true;
    this->minionSpawned = true;
    this->slice = 2 * M_PI / 360;
    this->sliceID = nSliceID;
    this->angle = slice * sliceID;
    this->fXPos = (float)(iXPos + radius*cos(angle));
    this->fYPos = (float)(iYPos + radius*sin(angle));
    this->iBlockID = 23;
}
FireBall::~FireBall(void) {
}

void FireBall::Update() {
    if(moveDirection) {
        this->angle = slice * sliceID;
        this->fXPos = (float)(iCenterX + radius*cos(angle));
        this->fYPos = (float)(iCenterY + radius*sin(angle));
        if(sliceID <= 0) {
            sliceID = 360;
        } else {
            sliceID -= 2;
        }
    } else {
        this->angle = slice * sliceID;
        this->fXPos = (float)(iCenterX + radius*cos(angle));
        this->fYPos = (float)(iCenterY + radius*sin(angle));
        if(sliceID > 359) {
            sliceID = 0;
        } else {
            sliceID += 2;
        }
    }
}
void FireBall::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    iIMG->Draw(rR, (int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos);
}
void FireBall::minionPhysics() {
}
void FireBall::updateXPos() {
}

void FireBall::collisionWithPlayer(bool TOP) {
    CCore::getMap()->playerDeath(true, false);
}

Flag::Flag(int iXPos, int iYPos) {
    this->iXPos = iXPos;
    this->iYPos = iYPos;
    this->iYTextPos = screen_height - 3*32;
    this->iPoints = -1;
    this->iBlockID = CCore::getMap()->getLevelType() == 4 || CCore::getMap()->getLevelType() == 3 ? 168 : 42;
    this->castleFlagExtraXPos = this->castleFlagY = 0;
}
Flag::~Flag(void)
{
}

void Flag::Update()
{
    iYPos += 4;
    iYTextPos -= 4;
}

void Flag::UpdateCastleFlag()
{
    if(castleFlagY <= 56)
    castleFlagY += 2;
}
void Flag::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    iIMG->Draw(rR, (int)(iXPos + CCore::getMap()->getXPos()), iYPos);
    if(iPoints > 0) {
        CCFG::getText()->Draw(rR, ToString(iPoints), (int)(iXPos + CCore::getMap()->getXPos() + 42), iYTextPos - 22, 8, 16);
    }
}
void Flag::DrawCastleFlag(SDL_Renderer* rR, CIMG* iIMG) {
    iIMG->Draw(rR, (int)(iXPos + CCore::getMap()->getXPos() + castleFlagExtraXPos + 7*32 - 14), screen_height + 14 - 6*32 - castleFlagY);
}


Flower::Flower(int iXPos, int fYPos, int iX, int iY) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)fYPos;
    this->iBlockID = 6;
    this->moveSpeed = 2;
    this->inSpawnState = true;
    this->minionSpawned = true;
    this->inSpawnY = 32;
    this->moveDirection = false;
    this->collisionOnlyWithPlayer = true;
    this->iX = iX;
    this->iY = iY;
}
Flower::~Flower(void) {
}

void Flower::Update() {
    if (inSpawnState) {
        if (inSpawnY <= 0) {
            inSpawnState = false;
        } else {
            if (fYPos > -5) {
                inSpawnY -= 2;
                fYPos -= 2;
            } else {
                inSpawnY -= 1;
                fYPos -= 1;
            }
        }
    }
}
bool Flower::updateMinion() {
    return minionSpawned;
}
void Flower::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    if(minionState >= 0)
    {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2, false);
        if (inSpawnState)
{
            CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 9 : 56)->getSprite()->getTexture()->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + (32 - inSpawnY) - CCore::getMap()->getMapBlock(iX, iY)->getYPos() + 2, false);
        }
    }
}

void Flower::collisionWithPlayer(bool TOP) {
    if(!inSpawnState && minionState >= 0) {
        CCore::getMap()->getPlayer()->setPowerLVL(CCore::getMap()->getPlayer()->getPowerLVL() + 1);
        minionState = -1;
    }
}
void Flower::setMinionState(int minionState) { }

Goombas::Goombas(int iX, int iY, int iBlockID, bool moveDirection) {
    this->fXPos = (float)iX;
    this->fYPos = (float)iY;
    this->iBlockID = iBlockID;
    this->moveDirection = moveDirection;
    this->moveSpeed = 1;
}
Goombas::~Goombas(void) {
}

void Goombas::Update() {
    if (minionState == 0) {
        updateXPos();
    } else if(minionState == -2) {
        Minion::minionDeathAnimation();
    } else if (SDL_GetTicks() - 500 >= (unsigned)deadTime) {
        minionState = -1;
    }
}
void Goombas::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2, false);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
    }
}

void Goombas::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect()) {
        setMinionState(-2);
    } else if(TOP) {
        if(minionState == 0) {
            minionState = 1;
            iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 1 : CCore::getMap()->getLevelType() == 1 ? 9 : 11;
            deadTime = SDL_GetTicks();
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->startJump(1);
            points(100);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
        }
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}

void Goombas::setMinionState(int minionState) {
    this->minionState = minionState;
    if (this->minionState == 1) {
        deadTime = SDL_GetTicks();
    }
    Minion::setMinionState(minionState);
}

Hammer::Hammer(int iXPos, int iYPos, bool moveDirection) {
    this->fXPos = (float)iXPos + 4;
    this->fYPos = (float)iYPos + 4;
    this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 47 : 48;
    this->iHitBoxX = 24;
    this->iHitBoxY = 24;
    this->moveDirection = moveDirection;
    this->jumpState = 1;
    this->jumpDistance = 48;
    this->currentJumpDistance = 0;
    this->moveSpeed = 0;
    this->minionState = 0;
    this->collisionOnlyWithPlayer = true;
    this->minionSpawned = true;
}
Hammer::~Hammer(void) {
}

void Hammer::Update() {
    if(minionState != -2) {
        ++moveSpeed;
        if(moveSpeed > 35) {
            fXPos += moveDirection ? 3 : -3;
        } else if(moveSpeed > 15) {
            fXPos += moveDirection ? 2.5f : -2.5f;
        } else {
            fXPos += moveDirection ? 2 : -2;
        }
    } else {
        Minion::minionDeathAnimation();
    }
}
void Hammer::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - 4, (int)fYPos - 4, false);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - 4, (int)fYPos - 4);
    }
}
void Hammer::minionPhysics() {
    if (jumpState == 1) {
        Minion::physicsState1();
    } else {
        currentFallingSpeed *= 1.06f;
        if (currentFallingSpeed > startJumpSpeed) {
            currentFallingSpeed = startJumpSpeed;
        }
        fYPos += currentFallingSpeed;
        if (fYPos >= screen_height) {
            minionState = -1;
        }
    }
}

void Hammer::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect()) {
        setMinionState(-2);
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}

HammerBro::HammerBro(int iXPos, int iYPos) {

    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos + 18;
    this->moveDIR = true;
    this->moveDirection = true;
    this->moveSpeed = 1;
    this->minionState = 0;
    this->iHitBoxX = 32;
    this->iHitBoxY = 48;
    this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 43 : 45;
    this->moveDistance = 16.0f;
    this->newY = true;
    srand((unsigned)time(NULL));
    this->nextJumpFrameID = rand()%175 + 125;
    this->currentJumpDistance = 0;
    this->jumpDistance = 0;
    this->hammerID = 0;
    this->nextHammerFrameID = 65;
}
HammerBro::~HammerBro(void) {
}

void HammerBro::Update() {
    if(minionState != -2) {
        moveDirection = !(CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX() / 2 > fXPos + iHitBoxX/2);
        if(moveDistance > 0) {
            moveDistance -= 0.5f;
            if(jumpState == 1) {
                fXPos += moveDIR ? -0.5f : 0.5f;
            }
            if(moveDIR) {
                if (!CCore::getMap()->checkCollisionLB((int)fXPos - moveSpeed, (int)fYPos - 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionLT((int)fXPos - moveSpeed, (int)fYPos + 2, true)) {
                    fXPos += -0.5f;
                }
            } else {
                if (!CCore::getMap()->checkCollisionRB((int)fXPos + moveSpeed, (int)fYPos - 2, iHitBoxX, iHitBoxY, true) && !CCore::getMap()->checkCollisionRT((int)fXPos + moveSpeed, (int)fYPos + 2, iHitBoxX, true)) {
                    fXPos += 0.5f;
                }
            }
        } else {
            moveDistance = 1*32 + 16;
            moveDIR = !moveDIR;
        }
        if(currentJumpDistance > 0) {
            if(newY) {
                fYPos -= 4;
                currentJumpDistance -= 4;
            } else {
                if(jumpDistance > 0) {
                    fYPos -= 4;
                    jumpDistance -= 4;
                } else {
                    fYPos += 4;
                    currentJumpDistance -= 4;
                }
            }
        } else if(nextJumpFrameID < 0) {
            currentJumpDistance = 4*32 + 16;
            jumpDistance = 16;
            nextJumpFrameID = rand()%295 + 215;
            jumpState = 1;
            if(fYPos + iHitBoxY < screen_height - 16 - 4*32) {
                newY = rand()%3 == 0;
            } else {
                newY = true;
            }
        } else {
            --nextJumpFrameID;
            jumpState = 2;
        }
        if(nextHammerFrameID < 15) {
            this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 44 : 46;
        } else {
            this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 43 : 45;
        }
        if(nextHammerFrameID < 0) {
            nextHammerFrameID = rand()%95 + 95;
            CCore::getMap()->addHammer((int)(fXPos + iHitBoxX/2), (int)(fYPos - 18), !moveDirection);
            if(hammerID < 2 - rand()%2) {
                ++hammerID;
                nextHammerFrameID /= 4;
            } else {
                hammerID = 0;
            }
        } else {
            --nextHammerFrameID;
        }
    } else {
        Minion::minionDeathAnimation();
    }
}
void HammerBro::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos - 18, moveDirection);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos - 18);
    }
}
void HammerBro::minionPhysics() {
    if (jumpState == 1) {
    } else {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true)) {
            Minion::physicsState2();
        } else {
            jumpState = 0;
        }
    }
}

void HammerBro::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect() || TOP) {
        setMinionState(-2);
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}
void HammerBro::collisionEffect() { }



Koppa::Koppa(int iX, int iY, int minionState, bool moveDirection, int iBlockID) {

    this->fXPos = (float)iX;
    this->fYPos = (float)iY;
    this->minionState = minionState;
    this->moveDirection = moveDirection;
    this->flyDirection = moveDirection;
    this->moveSpeed = 1;
    this->iBlockID = iBlockID;
    switch(minionState) {
        case 0:
            this->iHitBoxX = 32;
            this->iHitBoxY = 32;
            break;
        case 1:
            this->iHitBoxX = 32;
            this->iHitBoxY = 32;
            break;
        case 2:
            this->iHitBoxX = 32;
            this->iHitBoxY = 28;
            this->moveSpeed = 0;
            this->killOtherUnits = true;
            break;
        case 3:
            this->iHitBoxX = 32;
            this->iHitBoxY = 32;
            this->moveDirection = true;
            break;
    }
    this->iDistance = 7*32;
}
Koppa::~Koppa(void) {
}

void Koppa::minionPhysics() {
    if(minionState == 3) {
        if(flyDirection) {
            if(iDistance > 0) {
                fYPos -= 2;
                iDistance -= 2;
            } else {
                iDistance = 7*32;
                flyDirection = !flyDirection;
            }
        } else {
            if(iDistance > 0) {
                fYPos += 2;
                iDistance -= 2;
            } else {
                iDistance = 7*32;
                flyDirection = !flyDirection;
            }
        }
    }
    else if (jumpState == 1) {
        if(minionState == 0) {
            updateYPos(-2);
            currentJumpDistance += 2;
            if (jumpDistance <= currentJumpDistance) {
                jumpState = 2;
            }
        } else {
            Minion::physicsState1();
        }
    } else {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true) && !onAnotherMinion) {
            if(minionState == 0) {
                updateYPos(2);
                jumpState = 2;
                if (fYPos >= screen_height) {
                    minionState = -1;
                }
            } else {
                Minion::physicsState2();
            }
        } else {
            jumpState = 0;
            onAnotherMinion = false;
        }
    }
}
void Koppa::Update() {
    if (minionState == 0) {
        updateXPos();
        if(jumpState == 0) {
            startJump(1);
        }
    } else if(minionState == 1) {
        updateXPos();
    } else if(minionState == 2) {
        updateXPos();
    } else if(minionState == -2) {
        Minion::minionDeathAnimation();
    } else if (minionState != 3 && SDL_GetTicks() - 500 >= (unsigned)deadTime) {
        minionState = -1;
    }
}
void Koppa::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + (minionState <= 1 ? -14 : 2), !moveDirection);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
    }
}
void Koppa::updateXPos() {
    // ----- LEFT
    if (moveDirection) {
        if (CCore::getMap()->checkCollisionLB((int)fXPos - moveSpeed, (int)fYPos - 2, iHitBoxY, true) || CCore::getMap()->checkCollisionLT((int)fXPos - moveSpeed, (int)fYPos + 2, true)) {
            moveDirection = !moveDirection;
        } else {
            bool LB = CCore::getMap()->checkCollisionLB((int)fXPos + iHitBoxX/2, (int)fYPos, iHitBoxY + 2, false), RB = CCore::getMap()->checkCollisionRB((int)fXPos + iHitBoxX/2, (int)fYPos, iHitBoxX, iHitBoxY + 2, false);
            if((minionState == 1 && iBlockID != 4) && ((!LB && RB))) {
                moveDirection = !moveDirection;
                fXPos += moveSpeed;
            } else {
                fXPos -= moveSpeed;
            }
        }
    }
    // ----- RIGHT
    else {
        if (CCore::getMap()->checkCollisionRB((int)fXPos + moveSpeed, (int)fYPos - 2, iHitBoxX, iHitBoxY, true) || CCore::getMap()->checkCollisionRT((int)fXPos + moveSpeed, (int)fYPos + 2, iHitBoxX, true)) {
            moveDirection = !moveDirection;
        } else {
            bool LB = CCore::getMap()->checkCollisionLB((int)fXPos - iHitBoxX/2, (int)fYPos, iHitBoxY + 2, false), RB = CCore::getMap()->checkCollisionRB((int)fXPos - iHitBoxX/2, (int)fYPos, iHitBoxX, iHitBoxY + 2, false);
            if((minionState == 1 && iBlockID != 4) && ((LB && !RB))) {
                moveDirection = !moveDirection;
                fXPos -= moveSpeed;
            } else {
                fXPos += moveSpeed;
            }
        }
    }
    if(fXPos < -iHitBoxX) {
        minionState = -1;
    }
}

void Koppa::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect()) {
        setMinionState(-2);
    } else if(TOP) {
        if(minionState == 0 || minionState == 3) {
            minionState = 1;
            setMinion();
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->startJump(1);
            CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
            points(100);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
        } else if(minionState == 1) {
            minionState = 2;
            setMinion();
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->startJump(1);
            CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
            points(100);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
        } else {
            if(moveSpeed > 0) {
                moveSpeed = 0;
            } else {
                if((fXPos + iHitBoxX) / 2 < (CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()) / 2) {
                    moveDirection = true;
                } else {
                    moveDirection = false;
                }
                moveSpeed = 6;
            }
            CCore::getMap()->getPlayer()->setYPos((float)CCore::getMap()->getPlayer()->getYPos() - 4);
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->startJump(1);
            points(100);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
        }
    } else {
        if(minionState == 2) {
            if(moveSpeed == 0) {
                //moveDirection = !CCore::getMap()->getPlayer()->getMoveDirection();
                moveDirection = (fXPos + iHitBoxX/2 < CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2);
                if(moveDirection) fXPos -= CCore::getMap()->getPlayer()->getMoveSpeed() + 1;
                else fXPos += CCore::getMap()->getPlayer()->getMoveSpeed() + 1;
                moveSpeed = 6;
                CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSTOMP);
            } else {
                CCore::getMap()->playerDeath(true, false);
            }
        } else {
            CCore::getMap()->playerDeath(true, false);
        }
    }
}
void Koppa::collisionEffect() {
    if(minionState != 2 && moveSpeed != 0) {
        moveDirection = !moveDirection;
    }
}

void Koppa::setMinionState(int minionState) {
    this->minionState = minionState;
    if (this->minionState == 3) {
        deadTime = SDL_GetTicks();
    }
    Minion::setMinionState(minionState);
}

void Koppa::setMinion() {
    switch(minionState) {
        case 0: case 3:
            this->iHitBoxX = 32;
            this->iHitBoxY = 32;
            break;
        case 1:
            this->iHitBoxX = 32;
            this->iHitBoxY = 32;
            break;
        case 2:
            this->iHitBoxX = 32;
            this->iHitBoxY = 28;
            this->moveSpeed = 0;
            this->killOtherUnits = true;
            break;
    }
    switch(iBlockID) {
        case 7:
            iBlockID = 4;
            break;
        case 14:
            iBlockID = 12;
            break;
        case 17:
            iBlockID = 15;
            break;
        case 4:
            iBlockID = 5;
            break;
        case 12:
            iBlockID = 13;
            break;
        case 15:
            iBlockID = 16;
            break;
    }
}

Lakito::Lakito(int iXPos, int iYPos, int iMaxXPos) {

    this->fXPos = (float)iXPos + 16;
    this->fYPos = (float)iYPos;
    this->iMaxXPos = iMaxXPos;
    this->followPlayer = false;
    this->end = false;
    this->iBlockID = 50;
    this->collisionOnlyWithPlayer = true;
    this->iHitBoxX = 32;
    this->iHitBoxY = 26;
    this->moveDirection = true;
    this->nextSpikeyFrameID = 128;
    this->minionSpawned = true;
    srand((unsigned)time(NULL));
}
Lakito::~Lakito(void) {
}

void Lakito::Update() {
    if(minionState == -2) {
        Minion::minionDeathAnimation();
    } else {
        if(fXPos > iMaxXPos) {
            end = true;
        }
        if(end) {
            fXPos -= 1;
            if(fXPos < -iHitBoxX) {
                minionState = -1;
            }
            return;
        }
        if(fXPos < -CCore::getMap()->getXPos() - 64) {
            fXPos = -CCore::getMap()->getXPos() - 32;
        }
        if(fXPos <= CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 + 32*CCore::getMap()->getPlayer()->getMoveSpeed() && CCore::getMap()->getPlayer()->getMove() && CCore::getMap()->getPlayer()->getMoveDirection()) {
            moveSpeed = CCore::getMap()->getPlayer()->getMoveSpeed() + (fXPos < CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 + 32*CCore::getMap()->getPlayer()->getMoveSpeed());
            fXPos += moveSpeed;
            followPlayer = true;
            moveDirection = true;
        } else {
            if(followPlayer) {
                if(moveSpeed > 0) {
                    fXPos += moveSpeed;
                    --moveSpeed;
                } else {
                    followPlayer = false;
                }
            } else {
                if(!moveDirection) {
                    fXPos -= 1;
                    if(fXPos < CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 - 128) {
                        moveDirection = true;
                    }
                } else {
                    fXPos += 1;
                    if(fXPos > CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 + 128) {
                        moveDirection = false;
                    }
                }
            }
        }
        if(nextSpikeyFrameID < rand()%85) {
            iBlockID = 49;
        }
        if(nextSpikeyFrameID <= 0) {
            CCore::getMap()->addSpikey((int)fXPos, (int)(fYPos - 32));
            nextSpikeyFrameID = 135 + rand()%175;
            iBlockID = 50;
        } else {
            --nextSpikeyFrameID;
        }
    }
}
void Lakito::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos - 16, !moveDirection);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos - 16);
    }
}
void Lakito::minionPhysics() { }

void Lakito::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect() || TOP) {
        setMinionState(-2);
    } else if(!end) {
        CCore::getMap()->playerDeath(true, false);
    }
}

LevelEditor::LevelEditor(void) {
    this->rBlock.h = this->rBlock.w = 32;
    this->currentBlockID = 0;
}
LevelEditor::~LevelEditor(void) {
}

void LevelEditor::Update() {
    if(CCore::mouseRightPressed) {
        rDrag.w = CCore::mouseX - rDrag.x;
        rDrag.h = CCore::mouseY - rDrag.y;
    } else {
        rBlock.x = CCore::mouseX - (-(int)CCore::getMap()->getXPos() + CCore::mouseX)%32;
        rBlock.y = CCore::mouseY - (CCore::mouseY - 16)%32;
        rBlock.w = rBlock.h = 32;
        rDrag.x = CCore::mouseX;
        rDrag.y = CCore::mouseY;
    }
    editMap();
}
void LevelEditor::Draw(SDL_Renderer* rR) {
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rR, 255, 255, 255, 128);
    SDL_SetTextureAlphaMod(CCore::getMap()->getBlock(currentBlockID)->getSprite()->getTexture()->getIMG(), 225);
    if(CCore::mouseRightPressed) {
        drawStruct(rR);
        SDL_SetRenderDrawColor(rR, 242, 242, 242, 78);
        SDL_RenderFillRect(rR, &rDrag);
        SDL_SetRenderDrawColor(rR, 255, 255, 255, 235);
        SDL_RenderDrawRect(rR, &rDrag);
    } else {
        CCore::getMap()->getBlock(currentBlockID)->getSprite()->getTexture()->Draw(rR, rBlock.x, rBlock.y);
        SDL_RenderDrawRect(rR, &rBlock);
    }
    SDL_SetTextureAlphaMod(CCore::getMap()->getBlock(currentBlockID)->getSprite()->getTexture()->getIMG(), 255);
    SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE);
}

void LevelEditor::mouseWheel(int Y) {
    if(Y > 0) {
        if(currentBlockID > 0) {
            currentBlockID -= Y;
        }
    } else {
        if(currentBlockID < 100) {
            currentBlockID -= Y;
        }
    }
}
void LevelEditor::editMap() {
    if(CCore::mouseLeftPressed) {
        CCore::getMap()->getMapBlock((int)(((-CCore::getMap()->getXPos() + CCore::mouseX) - (int)((-CCore::getMap()->getXPos() + CCore::mouseX))%32)/32), (screen_height - CCore::mouseY + 16)/32)->setBlockID(currentBlockID);
    }
}

void LevelEditor::drawStruct(SDL_Renderer* rR) {
    int W = CCore::mouseX - CCore::mouseX%32 - rDrag.x + rDrag.x%32;
    int H = CCore::mouseY - (CCore::mouseY - 16)%32 - rDrag.y + rDrag.y%32;
    W += W > 0 ? 32 : 0;
    H += H > 0 ? 32 : -32;
    for(int i = 0, k = 0; k < (W > 0 ? W : -W); k += 32, i += W > 0 ? 32 : -32) {
        for(int j = 0, m = 0; m < (H > 0 ? H : -H); m += 32, j += H > 0 ? 32 : -32) {
            CCore::getMap()->getBlock(currentBlockID)->getSprite()->getTexture()->Draw(rR, rBlock.x + i, rBlock.y + j);
        }
    }
}

LevelText::LevelText(int iXPos, int iYPos, string sText) {
    this->iXPos = iXPos;
    this->iYPos = iYPos;
    this->sText = sText;
}
LevelText::~LevelText(void) {
}

int LevelText::getXPos() {
    return iXPos;
}
int LevelText::getYPos() {
    return iYPos;
}
void LevelText::setText(string sText) {
    this->sText = sText;
}
string LevelText::getText() {
    return sText;
}

LoadingMenu::LoadingMenu(void) {
    this->iTime = 0;
    this->loadingType = true;
}
LoadingMenu::~LoadingMenu(void) {
}

void LoadingMenu::Update() {
    if(SDL_GetTicks() >= iTime + 2500 + (loadingType ? 0 : 2750)) {
        if(loadingType) {
            if(!CCore::getMap()->getInEvent()) {
                CCore::getMap()->setSpawnPoint();
                CCore::getMap()->loadLVL();
            }
            CCFG::getMM()->setViewID(CCFG::getMM()->eGame);
            CCFG::getMusic()->changeMusic(true, true);
        } else {
            CCore::getMap()->resetGameData();
            CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
        }
    } else {
        CCFG::getMusic()->StopMusic();
    }
    CCore::getMap()->UpdateBlocks();
}
void LoadingMenu::Draw(SDL_Renderer* rR) {
    if(loadingType) {
        CCore::getMap()->DrawGameLayout(rR);
        CCFG::getText()->Draw(rR, "WORLD", 320, 144);
        CCFG::getText()->Draw(rR, CCore::getMap()->getLevelName(), 416, 144);
        CCore::getMap()->getPlayer()->getMarioSprite()->getTexture()->Draw(rR, 342, 210 - CCore::getMap()->getPlayer()->getHitBoxY()/2);
        CCFG::getText()->Draw(rR, "y", 384, 208);
        if(CCore::getMap()->getPlayer()->getNumOfLives() > 9) {
            CCore::getMap()->getBlock(180)->getSprite()->getTexture()->Draw(rR, 410, 210);
        }
        CCFG::getText()->Draw(rR, ToString(CCore::getMap()->getPlayer()->getNumOfLives()), 432, 208);
        CCFG::getText()->DrawCenterX(rR, "REMEMBER THAT YOU CAN RUN WITH " + CCFG::getKeyString(CCFG::keyIDShift), 400, 16);
    } else {
        CCore::getMap()->DrawGameLayout(rR);
        CCFG::getText()->DrawCenterX(rR, "GAME OVER", 240, 16);
    }
}

void LoadingMenu::updateTime()
{
    this->iTime = SDL_GetTicks();
}

MainMenu::MainMenu(void)
{
    this->lMO.push_back(new MenuOption("1 PLAYER GAME", 178, 276));
    this->lMO.push_back(new MenuOption("OPTIONS", 222, 308));
    this->lMO.push_back(new MenuOption("ABOUT", 237, 340));
    this->numOfMenuOptions = lMO.size();
    this->selectWorld = false;
    rSelectWorld.x = 122;
    rSelectWorld.y = 280;
    rSelectWorld.w = 306;
    rSelectWorld.h = 72;
    this->activeWorldID = this->activeSecondWorldID = 0;
}

MainMenu::~MainMenu(void)
{
}

void MainMenu::Update()
{
    CCFG::getMusic()->StopMusic();
    Menu::Update();
}
void MainMenu::Draw(SDL_Renderer* rR) {
    CCFG::getSMBLOGO()->Draw(rR, 80, 48);
    Menu::Draw(rR);
    CCFG::getText()->Draw(rR, "WWW.LUKASZJAKOWSKI.PL", 4, screen_height - 4 - 8, 8, 0, 0, 0);
    CCFG::getText()->Draw(rR, "WWW.LUKASZJAKOWSKI.PL", 5, screen_height - 5 - 8, 8, 255, 255, 255);
    if(selectWorld) {
        SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rR, 4, 4, 4, 235);
        SDL_RenderFillRect(rR, &rSelectWorld);
        SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
        rSelectWorld.x += 1;
        rSelectWorld.y += 1;
        rSelectWorld.h -= 2;
        rSelectWorld.w -= 2;
        SDL_RenderDrawRect(rR, &rSelectWorld);
        rSelectWorld.x -= 1;
        rSelectWorld.y -= 1;
        rSelectWorld.h += 2;
        rSelectWorld.w += 2;
        CCFG::getText()->Draw(rR, "SELECT WORLD", rSelectWorld.x + rSelectWorld.w/2 - CCFG::getText()->getTextWidth("SELECT WORLD")/2, rSelectWorld.y + 16, 16, 255, 255, 255);
        for(int i = 0, extraX = 0; i < 8; i++) {
            if(i == activeWorldID) {
                CCFG::getText()->Draw(rR, ToString(i + 1) + "-" + ToString(activeSecondWorldID + 1), rSelectWorld.x + 16*(i + 1) + 16*i + extraX, rSelectWorld.y + 16 + 24, 16, 255, 255, 255);
                extraX = 32;
                /*for(int j = 0; j < 4; j++) {
                    if(j == activeSecondWorldID) {
                        CCFG::getText()->Draw(rR, ToString(j + 1), rSelectWorld.x + 16*(i + 1) + 16*i, rSelectWorld.y + 40 + 24*j, 16, 255, 255, 255);
                    } else {
                        CCFG::getText()->Draw(rR, ToString(j + 1), rSelectWorld.x + 16*(i + 1) + 16*i, rSelectWorld.y + 40 + 24*j, 16, 90, 90, 90);
                    }
                }*/
            } else {
                CCFG::getText()->Draw(rR, ToString(i + 1), rSelectWorld.x + 16*(i + 1) + 16*i + extraX, rSelectWorld.y + 16 + 24, 16, 90, 90, 90);
            }
        }
        SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE);
        CCore::getMap()->setBackgroundColor(rR);
    }
}

void MainMenu::enter() {
    switch(activeMenuOption) {
        case 0:
            if(!selectWorld) {
                selectWorld = true;
            } else {
                CCFG::getMM()->getLoadingMenu()->updateTime();
                CCore::getMap()->resetGameData();
                CCore::getMap()->setCurrentLevelID(activeWorldID * 4 + activeSecondWorldID);
                CCFG::getMM()->setViewID(CCFG::getMM()->eGameLoading);
                CCFG::getMM()->getLoadingMenu()->loadingType = true;
                CCore::getMap()->setSpawnPointID(0);
                selectWorld = false;
            }
            break;
        case 1:
            CCFG::getMM()->getOptions()->setEscapeToMainMenu(true);
            CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->eOptions);
            CCFG::getMM()->getOptions()->updateVolumeRect();
            CCFG::getMM()->setViewID(CCFG::getMM()->eOptions);
            break;
        case 2:
            CCFG::getMM()->getAboutMenu()->updateTime();
            CCFG::getMM()->setViewID(CCFG::getMM()->eAbout);
            CCFG::getMusic()->PlayMusic(CCFG::getMusic()->mOVERWORLD);
            break;
    }
}
void MainMenu::escape() {
    selectWorld = false;
}
void MainMenu::updateActiveButton(int iDir) {
    switch(iDir) {
        case 0: case 2:
            if(!selectWorld) {
                Menu::updateActiveButton(iDir);
            } else {
                switch(iDir) {
                    case 0:
                        if(activeSecondWorldID < 1) {
                            activeSecondWorldID = 3;
                        } else {
                            --activeSecondWorldID;
                        }
                        break;
                    case 2:
                        if(activeSecondWorldID > 2) {
                            activeSecondWorldID = 0;
                        } else {
                            ++activeSecondWorldID;
                        }
                        break;
                }
            }
            break;
        case 1:
            if(selectWorld) {
                if(activeWorldID < 7) {
                    ++activeWorldID;
                } else {
                    activeWorldID = 0;
                }
            }
            break;
        case 3:
            if(selectWorld) {
                if(activeWorldID > 0) {
                    --activeWorldID;
                } else {
                    activeWorldID = 7;
                }
            }
            break;
    }
}


Map::Map(void) {
}
Map::Map(SDL_Renderer* rR) {
    oPlayer = new Player(rR, 84, 368);
    this->currentLevelID = 0;
    this->iMapWidth = 0;
    this->iMapHeight = 0;
    this->iLevelType = 0;
    this->drawLines = false;
    this->fXPos = 0;
    this->fYPos = 0;
    this->inEvent = false;
    this->iSpawnPointID = 0;
    this->bMoveMap = true;
    this->iFrameID = 0;
    this->bTP = false;
    CCFG::getText()->setFont(rR, "font");
    oEvent = new Event();
    oFlag = NULL;
    srand((unsigned)time(NULL));
    loadGameData(rR);
    loadLVL();
}
Map::~Map(void) {
    for(vector<Block*>::iterator i = vBlock.begin(); i != vBlock.end(); i++) {
        delete (*i);
    }
    for(vector<Block*>::iterator i = vMinion.begin(); i != vMinion.end(); i++) {
        delete (*i);
    }
    delete oEvent;
    delete oFlag;
}

void Map::Update() {
    UpdateBlocks();
    if(!oPlayer->getInLevelAnimation()) {
        UpdateMinionBlokcs();
        UpdateMinions();
        if(!inEvent) {
            UpdatePlayer();
            ++iFrameID;
            if(iFrameID > 32) {
                iFrameID = 0;
                if(iMapTime > 0) {
                    --iMapTime;
                    if(iMapTime == 90) {
                        CCFG::getMusic()->StopMusic();
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cLOWTIME);
                    } else if(iMapTime == 86) {
                        CCFG::getMusic()->changeMusic(true, true);
                    }
                    if(iMapTime <= 0) {
                        playerDeath(true, true);
                    }
                }
            }
        } else {
            oEvent->Animation();
        }
        for(unsigned int i = 0; i < vPlatform.size(); i++) {
            vPlatform[i]->Update();
        }
    } else {
        oPlayer->powerUPAnimation();
    }
    for(unsigned int i = 0; i < lBlockDebris.size(); i++) {
        if(lBlockDebris[i]->getDebrisState() != -1) {
            lBlockDebris[i]->Update();
        } else {
            delete lBlockDebris[i];
            lBlockDebris.erase(lBlockDebris.begin() + i);
        }
    }
    for(unsigned int i = 0; i < lPoints.size(); i++) {
        if(!lPoints[i]->getDelete()) {
            lPoints[i]->Update();
        } else {
            delete lPoints[i];
            lPoints.erase(lPoints.begin() + i);
        }
    }
    for(unsigned int i = 0; i < lCoin.size(); i++) {
        if(!lCoin[i]->getDelete()) {
            lCoin[i]->Update();
        } else {
            lPoints.push_back(new Points(lCoin[i]->getXPos(), lCoin[i]->getYPos(), "200"));
            delete lCoin[i];
            lCoin.erase(lCoin.begin() + i);
        }
    }
}
void Map::UpdatePlayer() {
    oPlayer->Update();
    checkSpawnPoint();
}
void Map::UpdateMinions() {
    for(int i = 0; i < iMinionListSize; i++) {
        for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
            if(lMinion[i][j]->updateMinion()) {
                lMinion[i][j]->Update();
            }
        }
    }
    // ----- UPDATE MINION LIST ID
    for(int i = 0; i < iMinionListSize; i++) {
        for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
            if(lMinion[i][j]->minionSpawned) {
                if(lMinion[i][j]->minionState == -1) {
                    delete lMinion[i][j];
                    lMinion[i].erase(lMinion[i].begin() + j);
                    jSize = lMinion[i].size();
                    continue;
                }
                if(floor(lMinion[i][j]->fXPos / 160) != i) {
                    lMinion[(int)floor((int)lMinion[i][j]->fXPos / 160)].push_back(lMinion[i][j]);
                    lMinion[i].erase(lMinion[i].begin() + j);
                    jSize = lMinion[i].size();
                }
            }
        }
    }
    for(unsigned int i = 0; i < lBubble.size(); i++) {
        lBubble[i]->Update();
        if(lBubble[i]->getDestroy()) {
            delete lBubble[i];
            lBubble.erase(lBubble.begin() + i);
        }
    }
}
void Map::UpdateMinionsCollisions() {
    // ----- COLLISIONS
    for(int i = 0; i < iMinionListSize; i++) {
        for(unsigned int j = 0; j < lMinion[i].size(); j++) {
            if(!lMinion[i][j]->collisionOnlyWithPlayer /*&& lMinion[i][j]->minionSpawned*/ && lMinion[i][j]->deadTime < 0) {
                // ----- WITH MINIONS IN SAME LIST
                for(unsigned int k = j + 1; k < lMinion[i].size(); k++) {
                    if(!lMinion[i][k]->collisionOnlyWithPlayer /*&& lMinion[i][k]->minionSpawned*/ && lMinion[i][k]->deadTime < 0) {
                        if(lMinion[i][j]->getXPos() < lMinion[i][k]->getXPos()) {
                            if(lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX >= lMinion[i][k]->getXPos() && lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX <= lMinion[i][k]->getXPos() + lMinion[i][k]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) || (lMinion[i][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
                                if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i][k]->minionSpawned) {
                                    lMinion[i][k]->setMinionState(-2);
                                    lMinion[i][j]->collisionWithAnotherUnit();
                                }
                                if(lMinion[i][k]->killOtherUnits && lMinion[i][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
                                    lMinion[i][j]->setMinionState(-2);
                                    lMinion[i][k]->collisionWithAnotherUnit();
                                }
                                if(lMinion[i][j]->getYPos() - 4 <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) {
                                    lMinion[i][k]->onAnotherMinion = true;
                                } else if(lMinion[i][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
                                    lMinion[i][j]->onAnotherMinion = true;
                                } else {
                                    lMinion[i][j]->collisionEffect();
                                    lMinion[i][k]->collisionEffect();
                                }
                            }
                        } else {
                            if(lMinion[i][k]->getXPos() + lMinion[i][k]->iHitBoxX >= lMinion[i][j]->getXPos() && lMinion[i][k]->getXPos() + lMinion[i][k]->iHitBoxX <= lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) || (lMinion[i][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
                                if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i][k]->minionSpawned) {
                                    lMinion[i][k]->setMinionState(-2);
                                    lMinion[i][j]->collisionWithAnotherUnit();
                                }
                                if(lMinion[i][k]->killOtherUnits && lMinion[i][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
                                    lMinion[i][j]->setMinionState(-2);
                                    lMinion[i][k]->collisionWithAnotherUnit();
                                }
                                if(lMinion[i][j]->getYPos() - 4 <= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i][k]->getYPos() + lMinion[i][k]->iHitBoxY) {
                                    lMinion[i][k]->onAnotherMinion = true;
                                } else if(lMinion[i][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
                                    lMinion[i][j]->onAnotherMinion = true;
                                } else {
                                    lMinion[i][j]->collisionEffect();
                                    lMinion[i][k]->collisionEffect();
                                }
                            }
                        }
                    }
                }
                // ----- WITH MINIONS IN OTHER LIST
                if(i + 1 < iMinionListSize) {
                    for(unsigned int k = 0; k < lMinion[i + 1].size(); k++) {
                        if(!lMinion[i + 1][k]->collisionOnlyWithPlayer /*&& lMinion[i + 1][k]->minionSpawned*/ && lMinion[i + 1][k]->deadTime < 0) {
                            if(lMinion[i][j]->getXPos() < lMinion[i + 1][k]->getXPos()) {
                                if(lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX >= lMinion[i + 1][k]->getXPos() && lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX <= lMinion[i + 1][k]->getXPos() + lMinion[i + 1][k]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) || (lMinion[i + 1][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
                                    if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i + 1][k]->minionSpawned) {
                                        lMinion[i + 1][k]->setMinionState(-2);
                                        lMinion[i][j]->collisionWithAnotherUnit();
                                    }
                                    if(lMinion[i + 1][k]->killOtherUnits && lMinion[i + 1][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
                                        lMinion[i][j]->setMinionState(-2);
                                        lMinion[i + 1][k]->collisionWithAnotherUnit();
                                    }
                                    if(lMinion[i][j]->getYPos() - 4 <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) {
                                        lMinion[i + 1][k]->onAnotherMinion = true;
                                    } else if(lMinion[i + 1][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
                                        lMinion[i][j]->onAnotherMinion = true;
                                    } else {
                                        lMinion[i][j]->collisionEffect();
                                        lMinion[i + 1][k]->collisionEffect();
                                    }
                                }
                            } else {
                                if(lMinion[i + 1][k]->getXPos() + lMinion[i + 1][k]->iHitBoxX >= lMinion[i][j]->getXPos() && lMinion[i + 1][k]->getXPos() + lMinion[i + 1][k]->iHitBoxX < lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX && ((lMinion[i][j]->getYPos() <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) || (lMinion[i + 1][k]->getYPos() <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY))) {
                                    if(lMinion[i][j]->killOtherUnits && lMinion[i][j]->moveSpeed > 0 && lMinion[i + 1][k]->minionSpawned) {
                                        lMinion[i + 1][k]->setMinionState(-2);
                                        lMinion[i][j]->collisionWithAnotherUnit();
                                    }
                                    if(lMinion[i + 1][k]->killOtherUnits && lMinion[i + 1][k]->moveSpeed > 0 && lMinion[i][j]->minionSpawned) {
                                        lMinion[i][j]->setMinionState(-2);
                                        lMinion[i + 1][k]->collisionWithAnotherUnit();
                                    }
                                    /*
                                    if(lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY < lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) {
                                        lMinion[i][j]->onAnotherMinion = true;
                                        continue;
                                    } else {
                                        lMinion[i + 1][k]->onAnotherMinion = true;
                                        continue;
                                    }*/
                                    if(lMinion[i][j]->getYPos() - 4 <= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY && lMinion[i][j]->getYPos() + 4 >= lMinion[i + 1][k]->getYPos() + lMinion[i + 1][k]->iHitBoxY) {
                                        lMinion[i + 1][k]->onAnotherMinion = true;
                                    } else if(lMinion[i + 1][k]->getYPos() - 4 <= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY && lMinion[i + 1][k]->getYPos() + 4 >= lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY) {
                                        lMinion[i][j]->onAnotherMinion = true;
                                    } else {
                                        lMinion[i][j]->collisionEffect();
                                        lMinion[i + 1][k]->collisionEffect();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if(!inEvent && !oPlayer->getInLevelAnimation()) {
        // ----- COLLISION WITH PLAYER
        for(int i = getListID(-(int)fXPos + oPlayer->getXPos()) - (getListID(-(int)fXPos + oPlayer->getXPos()) > 0 ? 1 : 0), iSize = i + 2; i < iSize; i++) {
            for(unsigned int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
                if(lMinion[i][j]->deadTime < 0) {
                    if((oPlayer->getXPos() - fXPos >= lMinion[i][j]->getXPos() && oPlayer->getXPos() - fXPos <= lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX) || (oPlayer->getXPos() - fXPos + oPlayer->getHitBoxX() >= lMinion[i][j]->getXPos() && oPlayer->getXPos() - fXPos + oPlayer->getHitBoxX() <= lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX)) {
                        if(lMinion[i][j]->getYPos() - 2 <= oPlayer->getYPos() + oPlayer->getHitBoxY() && lMinion[i][j]->getYPos() + 16 >= oPlayer->getYPos() + oPlayer->getHitBoxY()) {
                            lMinion[i][j]->collisionWithPlayer(true);
                        } else if((lMinion[i][j]->getYPos() <= oPlayer->getYPos() + oPlayer->getHitBoxY() && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= oPlayer->getYPos() + oPlayer->getHitBoxY()) || (lMinion[i][j]->getYPos() <= oPlayer->getYPos() && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= oPlayer->getYPos())) {
                            lMinion[i][j]->collisionWithPlayer(false);
                        }
                    }
                }
            }
        }
    }
}
void Map::UpdateBlocks() {
    vBlock[2]->getSprite()->Update();
    vBlock[8]->getSprite()->Update();
    vBlock[29]->getSprite()->Update();
    vBlock[55]->getSprite()->Update();
    vBlock[57]->getSprite()->Update();
    vBlock[70]->getSprite()->Update();
    vBlock[71]->getSprite()->Update();
    vBlock[72]->getSprite()->Update();
    vBlock[73]->getSprite()->Update();
    vBlock[82]->getSprite()->Update();
}
void Map::UpdateMinionBlokcs() {
    vMinion[0]->getSprite()->Update();
    vMinion[4]->getSprite()->Update();
    vMinion[6]->getSprite()->Update();
    vMinion[7]->getSprite()->Update();
    vMinion[8]->getSprite()->Update();
    vMinion[10]->getSprite()->Update();
    vMinion[12]->getSprite()->Update();
    vMinion[14]->getSprite()->Update();
    vMinion[15]->getSprite()->Update();
    vMinion[17]->getSprite()->Update();
    vMinion[18]->getSprite()->Update();
    vMinion[19]->getSprite()->Update();
    vMinion[20]->getSprite()->Update();
    vMinion[21]->getSprite()->Update();
    vMinion[22]->getSprite()->Update();
    vMinion[23]->getSprite()->Update();
    vMinion[24]->getSprite()->Update();
    vMinion[30]->getSprite()->Update();
    vMinion[31]->getSprite()->Update();
    vMinion[43]->getSprite()->Update();
    vMinion[44]->getSprite()->Update();
    vMinion[45]->getSprite()->Update();
    vMinion[46]->getSprite()->Update();
    vMinion[47]->getSprite()->Update();
    vMinion[48]->getSprite()->Update();
    vMinion[51]->getSprite()->Update();
    vMinion[52]->getSprite()->Update();
    vMinion[53]->getSprite()->Update();
    vMinion[55]->getSprite()->Update();
    vMinion[57]->getSprite()->Update();
    vMinion[62]->getSprite()->Update();
}

void Map::Draw(SDL_Renderer* rR) {
    DrawMap(rR);
    for(unsigned int i = 0; i < vPlatform.size(); i++) {
        vPlatform[i]->Draw(rR);
    }
    DrawMinions(rR);
    for(unsigned int i = 0; i < lPoints.size(); i++) {
        lPoints[i]->Draw(rR);
    }
    for(unsigned int i = 0; i < lCoin.size(); i++) {
        lCoin[i]->Draw(rR);
    }
    for(unsigned int i = 0; i < lBlockDebris.size(); i++) {
        lBlockDebris[i]->Draw(rR);
    }
    for(unsigned int i = 0; i < vLevelText.size(); i++) {
        CCFG::getText()->Draw(rR, vLevelText[i]->getText(), vLevelText[i]->getXPos() + (int)fXPos, vLevelText[i]->getYPos());
    }
    /*
    if(drawLines) {
        aa->Draw(rR, (int)fXPos, -16);
        DrawLines(rR);
    }*/
    for(unsigned int i = 0; i < lBubble.size(); i++) {
        lBubble[i]->Draw(rR, vBlock[lBubble[i]->getBlockID()]->getSprite()->getTexture());
    }
    oPlayer->Draw(rR);
    if(inEvent) {
        oEvent->Draw(rR);
    }
    DrawGameLayout(rR);
}
void Map::DrawMap(SDL_Renderer* rR) {
    if(oFlag != NULL) {
        oFlag->DrawCastleFlag(rR, vBlock[51]->getSprite()->getTexture());
    }
    for(int i = getStartBlock(), iEnd = getEndBlock(); i < iEnd && i < iMapWidth; i++) {
        for(int j = iMapHeight - 1; j >= 0; j--) {
            if(lMap[i][j]->getBlockID() != 0) {
                vBlock[lMap[i][j]->getBlockID()]->Draw(rR, 32 * i + (int)fXPos, screen_height - 32 * j - 16 - lMap[i][j]->updateYPos());
            }
        }
    }
    if(oFlag != NULL) {
        oFlag->Draw(rR, vBlock[oFlag->iBlockID]->getSprite()->getTexture());
    }
}
void Map::DrawMinions(SDL_Renderer* rR) {
    for(int i = 0; i < iMinionListSize; i++) {
        for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
            lMinion[i][j]->Draw(rR, vMinion[lMinion[i][j]->getBloockID()]->getSprite()->getTexture());
            //CCFG::getText()->DrawWS(rR, ToString(i), lMinion[i][j]->getXPos() + (int)fXPos, lMinion[i][j]->getYPos(), 0, 0, 0, 8);
        }
    }
}
void Map::DrawGameLayout(SDL_Renderer* rR)
{
    CCFG::getText()->Draw(rR, "MARIO", 54, 16);
    if(oPlayer->getScore() < 100)
	{
        CCFG::getText()->Draw(rR, "00000" + ToString(oPlayer->getScore()), 54, 32);
    }
	else if(oPlayer->getScore() < 1000)
	{
        CCFG::getText()->Draw(rR, "000" + ToString(oPlayer->getScore()), 54, 32);
    }
	else if(oPlayer->getScore() < 10000)
	{
        CCFG::getText()->Draw(rR, "00" + ToString(oPlayer->getScore()), 54, 32);
    }
	else if(oPlayer->getScore() < 100000)
	{
        CCFG::getText()->Draw(rR, "0" + ToString(oPlayer->getScore()), 54, 32);
    }
	else
	{
        CCFG::getText()->Draw(rR, ToString(oPlayer->getScore()), 54, 32);
    }
    CCFG::getText()->Draw(rR, "WORLD", 462, 16);
    CCFG::getText()->Draw(rR, getLevelName(), 480, 32);
    if(iLevelType != 1)
	{
        vBlock[2]->Draw(rR, 268, 32);
    }
	else
	{
        vBlock[57]->Draw(rR, 268, 32);
    }
    CCFG::getText()->Draw(rR, "y", 286, 32);
    CCFG::getText()->Draw(rR, (oPlayer->getCoins() < 10 ? "0" : "") + ToString(oPlayer->getCoins()), 302, 32);
    CCFG::getText()->Draw(rR, "TIME", 672, 16);
    if(CCFG::getMM()->getViewID() == CCFG::getMM()->eGame)
	{
        if(iMapTime > 100)
		{
            CCFG::getText()->Draw(rR, ToString(iMapTime), 680, 32);
        }
		else if(iMapTime > 10)
		{
            CCFG::getText()->Draw(rR, "0" + ToString(iMapTime), 680, 32);
        }
		else
		{
            CCFG::getText()->Draw(rR, "00" + ToString(iMapTime), 680, 32);
        }
    }
}
void Map::DrawLines(SDL_Renderer* rR)
{
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND); // APLHA ON !
    SDL_SetRenderDrawColor(rR, 255, 255, 255, 128);
    for(int i = 0; i < screen_width / 32 + 1; i++)
	{
        SDL_RenderDrawLine(rR, 32 * i - (-(int)fXPos) % 32, 0, 32 * i - (-(int)fXPos) % 32, screen_height);
    }
    for(int i = 0; i < screen_height / 32 + 1; i++)
	{
        SDL_RenderDrawLine(rR, 0, 32 * i - 16 + (int)fYPos, screen_width, 32 * i - 16 + (int)fYPos);
    }
    for(int i = 0; i < screen_width / 32 + 1; i++)
	{
        for(int j = 0; j < screen_height / 32; j++)
		{
            CCFG::getText()->Draw(rR, ToString(i + (-((int)fXPos + (-(int)fXPos) % 32)) / 32), 32 * i + 16 - (-(int)fXPos) % 32 - CCFG::getText()->getTextWidth(ToString(i + (-((int)fXPos + (-(int)fXPos) % 32)) / 32), 8) / 2, screen_height - 9 - 32 * j, 8);
            CCFG::getText()->Draw(rR, ToString(j), 32 * i + 16 - (-(int)fXPos) % 32 - CCFG::getText()->getTextWidth(ToString(j), 8) / 2 + 1, screen_height - 32 * j, 8);
        }
    }
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE); // APLHA OFF !
}

void Map::moveMap(int nX, int nY)
{
    if (fXPos + nX > 0)
	{
        oPlayer->updateXPos((int)(nX - fXPos));
        fXPos = 0;
    }
    else
	{
        this->fXPos += nX;
    }
}

int Map::getStartBlock()
{
    return (int)(-fXPos - (-(int)fXPos) % 32) / 32;
}

int Map::getEndBlock()
{
    return (int)(-fXPos - (-(int)fXPos) % 32 + screen_width) / 32 + 2;
}


/* ---------------- COLLISION ---------------- */
Vector2* Map::getBlockID(int nX, int nY) {
    return new Vector2((int)(nX < 0 ? 0 : nX) / 32, (int)(nY > screen_height - 16 ? 0 : (screen_height - 16 - nY + 32) / 32));
}
int Map::getBlockIDX(int nX) {
    return (int)(nX < 0 ? 0 : nX) / 32;
}
int Map::getBlockIDY(int nY) {
    return (int)(nY > screen_height - 16 ? 0 : (screen_height - 16 - nY + 32) / 32);
}
bool Map::checkCollisionLB(int nX, int nY, int nHitBoxY, bool checkVisible) {
    return checkCollision(getBlockID(nX, nY + nHitBoxY), checkVisible);
}
bool Map::checkCollisionLT(int nX, int nY, bool checkVisible) {
    return checkCollision(getBlockID(nX, nY), checkVisible);
}
bool Map::checkCollisionLC(int nX, int nY, int nHitBoxY, bool checkVisible) {
    return checkCollision(getBlockID(nX, nY + nHitBoxY), checkVisible);
}
bool Map::checkCollisionRC(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible) {
    return checkCollision(getBlockID(nX + nHitBoxX, nY + nHitBoxY), checkVisible);
}
bool Map::checkCollisionRB(int nX, int nY, int nHitBoxX, int nHitBoxY, bool checkVisible) {
    return checkCollision(getBlockID(nX + nHitBoxX, nY + nHitBoxY), checkVisible);
}
bool Map::checkCollisionRT(int nX, int nY, int nHitBoxX, bool checkVisible) {
    return checkCollision(getBlockID(nX + nHitBoxX, nY), checkVisible);
}
int Map::checkCollisionWithPlatform(int nX, int nY, int iHitBoxX, int iHitBoxY) {
    for(unsigned int i = 0; i < vPlatform.size(); i++) {
        if(-fXPos + nX + iHitBoxX >= vPlatform[i]->getXPos() && - fXPos + nX <= vPlatform[i]->getXPos() + vPlatform[i]->getSize() * 16) {
            if(nY + iHitBoxY >= vPlatform[i]->getYPos() && nY + iHitBoxY <= vPlatform[i]->getYPos() + 16) {
                return i;
            }
        }
    }
    return -1;
}
bool Map::checkCollision(Vector2* nV, bool checkVisible) {
    bool output = vBlock[lMap[nV->getX()][nV->getY()]->getBlockID()]->getCollision() && (checkVisible ? vBlock[lMap[nV->getX()][nV->getY()]->getBlockID()]->getVisible() : true);
    delete nV;
    return output;
}
void Map::checkCollisionOnTopOfTheBlock(int nX, int nY) {
    switch(lMap[nX][nY + 1]->getBlockID()) {
        case 29: case 71: case 72: case 73:// COIN
            lMap[nX][nY + 1]->setBlockID(0);
            lCoin.push_back(new Coin(nX * 32 + 7, screen_height - nY * 32 - 48));
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
            oPlayer->setCoins(oPlayer->getCoins() + 1);
            return;
            break;
    }
    for(int i = (nX - nX%5)/5, iEnd = i + 3; i < iEnd && i < iMinionListSize; i++)
	{
        for(unsigned int j = 0; j < lMinion[i].size(); j++)
		{
            if(!lMinion[i][j]->collisionOnlyWithPlayer && lMinion[i][j]->getMinionState() >= 0 && ((lMinion[i][j]->getXPos() >= nX*32 && lMinion[i][j]->getXPos() <= nX*32 + 32) || (lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX >= nX*32 && lMinion[i][j]->getXPos() + lMinion[i][j]->iHitBoxX <= nX*32 + 32))) {
                if(lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY >= screen_height - 24 - nY*32 && lMinion[i][j]->getYPos() + lMinion[i][j]->iHitBoxY <= screen_height - nY*32 + 16) {
                    lMinion[i][j]->moveDirection = !lMinion[i][j]->moveDirection;
                    lMinion[i][j]->setMinionState(-2);
                }
            }
        }
    }
}
/* ---------------- COLLISION ---------------- */

/* ----------------- MINIONS ----------------- */
int Map::getListID(int nX) {
    return (int)(nX / 160);
}
void Map::addPoints(int X, int Y, string sText, int iW, int iH)
{
    lPoints.push_back(new Points(X, Y, sText, iW, iH));
}
void Map::addGoombas(int iX, int iY, bool moveDirection) {
    lMinion[getListID(iX)].push_back(new Goombas(iX, iY, iLevelType == 0 || iLevelType == 4 ? 0 : iLevelType == 1 ? 8 : 10, moveDirection));
}
void Map::addKoppa(int iX, int iY, int minionState, bool moveDirection) {
    int tempBlock;
    switch(minionState) {
        case 0: case 3:
            tempBlock = iLevelType == 0 || iLevelType == 4 ? 7 : iLevelType == 1 ? 14 : 17;
            break;
        case 1:
            tempBlock = iLevelType == 0 || iLevelType == 4 ? 4 : iLevelType == 1 ? 12 : 15;
            break;
        case 2:
            tempBlock = iLevelType == 0 || iLevelType == 4 ? 5 : iLevelType == 1 ? 13 : 16;
            break;
    }
    lMinion[getListID(iX)].push_back(new Koppa(iX, iY, minionState, moveDirection, tempBlock));
}
void Map::addBeetle(int X, int Y, bool moveDirection) {
    lMinion[getListID(X)].push_back(new Beetle(X, Y, moveDirection));
}
void Map::addPlant(int iX, int iY) {
    lMinion[getListID(iX)].push_back(new Plant(iX, iY, iLevelType == 0 || iLevelType == 4 ? 18 : 19));
}
void Map::addToad(int X, int Y, bool peach) {
    lMinion[getListID(X)].push_back(new Toad(X, Y, peach));
}
void Map::addSquid(int X, int Y) {
    lMinion[getListID(X)].push_back(new Squid(X, Y));
}
void Map::addHammer(int X, int Y, bool moveDirection) {
    lMinion[getListID(X)].push_back(new Hammer(X, Y, moveDirection));
}
void Map::addHammerBro(int X, int Y) {
    lMinion[getListID(X)].push_back(new HammerBro(X, Y));
}
void Map::addFireBall(int X, int Y, int iWidth, int iSliceID, bool DIR) {
    for(int i = 0; i < iWidth; i++) {
        lMinion[getListID((int)X)].push_back(new FireBall(X + 8, Y + 8, 14*i, iSliceID, DIR));
    }
}
void Map::addSpikey(int X, int Y) {
    lMinion[getListID(X)].push_back(new Spikey(X, Y));
}
void Map::addPlayerFireBall(int X, int Y, bool moveDirection) {
    lMinion[getListID(X)].push_back(new PlayerFireBall(X, Y, moveDirection));
}
void Map::addBowser(int X, int Y, bool spawnHammer) {
    lMinion[getListID(X)].push_back(new Bowser((float)X, (float)Y, spawnHammer));
}
void Map::addUpFire(int X, int iYEnd) {
    lMinion[getListID(X)].push_back(new UpFire(X, iYEnd));
}
void Map::addFire(float fX, float fY, int toYPos) {
    lMinion[getListID((int)fX)].push_back(new Fire(fX, fY, toYPos));
}
void Map::addCheep(int X, int Y, int minionType, int moveSpeed, bool moveDirection) {
    lMinion[getListID(X)].push_back(new Cheep(X, Y, minionType, moveSpeed, moveDirection));
}
void Map::addCheepSpawner(int X, int XEnd) {
    lMinion[getListID(X)].push_back(new CheepSpawner(X, XEnd));
}
void Map::addBubble(int X, int Y) {
    lBubble.push_back(new Bubble(X, Y));
}
void Map::addLakito(int X, int Y, int iMaxXPos) {
    lMinion[getListID(X)].push_back(new Lakito(X, Y, iMaxXPos));
}
void Map::addVine(int X, int Y, int minionState, int iBlockID) {
    lMinion[getListID(X)].push_back(new Vine(X, Y, minionState, iBlockID));
    if(minionState == 0) {
        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cVINE);
    }
}
void Map::addSpring(int X, int Y) {
    lMinion[getListID(X)].push_back(new Spring(X, Y));
    //lMap[X/32][(screen_height - 16 - Y)/32 - 1]->setBlockID(83);
}
void Map::addBulletBillSpawner(int X, int Y, int minionState) {
    lMinion[getListID(X*32)].push_back(new BulletBillSpawner(X*32, screen_height - Y*32, minionState));
}
void Map::addBulletBill(int X, int Y, bool moveDirection, int minionState) {
    lMinion[getListID(X)].push_back(new BulletBill(X, Y, moveDirection, minionState));
}
void Map::lockMinions() {
    for(unsigned int i = 0; i < lMinion.size(); i++) {
        for(unsigned int j = 0; j < lMinion[i].size(); j++) {
            lMinion[i][j]->lockMinion();
        }
    }
}
void Map::addText(int X, int Y, string sText) {
    vLevelText.push_back(new LevelText(X, Y, sText));
}
int Map::getNumOfMinions() {
    int iOutput = 0;
    for(int i = 0, size = lMinion.size(); i < size; i++) {
        iOutput += lMinion[i].size();
    }
    return iOutput;
}
/* ----------------- MINIONS ----------------- */

/* ---------- LOAD GAME DATA ---------- */
void Map::loadGameData(SDL_Renderer* rR) {
    vector<string> tSprite;
    vector<unsigned int> iDelay;
    // ----- 0 Transparent -----
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(0, new Sprite(rR, tSprite, iDelay, false), false, true, false, false));
    tSprite.clear();
    iDelay.clear();
    // ----- 1 -----
    tSprite.push_back("gnd_red_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(1, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 2 -----
    tSprite.push_back("coin_0");
    iDelay.push_back(300);
    tSprite.push_back("coin_2");
    iDelay.push_back(30);
    tSprite.push_back("coin_1");
    iDelay.push_back(130);
    tSprite.push_back("coin_2");
    iDelay.push_back(140);
    vBlock.push_back(new Block(2, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 3 -----
    tSprite.push_back("bush_center_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(3, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 4 -----
    tSprite.push_back("bush_center_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(4, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 5 -----
    tSprite.push_back("bush_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(5, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 6 -----
    tSprite.push_back("bush_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(6, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 7 -----
    tSprite.push_back("bush_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(7, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 8 -----
    tSprite.push_back("blockq_0");
    iDelay.push_back(300);
    tSprite.push_back("blockq_2");
    iDelay.push_back(30);
    tSprite.push_back("blockq_1");
    iDelay.push_back(130);
    tSprite.push_back("blockq_2");
    iDelay.push_back(140);
    vBlock.push_back(new Block(8, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 9 -----
    tSprite.push_back("blockq_used");
    iDelay.push_back(0);
    vBlock.push_back(new Block(9, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 10 -----
    tSprite.push_back("grass_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(10, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 11 -----
    tSprite.push_back("grass_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(11, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 12 -----
    tSprite.push_back("grass_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(12, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 13 -----
    tSprite.push_back("brickred");
    iDelay.push_back(0);
    vBlock.push_back(new Block(13, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 14 -----
    tSprite.push_back("cloud_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(14, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 15 -----
    tSprite.push_back("cloud_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(15, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 16 -----
    tSprite.push_back("cloud_center_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(16, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 17 -----
    tSprite.push_back("cloud_center_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(17, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 18 -----
    tSprite.push_back("cloud_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(18, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 19 -----
    tSprite.push_back("cloud_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(19, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 20 -----
    tSprite.push_back("pipe_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(20, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 21 -----
    tSprite.push_back("pipe_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(21, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 22 -----
    tSprite.push_back("pipe_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(22, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 23 -----
    tSprite.push_back("pipe_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(23, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 24 BlockQ2 -----
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(24, new Sprite(rR, tSprite, iDelay, false), true, false, true, false));
    tSprite.clear();
    iDelay.clear();
    // ----- 25 -----
    tSprite.push_back("gnd_red2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(25, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 26 -----
    tSprite.push_back("gnd1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(26, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 27 -----
    tSprite.push_back("gnd1_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(27, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 28 -----
    tSprite.push_back("brick1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(28, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 29 -----
    tSprite.push_back("coin_use0");
    iDelay.push_back(300);
    tSprite.push_back("coin_use2");
    iDelay.push_back(30);
    tSprite.push_back("coin_use1");
    iDelay.push_back(130);
    tSprite.push_back("coin_use2");
    iDelay.push_back(140);
    vBlock.push_back(new Block(29, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 30 -----
    tSprite.push_back("pipe1_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(30, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 31 -----
    tSprite.push_back("pipe1_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(31, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 32 -----
    tSprite.push_back("pipe1_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(32, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 33 -----
    tSprite.push_back("pipe1_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(33, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 34 -----
    tSprite.push_back("pipe1_hor_bot_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(34, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 35 -----
    tSprite.push_back("pipe1_hor_top_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(35, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 36 -----
    tSprite.push_back("pipe1_hor_top_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(36, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 37 -----
    tSprite.push_back("pipe1_hor_bot_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(37, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 38 -----
    tSprite.push_back("pipe1_hor_bot_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(38, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 39 -----
    tSprite.push_back("pipe1_hor_top_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(39, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 40 -----
    tSprite.push_back("end0_l");
    iDelay.push_back(0);
    vBlock.push_back(new Block(40, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 41 -----
    tSprite.push_back("end0_dot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(41, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 42 -----
    tSprite.push_back("end0_flag");
    iDelay.push_back(0);
    vBlock.push_back(new Block(42, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 43 -----
    tSprite.push_back("castle0_brick");
    iDelay.push_back(0);
    vBlock.push_back(new Block(43, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 44 -----
    tSprite.push_back("castle0_top0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(44, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 45 -----
    tSprite.push_back("castle0_top1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(45, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 46 -----
    tSprite.push_back("castle0_center_center_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(46, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 47 -----
    tSprite.push_back("castle0_center_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(47, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 48 -----
    tSprite.push_back("castle0_center_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(48, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 49 -----
    tSprite.push_back("castle0_center_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(49, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 50 -----
    tSprite.push_back("coin_an0");
    iDelay.push_back(0);
    tSprite.push_back("coin_an1");
    iDelay.push_back(0);
    tSprite.push_back("coin_an2");
    iDelay.push_back(0);
    tSprite.push_back("coin_an3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(50, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 51 -----
    tSprite.push_back("castle_flag");
    iDelay.push_back(0);
    vBlock.push_back(new Block(51, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 52 -----
    tSprite.push_back("firework0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(52, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 53 -----
    tSprite.push_back("firework1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(53, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 54 -----
    tSprite.push_back("firework2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(54, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 55 -----
    tSprite.push_back("blockq1_0");
    iDelay.push_back(300);
    tSprite.push_back("blockq1_2");
    iDelay.push_back(30);
    tSprite.push_back("blockq1_1");
    iDelay.push_back(130);
    tSprite.push_back("blockq1_2");
    iDelay.push_back(140);
    vBlock.push_back(new Block(55, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 56 -----
    tSprite.push_back("blockq1_used");
    iDelay.push_back(0);
    vBlock.push_back(new Block(56, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 57 -----
    tSprite.push_back("coin1_0");
    iDelay.push_back(300);
    tSprite.push_back("coin1_2");
    iDelay.push_back(30);
    tSprite.push_back("coin1_1");
    iDelay.push_back(130);
    tSprite.push_back("coin1_2");
    iDelay.push_back(140);
    vBlock.push_back(new Block(57, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 58 -----
    tSprite.push_back("pipe_hor_bot_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(58, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 59 -----
    tSprite.push_back("pipe_hor_top_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(59, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 60 -----
    tSprite.push_back("pipe_hor_top_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(60, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 61 -----
    tSprite.push_back("pipe_hor_bot_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(61, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 62 -----
    tSprite.push_back("pipe_hor_bot_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(62, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 63 -----
    tSprite.push_back("pipe_hor_top_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(63, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 64 -----
    tSprite.push_back("block_debris0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(64, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 65 -----
    tSprite.push_back("block_debris1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(65, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 66 -----
    tSprite.push_back("block_debris2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(66, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 67 -----
    tSprite.push_back("t_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(67, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 68 -----
    tSprite.push_back("t_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(68, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 69 -----
    tSprite.push_back("t_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(69, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 70 -----
    tSprite.push_back("t_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(70, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 71 -----
    tSprite.push_back("coin_use00");
    iDelay.push_back(300);
    tSprite.push_back("coin_use02");
    iDelay.push_back(30);
    tSprite.push_back("coin_use01");
    iDelay.push_back(130);
    tSprite.push_back("coin_use02");
    iDelay.push_back(140);
    vBlock.push_back(new Block(71, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 72 -----
    tSprite.push_back("coin_use20");
    iDelay.push_back(300);
    tSprite.push_back("coin_use22");
    iDelay.push_back(30);
    tSprite.push_back("coin_use21");
    iDelay.push_back(130);
    tSprite.push_back("coin_use22");
    iDelay.push_back(140);
    vBlock.push_back(new Block(72, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 73 -----
    tSprite.push_back("coin_use30");
    iDelay.push_back(300);
    tSprite.push_back("coin_use32");
    iDelay.push_back(30);
    tSprite.push_back("coin_use31");
    iDelay.push_back(130);
    tSprite.push_back("coin_use32");
    iDelay.push_back(140);
    vBlock.push_back(new Block(73, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 74 -----
    tSprite.push_back("platform");
    iDelay.push_back(0);
    vBlock.push_back(new Block(74, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 75 -----
    tSprite.push_back("gnd_4");
    iDelay.push_back(0);
    vBlock.push_back(new Block(75, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 76 -----
    tSprite.push_back("bridge_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(76, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 77 -----
    tSprite.push_back("lava_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(77, new Sprite(rR, tSprite, iDelay, false), false, true, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 78 -----
    tSprite.push_back("lava_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(78, new Sprite(rR, tSprite, iDelay, false), false, true, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 79 -----
    tSprite.push_back("bridge_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(78, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 80 -----
    tSprite.push_back("blockq2_used");
    iDelay.push_back(0);
    vBlock.push_back(new Block(80, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 81 -----
    tSprite.push_back("brick2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(81, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 82 -----
    tSprite.push_back("axe_0");
    iDelay.push_back(225);
    tSprite.push_back("axe_1");
    iDelay.push_back(225);
    tSprite.push_back("axe_2");
    iDelay.push_back(200);
    vBlock.push_back(new Block(82, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 83 ----- END BRIDGE
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(83, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 84 -----
    tSprite.push_back("tree_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(84, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 85 -----
    tSprite.push_back("tree_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(85, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 86 -----
    tSprite.push_back("tree_3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(86, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 87 -----
    tSprite.push_back("tree1_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(87, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 88 -----
    tSprite.push_back("tree1_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(88, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 89 -----
    tSprite.push_back("tree1_3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(89, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 90 -----
    tSprite.push_back("fence");
    iDelay.push_back(0);
    vBlock.push_back(new Block(90, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 91 -----
    tSprite.push_back("tree_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(91, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 92 -----
    tSprite.push_back("uw_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(92, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 93 -----
    tSprite.push_back("uw_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(93, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 94 -----
    tSprite.push_back("water_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(94, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 95 -----
    tSprite.push_back("water_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(95, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 96 -----
    tSprite.push_back("bubble");
    iDelay.push_back(0);
    vBlock.push_back(new Block(96, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 97 -----
    tSprite.push_back("pipe2_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(97, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 98 -----
    tSprite.push_back("pipe2_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(98, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 99 -----
    tSprite.push_back("pipe2_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(99, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 100 -----
    tSprite.push_back("pipe2_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(100, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 101 -----
    tSprite.push_back("pipe2_hor_bot_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(101, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 102 -----
    tSprite.push_back("pipe2_hor_top_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(102, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 103 -----
    tSprite.push_back("pipe2_hor_top_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(103, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 104 -----
    tSprite.push_back("pipe2_hor_bot_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(104, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 105 -----
    tSprite.push_back("pipe2_hor_bot_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(105, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 106 -----
    tSprite.push_back("pipe2_hor_top_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(106, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 107 -----
    tSprite.push_back("bridge2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(107, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 108 -----
    tSprite.push_back("bridge3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(108, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 109 -----
    tSprite.push_back("platform1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(109, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 110 -----
    tSprite.push_back("water_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(110, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 111 -----
    tSprite.push_back("water_3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(111, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 112 -----
    tSprite.push_back("pipe3_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(112, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 113 -----
    tSprite.push_back("pipe3_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(113, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 114 -----
    tSprite.push_back("pipe3_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(114, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 115 -----
    tSprite.push_back("pipe3_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(115, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 116 -----
    tSprite.push_back("pipe3_hor_bot_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(116, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 117 -----
    tSprite.push_back("pipe3_hor_top_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(117, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 118 -----
    tSprite.push_back("pipe3_hor_top_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(118, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 119 -----
    tSprite.push_back("pipe3_hor_bot_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(119, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 120 -----
    tSprite.push_back("pipe3_hor_bot_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(120, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 121 -----
    tSprite.push_back("pipe3_hor_top_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(121, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 122 -----
    tSprite.push_back("bridge4");
    iDelay.push_back(0);
    vBlock.push_back(new Block(122, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 123 -----
    tSprite.push_back("end1_l");
    iDelay.push_back(0);
    vBlock.push_back(new Block(123, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 124 -----
    tSprite.push_back("end1_dot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(124, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 125 -----
    tSprite.push_back("bonus");
    iDelay.push_back(0);
    vBlock.push_back(new Block(125, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 126 -----
    tSprite.push_back("platformbonus");
    iDelay.push_back(0);
    vBlock.push_back(new Block(126, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 127 ----- // -- BONUS END
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(127, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 128 ----- // -- SPAWN VINE
    tSprite.push_back("brickred");
    iDelay.push_back(0);
    vBlock.push_back(new Block(128, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 129 ----- // -- SPAWN VINE
    tSprite.push_back("brick1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(129, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 130 -----
    tSprite.push_back("vine");
    iDelay.push_back(0);
    vBlock.push_back(new Block(130, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 131 -----
    tSprite.push_back("vine1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(131, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 132 -----
    tSprite.push_back("seesaw_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(132, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 133 -----
    tSprite.push_back("seesaw_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(133, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 134 -----
    tSprite.push_back("seesaw_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(134, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 135 -----
    tSprite.push_back("seesaw_3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(135, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 136 -----
    tSprite.push_back("pipe4_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(136, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 137 -----
    tSprite.push_back("pipe4_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(137, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 138 -----
    tSprite.push_back("pipe4_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(138, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 139 -----
    tSprite.push_back("pipe4_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(139, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 140 -----
    tSprite.push_back("t_left1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(140, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 141 -----
    tSprite.push_back("t_center1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(141, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 142 -----
    tSprite.push_back("t_right1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(142, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 143 -----
    tSprite.push_back("t_bot0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(143, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 144 -----
    tSprite.push_back("t_bot1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(144, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 145 -----
    tSprite.push_back("b_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(145, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 146 -----
    tSprite.push_back("b_top1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(146, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 147 -----
    tSprite.push_back("b_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(147, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 148 -----
    tSprite.push_back("cloud_left_bot1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(148, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 149 -----
    tSprite.push_back("cloud_center_bot1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(149, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 150 -----
    tSprite.push_back("cloud_center_top1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(150, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 151 -----
    tSprite.push_back("t_left2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(151, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 152 -----
    tSprite.push_back("t_center2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(152, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 153 -----
    tSprite.push_back("t_right2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(153, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 154 -----
    tSprite.push_back("t_bot2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(154, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 155 -----
    tSprite.push_back("castle1_brick");
    iDelay.push_back(0);
    vBlock.push_back(new Block(155, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 156 -----
    tSprite.push_back("castle1_top0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(156, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 157 -----
    tSprite.push_back("castle1_top1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(157, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 158 -----
    tSprite.push_back("castle1_center_center_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(158, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 159 -----
    tSprite.push_back("castle1_center_center");
    iDelay.push_back(0);
    vBlock.push_back(new Block(159, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 160 -----
    tSprite.push_back("castle1_center_left");
    iDelay.push_back(0);
    vBlock.push_back(new Block(160, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 161 -----
    tSprite.push_back("castle1_center_right");
    iDelay.push_back(0);
    vBlock.push_back(new Block(161, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 162 -----
    tSprite.push_back("seesaw1_0");
    iDelay.push_back(0);
    vBlock.push_back(new Block(162, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 163 -----
    tSprite.push_back("seesaw1_1");
    iDelay.push_back(0);
    vBlock.push_back(new Block(163, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 164 -----
    tSprite.push_back("seesaw1_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(164, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 165 -----
    tSprite.push_back("seesaw1_3");
    iDelay.push_back(0);
    vBlock.push_back(new Block(165, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 166 -----
    tSprite.push_back("gnd2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(166, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 167 -----
    tSprite.push_back("gnd2_2");
    iDelay.push_back(0);
    vBlock.push_back(new Block(167, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 168 -----
    tSprite.push_back("end1_flag");
    iDelay.push_back(0);
    vBlock.push_back(new Block(168, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 169 ----- TP
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(169, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 170 ----- TP2
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(170, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 171 ----- TP3 - bTP
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(171, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 172 -----
    tSprite.push_back("pipe5_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(172, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 173 -----
    tSprite.push_back("pipe5_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(173, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 174 -----
    tSprite.push_back("pipe5_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(174, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 175 -----
    tSprite.push_back("pipe5_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(175, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 176 -----
    tSprite.push_back("pipe6_left_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(176, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 177 -----
    tSprite.push_back("pipe6_left_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(177, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 178 -----
    tSprite.push_back("pipe6_right_bot");
    iDelay.push_back(0);
    vBlock.push_back(new Block(178, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 179 -----
    tSprite.push_back("pipe6_right_top");
    iDelay.push_back(0);
    vBlock.push_back(new Block(179, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 180 -----
    tSprite.push_back("crown");
    iDelay.push_back(0);
    vBlock.push_back(new Block(180, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 181 -----
    tSprite.push_back("gnd_5");
    iDelay.push_back(0);
    vBlock.push_back(new Block(166, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 182 ----- ENDUSE
    tSprite.push_back("transp");
    iDelay.push_back(0);
    vBlock.push_back(new Block(182, new Sprite(rR, tSprite, iDelay, false), false, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // --------------- MINION ---------------
    // ----- 0 -----
    tSprite.push_back("goombas_0");
    iDelay.push_back(200);
    tSprite.push_back("goombas_1");
    iDelay.push_back(200);
    vMinion.push_back(new Block(0, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 1 -----
    tSprite.push_back("goombas_ded");
    iDelay.push_back(0);
    vMinion.push_back(new Block(1, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 2 -----
    tSprite.push_back("mushroom");
    iDelay.push_back(0);
    vMinion.push_back(new Block(2, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 3 -----
    tSprite.push_back("mushroom_1up");
    iDelay.push_back(0);
    vMinion.push_back(new Block(3, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 4 -----
    tSprite.push_back("koopa_0");
    iDelay.push_back(200);
    tSprite.push_back("koopa_1");
    iDelay.push_back(200);
    vMinion.push_back(new Block(4, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 5 -----
    tSprite.push_back("koopa_ded");
    iDelay.push_back(0);
    vMinion.push_back(new Block(5, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 6 -----
    tSprite.push_back("flower0");
    iDelay.push_back(50);
    tSprite.push_back("flower1");
    iDelay.push_back(50);
    tSprite.push_back("flower2");
    iDelay.push_back(50);
    tSprite.push_back("flower3");
    iDelay.push_back(50);
    vMinion.push_back(new Block(6, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 7 -----
    tSprite.push_back("koopa_2");
    iDelay.push_back(200);
    tSprite.push_back("koopa_3");
    iDelay.push_back(200);
    vMinion.push_back(new Block(7, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 8 -----
    tSprite.push_back("goombas1_0");
    iDelay.push_back(200);
    tSprite.push_back("goombas1_1");
    iDelay.push_back(200);
    vMinion.push_back(new Block(8, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 9 -----
    tSprite.push_back("goombas1_ded");
    iDelay.push_back(0);
    vMinion.push_back(new Block(9, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 10 -----
    tSprite.push_back("goombas2_0");
    iDelay.push_back(200);
    tSprite.push_back("goombas2_1");
    iDelay.push_back(200);
    vMinion.push_back(new Block(10, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 11 -----
    tSprite.push_back("goombas2_ded");
    iDelay.push_back(0);
    vMinion.push_back(new Block(11, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 12 -----
    tSprite.push_back("koopa1_0");
    iDelay.push_back(200);
    tSprite.push_back("koopa1_1");
    iDelay.push_back(200);
    vMinion.push_back(new Block(12, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 13 -----
    tSprite.push_back("koopa1_ded");
    iDelay.push_back(0);
    vMinion.push_back(new Block(13, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 14 -----
    tSprite.push_back("koopa1_2");
    iDelay.push_back(200);
    tSprite.push_back("koopa1_3");
    iDelay.push_back(200);
    vMinion.push_back(new Block(14, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 15 -----
    tSprite.push_back("koopa2_0");
    iDelay.push_back(200);
    tSprite.push_back("koopa2_1");
    iDelay.push_back(200);
    vMinion.push_back(new Block(15, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 16 -----
    tSprite.push_back("koopa2_ded");
    iDelay.push_back(0);
    vMinion.push_back(new Block(16, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 17 -----
    tSprite.push_back("koopa2_2");
    iDelay.push_back(200);
    tSprite.push_back("koopa2_3");
    iDelay.push_back(200);
    vMinion.push_back(new Block(17, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 18 -----
    tSprite.push_back("plant_0");
    iDelay.push_back(125);
    tSprite.push_back("plant_1");
    iDelay.push_back(125);
    vMinion.push_back(new Block(18, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 19 -----
    tSprite.push_back("plant1_0");
    iDelay.push_back(125);
    tSprite.push_back("plant1_1");
    iDelay.push_back(125);
    vMinion.push_back(new Block(19, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 20 -----
    tSprite.push_back("bowser0");
    iDelay.push_back(285);
    tSprite.push_back("bowser1");
    iDelay.push_back(285);
    vMinion.push_back(new Block(20, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 21 -----
    tSprite.push_back("bowser2");
    iDelay.push_back(285);
    tSprite.push_back("bowser3");
    iDelay.push_back(285);
    vMinion.push_back(new Block(21, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 22 -----
    tSprite.push_back("fire_0");
    iDelay.push_back(35);
    tSprite.push_back("fire_1");
    iDelay.push_back(35);
    vMinion.push_back(new Block(22, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 23 -----
    tSprite.push_back("fireball_0");
    iDelay.push_back(75);
    tSprite.push_back("fireball_1");
    iDelay.push_back(75);
    tSprite.push_back("fireball_2");
    iDelay.push_back(75);
    tSprite.push_back("fireball_3");
    iDelay.push_back(75);
    vMinion.push_back(new Block(23, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 24 -----
    tSprite.push_back("star_0");
    iDelay.push_back(75);
    tSprite.push_back("star_1");
    iDelay.push_back(75);
    tSprite.push_back("star_2");
    iDelay.push_back(75);
    tSprite.push_back("star_3");
    iDelay.push_back(75);
    vMinion.push_back(new Block(24, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 25 -----
    tSprite.push_back("mushroom1_1up");
    iDelay.push_back(0);
    vMinion.push_back(new Block(25, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 26 -----
    tSprite.push_back("toad");
    iDelay.push_back(0);
    vMinion.push_back(new Block(26, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 27 -----
    tSprite.push_back("peach");
    iDelay.push_back(0);
    vMinion.push_back(new Block(27, new Sprite(rR, tSprite, iDelay, false), false, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 28 -----
    tSprite.push_back("squid0");
    iDelay.push_back(0);
    vMinion.push_back(new Block(28, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 29 -----
    tSprite.push_back("squid1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(29, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 30 -----
    tSprite.push_back("cheep0");
    iDelay.push_back(120);
    tSprite.push_back("cheep1");
    iDelay.push_back(120);
    vMinion.push_back(new Block(30, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 31 -----
    tSprite.push_back("cheep2");
    iDelay.push_back(110);
    tSprite.push_back("cheep3");
    iDelay.push_back(110);
    vMinion.push_back(new Block(31, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 32 -----
    tSprite.push_back("upfire");
    iDelay.push_back(0);
    vMinion.push_back(new Block(32, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 33 -----
    tSprite.push_back("vine_top");
    iDelay.push_back(0);
    vMinion.push_back(new Block(33, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 34 -----
    tSprite.push_back("vine");
    iDelay.push_back(0);
    vMinion.push_back(new Block(34, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 35 -----
    tSprite.push_back("vine1_top");
    iDelay.push_back(0);
    vMinion.push_back(new Block(35, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 36 -----
    tSprite.push_back("vine1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(36, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 37 -----
    tSprite.push_back("spring_0");
    iDelay.push_back(0);
    vMinion.push_back(new Block(37, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 38 -----
    tSprite.push_back("spring_1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(38, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 39 -----
    tSprite.push_back("spring_2");
    iDelay.push_back(0);
    vMinion.push_back(new Block(39, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 40 -----
    tSprite.push_back("spring1_0");
    iDelay.push_back(0);
    vMinion.push_back(new Block(40, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 41 -----
    tSprite.push_back("spring1_1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(41, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 42 -----
    tSprite.push_back("spring1_2");
    iDelay.push_back(0);
    vMinion.push_back(new Block(42, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 43 -----
    tSprite.push_back("hammerbro_0");
    iDelay.push_back(175);
    tSprite.push_back("hammerbro_1");
    iDelay.push_back(175);
    vMinion.push_back(new Block(43, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 44 -----
    tSprite.push_back("hammerbro_2");
    iDelay.push_back(155);
    tSprite.push_back("hammerbro_3");
    iDelay.push_back(155);
    vMinion.push_back(new Block(44, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 45 -----
    tSprite.push_back("hammerbro1_0");
    iDelay.push_back(175);
    tSprite.push_back("hammerbro1_1");
    iDelay.push_back(175);
    vMinion.push_back(new Block(45, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 46 -----
    tSprite.push_back("hammerbro1_2");
    iDelay.push_back(155);
    tSprite.push_back("hammerbro1_3");
    iDelay.push_back(155);
    vMinion.push_back(new Block(46, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 47 -----
    tSprite.push_back("hammer_0");
    iDelay.push_back(95);
    tSprite.push_back("hammer_1");
    iDelay.push_back(95);
    tSprite.push_back("hammer_2");
    iDelay.push_back(95);
    tSprite.push_back("hammer_3");
    iDelay.push_back(95);
    vMinion.push_back(new Block(47, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 48 -----
    tSprite.push_back("hammer1_0");
    iDelay.push_back(95);
    tSprite.push_back("hammer1_1");
    iDelay.push_back(95);
    tSprite.push_back("hammer1_2");
    iDelay.push_back(95);
    tSprite.push_back("hammer1_3");
    iDelay.push_back(95);
    vMinion.push_back(new Block(48, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 49 -----
    tSprite.push_back("lakito_0");
    iDelay.push_back(0);
    vMinion.push_back(new Block(49, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 50 -----
    tSprite.push_back("lakito_1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(50, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 51 -----
    tSprite.push_back("spikey0_0");
    iDelay.push_back(135);
    tSprite.push_back("spikey0_1");
    iDelay.push_back(135);
    vMinion.push_back(new Block(51, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 52 -----
    tSprite.push_back("spikey1_0");
    iDelay.push_back(75);
    tSprite.push_back("spikey1_1");
    iDelay.push_back(75);
    vMinion.push_back(new Block(52, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 53 -----
    tSprite.push_back("beetle_0");
    iDelay.push_back(155);
    tSprite.push_back("beetle_1");
    iDelay.push_back(155);
    vMinion.push_back(new Block(53, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 54 -----
    tSprite.push_back("beetle_2");
    iDelay.push_back(0);
    vMinion.push_back(new Block(54, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 55 -----
    tSprite.push_back("beetle1_0");
    iDelay.push_back(155);
    tSprite.push_back("beetle1_1");
    iDelay.push_back(155);
    vMinion.push_back(new Block(55, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 56 -----
    tSprite.push_back("beetle1_2");
    iDelay.push_back(0);
    vMinion.push_back(new Block(56, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 57 -----
    tSprite.push_back("beetle2_0");
    iDelay.push_back(155);
    tSprite.push_back("beetle2_1");
    iDelay.push_back(155);
    vMinion.push_back(new Block(57, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 58 -----
    tSprite.push_back("beetle2_2");
    iDelay.push_back(0);
    vMinion.push_back(new Block(58, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 59 -----
    tSprite.push_back("bulletbill");
    iDelay.push_back(0);
    vMinion.push_back(new Block(59, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 60 -----
    tSprite.push_back("bulletbill1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(60, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 61 -----
    tSprite.push_back("hammer1_0");
    iDelay.push_back(0);
    vMinion.push_back(new Block(61, new Sprite(rR, tSprite, iDelay, false), true, false, true, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 62 -----
    tSprite.push_back("fireball_0");
    iDelay.push_back(155);
    tSprite.push_back("fireball_1");
    iDelay.push_back(155);
    tSprite.push_back("fireball_2");
    iDelay.push_back(155);
    tSprite.push_back("fireball_3");
    iDelay.push_back(155);
    vMinion.push_back(new Block(62, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 63 -----
    tSprite.push_back("firework0");
    iDelay.push_back(0);
    vMinion.push_back(new Block(63, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 64 -----
    tSprite.push_back("firework1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(64, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    // ----- 65 -----
    tSprite.push_back("firework1");
    iDelay.push_back(0);
    vMinion.push_back(new Block(65, new Sprite(rR, tSprite, iDelay, false), true, false, false, true));
    tSprite.clear();
    iDelay.clear();
    iBlockSize = vBlock.size();
    iMinionSize = vMinion.size();
}

void Map::clearMap() {
    for(int i = 0; i < iMapWidth; i++)
    {
        for(int j = 0; j < iMapHeight; j++)
        {
            delete lMap[i][j];
        }
        lMap[i].clear();
    }
    lMap.clear();
    this->iMapWidth = this->iMapHeight = 0;
    if(oFlag != NULL)
    {
        delete oFlag;
        oFlag = NULL;
    }
    oEvent->eventTypeID = oEvent->eNormal;
    clearLevelText();
}
void Map::clearMinions() {
    for(int i = 0; i < iMinionListSize; i++) {
        for(int j = 0, jSize = lMinion[i].size(); j < jSize; j++) {
            delete lMinion[i][j];
            jSize = lMinion[i].size();
        }
        lMinion[i].clear();
    }
    clearPlatforms();
}
void Map::clearPlatforms() {
    for(unsigned int i = 0; i < vPlatform.size(); i++) {
        delete vPlatform[i];
    }
    vPlatform.clear();
}
void Map::clearBubbles() {
    for(unsigned int i = 0; i < lBubble.size(); i++) {
        delete lBubble[i];
    }
    lBubble.clear();
}

void Map::setBackgroundColor(SDL_Renderer* rR) {
    switch(iLevelType) {
        case 0: case 2:
            SDL_SetRenderDrawColor(rR, 93, 148, 252, 255);
            break;
        case 1: case 3: case 4:
            SDL_SetRenderDrawColor(rR, 0, 0, 0, 255);
            break;
        default:
            SDL_SetRenderDrawColor(rR, 93, 148, 252, 255);
            break;
    }
}
string Map::getLevelName() {
    return "" + ToString(1 + currentLevelID/4) + "-" + ToString(currentLevelID%4 + 1);
}
void Map::loadMinionsLVL_1_1() {
    clearMinions();
    addGoombas(704, 368, true);
    addGoombas(1280, 368, true);
    addGoombas(1632, 368, true);
    addGoombas(1680, 368, true);
    addGoombas(2560, 112, true);
    addGoombas(2624, 112, true);
    addGoombas(3104, 368, true);
    addGoombas(3152, 368, true);
    addKoppa(107*32, 368, 1, true);
    addGoombas(3648, 368, true);
    addGoombas(3696, 368, true);
    addGoombas(3968, 368, true);
    addGoombas(4016, 368, true);
    addGoombas(4096, 368, true);
    addGoombas(4144, 368, true);
    addGoombas(5568, 368, true);
    addGoombas(5612, 368, true);
}
void Map::loadMinionsLVL_1_2() {
    clearMinions();
    this->iLevelType = 1;
    addGoombas(16*32, 368, true);
    addGoombas(17*32 + 8, 368 - 32, true);
    addGoombas(29*32, 368, true);
    addKoppa(44*32, 368, 1, true);
    addKoppa(45*32 + 16, 368, 1, true);
    addKoppa(59*32, 368, 1, true);
    addGoombas(62*32, 368, true);
    addGoombas(64*32, 368, true);
    addGoombas(73*32, 368 - 8*32, true);
    addGoombas(76*32, 368 - 4*32, true);
    addGoombas(77*32 + 16, 368 - 4*32, true);
    addGoombas(99*32, 368, true);
    addGoombas(100*32 + 16, 368, true);
    addGoombas(102*32, 368, true);
    addGoombas(113*32, 368, true);
    addGoombas(135*32, 368 - 3*32, true);
    addGoombas(136*32 + 16, 368 - 4*32, true);
    this->iLevelType = 3;
    addKoppa(146*32, 368, 1, false);
    this->iLevelType = 1;
    addPlant(103*32 + 16, screen_height - 10 - 4*32);
    addPlant(109*32 + 16, screen_height - 10 - 5*32);
    addPlant(115*32 + 16, screen_height - 10 - 3*32);
    this->iLevelType = 0;
    addPlant(284*32 + 16, screen_height - 10 - 3*32);
    this->iLevelType = 1;
}
void Map::loadMinionsLVL_1_3() {
    clearMinions();
    this->iLevelType = 3;
    addKoppa(30*32 - 8, screen_height - 16 - 10*32, 1, true);
    addKoppa(110*32 - 8, screen_height - 16 - 8*32, 1, true);
    addKoppa(74*32 - 8, screen_height - 16 - 10*32, 3, false);
    addKoppa(114*32 - 8, screen_height - 16 - 9*32, 3, false);
    addKoppa(133*32 - 8, screen_height - 16 - 2*32, 1, true);
    this->iLevelType = 0;
    addGoombas(44*32, screen_height - 16 - 11*32, true);
    addGoombas(46*32, screen_height - 16 - 11*32, true);
    addGoombas(80*32, screen_height - 16 - 9*32, true);
}
void Map::loadMinionsLVL_1_4() {
    clearMinions();
    addBowser(135*32, screen_height - 16 - 6*32);
    addToad(153*32, screen_height - 3*32, false);
    addFireBall(30*32, screen_height - 16 - 4*32, 6, rand()%360, true);
    addFireBall(49*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(60*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(67*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(76*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(84*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(88*32, screen_height - 16 - 10*32, 6, rand()%360, false);
}
void Map::loadMinionsLVL_2_1() {
    clearMinions();
    addSpring(188*32, 336);
    addGoombas(24*32, screen_height - 16 - 7*32, true);
    addGoombas(42*32, screen_height - 16 - 2*32, true);
    addGoombas(43*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(59*32, screen_height - 16 - 2*32, true);
    addGoombas(60*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(68*32, screen_height - 16 - 2*32, true);
    addGoombas(69*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(71*32, screen_height - 16 - 2*32, true);
    addGoombas(87*32, screen_height - 16 - 2*32, true);
    addGoombas(88*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(90*32, screen_height - 16 - 2*32, true);
    addGoombas(102*32 + 16, screen_height - 16 - 6*32, true);
    addGoombas(114*32 + 16, screen_height - 16 - 4*32, true);
    addGoombas(120*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(162*32, screen_height - 16 - 2*32, true);
    addGoombas(163*32 + 16, screen_height - 16 - 2*32, true);
    addKoppa(32*32 - 2, screen_height - 16 - 2*32, 1, true);
    addKoppa(33*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(55*32, screen_height - 16 - 6*32, 1, true);
    addKoppa(66*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(137*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(151*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(169*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(171*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(185*32, screen_height - 16 - 2*32, 1, true);
    addPlant(46*32 + 16, screen_height - 10 - 5*32);
    addPlant(74*32 + 16, screen_height - 10 - 5*32);
    addPlant(103*32 + 16, screen_height - 10 - 5*32);
    addPlant(115*32 + 16, screen_height - 10 - 3*32);
    addPlant(122*32 + 16, screen_height - 10 - 5*32);
    addPlant(130*32 + 16, screen_height - 10 - 6*32);
    addPlant(176*32 + 16, screen_height - 10 - 4*32);
}
void Map::loadMinionsLVL_2_2() {
    clearMinions();
    addSquid(22*32, screen_height - 16 - 3*32);
    addSquid(46*32, screen_height - 16 - 5*32);
    addSquid(55*32, screen_height - 16 - 4*32);
    addSquid(83*32, screen_height - 16 - 6*32);
    addSquid(94*32, screen_height - 16 - 11*32);
    addSquid(105*32, screen_height - 16 - 3*32);
    addCheep(75*32 + 28, screen_height - 16 - 4*32, 0, 1);
    addCheep(78*32 + 28, screen_height - 16 - 7*32, 0, 1);
    addCheep(81*32 + 28, screen_height - 16 - 2*32 - 28, 0, 1);
    addCheep(94*32 + 14, screen_height - 16 - 8*32, 0, 1);
    addCheep(101*32 + 28, screen_height - 16 - 4*32, 0, 1);
    addCheep(97*32 + 8, screen_height - 16 - 11*32, 1, 1);
    addCheep(117*32 + 8, screen_height - 16 - 10*32, 0, 1);
    addCheep(127*32 + 24, screen_height - 16 - 4*32, 1, 1);
    addCheep(131*32 + 8, screen_height - 16 - 3*32 - 4, 0, 1);
    addCheep(136*32 + 16, screen_height - 16 - 6*32, 0, 1);
    addCheep(145*32 + 8, screen_height - 16 - 4*32, 0, 1);
    addCheep(149*32 + 28, screen_height - 16 - 8*32 - 4, 1, 1);
    addCheep(164*32, screen_height - 16 - 11*32, 0, 1);
    addCheep(167*32, screen_height - 16 - 3*32, 1, 1);
    addCheep(175*32, screen_height - 16 - 6*32 - 4, 0, 1);
    addCheep(183*32, screen_height - 16 - 10*32, 1, 1);
    addCheep(186*32 + 16, screen_height - 16 - 7*32, 1, 1);
    this->iLevelType = 0;
    addPlant(274*32 + 16, screen_height - 10 - 3*32);
    this->iLevelType = 2;
}
void Map::loadMinionsLVL_2_3() {
    clearMinions();
    addCheepSpawner(5*32, 200*32);
}
void Map::loadMinionsLVL_2_4() {
    clearMinions();
    addBowser(135*32, screen_height - 16 - 6*32);
    addToad(153*32, screen_height - 3*32, false);
    addFireBall(49*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(55*32, screen_height - 16 - 9*32, 6, rand()%360, true);
    addFireBall(61*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(73*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(82*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(92*32, screen_height - 16 - 4*32, 6, rand()%360, false);
    addUpFire(16*32 + 4, 9*32);
    addUpFire(30*32, 9*32);
}
void Map::loadMinionsLVL_3_1() {
    clearMinions();
    addSpring(126*32, 336);
    addGoombas(37*32, screen_height - 16 - 2*32, true);
    addGoombas(53*32, screen_height - 16 - 2*32, true);
    addGoombas(54*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(56*32, screen_height - 16 - 2*32, true);
    addGoombas(82*32, screen_height - 16 - 6*32, true);
    addGoombas(83*32 + 16, screen_height - 16 - 6*32, true);
    addGoombas(85*32, screen_height - 16 - 6*32, true);
    addGoombas(94*32, screen_height - 16 - 2*32, true);
    addGoombas(95*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(139*32 - 4, screen_height - 16 - 6*32, true);
    addGoombas(140*32, screen_height - 16 - 7*32, true);
    addGoombas(154*32, screen_height - 16 - 2*32, true);
    addGoombas(155*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(157*32, screen_height - 16 - 2*32, true);
    addPlant(32*32 + 16, screen_height - 10 - 4*32);
    addPlant(38*32 + 16, screen_height - 10 - 5*32);
    addPlant(57*32 + 16, screen_height - 10 - 4*32);
    addPlant(67*32 + 16, screen_height - 10 - 3*32);
    addPlant(103*32 + 16, screen_height - 10 - 5*32);
    addKoppa(25*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(28*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(65*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(101*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(149*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(152*32, screen_height - 16 - 10*32, 1, true);
    addKoppa(165*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(168*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(170*32, screen_height - 16 - 6*32, 1, true);
    addKoppa(171*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(188*32, screen_height - 16 - 8*32, 1, true);
    addKoppa(191*32, screen_height - 16 - 10*32, 1, true);
    addHammerBro(113*32, screen_height - 16 - 7*32);
    addHammerBro(116*32, screen_height - 16 - 3*32);
}
void Map::loadMinionsLVL_3_2() {
    clearMinions();
    addKoppa(17*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(33*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(34*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(36*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(43*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(44*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(66*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(78*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(98*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(111*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(134*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(140*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(141*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(143*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(150*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(151*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(162*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(163*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(165*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(175*32, screen_height - 16 - 2*32, 1, true);
    addGoombas(24*32, screen_height - 16 - 2*32, true);
    addGoombas(25*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(27*32, screen_height - 16 - 2*32, true);
    addGoombas(71*32, screen_height - 16 - 2*32, true);
    addGoombas(72*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(74*32, screen_height - 16 - 2*32, true);
    addGoombas(119*32, screen_height - 16 - 2*32, true);
    addGoombas(120*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(122*32, screen_height - 16 - 2*32, true);
    addGoombas(179*32, screen_height - 16 - 2*32, true);
    addGoombas(180*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(182*32, screen_height - 16 - 2*32, true);
    addGoombas(188*32, screen_height - 16 - 2*32, true);
    addGoombas(189*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(191*32, screen_height - 16 - 2*32, true);
    addPlant(169*32 + 16, screen_height - 10 - 4*32);
}
void Map::loadMinionsLVL_3_3() {
    clearMinions();
    addGoombas(26*32, screen_height - 16 - 8*32, true);
    iLevelType = 3;
    addKoppa(52*32, screen_height - 16 - 8*32, 1, true);
    addKoppa(54*32, screen_height - 16 - 4*32, 1, true);
    addKoppa(73*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(114*32 - 8, screen_height - 16 - 9*32, 3, false);
    addKoppa(124*32, screen_height - 16 - 5*32, 1, true);
    addKoppa(126*32, screen_height - 16 - 5*32, 1, true);
    iLevelType = 4;
}
void Map::loadMinionsLVL_3_4() {
    clearMinions();
    addBowser(135*32, screen_height - 16 - 6*32);
    addToad(153*32, screen_height - 3*32, false);
    addFireBall(19*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addFireBall(24*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addFireBall(29*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addFireBall(54*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addFireBall(54*32, screen_height - 16 - 9*32, 6, rand()%360, false);
    addFireBall(64*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addFireBall(64*32, screen_height - 16 - 9*32, 6, rand()%360, false);
    addFireBall(80*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addFireBall(80*32, screen_height - 16 - 9*32, 6, rand()%360, false);
    addUpFire(16*32, 9*32);
    addUpFire(26*32, 9*32);
    addUpFire(88*32, 9*32);
    addUpFire(97*32, 9*32);
    addUpFire(103*32, 9*32);
    addUpFire(109*32, 9*32);
}
void Map::loadMinionsLVL_4_1() {
    clearMinions();
    addPlant(21*32 + 16, screen_height - 10 - 4*32);
    addPlant(116*32 + 16, screen_height - 10 - 5*32);
    addPlant(132*32 + 16, screen_height - 10 - 5*32);
    addPlant(163*32 + 16, screen_height - 10 - 3*32);
    addLakito(26*32, screen_height - 16 - 11*32, 207*32);
}
void Map::loadMinionsLVL_4_2() {
    clearMinions();
    addGoombas(43*32, screen_height - 16 - 6*32, true);
    addGoombas(44*32 + 16, screen_height - 16 - 6*32, true);
    addGoombas(46*32, screen_height - 16 - 6*32, true);
    addKoppa(77*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(100*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(101*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(137*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(168*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(169*32 + 16, screen_height - 16 - 2*32, 1, true);
    addPlant(72*32 + 16, screen_height - 10 - 4*32);
    addPlant(78*32 + 16, screen_height - 10 - 8*32);
    addPlant(84*32 + 16, screen_height - 10 - 4*32);
    addPlant(107*32 + 16, screen_height - 10 - 5*32);
    addPlant(138*32 + 16, screen_height - 10 - 4*32);
    addPlant(142*32 + 16, screen_height - 10 - 5*32);
    addPlant(180*32 + 16, screen_height - 10 - 8*32);
    addBeetle(83*32, screen_height - 16 - 2*32, true);
    addBeetle(88*32, screen_height - 16 - 2*32, true);
    addBeetle(154*32, screen_height - 16 - 5*32, true);
    addBeetle(179*32, screen_height - 16 - 2*32, true);
    this->iLevelType = 0;
    addPlant(394*32 + 16, screen_height - 10 - 3*32);
    this->iLevelType = 1;
}
void Map::loadMinionsLVL_4_3() {
    clearMinions();
    this->iLevelType = 3;
    addKoppa(28*32 - 2, screen_height - 16 - 6*32, 1, true);
    addKoppa(29*32, screen_height - 16 - 6*32, 1, true);
    addKoppa(39*32, screen_height - 16 - 3*32, 1, true);
    addKoppa(68*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(78*32, screen_height - 16 - 10*32, 1, true);
    addKoppa(35*32, screen_height - 16 - 11*32, 3, false);
    this->iLevelType = 0;
}
void Map::loadMinionsLVL_4_4() {
    clearMinions();
    addBowser(167*32, screen_height - 16 - 6*32);
    addToad(186*32, screen_height - 3*32, false);
    addFireBall(53*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(60*32, screen_height - 16 - 5*32, 6, rand()%360, false);
    addFireBall(115*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(122*32, screen_height - 16 - 4*32, 6, rand()%360, true);
    addFireBall(162*32, screen_height - 16 - 4*32, 6, rand()%360, true);
    addUpFire(165*32, 9*32);
    this->iLevelType = 1;
    addPlant(40*32 + 16, screen_height - 10 - 4*32);
    this->iLevelType = 3;
}
void Map::loadMinionsLVL_5_1() {
    clearMinions();
    addGoombas(19*32, screen_height - 16 - 2*32, true);
    addGoombas(20*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(22*32, screen_height - 16 - 2*32, true);
    addGoombas(30*32, screen_height - 16 - 2*32, true);
    addGoombas(31*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(33*32, screen_height - 16 - 2*32, true);
    addGoombas(65*32, screen_height - 16 - 2*32, true);
    addGoombas(66*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(68*32, screen_height - 16 - 2*32, true);
    addGoombas(76*32, screen_height - 16 - 2*32, true);
    addGoombas(77*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(103*32, screen_height - 16 - 2*32, true);
    addGoombas(104*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(106*32, screen_height - 16 - 2*32, true);
    addGoombas(121*32, screen_height - 16 - 2*32, true);
    addGoombas(122*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(124*32, screen_height - 16 - 2*32, true);
    addGoombas(135*32, screen_height - 16 - 2*32, true);
    addGoombas(136*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(138*32, screen_height - 16 - 2*32, true);
    addKoppa(16*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(41*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(42*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(61*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(87*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(127*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(144*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(145*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(178*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(182*32, screen_height - 16 - 6*32, 1, true);
    addPlant(44*32 + 16, screen_height - 10 - 4*32);
    addPlant(51*32 + 16, screen_height - 10 - 4*32);
    addPlant(156*32 + 16, screen_height - 10 - 7*32);
    addPlant(163*32 + 16, screen_height - 10 - 3*32);
}
void Map::loadMinionsLVL_5_2() {
    clearMinions();
    addGoombas(143*32, screen_height - 16 - 4*32, true);
    addGoombas(145*32, screen_height - 16 - 6*32, true);
    addGoombas(235*32, screen_height - 16 - 2*32, true);
    addGoombas(236*32 + 16, screen_height - 16 - 2*32, true);
    addKoppa(103*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(120*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(186*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(243*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(246*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(266*32, screen_height - 16 - 7*32, 0, true);
    addHammerBro(126*32, screen_height - 16 - 6*32);
    addHammerBro(161*32, screen_height - 16 - 7*32);
    addHammerBro(200*32, screen_height - 16 - 7*32);
    addHammerBro(204*32, screen_height - 16 - 11*32);
    addPlant(135*32 + 16, screen_height - 10 - 4*32);
    addPlant(195*32 + 16, screen_height - 10 - 3*32);
    addBeetle(216*32, screen_height - 16 - 2*32, true);
    addBeetle(217*32 + 4, screen_height - 16 - 2*32, true);
    addBeetle(218*32 + 8, screen_height - 16 - 2*32, true);
    this->iLevelType = 3;
    addKoppa(237*32, screen_height - 16 - 10*32, 1, true);
    this->iLevelType = 0;
    // -- MAP 5-2-2
    addSquid(17*32, screen_height - 16 - 4*32);
    addSquid(34*32, screen_height - 16 - 4*32);
    addSquid(43*32 + 16, screen_height - 16 - 4*32);
    addCheep(27*32 + 16, screen_height - 9*32, 0, 1);
    addCheep(38*32 + 28, screen_height - 4*32, 0, 1);
    addCheep(48*32 + 16, screen_height - 6*32, 1, 1);
    addCheep(53*32 + 16, screen_height - 11*32, 0, 1);
}
void Map::loadMinionsLVL_5_3() {
    clearMinions();
    this->iLevelType = 3;
    addKoppa(30*32 - 8, screen_height - 16 - 10*32, 1, true);
    addKoppa(110*32 - 8, screen_height - 16 - 8*32, 1, true);
    addKoppa(74*32 - 8, screen_height - 16 - 10*32, 3, false);
    addKoppa(114*32 - 8, screen_height - 16 - 9*32, 3, false);
    addKoppa(133*32 - 8, screen_height - 16 - 2*32, 1, true);
    this->iLevelType = 0;
    addGoombas(44*32, screen_height - 16 - 11*32, true);
    addGoombas(46*32, screen_height - 16 - 11*32, true);
    addGoombas(80*32, screen_height - 16 - 9*32, true);
    addBulletBillSpawner(174, 3, 1);
}
void Map::loadMinionsLVL_5_4() {
    clearMinions();
    addBowser(135*32, screen_height - 16 - 6*32);
    addToad(153*32, screen_height - 3*32, false);
    addFireBall(49*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(55*32, screen_height - 16 - 9*32, 6, rand()%360, true);
    addFireBall(61*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(73*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(82*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(92*32, screen_height - 16 - 4*32, 6, rand()%360, false);
    addFireBall(23*32, screen_height - 16 - 7*32, 12, rand()%360, true);
    addFireBall(43*32, screen_height - 16 - 1*32, 6, rand()%360, false);
    addFireBall(55*32, screen_height - 16 - 1*32, 6, rand()%360, false);
    addFireBall(67*32, screen_height - 16 - 1*32, 6, rand()%360, true);
    addFireBall(103*32, screen_height - 16 - 3*32, 6, rand()%360, true);
    addUpFire(16*32 + 4, 9*32);
    addUpFire(30*32, 9*32);
    addUpFire(109*32, 9*32);
    addUpFire(113*32, 9*32);
    addUpFire(131*32, 9*32);
}
void Map::loadMinionsLVL_6_1() {
    clearMinions();
    this->iLevelType = 0;
    addLakito(23*32, screen_height - 16 - 11*32, 176*32);
    addPlant(102*32 + 16, screen_height - 10 - 4*32);
    this->iLevelType = 4;
}
void Map::loadMinionsLVL_6_2() {
    clearMinions();
    addSquid(17*32, screen_height - 16 - 4*32);
    addSquid(34*32, screen_height - 16 - 4*32);
    addSquid(43*32 + 16, screen_height - 16 - 4*32);
    addCheep(27*32 + 16, screen_height - 9*32, 0, 1);
    addCheep(38*32 + 28, screen_height - 4*32, 0, 1);
    addCheep(48*32 + 16, screen_height - 6*32, 1, 1);
    addCheep(53*32 + 16, screen_height - 11*32, 0, 1);
    addKoppa(111*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(128*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(291*32, screen_height - 16 - 11*32, 0, true);
    addPlant(104*32 + 16, screen_height - 10 - 5*32);
    addPlant(113*32 + 16, screen_height - 10 - 5*32);
    addPlant(117*32 + 16, screen_height - 10 - 8*32);
    addPlant(120*32 + 16, screen_height - 10 - 3*32);
    addPlant(122*32 + 16, screen_height - 10 - 3*32);
    addPlant(131*32 + 16, screen_height - 10 - 5*32);
    addPlant(141*32 + 16, screen_height - 10 - 6*32);
    addPlant(147*32 + 16, screen_height - 10 - 7*32);
    addPlant(152*32 + 16, screen_height - 10 - 3*32);
    addPlant(165*32 + 16, screen_height - 10 - 4*32);
    addPlant(169*32 + 16, screen_height - 10 - 3*32);
    addPlant(172*32 + 16, screen_height - 10 - 7*32);
    addPlant(179*32 + 16, screen_height - 10 - 4*32);
    addPlant(190*32 + 16, screen_height - 10 - 3*32);
    addPlant(196*32 + 16, screen_height - 10 - 8*32);
    addPlant(200*32 + 16, screen_height - 10 - 3*32);
    addPlant(216*32 + 16, screen_height - 10 - 3*32);
    addPlant(220*32 + 16, screen_height - 10 - 3*32);
    addPlant(238*32 + 16, screen_height - 10 - 4*32);
    addPlant(252*32 + 16, screen_height - 10 - 8*32);
    addPlant(259*32 + 16, screen_height - 10 - 5*32);
    addPlant(264*32 + 16, screen_height - 10 - 3*32);
    addPlant(266*32 + 16, screen_height - 10 - 4*32);
    addPlant(268*32 + 16, screen_height - 10 - 5*32);
    addPlant(274*32 + 16, screen_height - 10 - 4*32);
    addPlant(286*32 + 16, screen_height - 10 - 5*32);
    addBeetle(139*32, screen_height - 16 - 2*32, true);
    addBeetle(177*32, screen_height - 16 - 2*32, true);
    addBeetle(205*32, screen_height - 16 - 10*32, true);
    addBeetle(248*32, screen_height - 16 - 2*32, true);
    addGoombas(152*32, screen_height - 16 - 10*32, true);
    addGoombas(254*32, screen_height - 16 - 2*32, true);
}
void Map::loadMinionsLVL_6_3() {
    clearMinions();
    addBulletBillSpawner(174, 3, 1);
}
void Map::loadMinionsLVL_6_4() {
    clearMinions();
    addBowser(135*32, screen_height - 16 - 6*32, true);
    addToad(153*32, screen_height - 3*32, false);
    addFireBall(30*32, screen_height - 16 - 4*32, 6, rand()%360, true);
    addFireBall(49*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(60*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(67*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(76*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(84*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addFireBall(88*32, screen_height - 16 - 10*32, 6, rand()%360, false);
    addFireBall(23*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(37*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(80*32, screen_height - 16 - 10*32, 6, rand()%360, false);
    addFireBall(92*32, screen_height - 16 - 5*32, 6, rand()%360, true);
    addUpFire(27*32, 9*32);
    addUpFire(33*32, 9*32);
    addUpFire(131*32, 9*32);
}
void Map::loadMinionsLVL_7_1() {
    clearMinions();
    addKoppa(26*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(44*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(53*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(65*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(114*32, screen_height - 16 - 2*32, 1, true);
    addPlant(76*32 + 16, screen_height - 10 - 4*32);
    addPlant(93*32 + 16, screen_height - 10 - 4*32);
    addPlant(109*32 + 16, screen_height - 10 - 4*32);
    addPlant(115*32 + 16, screen_height - 10 - 3*32);
    addPlant(128*32 + 16, screen_height - 10 - 3*32);
    addHammerBro(85*32, screen_height - 16 - 7*32);
    addHammerBro(87*32, screen_height - 16 - 11*32);
    addHammerBro(137*32, screen_height - 16 - 7*32);
    addHammerBro(135*32, screen_height - 16 - 3*32);
    addBeetle(169*32, screen_height - 16 - 10*32, true);
}
void Map::loadMinionsLVL_7_2() {
    clearMinions();
    addSquid(22*32, screen_height - 16 - 3*32);
    addSquid(24*32, screen_height - 16 - 7*32);
    addSquid(46*32, screen_height - 16 - 5*32);
    addSquid(52*32, screen_height - 16 - 7*32);
    addSquid(55*32, screen_height - 16 - 4*32);
    addSquid(77*32, screen_height - 16 - 8*32);
    addSquid(83*32, screen_height - 16 - 6*32);
    addSquid(90*32, screen_height - 16 - 3*32);
    addSquid(94*32, screen_height - 16 - 11*32);
    addSquid(105*32, screen_height - 16 - 3*32);
    addSquid(150*32, screen_height - 16 - 6*32);
    addSquid(173*32, screen_height - 16 - 3*32);
    addSquid(179*32, screen_height - 16 - 3*32);
    addCheep(75*32 + 28, screen_height - 16 - 4*32, 0, 1);
    addCheep(78*32 + 28, screen_height - 16 - 7*32, 0, 1);
    addCheep(81*32 + 28, screen_height - 16 - 2*32 - 28, 0, 1);
    addCheep(94*32 + 14, screen_height - 16 - 8*32, 0, 1);
    addCheep(101*32 + 28, screen_height - 16 - 4*32, 0, 1);
    addCheep(97*32 + 8, screen_height - 16 - 11*32, 1, 1);
    addCheep(117*32 + 8, screen_height - 16 - 10*32, 0, 1);
    addCheep(127*32 + 24, screen_height - 16 - 4*32, 1, 1);
    addCheep(131*32 + 8, screen_height - 16 - 3*32 - 4, 0, 1);
    addCheep(136*32 + 16, screen_height - 16 - 6*32, 0, 1);
    addCheep(145*32 + 8, screen_height - 16 - 4*32, 0, 1);
    addCheep(149*32 + 28, screen_height - 16 - 8*32 - 4, 1, 1);
    addCheep(164*32, screen_height - 16 - 11*32, 0, 1);
    addCheep(167*32, screen_height - 16 - 3*32, 1, 1);
    addCheep(175*32, screen_height - 16 - 6*32 - 4, 0, 1);
    addCheep(183*32, screen_height - 16 - 10*32, 1, 1);
    addCheep(186*32 + 16, screen_height - 16 - 7*32, 1, 1);
    this->iLevelType = 0;
    addPlant(274*32 + 16, screen_height - 10 - 3*32);
    this->iLevelType = 2;
}
void Map::loadMinionsLVL_7_3() {
    clearMinions();
    addCheepSpawner(5*32, 200*32);
    addKoppa(52*32, screen_height - 16 - 6*32, 0, true);
    addKoppa(140*32, screen_height - 16 - 7*32, 0, true);
    addKoppa(156*32, screen_height - 16 - 5*32, 0, true);
    this->iLevelType = 3;
    addKoppa(79*32, screen_height - 16 - 5*32, 1, true);
    addKoppa(95*32, screen_height - 16 - 5*32, 1, true);
    addKoppa(119*32, screen_height - 16 - 2*32, 1, true);
    this->iLevelType = 0;
}
void Map::loadMinionsLVL_7_4() {
    clearMinions();
    addUpFire(20*32, 9*32);
    addUpFire(260*32, 9*32);
    addFireBall(167*32, screen_height - 16 - 7*32, 6, rand()%360, true);
    addBowser(263*32, screen_height - 16 - 6*32, true);
    addToad(281*32, screen_height - 3*32, false);
}
void Map::loadMinionsLVL_8_1() {
    clearMinions();
    addGoombas(23*32, screen_height - 16 - 2*32, true);
    addGoombas(24*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(26*32, screen_height - 16 - 2*32, true);
    addGoombas(30*32, screen_height - 16 - 2*32, true);
    addGoombas(31*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(33*32, screen_height - 16 - 2*32, true);
    addGoombas(69*32, screen_height - 16 - 2*32, true);
    addGoombas(70*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(72*32, screen_height - 16 - 2*32, true);
    addGoombas(108*32, screen_height - 16 - 2*32, true);
    addGoombas(109*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(111*32, screen_height - 16 - 2*32, true);
    addGoombas(148*32, screen_height - 16 - 2*32, true);
    addGoombas(149*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(151*32, screen_height - 16 - 2*32, true);
    addGoombas(232*32, screen_height - 16 - 2*32, true);
    addGoombas(233*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(235*32, screen_height - 16 - 2*32, true);
    addGoombas(257*32, screen_height - 16 - 2*32, true);
    addGoombas(258*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(260*32, screen_height - 16 - 2*32, true);
    addGoombas(264*32, screen_height - 16 - 2*32, true);
    addGoombas(265*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(267*32, screen_height - 16 - 2*32, true);
    addGoombas(272*32, screen_height - 16 - 2*32, true);
    addGoombas(273*32 + 16, screen_height - 16 - 2*32, true);
    addKoppa(43*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(44*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(61*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(119*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(124*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(125*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(127*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(130*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(131*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(133*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(161*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(172*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(177*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(207*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(208*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(305*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(332*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(339*32, screen_height - 16 - 2*32, 1, true);
    addKoppa(340*32 + 16, screen_height - 16 - 2*32, 1, true);
    addKoppa(342*32, screen_height - 16 - 2*32, 1, true);
    addBeetle(18*32, screen_height - 16 - 2*32, true);
    addBeetle(81*32, screen_height - 16 - 2*32, true);
    addBeetle(254*32, screen_height - 16 - 2*32, true);
    addBeetle(283*32, screen_height - 16 - 2*32, true);
    addPlant(35*32 + 16, screen_height - 10 - 5*32);
    addPlant(76*32 + 16, screen_height - 10 - 5*32);
    addPlant(82*32 + 16, screen_height - 10 - 4*32);
    addPlant(94*32 + 16, screen_height - 10 - 5*32);
    addPlant(104*32 + 16, screen_height - 10 - 5*32);
    addPlant(140*32 + 16, screen_height - 10 - 4*32);
    addPlant(238*32 + 16, screen_height - 10 - 4*32);
    addPlant(242*32 + 16, screen_height - 10 - 5*32);
    addPlant(344*32 + 16, screen_height - 10 - 4*32);
    addPlant(355*32 + 16, screen_height - 10 - 3*32);
}
void Map::loadMinionsLVL_8_2() {
    clearMinions();
    addGoombas(184*32, screen_height - 16 - 5*32, true);
    addGoombas(186*32, screen_height - 16 - 7*32, true);
    addKoppa(18*32 - 8, screen_height - 16 - 5*32, 0, true);
    addKoppa(24*32, screen_height - 16 - 10*32, 0, true);
    addKoppa(57*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(66*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(69*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(92*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(95*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(139*32, screen_height - 16 - 2*32, 0, true);
    addKoppa(170*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(172*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(175*32, screen_height - 16 - 6*32, 0, true);
    addKoppa(203*32, screen_height - 16 - 8*32, 0, true);
    addBeetle(111*32, screen_height - 16 - 2*32, true);
    addBeetle(121*32, screen_height - 16 - 2*32, true);
    addBeetle(123*32, screen_height - 16 - 2*32, true);
    addBeetle(189*32, screen_height - 16 - 2*32, true);
    addLakito(16*32, screen_height - 16 - 11*32, 216*32);
    addPlant(131*32 + 16, screen_height - 10 - 3*32);
    addPlant(142*32 + 16, screen_height - 10 - 3*32);
    addPlant(156*32 + 16, screen_height - 10 - 5*32);
    addPlant(163*32 + 16, screen_height - 10 - 3*32);
    addPlant(131*32 + 16, screen_height - 10 - 3*32);
}
void Map::loadMinionsLVL_8_3() {
    clearMinions();
    addKoppa(30*32, screen_height - 16 - 4*32, 0, true);
    addKoppa(93*32, screen_height - 16 - 3*32, 0, true);
    addKoppa(137*32, screen_height - 16 - 2*32, 1, true);
    addPlant(53*32 + 16, screen_height - 10 - 5*32);
    addPlant(126*32 + 16, screen_height - 10 - 5*32);
    addPlant(168*32 + 16, screen_height - 10 - 4*32);
    addHammerBro(63*32, screen_height - 16 - 3*32);
    addHammerBro(65*32, screen_height - 16 - 7*32);
    addHammerBro(117*32, screen_height - 16 - 7*32);
    addHammerBro(119*32, screen_height - 16 - 3*32);
    addHammerBro(146*32, screen_height - 16 - 3*32);
    addHammerBro(159*32, screen_height - 16 - 3*32);
    addHammerBro(177*32, screen_height - 16 - 3*32);
    addHammerBro(185*32, screen_height - 16 - 3*32);
}
void Map::loadMinionsLVL_8_4() {
    clearMinions();
    addPlant(19*32 + 16, screen_height - 10 - 3*32);
    addPlant(51*32 + 16, screen_height - 10 - 3*32);
    addPlant(81*32 + 16, screen_height - 10 - 6*32);
    addPlant(126*32 + 16, screen_height - 10 - 3*32);
    addPlant(133*32 + 16, screen_height - 10 - 4*32);
    addPlant(143*32 + 16, screen_height - 10 - 3*32);
    addPlant(153*32 + 16, screen_height - 10 - 4*32);
    addPlant(163*32 + 16, screen_height - 10 - 8*32);
    addPlant(215*32 + 16, screen_height - 10 - 3*32);
    addPlant(302*32 + 16, screen_height - 10 - 3*32);
    addPlant(224*32 + 16, screen_height - 10 - 6*32);
    addPlant(232*32 + 16, screen_height - 10 - 7*32);
    addPlant(248*32 + 16, screen_height - 10 - 6*32);
    addPlant(309*32 + 16, screen_height - 10 - 3*32);
    addBeetle(139*32, screen_height - 16 - 2*32, true);
    addBeetle(141*32, screen_height - 16 - 2*32, true);
    addGoombas(56*32, screen_height - 16 - 2*32, true);
    addGoombas(57*32 + 16, screen_height - 16 - 2*32, true);
    addGoombas(59*32, screen_height - 16 - 2*32, true);
    addHammerBro(316*32, screen_height - 16 - 3*32);
    this->iLevelType = 1;
    addKoppa(150*32 - 8, screen_height - 16 - 4*32, 0, true);
    addKoppa(152*32 - 8, screen_height - 16 - 3*32, 0, true);
    addKoppa(165*32 - 8, screen_height - 16 - 3*32, 0, true);
    addKoppa(167*32 - 8, screen_height - 16 - 4*32, 0, true);
    this->iLevelType = 3;
    addCheepSpawner(224*32, 237*32);
    addBowser(338*32, screen_height - 16 - 6*32, true);
    addToad(356*32, screen_height - 3*32, true);
    addUpFire(332*32, 9*32);
    addFireBall(410*32, screen_height - 16 - 7*32, 6, rand()%360, true);
    addFireBall(421*32, screen_height - 16 - 4*32, 6, rand()%360, false);
    addFireBall(430*32, screen_height - 16 - 8*32, 6, rand()%360, true);
    addFireBall(446*32, screen_height - 16 - 6*32, 6, rand()%360, true);
    addFireBall(454*32, screen_height - 16 - 7*32, 6, rand()%360, false);
    addSquid(418*32, screen_height - 16 - 3*32);
    addSquid(441*32, screen_height - 16 - 4*32);
    addSquid(443*32, screen_height - 16 - 8*32);
}
void Map::createMap() {
    // ----- MIONION LIST -----
    for(int i = 0; i < iMapWidth; i += 5) {
        vector<Minion*> temp;
        lMinion.push_back(temp);
    }
    iMinionListSize = lMinion.size();
    // ----- MIONION LIST -----
    // ----- CREATE MAP -----
    for(int i = 0; i < iMapWidth; i++) {
        vector<MapLevel*> temp;
        for(int i = 0; i < iMapHeight; i++) {
            temp.push_back(new MapLevel(0));
        }
        lMap.push_back(temp);
    }
    // ----- CREATE MAP -----
    this->underWater = false;
    this->bTP = false;
}
void Map::checkSpawnPoint() {
    if(getNumOfSpawnPoints() > 1) {
        for(int i = iSpawnPointID + 1; i < getNumOfSpawnPoints(); i++) {
            if(getSpawnPointXPos(i) > oPlayer->getXPos() - fXPos && getSpawnPointXPos(i) < oPlayer->getXPos() - fXPos + 128) {
                iSpawnPointID = i;
            }
        }
    }
}
int Map::getNumOfSpawnPoints() {
    switch(currentLevelID) {
        case 0: case 1: case 2: case 4: case 5: case 8: case 9: case 10: case 13: case 14: case 16: case 17: case 18: case 20: case 21: case 22: case 24: case 25: case 26:
            return 2;
    }
    return 1;
}
int Map::getSpawnPointXPos(int iID) {
    switch(currentLevelID) {
        case 0:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 82*32;
            }
        case 1:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 89*32;
            }
        case 2:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 66*32;
            }
        case 3:
            return 64;
        case 4:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 98*32;
            }
        case 5:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 86*32;
            }
        case 6:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 114*32;
            }
        case 7:
            return 64;
        case 8:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 90*32;
            }
        case 9:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 98*32;
            }
        case 10:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 66*32;
            }
        case 13:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 95*32;
            }
        case 14:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 65*32;
            }
        case 16:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 97*32;
            }
        case 17:
            switch(iID) {
                case 0:
                    return 84 + 80*32;
                case 1:
                    return 177*32;
            }
        case 18:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 66*32;
            }
        case 20:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 98*32;
            }
        case 21:
            switch(iID) {
                case 0:
                    return 84 + 85*32;
                case 1:
                    return 183*32;
            }
        case 22:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 98*32;
            }
        case 24:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 98*32;
            }
        case 25:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 86*32;
            }
        case 26:
            switch(iID) {
                case 0:
                    return 84;
                case 1:
                    return 113*32;
            }
    }
    return 84;
}
int Map::getSpawnPointYPos(int iID) {
    switch(currentLevelID) {
        case 1:
            switch(iID) {
                case 0:
                    return 64;
            }
        case 5: case 25:
            switch(iID) {
                case 0:
                    return 64;
                case 1:
                    return screen_height - 48 - oPlayer->getHitBoxY();;
            }
        case 3: case 7: case 11: case 15: case 19: case 23: case 27: case 31:
            return 150;
    }
    return screen_height - 48 - oPlayer->getHitBoxY();
}
void Map::setSpawnPoint() {
    float X = (float)getSpawnPointXPos(iSpawnPointID);
    if(X > 6*32) {
        fXPos = -(X - 6*32);
        oPlayer->setXPos(6*32);
        oPlayer->setYPos((float)getSpawnPointYPos(iSpawnPointID));
    } else {
        fXPos = 0;
        oPlayer->setXPos(X);
        oPlayer->setYPos((float)getSpawnPointYPos(iSpawnPointID));
    }
    oPlayer->setMoveDirection(true);
}
void Map::resetGameData() {
    this->currentLevelID = 0;
    this->iSpawnPointID = 0;
    oPlayer->setCoins(0);
    oPlayer->setScore(0);
    oPlayer->resetPowerLVL();
    oPlayer->setNumOfLives(3);
    setSpawnPoint();
    loadLVL();
}
void Map::loadLVL() {
    clearPipeEvents();
    switch(currentLevelID) {
        case 0:
            loadLVL_1_1();
            break;
        case 1:
            loadLVL_1_2();
            break;
        case 2:
            loadLVL_1_3();
            break;
        case 3:
            loadLVL_1_4();
            break;
        case 4:
            loadLVL_2_1();
            break;
        case 5:
            loadLVL_2_2();
            break;
        case 6:
            loadLVL_2_3();
            break;
        case 7:
            loadLVL_2_4();
            break;
        case 8:
            loadLVL_3_1();
            break;
        case 9:
            loadLVL_3_2();
            break;
        case 10:
            loadLVL_3_3();
            break;
        case 11:
            loadLVL_3_4();
            break;
        case 12:
            loadLVL_4_1();
            break;
        case 13:
            loadLVL_4_2();
            break;
        case 14:
            loadLVL_4_3();
            break;
        case 15:
            loadLVL_4_4();
            break;
        case 16:
            loadLVL_5_1();
            break;
        case 17:
            loadLVL_5_2();
            break;
        case 18:
            loadLVL_5_3();
            break;
        case 19:
            loadLVL_5_4();
            break;
        case 20:
            loadLVL_6_1();
            break;
        case 21:
            loadLVL_6_2();
            break;
        case 22:
            loadLVL_6_3();
            break;
        case 23:
            loadLVL_6_4();
            break;
        case 24:
            loadLVL_7_1();
            break;
        case 25:
            loadLVL_7_2();
            break;
        case 26:
            loadLVL_7_3();
            break;
        case 27:
            loadLVL_7_4();
            break;
        case 28:
            loadLVL_8_1();
            break;
        case 29:
            loadLVL_8_2();
            break;
        case 30:
            loadLVL_8_3();
            break;
        case 31:
            loadLVL_8_4();
            break;
    }
}
// ---------- LEVELTEXT -----
void Map::clearLevelText() {
    for(unsigned int i = 0; i < vLevelText.size(); i++) {
        delete vLevelText[i];
    }
    vLevelText.clear();
}
// ---------- LOADPIPEEVENTS -----
void Map::clearPipeEvents() {
    for(unsigned int i = 0; i < lPipe.size(); i++) {
        delete lPipe[i];
    }
    lPipe.clear();
}
void Map::loadPipeEventsLVL_1_1() {
    lPipe.push_back(new Pipe(0, 57, 5, 58, 5, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 163 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_1_2() {
    lPipe.push_back(new Pipe(0, 103, 4, 104, 4, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 115 * 32, screen_height - 16 - 3 * 32, currentLevelID, 1, true, 350, 1, false));
    lPipe.push_back(new Pipe(1, 166, 6, 166, 5, 284 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
    // -- WARP ZONE 2
    lPipe.push_back(new Pipe(0, 186, 4, 187, 4, 84, screen_height - 80, 4, 0, true, 350, 1, false));
    // -- WARP ZONE 3
    lPipe.push_back(new Pipe(0, 182, 4, 183, 4, 84, screen_height - 80, 8, 4, true, 350, 1, false));
    // -- WARP ZONE 4
    lPipe.push_back(new Pipe(0, 178, 4, 179, 4, 84, screen_height - 80, 12, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_2_1() {
    lPipe.push_back(new Pipe(0, 103, 5, 104, 5, 232 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 243, 3, 243, 2, 115 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_2_2() {
    lPipe.push_back(new Pipe(1, 189, 7, 189, 6, 274 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_3_1() {
    lPipe.push_back(new Pipe(0, 38, 5, 39, 5, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 67 * 32, screen_height - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_4_1() {
    lPipe.push_back(new Pipe(0, 132, 5, 133, 5, 252 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 263, 3, 263, 2, 163 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_4_2() {
    lPipe.push_back(new Pipe(0, 84, 4, 85, 4, 272 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 283, 3, 283, 2, 131 * 32, screen_height - 16 - 3 * 32, currentLevelID, 1, true, 350, 1, false));
    lPipe.push_back(new Pipe(1, 187, 6, 187, 5, 394 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
    // -- WARP ZONE 5
    lPipe.push_back(new Pipe(0, 214, 4, 215, 4, 84, screen_height - 80, 16, 0, true, 350, 1, false));
    // -- WARP ZONE 6
    lPipe.push_back(new Pipe(0, 368, 4, 369, 4, 84, screen_height - 80, 20, 4, true, 350, 1, false));
    // -- WARP ZONE 7
    lPipe.push_back(new Pipe(0, 364, 4, 365, 4, 84, screen_height - 80, 24, 0, true, 350, 1, false));
    // -- WARP ZONE 8
    lPipe.push_back(new Pipe(0, 360, 4, 361, 4, 84, screen_height - 80, 28, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_5_1() {
    lPipe.push_back(new Pipe(0, 156, 7, 157, 7, 230 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 241, 3, 241, 2, 163 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_5_2() {
    lPipe.push_back(new Pipe(0, 135, 4, 136, 4, 0, 32, currentLevelID, 2, true, 350, 1, true));
    lPipe.push_back(new Pipe(1, 62, 7, 62, 6, 195 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_6_2() {
    lPipe.push_back(new Pipe(0, 141, 6, 142, 6, 0, 32, currentLevelID, 2, true, 350, 1, true));
    lPipe.push_back(new Pipe(1, 62, 7, 62, 6, 200 * 32, screen_height - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));
    lPipe.push_back(new Pipe(0, 104, 5, 105, 5, 332 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 343, 3, 343, 2, 120 * 32, screen_height - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));
    lPipe.push_back(new Pipe(0, 238, 4, 239, 4, 362 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 373, 3, 373, 2, 264 * 32, screen_height - 16 - 3 * 32, currentLevelID, 4, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_7_1() {
    lPipe.push_back(new Pipe(0, 93, 4, 94, 4, 212 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 223, 3, 223, 2, 115 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_7_2() {
    lPipe.push_back(new Pipe(1, 189, 7, 189, 6, 274 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_8_1() {
    lPipe.push_back(new Pipe(0, 104, 5, 105, 5, 412 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 423, 3, 423, 2, 115 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_8_2() {
    lPipe.push_back(new Pipe(0, 156, 5, 157, 5, 242 * 32, 32, currentLevelID, 1, false, 350, 1, false));
    lPipe.push_back(new Pipe(1, 253, 3, 253, 2, 163 * 32, screen_height - 16 - 3 * 32, currentLevelID, 0, true, 350, 1, false));
}
void Map::loadPipeEventsLVL_8_4() {
    lPipe.push_back(new Pipe(2, 81, 6, 82, 6, 126 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
    lPipe.push_back(new Pipe(2, 163, 8, 164, 8, 215 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
    lPipe.push_back(new Pipe(2, 248, 6, 249, 6, 393 * 32, screen_height - 16 - 3 * 32, currentLevelID, 2, true, 250, 1, true));
    lPipe.push_back(new Pipe(1, 458, 7, 458, 6, 302 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 350, 1, false));
    lPipe.push_back(new Pipe(2, 51, 4, 52, 4, 19 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
    lPipe.push_back(new Pipe(2, 143, 3, 144, 3, 19 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
    lPipe.push_back(new Pipe(2, 232, 7, 232, 7, 19 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
    lPipe.push_back(new Pipe(2, 309, 3, 310, 3, 19 * 32, screen_height - 16 - 3 * 32, currentLevelID, 3, true, 250, 1, false));
}
// ---------- LOADPIPEEVENTS -----
void Map::loadLVL_1_1() {
    clearMap();
    this->iMapWidth = 260;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    // ----- MINIONS
    loadMinionsLVL_1_1();
    // ----- PIPEEVENT
    loadPipeEventsLVL_1_1();
    // ----- Bush -----
    structBush(0, 2, 2);
    structBush(16, 2, 1);
    structBush(48, 2, 2);
    structBush(64, 2, 1);
    structBush(96, 2, 2);
    structBush(112, 2, 1);
    structBush(144, 2, 2);
    structBush(160, 2, 1);
    structBush(192, 2, 2);
    structBush(208, 2, 1);
    // ----- Bush -----
    // ----- Clouds -----
    structCloud(8, 10, 1);
    structCloud(19, 11, 1);
    structCloud(27, 10, 3);
    structCloud(36, 11, 2);
    structCloud(56, 10, 1);
    structCloud(67, 11, 1);
    structCloud(75, 10, 3);
    structCloud(84, 11, 2);
    structCloud(104, 10, 1);
    structCloud(115, 11, 1);
    structCloud(123, 10, 3);
    structCloud(132, 11, 2);
    structCloud(152, 10, 1);
    structCloud(163, 11, 1);
    structCloud(171, 10, 3);
    structCloud(180, 11, 2);
    structCloud(200, 10, 1);
    structCloud(211, 11, 1);
    structCloud(219, 10, 3);
    // ----- Clouds -----
    // ----- Grass -----
    structGrass(11, 2, 3);
    structGrass(23, 2, 1);
    structGrass(41, 2, 2);
    structGrass(59, 2, 3);
    structGrass(71, 2, 1);
    structGrass(89, 2, 2);
    structGrass(107, 2, 3);
    structGrass(119, 2, 1);
    structGrass(137, 2, 2);
    structGrass(157, 2, 1);
    structGrass(167, 2, 1);
    structGrass(205, 2, 1);
    structGrass(215, 2, 1);
    // ----- Grass -----
    // ----- GND -----
    structGND(0, 0, 69, 2);
    structGND(71, 0, 15, 2);
    structGND(89, 0, 64, 2);
    structGND(155, 0, 85, 2);
    // ----- GND -----
    // ----- GND 2 -----
    structGND2(134, 2, 4, true);
    structGND2(140, 2, 4, false);
    structGND2(148, 2, 4, true);
    structGND2(152, 2, 1, 4);
    structGND2(155, 2, 4, false);
    structGND2(181, 2, 8, true);
    structGND2(189, 2, 1, 8);
    structGND2(198, 2, 1, 1);
    // ----- GND 2 -----
    // ----- BRICK -----
    struckBlockQ(16, 5, 1);
    structBrick(20, 5, 1, 1);
    struckBlockQ(21, 5, 1);
    lMap[21][5]->setSpawnMushroom(true);
    structBrick(22, 5, 1, 1);
    struckBlockQ(22, 9, 1);
    struckBlockQ(23, 5, 1);
    structBrick(24, 5, 1, 1);
    struckBlockQ2(64, 6, 1);
    lMap[64][6]->setSpawnMushroom(true);
    lMap[64][6]->setPowerUP(false);
    structBrick(77, 5, 1, 1);
    struckBlockQ(78, 5, 1);
    lMap[78][5]->setSpawnMushroom(true);
    structBrick(79, 5, 1, 1);
    structBrick(80, 9, 8, 1);
    structBrick(91, 9, 3, 1);
    struckBlockQ(94, 9, 1);
    structBrick(94, 5, 1, 1);
    lMap[94][5]->setNumOfUse(4);
    structBrick(100, 5, 2, 1);
    struckBlockQ(106, 5, 1);
    struckBlockQ(109, 5, 1);
    struckBlockQ(109, 9, 1);
    lMap[109][9]->setSpawnMushroom(true);
    struckBlockQ(112, 5, 1);
    structBrick(118, 5, 1, 1);
    structBrick(121, 9, 3, 1);
    structBrick(128, 9, 1, 1);
    struckBlockQ(129, 9, 2);
    structBrick(131, 9, 1, 1);
    structBrick(129, 5, 2, 1);
    structBrick(168, 5, 2, 1);
    struckBlockQ(170, 5, 1);
    structBrick(171, 5, 1, 1);
    lMap[101][5]->setSpawnStar(true);
    // ----- BRICK -----
    // ----- PIPES -----
    structPipe(28, 2, 1);
    structPipe(38, 2, 2);
    structPipe(46, 2, 3);
    structPipe(57, 2, 3);
    structPipe(163, 2, 1);
    structPipe(179, 2, 1);
    // ----- PIPES -----
    // ----- END
    structEnd(198, 3, 9);
    structCastleSmall(202, 2);
    // ----- MAP 1_1_2 -----
    this->iLevelType = 1;
    structGND(240, 0, 17, 2);
    structBrick(240, 2, 1, 11);
    structBrick(244, 2, 7, 3);
    structBrick(244, 12, 7, 1);
    structPipeVertical(255, 2, 10);
    structPipeHorizontal(253, 2, 1);
    structCoins(244, 5, 7, 1);
    structCoins(244, 7, 7, 1);
    structCoins(245, 9, 5, 1);
    // ----- END LEVEL
    this->iLevelType = 0;
}
void Map::loadLVL_1_2() {
    clearMap();
    this->iMapWidth = 330;
    this->iMapHeight = 25;
    this->iLevelType = 1;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    // ---------- LOAD LISTS ----------
    // ----- MINIONS
    loadMinionsLVL_1_2();
    // ----- PIPEEVENTS
    loadPipeEventsLVL_1_2();
    vPlatform.push_back(new Platform(6, 0, 139*32 + 16, 139*32 + 16, -32, screen_height + 32, 139*32 + 16, 9*32 + 16, true));
    vPlatform.push_back(new Platform(6, 0, 139*32 + 16, 139*32 + 16, -32, screen_height + 32, 139*32 + 16, 1*32 + 16, true));
    vPlatform.push_back(new Platform(6, 1, 154*32 + 16, 154*32 + 16, -32, screen_height + 32, 154*32 + 16, 10*32, true));
    vPlatform.push_back(new Platform(6, 1, 154*32 + 16, 154*32 + 16, -32, screen_height + 32, 154*32 + 16, 2*32, true));
    vLevelText.push_back(new LevelText(178*32, screen_height - 16 - 8*32, "WELCOME TO WARP ZONEz"));
    vLevelText.push_back(new LevelText(178*32 + 16, screen_height - 6*32, "4"));
    vLevelText.push_back(new LevelText(182*32 + 16, screen_height - 6*32, "3"));
    vLevelText.push_back(new LevelText(186*32 + 16, screen_height - 6*32, "2"));
    // ----- GND -----
    structGND(0, 0, 80, 2);
    structGND(83, 0, 37, 2);
    structGND(122, 0, 2, 2);
    structGND(126, 0, 12, 2);
    structGND(145, 0, 8, 2);
    structGND(160, 0, 43, 2);
    // ----- GND -----
    // ----- Brick -----
    structBrick(0, 2, 1, 11);
    structBrick(122, 2, 2, 3);
    structBrick(6, 12, 132, 1);
    structBrick(29, 6, 1, 1);
    lMap[29][6]->setNumOfUse(10);
    structBrick(39, 5, 1, 3);
    structBrick(40, 5, 1, 1);
    structBrick(41, 5, 1, 3);
    structBrick(42, 7, 2, 1);
    structBrick(44, 5, 1, 3);
    structBrick(45, 5, 1, 1);
    structBrick(46, 5, 1, 3);
    structBrick(52, 5, 2, 5);
    structBrick(54, 3, 2, 3);
    structBrick(54, 10, 2, 2);
    structBrick(58, 10, 6, 2);
    structBrick(58, 5, 6, 1);
    structBrick(62, 6, 2, 4);
    structBrick(66, 10, 4, 2);
    structBrick(67, 5, 1, 5);
    structBrick(68, 5, 1, 1);
    structBrick(69, 5, 1, 2);
    lMap[69][6]->setSpawnMushroom(true);
    structBrick(72, 5, 2, 5);
    lMap[73][6]->setNumOfUse(10);
    structBrick(76, 5, 4, 1);
    structBrick(76, 10, 4, 2);
    structBrick(84, 6, 6, 2);
    lMap[89][12]->setSpawnMushroom(true);
    lMap[89][12]->setPowerUP(false);
    structBrick(145, 6, 6, 1);
    lMap[150][6]->setSpawnMushroom(true);
    structBrick(160, 2, 17, 3);
    structBrick(170, 5, 7, 8);
    structBrick(161, 12, 7, 1);
    structBrick(177, 12, 10, 1);
    structBrick(190, 2, 13, 17);
    lMap[46][7]->setSpawnStar(true);
    // ----- Brick -----
    // ----- GND2 -----
    structGND2(17, 2, 1, 1);
    structGND2(19, 2, 1, 2);
    structGND2(21, 2, 1, 3);
    structGND2(23, 2, 1, 4);
    structGND2(25, 2, 1, 4);
    structGND2(27, 2, 1, 3);
    structGND2(31, 2, 1, 3);
    structGND2(33, 2, 1, 2);
    structGND2(133, 2, 4, true);
    structGND2(137, 2, 1, 4);
    // ----- GND2 -----
    // ----- BLOCKQ -----
    struckBlockQ(10, 5, 5);
    lMap[10][5]->setSpawnMushroom(true);
    // ----- BLOCKQ -----
    // ----- PIPES -----
    structPipe(103, 2, 2);
    structPipe(109, 2, 3);
    structPipe(115, 2, 1);
    structPipeVertical(168, 5, 7);
    structPipeHorizontal(166, 5, 1);
    structPipe(178, 2, 2);
    structPipe(182, 2, 2);
    structPipe(186, 2, 2);
    // ----- PIPES -----
    // ----- COIN -----
    structCoins(40, 6, 1, 1);
    structCoins(41, 9, 4, 1);
    structCoins(45, 6, 1, 1);
    structCoins(58, 6, 4, 1);
    structCoins(68, 6, 1, 1);
    structCoins(84, 9, 6, 1);
    // ----- MAP 1-1-2
    this->iLevelType = 0;
    structGND(210, 0, 24, 2);
    structCastleSmall(210, 2);
    structCloud(213, 10, 2);
    structCloud(219, 6, 1);
    structCloud(228, 11, 2);
    structPipe(222, 2, 3);
    structPipeHorizontal(220, 2, 1);
    // ----- MAP 1-1-2
    // ----- MAP 1-1-3
    this->iLevelType = 1;
    structGND(240, 0, 17, 2);
    structBrick(240, 2, 1, 11);
    structBrick(243, 5, 10, 1);
    structBrick(243, 9, 10, 4);
    structBrick(253, 4, 2, 9);
    lMap[252][5]->setNumOfUse(10);
    structCoins(243, 2, 9, 1);
    structCoins(244, 6, 8, 1);
    structPipeVertical(255, 2, 10);
    structPipeHorizontal(253, 2, 1);
    // ----- MAP 1-1-3
    // ----- MAP 1-1-4
    this->iLevelType = 0;
    structCloud(285, 11, 2);
    structCloud(305, 10, 1);
    structCloud(316, 11, 1);
    structGrass(310, 2, 1);
    structGrass(320, 2, 1);
    structBush(297, 2, 2);
    structBush(313, 2, 1);
    structGND(281, 0, 42, 2);
    structPipe(284, 2, 1);
    structGND2(286, 2, 8, true);
    structGND2(294, 2, 1, 8);
    structGND2(303, 2, 1, 1);
    structEnd(303, 3, 9);
    structCastleSmall(307, 2);
    // ----- MAP 1-1-4
    // ----- END
    this->iLevelType = 1;
}
void Map::loadLVL_1_3() {
    clearMap();
    this->iMapWidth = 270;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_1_3();
    // ---------- LOAD LISTS ----------
    vPlatform.push_back(new Platform(6, 2, 55*32, 55*32, screen_height - 16 - 9*32, screen_height - 32, 55*32, (float)screen_height - 32, true));
    vPlatform.push_back(new Platform(6, 3, 83*32 - 16, 86*32 + 16, screen_height - 16 - 6*32, screen_height - 16 - 6*32, 83*32 - 16, (float)screen_height - 16 - 6*32, true));
    vPlatform.push_back(new Platform(6, 3, 91*32 - 16, 93*32 + 16, screen_height - 16 - 5*32, screen_height - 16 - 5*32, 93*32 + 16, (float)screen_height - 16 - 5*32, false));
    vPlatform.push_back(new Platform(6, 3, 127*32 - 16, 131*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 127*32 - 16, (float)screen_height - 16 - 8*32, false));
    // ----- CLOUDS
    structCloud(3, 10, 2);
    structCloud(9, 6, 1);
    structCloud(11, 18, 2);
    structCloud(18, 11, 2);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(46, 2, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(66, 11, 2);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(94, 2, 1);
    structCloud(99, 10, 2);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structCloud(142, 2, 1);
    structCloud(147, 10, 2);
    structCloud(153, 6, 1);
    structCloud(162, 11, 2);
    // ----- GND
    structGND(0, 0, 16, 2);
    structGND(129, 0, 47, 2);
    // ----- T
    structT(18, 0, 4, 3);
    structT(24, 0, 8, 6);
    structT(26, 6, 5, 4);
    structT(32, 0, 3, 3);
    structT(35, 0, 5, 7);
    structT(40, 0, 7, 11);
    structT(50, 0, 4, 2);
    structT(59, 0, 5, 2);
    structT(60, 2, 4, 8);
    structT(50, 0, 4, 2);
    structT(65, 0, 5, 2);
    structT(70, 0, 3, 6);
    structT(76, 0, 6, 9);
    structT(98, 0, 4, 4);
    structT(104, 0, 8, 8);
    structT(113, 0, 3, 2);
    structT(116, 0, 4, 6);
    structT(122, 0, 4, 6);
    // ----- GND2
    struckBlockQ(59, 4, 1);
    lMap[59][4]->setSpawnMushroom(true);
    structGND2(138, 2, 2, 4);
    structGND2(140, 2, 2, 6);
    structGND2(142, 2, 2, 8);
    // ----- COINS
    structCoins(27, 10, 3, 1);
    structCoins(33, 3, 1, 1);
    structCoins(37, 12, 2, 1);
    structCoins(50, 8, 2, 1);
    structCoins(60, 10, 4, 1);
    structCoins(85, 9, 2, 1);
    structCoins(93, 10, 2, 1);
    structCoins(97, 10, 2, 1);
    structCoins(113, 2, 3, 1);
    structCoins(120, 9, 2, 1);
    // ----- END
    structGND2(152, 2, 1, 1);
    structEnd(152, 3, 9);
    // ----- CASTLE
    structCastleSmall(0, 2);
    structCastleBig(155, 2);
    structCastleWall(164, 2, 12, 6);
    this->iLevelType = 0;
}
void Map::loadLVL_1_4() {
    clearMap();
    this->iMapWidth = 180;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_1_4();
    vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 136*32 - 16, (float)screen_height - 16 - 8*32, false));
    structGND(0, 0, 3, 8);
    structGND(3, 0, 1, 7);
    structGND(4, 0, 1, 6);
    structGND(5, 0, 8, 5);
    structGND(15, 0, 11, 5);
    structGND(29, 0, 3, 5);
    structGND(35, 0, 37, 6);
    structGND(72, 0, 32, 5);
    structGND(104, 0, 24, 2);
    structGND(116, 2, 4, 3);
    structGND(123, 2, 5, 3);
    structGND(141, 0, 3, 6);
    structGND(144, 0, 19, 2);
    structGND(0, 10, 24, 3);
    structGND(23, 9, 1, 4);
    structGND(24, 12, 13, 1);
    structGND(37, 9, 35, 4);
    structGND(72, 12, 91, 1);
    structGND(80, 11, 1, 1);
    structGND(88, 11, 1, 1);
    structGND(97, 10, 7, 2);
    structGND(123, 10, 5, 2);
    structGND(142, 9, 2, 3);
    structBridge(128, 4, 13);
    structLava(13, 0, 2, 3);
    structLava(26, 0, 3, 2);
    structLava(32, 0, 3, 2);
    structLava(128, 0, 13, 2);
    lMap[23][8]->setBlockID(56);
    lMap[37][8]->setBlockID(56);
    lMap[80][10]->setBlockID(56);
    lMap[92][5]->setBlockID(56);
    lMap[30][4]->setBlockID(56);
    lMap[49][8]->setBlockID(56);
    lMap[60][8]->setBlockID(56);
    lMap[67][8]->setBlockID(56);
    lMap[76][5]->setBlockID(56);
    lMap[84][5]->setBlockID(56);
    lMap[88][10]->setBlockID(56);
    struckBlockQ(30, 8, 1);
    lMap[30][8]->setSpawnMushroom(true);
    struckBlockQ2(106, 5, 1);
    struckBlockQ2(109, 5, 1);
    struckBlockQ2(112, 5, 1);
    struckBlockQ2(107, 9, 1);
    struckBlockQ2(110, 9, 1);
    struckBlockQ2(113, 9, 1);
    this->iLevelType = 3;
}
void Map::loadLVL_2_1() {
    clearMap();
    this->iMapWidth = 365;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    structGND(0, 0, 92, 2);
    structGND(96, 0, 10, 2);
    structGND(109, 0, 30, 2);
    structGND(142, 0, 10, 2);
    structGND(154, 0, 70, 2);
    loadMinionsLVL_2_1();
    loadPipeEventsLVL_2_1();
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structFence(14, 2, 4);
    structFence(38, 2, 2);
    structFence(41, 2, 1);
    structFence(62, 2, 4);
    structFence(86, 2, 2);
    structFence(89, 2, 1);
    structFence(110, 2, 4);
    structFence(134, 2, 2);
    structFence(137, 2, 1);
    structFence(158, 2, 4);
    structFence(182, 2, 2);
    structFence(185, 2, 1);
    structFence(209, 2, 1);
    structPipe(46, 2, 3);
    structPipe(74, 2, 3);
    structPipe(103, 2, 3);
    structPipe(115, 2, 1);
    structPipe(122, 2, 3);
    structPipe(126, 2, 2);
    structPipe(130, 2, 4);
    structPipe(176, 2, 2);
    structCastleBig(-2, 2);
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(88, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(109, 2, 1, true);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(155, 2, 1, false);
    structTree(157, 2, 1, true);
    structTree(165, 2, 1, true);
    structTree(167, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(184, 2, 1, false);
    structTree(187, 2, 1, true);
    structTree(203, 2, 1, false);
    structTree(213, 2, 1, true);
    structTree(215, 2, 1, false);
    structTree(216, 2, 1, false);
    structBrick(15, 5, 3, 1);
    structBrick(28, 9, 4, 1);
    structBrick(68, 5, 1, 1);
    structBrick(69, 9, 4, 1);
    structBrick(81, 9, 5, 1);
    structBrick(92, 9, 4, 1);
    structBrick(125, 9, 4, 1);
    structBrick(161, 5, 1, 1);
    lMap[161][5]->setNumOfUse(7);
    structBrick(164, 9, 5, 1);
    structBrick(172, 9, 1, 1);
    structBrick(185, 5, 2, 1);
    lMap[69][9]->setSpawnStar(true);
    lMap[16][5]->setSpawnMushroom(true);
    lMap[28][9]->setSpawnMushroom(true);
    lMap[28][9]->setPowerUP(false);
    lMap[53][5]->setSpawnMushroom(true);
    lMap[125][9]->setSpawnMushroom(true);
    lMap[172][9]->setSpawnMushroom(true);
    struckBlockQ(53, 5, 5);
    struckBlockQ(53, 9, 5);
    struckBlockQ(79, 5, 4);
    struckBlockQ(85, 5, 3);
    struckBlockQ(170, 5, 1);
    struckBlockQ2(28, 5, 1);
    struckBlockQ2(186, 9, 1);
    structGND2(20,2, 5, true);
    structGND2(34, 2, 1, 4);
    structGND2(35, 2, 1, 2);
    structGND2(154, 2, 1, 3);
    structGND2(190, 2, 2, 10);
    structGND2(200, 2, 1, 1);
    structEnd(200, 3, 9);
    structCastleSmall(204, 2);
    // -- MAP 2-1-2
    this->iLevelType = 1;
    structGND(230, 0, 17, 2);
    structBrick(230, 2, 1, 11);
    structBrick(234, 2, 7, 3);
    structBrick(234, 12, 7, 1);
    structPipeVertical(245, 2, 10);
    structPipeHorizontal(243, 2, 1);
    structCoins(234, 5, 7, 1);
    structCoins(234, 7, 7, 1);
    structCoins(235, 9, 5, 1);
    // -- MAP 2-1-3
    this->iLevelType = 0;
    lMap[83][9]->setBlockID(128);
    structBonus(270, 1, 4);
    structBonus(275, 1, 57);
    lMap[274][0]->setBlockID(130); // VINE
    lMap[274][1]->setBlockID(83);
    structCoins(285, 8, 16, 1);
    structCoins(302, 10, 3, 1);
    structCoins(306, 9, 16, 1);
    structCoins(323, 10, 3, 1);
    structCoins(333, 2, 3, 1);
    structBonusEnd(332);
    vPlatform.push_back(new Platform(6, 4, 286*32 + 16, 335*32 + 16, screen_height - 16 - 4*32, screen_height - 16 - 4*32, 286*32.0f + 16, screen_height - 16.0f - 4*32, true));
    // -- BONUS END
    this->iLevelType = 0;
}
void Map::loadLVL_2_2() {
    clearMap();
    this->iMapWidth = 320;
    this->iMapHeight = 25;
    this->iLevelType = 2;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_2_2();
    loadPipeEventsLVL_2_2();
    this->underWater = true;
    structWater(0, 0, 200, 13);
    structGND(0, 0, 66, 2);
    structGND(71, 0, 60, 2);
    structGND(140, 0, 17, 2);
    structGND(164, 0, 38, 2);
    structGND(64, 2, 1, 3);
    structGND(65, 2, 1, 5);
    structGND(71, 2, 1, 5);
    structGND(72, 2, 1, 3);
    structGND(78, 2, 2, 3);
    structGND(78, 10, 2, 3);
    structGND(82, 9, 3, 1);
    structGND(129, 2, 1, 4);
    structGND(130, 2, 1, 2);
    structGND(141, 2, 1, 4);
    structGND(140, 2, 1, 2);
    structGND(131, 10, 1, 3);
    structGND(132, 10, 8, 1);
    structGND(18, 5, 3, 1);
    structGND(42, 5, 2, 1);
    structGND(102, 5, 2, 1);
    structGND(115, 6, 2, 1);
    structGND(156, 2, 1, 8);
    structGND(157, 9, 2, 1);
    structGND(164, 2, 1, 8);
    structGND(162, 9, 2, 1);
    structGND(172, 5, 5, 1);
    structGND(172, 9, 5, 1);
    structGND(180, 5, 4, 1);
    structGND(180, 9, 4, 1);
    structCoins(14, 2, 2, 1);
    structCoins(27, 9, 3, 1);
    structCoins(36, 2, 3, 1);
    structCoins(67, 4, 3, 1);
    structCoins(101, 3, 3, 1);
    structCoins(113, 8, 3, 1);
    structCoins(133, 3, 1, 1);
    structCoins(134, 2, 3, 1);
    structCoins(137, 3, 1, 1);
    structCoins(159, 5, 3, 1);
    structCoins(159, 2, 3, 1);
    structUW1(11, 2, 1, 3);
    structUW1(33, 2, 1, 5);
    structUW1(42, 6, 1, 2);
    structUW1(50, 2, 1, 4);
    structUW1(83, 10, 1, 2);
    structUW1(89, 2, 1, 3);
    structUW1(102, 6, 1, 4);
    structUW1(120, 2, 1, 4);
    structUW1(147, 2, 1, 2);
    structUW1(149, 2, 1, 3);
    structUW1(173, 10, 1, 2);
    structPipeHorizontal(189, 6, 1);
    structGND(185, 2, 17, 1);
    structGND(186, 3, 16, 1);
    structGND(187, 4, 15, 1);
    structGND(188, 5, 14, 1);
    structGND(190, 6, 12, 3);
    structGND(188, 9, 14, 4);
    // -- MAP 2-2-2
    this->iLevelType = 0;
    structGND(220, 0, 24, 2);
    structCastleSmall(220, 2);
    structCloud(223, 10, 2);
    structCloud(229, 6, 1);
    structCloud(238, 11, 2);
    structPipe(232, 2, 3);
    structPipeHorizontal(230, 2, 1);
    // -- MAP 2-2-3
    structCloud(275, 11, 2);
    structCloud(295, 10, 1);
    structCloud(306, 11, 1);
    structGrass(300, 2, 1);
    structGrass(310, 2, 1);
    structBush(287, 2, 2);
    structBush(303, 2, 1);
    structGND(271, 0, 42, 2);
    structPipe(274, 2, 1);
    structGND2(276, 2, 8, true);
    structGND2(284, 2, 1, 8);
    structGND2(293, 2, 1, 1);
    structEnd(293, 3, 9);
    structCastleSmall(297, 2);
    // -- END
    this->iLevelType = 2;
}
void Map::loadLVL_2_3() {
    clearMap();
    this->iMapWidth = 250;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_2_3();
    structGND(0, 0, 7, 2);
    structGND(207, 0, 43, 2);
    structCloud(3, 10, 2);
    structCloud(9, 6, 1);
    structCloud(18, 11, 2);
    structCloud(28, 2, 1);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(46, 2, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(66, 11, 2);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(94, 2, 1);
    structCloud(99, 10, 2);
    structCloud(105, 6, 1);
    structCloud(114, 11, 2);
    structCloud(124, 2, 1);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structCloud(142, 2, 1);
    structCloud(147, 10, 2);
    structCloud(153, 6, 1);
    structCloud(162, 11, 2);
    structCloud(172, 2, 1);
    structCloud(179, 6, 1);
    structCloud(182, 7, 1);
    structCloud(190, 2, 1);
    structCloud(195, 10, 2);
    structCloud(201, 6, 1);
    structCloud(210, 11, 2);
    structCloud(220, 2, 1);
    structCloud(227, 6, 1);
    structCloud(239, 9, 2);
    structT(8, 0, 8, 2);
    structGND2(10, 2, 3, true);
    structGND2(13, 2, 2, 3);
    structBridge2(15, 4, 16);
    structBridge2(32, 4, 15);
    structBridge2(48, 4, 15);
    structBridge2(69, 4, 10);
    structBridge2(85, 4, 10);
    structBridge2(100, 5, 5);
    structT(112, 0, 8, 2);
    structBridge2(122, 4, 3);
    structBridge2(128, 4, 15);
    structBridge2(147, 2, 8);
    structBridge2(160, 4, 8);
    structBridge2(171, 4, 2);
    structBridge2(175, 4, 2);
    structBridge2(179, 4, 2);
    structBridge2(184, 4, 9);
    structGND2(31, 0, 1, 5);
    structGND2(47, 0, 1, 5);
    structGND2(63, 0, 1, 5);
    structGND2(68, 0, 1, 5);
    structGND2(79, 0, 1, 5);
    structGND2(84, 0, 1, 5);
    structGND2(95, 0, 1, 5);
    structGND2(99, 0, 1, 6);
    structGND2(105, 0, 1, 6);
    structGND2(127, 0, 1, 5);
    structGND2(143, 0, 1, 5);
    structGND2(146, 0, 1, 3);
    structGND2(155, 0, 1, 3);
    structGND2(159, 0, 1, 5);
    structGND2(168, 0, 1, 5);
    structGND2(183, 0, 1, 5);
    structT(192, 0, 13, 2);
    structGND2(193, 2, 1, 3);
    structGND2(194, 2, 3, false);
    structGND2(208, 2, 8, true);
    structGND2(216, 2, 1, 8);
    structCoins(36, 9, 4, 1);
    structCoins(55, 9, 1, 1);
    structCoins(57, 9, 1, 1);
    structCoins(59, 9, 1, 1);
    structCoins(56, 8, 1, 1);
    structCoins(58, 8, 1, 1);
    structCoins(72, 9, 1, 1);
    structCoins(73, 10, 2, 1);
    structCoins(75, 9, 1, 1);
    structCoins(97, 9, 3, 1);
    structCoins(108, 9, 3, 1);
    structCoins(133, 9, 6, 1);
    structCoins(149, 6, 4, 1);
    structCoins(173, 8, 6, 1);
    struckBlockQ(102, 9, 1);
    lMap[102][9]->setSpawnMushroom(true);
    structCastleSmall(0, 2);
    structGND2(225, 2, 1, 1);
    structEnd(225, 3, 9);
    structCastleBig(228, 2);
    structCastleWall(237, 2, 13, 6);
    // -- END
    this->iLevelType = 0;
}
void Map::loadLVL_2_4() {
    clearMap();
    this->iMapWidth = 190;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    // ----- MINIONS
    loadMinionsLVL_2_4();
    vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, screen_height + 32, 85*32 + 24, 9*32 + 16, true));
    vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, screen_height + 32, 85*32 + 24, 1*32 + 16, true));
    vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, screen_height + 32, 88*32 + 24, 10*32, true));
    vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, screen_height + 32, 88*32 + 24, 2*32, true));
    vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 136*32 - 16, (float)screen_height - 16 - 8*32, false));
    structGND(32, 0, 52, 2);
    structGND(92, 0, 17, 2);
    structGND(115, 0, 13, 2);
    structGND(141, 0, 40, 2);
    structGND(0, 0, 16, 5);
    structGND(0, 5, 5, 1);
    structGND(0, 6, 4, 1);
    structGND(0, 7, 3, 1);
    structGND(18, 5, 2, 1);
    structGND(22, 7, 3, 1);
    structGND(27, 5, 2, 1);
    structGND(32, 2, 2, 3);
    structGND(37, 5, 37, 1);
    structGND(80, 2, 4, 3);
    structGND(92, 2, 7, 3);
    structGND(108, 2, 1, 3);
    structGND(111, 0, 2, 5);
    structGND(115, 2, 5, 3);
    structGND(122, 2, 2, 3);
    structGND(126, 2, 2, 3);
    structGND(141, 2, 3, 4);
    structGND(0, 10, 16, 3);
    structGND(34, 9, 49, 4);
    structGND(93, 10, 6, 3);
    structGND(115, 10, 13, 3);
    structGND(128, 12, 42, 1);
    structGND(142, 9, 2, 3);
    lMap[23][7]->setBlockID(56);
    lMap[43][1]->setBlockID(56);
    lMap[49][5]->setBlockID(56);
    lMap[55][9]->setBlockID(56);
    lMap[55][1]->setBlockID(56);
    lMap[61][5]->setBlockID(56);
    lMap[67][1]->setBlockID(56);
    lMap[73][5]->setBlockID(56);
    lMap[82][8]->setBlockID(56);
    lMap[92][4]->setBlockID(56);
    lMap[103][3]->setBlockID(56);
    structCoins(102, 2, 3, 1);
    structCoins(102, 6, 3, 1);
    struckBlockQ(23, 11, 1);
    lMap[23][11]->setSpawnMushroom(true);
    structBridge(128, 4, 13);
    structBrick(128, 9, 6, 1);
    structLava(16, 0, 16, 2);
    structLava(109, 0, 2, 2);
    structLava(113, 0, 2, 2);
    structLava(128, 0, 13, 2);
    structPlatformLine(86);
    structPlatformLine(89);
    this->iLevelType = 3;
}
void Map::loadLVL_3_1() {
    clearMap();
    this->iMapWidth = 375;
    this->iMapHeight = 25;
    this->iLevelType = 4;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_3_1();
    loadPipeEventsLVL_3_1();
    structGND(0, 0, 45, 2);
    structGND(48, 0, 29, 2);
    structGND(85, 0, 1, 2);
    structGND(88, 0, 40, 2);
    structGND(132, 0, 10, 2);
    structGND(144, 0, 33, 2);
    structGND(180, 0, 42, 2);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structCastleBig(-2, 2);
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(23, 2, 1, false);
    structTree(24, 2, 1, false);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(107, 2, 1, false);
    structTree(109, 2, 1, true);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(155, 2, 1, false);
    structTree(157, 2, 1, true);
    structTree(165, 2, 1, true);
    structTree(167, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(203, 2, 1, false);
    structTree(213, 2, 1, true);
    structTree(215, 2, 1, false);
    structTree(216, 2, 1, false);
    structFence(14, 2, 4);
    structFence(41, 2, 1);
    structFence(62, 2, 4);
    structFence(110, 2, 4);
    structFence(134, 2, 2);
    structFence(158, 2, 4);
    structFence(182, 2, 1);
    structFence(209, 2, 1);
    structGND2(73, 2, 4, true);
    structBridge2(77, 5, 8);
    structGND2(85, 2, 1, 4);
    structGND2(88, 2, 1, 4);
    structGND2(89, 2, 1, 2);
    structWater(77, 0, 8, 3);
    structWater(86, 0, 2, 3);
    structGND2(136, 2, 6, true);
    structGND2(174, 2, 1, 3);
    structGND2(175, 2, 1, 6);
    structGND2(183, 2, 8, true);
    structGND2(191, 2, 1, 8);
    structPipe(32, 2, 2);
    structPipe(38, 2, 3);
    structPipe(57, 2, 2);
    structPipe(67, 2, 1);
    structPipe(103, 2, 3);
    structBrick(26, 5, 3, 1);
    structBrick(61, 5, 1, 1);
    structBrick(90, 9, 3, 1);
    lMap[90][9]->setSpawnStar(true);
    structBrick(111, 5, 12, 1);
    structBrick(111, 9, 11, 1);
    structBrick(129, 6, 3, 1);
    structBrick(129, 9, 3, 1);
    structBrick(150, 5, 3, 1);
    structBrick(155, 5, 3, 1);
    structBrick(150, 9, 3, 1);
    structBrick(155, 9, 3, 1);
    structBrick(166, 5, 5, 1);
    lMap[167][5]->setNumOfUse(8);
    struckBlockQ(16, 5, 1);
    struckBlockQ(19, 6, 1);
    struckBlockQ(22, 6, 1);
    lMap[22][6]->setSpawnMushroom(true);
    struckBlockQ(113, 9, 1);
    struckBlockQ(117, 9, 1);
    lMap[117][9]->setSpawnMushroom(true);
    struckBlockQ(151, 5, 1);
    struckBlockQ(156, 5, 1);
    struckBlockQ(151, 9, 1);
    struckBlockQ(156, 9, 1);
    lMap[156][5]->setSpawnMushroom(true);
    struckBlockQ2(82, 9, 1);
    lMap[82][9]->setSpawnMushroom(true);
    lMap[82][9]->setPowerUP(false);
    structGND2(200, 2, 1, 1);
    structEnd(200, 3, 9);
    structCastleSmall(204, 2);
    // -- MAP 3-1-2
    this->iLevelType = 1;
    structGND(240, 0, 17, 2);
    structBrick(240, 2, 1, 11);
    structBrick(243, 6, 1, 2);
    structBrick(243, 8, 2, 1);
    structBrick(243, 9, 3, 1);
    structBrick(252, 6, 1, 2);
    structBrick(251, 8, 2, 1);
    structBrick(250, 9, 3, 1);
    structBrick(244, 5, 1, 1);
    structBrick(245, 6, 1, 1);
    structBrick(246, 7, 1, 1);
    structBrick(251, 5, 1, 1);
    structBrick(250, 6, 1, 1);
    structBrick(249, 7, 1, 1);
    structBrick(247, 8, 2, 2);
    structCoins(244, 6, 1, 1);
    structCoins(245, 7, 1, 1);
    structCoins(246, 8, 1, 1);
    structCoins(251, 6, 1, 1);
    structCoins(250, 7, 1, 1);
    structCoins(249, 8, 1, 1);
    structCoins(246, 10, 4, 1);
    structCoins(247, 11, 2, 1);
    lMap[245][9]->setSpawnMushroom(true);
    structPipeVertical(255, 2, 10);
    structPipeHorizontal(253, 2, 1);
    // -- MAP 3-1-3 BONUS
    this->iLevelType = 4;
    lMap[131][9]->setBlockID(128);
    structBonus(270, 1, 4);
    lMap[274][0]->setBlockID(131); // VINE
    lMap[274][1]->setBlockID(83);
    structBonus(275, 1, 78);
    structBonus(301, 6, 1);
    structBonus(320, 6, 1);
    structBonus(320, 7, 1);
    structBonus(331, 6, 1);
    structBonus(331, 7, 1);
    structBonus(337, 8, 2);
    structBonus(341, 8, 1);
    structBonus(343, 8, 1);
    structBonus(345, 8, 1);
    structBonus(347, 8, 1);
    structBonus(349, 8, 1);
    structCoins(284, 8, 16, 1);
    structCoins(303, 8, 16, 1);
    structCoins(322, 9, 8, 1);
    structCoins(341, 9, 10, 1);
    structCoins(355, 3, 3, 1);
    structBonusEnd(353);
    vPlatform.push_back(new Platform(6, 4, 286*32 + 16, 355*32 + 16, screen_height - 16 - 4*32, screen_height - 16 - 4*32, 286*32.0f + 16, screen_height - 16.0f - 4*32, true));
    // -- END
    this->iLevelType = 4;
}
void Map::loadLVL_3_2() {
    clearMap();
    this->iMapWidth = 230;
    this->iMapHeight = 25;
    this->iLevelType = 4;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_3_2();
    structGND(0, 0, 80, 2);
    structGND(82, 0, 41, 2);
    structGND(125, 0, 3, 2);
    structGND(130, 0, 100, 2);
    structCloud(0, 10, 2);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structCloud(222, 10, 1);
    structFence(14, 2, 4);
    structFence(38, 2, 2);
    structFence(41, 2, 1);
    structFence(62, 2, 4);
    structFence(86, 2, 2);
    structFence(89, 2, 1);
    structFence(110, 2, 4);
    structFence(134, 2, 2);
    structFence(137, 2, 1);
    structFence(158, 2, 4);
    structFence(182, 2, 2);
    structFence(185, 2, 1);
    structFence(206, 2, 3);
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(23, 2, 1, false);
    structTree(24, 2, 1, false);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(88, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(107, 2, 1, false);
    structTree(109, 2, 1, true);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(139, 2, 1, true);
    structTree(155, 2, 1, false);
    structTree(157, 2, 1, true);
    structTree(165, 2, 1, true);
    structTree(167, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(184, 2, 1, false);
    structTree(187, 2, 1, true);
    structTree(203, 2, 1, false);
    structTree(205, 2, 1, true);
    structGND2(49, 2, 1, 1);
    structGND2(60, 2, 1, 3);
    structGND2(75, 2, 1, 2);
    structGND2(79, 2, 1, 2);
    structGND2(126, 2, 1, 2);
    structGND2(192, 2, 8, true);
    structGND2(200, 2, 1, 8);
    structCoins(55, 5, 3, 1);
    structCoins(168, 8, 4, 1);
    struckBlockQ(60, 8, 1);
    lMap[60][8]->setSpawnMushroom(true);
    structBrick(77, 5, 1, 1);
    lMap[77][5]->setNumOfUse(8);
    structBrick(77, 9, 1, 1);
    lMap[77][9]->setSpawnStar(true);
    structBrick(126, 8, 1, 1);
    structPipe(169, 2, 2);
    structGND2(209, 2, 1, 1);
    structEnd(209, 3, 9);
    structCastleSmall(0, 2);
    structCastleSmall(213, 2);
}
void Map::loadLVL_3_3() {
    clearMap();
    this->iMapWidth = 180;
    this->iMapHeight = 25;
    this->iLevelType = 4;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_3_3();
    structGND(0, 0, 16, 2);
    structGND(144, 0, 31, 2);
    structCloud(3, 10, 2);
    structCloud(9, 6, 1);
    structCloud(18, 11, 2);
    structCloud(28, 2, 1);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(94, 2, 1);
    structCloud(99, 10, 2);
    structCloud(105, 6, 1);
    structCloud(114, 11, 1);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structCloud(142, 2, 1);
    structCloud(147, 10, 2);
    structCloud(153, 6, 1);
    structCloud(162, 11, 2);
    structT(18, 0, 5, 5);
    structT(22, 0, 6, 8);
    structT(30, 0, 3, 2);
    structT(36, 0, 7, 3);
    structT(43, 0, 4, 6);
    structT(47, 0, 6, 8);
    structT(46, 0, 10, 4);
    structT(55, 0, 4, 12);
    structT(65, 0, 3, 8);
    structT(69, 0, 3, 8);
    structT(73, 0, 3, 8);
    structT(77, 0, 3, 11);
    structT(65, 0, 16, 2);
    structT(84, 0, 4, 4);
    structT(97, 0, 3, 6);
    structT(104, 0, 4, 10);
    structT(108, 0, 3, 8);
    structT(107, 0, 5, 4);
    structT(116, 0, 3, 2);
    structT(119, 0, 12, 5);
    structCoins(31, 2, 2, 1);
    structCoins(37, 8, 1, 1);
    structCoins(42, 8, 3, 1);
    structCoins(52, 5, 3, 1);
    structCoins(56, 12, 2, 1);
    structCoins(66, 8, 1, 1);
    structCoins(70, 8, 1, 1);
    structCoins(74, 8, 1, 1);
    structCoins(78, 11, 1, 1);
    structCoins(81, 9, 1, 1);
    structCoins(88, 9, 1, 1);
    structCoins(90, 9, 1, 1);
    structCoins(105, 10, 2, 1);
    structCoins(108, 4, 1, 1);
    struckBlockQ(49, 11, 1);
    lMap[49][11]->setSpawnMushroom(true);
    structCastleSmall(0, 2);
    structCastleBig(154, 2);
    structCastleWall(163, 2, 10, 6);
    this->iLevelType = 0;
    structGND2(151, 2, 1, 1);
    structEnd(151, 3, 9);
    // -- SEESAW
    structSeeSaw(82, 12, 8);
    vPlatform.push_back(new Platform(6, 6, 81*32, 81*32, screen_height - 16, screen_height - 16 - 11*32, 81*32, (float)screen_height - 16 - 8*32, true, 1));
    vPlatform.push_back(new Platform(6, 6, 88*32, 88*32, screen_height - 16, screen_height - 16 - 11*32, 88*32, (float)screen_height - 16 - 6*32, true, 0));
    structSeeSaw(137, 12, 5);
    vPlatform.push_back(new Platform(6, 6, 136*32, 136*32, screen_height - 16, screen_height - 16 - 11*32, 136*32, (float)screen_height - 16 - 9*32, true, 3));
    vPlatform.push_back(new Platform(6, 6, 140*32, 140*32, screen_height - 16, screen_height - 16 - 11*32, 140*32, (float)screen_height - 16 - 6*32, true, 2));
    // -- SEESAW
    vPlatform.push_back(new Platform(6, 3, 27*32 - 16, 30*32 + 16, screen_height - 16 - 10*32, screen_height - 16 - 10*32, 27*32 - 16, (float)screen_height - 16 - 10*32, false));
    vPlatform.push_back(new Platform(6, 3, 30*32 - 16, 33*32 + 16, screen_height - 16 - 6*32, screen_height - 16 - 6*32, 33*32 + 16, (float)screen_height - 16 - 6*32, true));
    vPlatform.push_back(new Platform(6, 3, 91*32 - 32, 94*32, screen_height - 16 - 5*32, screen_height - 16 - 5*32, 94*32, (float)screen_height - 16 - 5*32, true));
    vPlatform.push_back(new Platform(6, 3, 92*32, 96*32, screen_height - 16 - 9*32, screen_height - 16 - 9*32, 92*32, (float)screen_height - 16 - 9*32, false));
    vPlatform.push_back(new Platform(6, 3, 100*32 - 16, 103*32 + 16, screen_height - 16 - 3*32, screen_height - 16 - 3*32, 103*32 + 16, (float)screen_height - 16 - 3*32, true));
    vPlatform.push_back(new Platform(6, 3, 129*32 - 16, 132*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 132*32 + 16, (float)screen_height - 16 - 8*32, true));
    vPlatform.push_back(new Platform(6, 5, 60*32, 60*32, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 60*32, (float)screen_height - 16 - 8*32, true));
    this->iLevelType = 4;
}
void Map::loadLVL_3_4() {
    clearMap();
    this->iMapWidth = 225;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_3_4();
    vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 136*32 - 16, (float)screen_height - 16 - 8*32, false));
    structGND(0, 0, 16, 5);
    structGND(0, 5, 5, 1);
    structGND(0, 6, 4, 1);
    structGND(0, 7, 3, 1);
    structGND(18, 0, 3, 5);
    structGND(23, 0, 3, 5);
    structGND(28, 0, 3, 5);
    structGND(33, 0, 2, 5);
    structGND(33, 0, 13, 2);
    structGND(53, 2, 3, 1);
    structGND(53, 10, 3, 2);
    structGND(63, 2, 3, 1);
    structGND(63, 10, 3, 2);
    structGND(79, 2, 3, 1);
    structGND(79, 10, 3, 2);
    structGND(90, 0, 6, 5);
    structGND(99, 0, 3, 5);
    structGND(105, 0, 3, 5);
    structGND(111, 10, 15, 2);
    structGND(111, 0, 5, 5);
    structGND(116, 0, 2, 2);
    structGND(118, 0, 10, 5);
    structGND(141, 0, 3, 6);
    structGND(142, 9, 2, 3);
    structGND(0, 10, 16, 3);
    structGND(16, 12, 149, 1);
    structGND(35, 10, 11, 2);
    structGND(90, 10, 6, 2);
    structGND(144, 0, 26, 2);
    structGND(48, 0, 40, 2);
    structLava(46, 0, 2, 2);
    structLava(88, 0, 2, 2);
    structLava(96, 0, 3, 2);
    structLava(102, 0, 3, 2);
    structLava(108, 0, 3, 2);
    structBrick(132, 9, 2, 3);
    structLava(128, 0, 13, 2);
    structBridge(128, 4, 13);
    lMap[19][3]->setBlockID(56);
    lMap[24][3]->setBlockID(56);
    lMap[29][3]->setBlockID(56);
    lMap[54][9]->setBlockID(56);
    lMap[54][3]->setBlockID(56);
    lMap[64][3]->setBlockID(56);
    lMap[64][9]->setBlockID(56);
    lMap[80][3]->setBlockID(56);
    lMap[80][9]->setBlockID(56);
    struckBlockQ(42, 5, 3);
    lMap[43][5]->setSpawnMushroom(true);
    structCoins(81, 8, 3, 1);
}
void Map::loadLVL_4_1() {
    clearMap();
    this->iMapWidth = 325;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_4_1();
    loadPipeEventsLVL_4_1();
    structGND(0, 0, 32, 2);
    structGND(34, 0, 44, 2);
    structGND(82, 0, 67, 2);
    structGND(151, 0, 23, 2);
    structGND(177, 0, 3, 2);
    structGND(182, 0, 8, 2);
    structGND(192, 0, 58, 2);
    structCloud(8, 10, 1);
    structCloud(19, 11, 1);
    structCloud(27, 10, 3);
    structCloud(36, 11, 2);
    structCloud(56, 10, 1);
    structCloud(67, 11, 1);
    structCloud(75, 10, 3);
    structCloud(85, 11, 2);
    structCloud(104, 10, 1);
    structCloud(115, 11, 1);
    structCloud(123, 10, 3);
    structCloud(132, 11, 2);
    structCloud(152, 10, 1);
    structCloud(163, 11, 1);
    structCloud(171, 10, 3);
    structCloud(180, 11, 2);
    structCloud(200, 10, 1);
    structCloud(211, 11, 1);
    structCloud(219, 10, 3);
    structCloud(228, 11, 2);
    structGrass(11, 2, 3);
    structGrass(23, 2, 1);
    structGrass(41, 2, 2);
    structGrass(59, 2, 3);
    structGrass(71, 2, 1);
    structGrass(89, 2, 2);
    structGrass(107, 2, 3);
    structGrass(119, 2, 1);
    structGrass(137, 2, 2);
    structGrass(155, 2, 3);
    structGrass(167, 2, 1);
    structGrass(185, 2, 2);
    structGrass(203, 2, 3);
    structGrass(215, 2, 1);
    structGrass(233, 2, 2);
    structBush(16, 2, 1);
    structBush(48, 2, 2);
    structBush(64, 2, 1);
    structBush(96, 2, 2);
    structBush(112, 2, 1);
    structBush(144, 2, 2);
    structBush(160, 2, 1);
    structBush(192, 2, 2);
    structPipe(21, 2, 2);
    structPipe(116, 2, 3);
    structPipe(132, 2, 3);
    structPipe(163, 2, 1);
    struckBlockQ(25, 5, 1);
    lMap[25][5]->setSpawnMushroom(true);
    struckBlockQ(25, 9, 1);
    struckBlockQ(64, 5, 1);
    struckBlockQ(64, 9, 1);
    struckBlockQ(66, 5, 1);
    struckBlockQ(66, 9, 1);
    struckBlockQ(90, 5, 4);
    struckBlockQ2(92, 9, 1);
    lMap[92][9]->setSpawnMushroom(true);
    lMap[92][9]->setPowerUP(false);
    struckBlockQ(146, 5, 3);
    struckBlockQ(151, 5, 3);
    struckBlockQ(148, 9, 4);
    lMap[148][5]->setSpawnMushroom(true);
    structBrick(149, 5, 2, 1);
    structBrick(220, 5, 1, 1);
    lMap[220][5]->setNumOfUse(6);
    structCoins(41, 5, 1, 1);
    structCoins(42, 6, 2, 1);
    structCoins(44, 5, 1, 1);
    structCoins(105, 8, 4, 1);
    structCoins(119, 8, 4, 1);
    structCoins(135, 8, 4, 1);
    structGND2(103, 2, 1, 3);
    structGND2(189, 2, 1, 3);
    structGND2(208, 2, 8, true);
    structGND2(216, 2, 1, 8);
    structCastleBig(-2, 2);
    structCastleSmall(229, 2);
    structGND2(225, 2, 1, 1);
    structEnd(225, 3, 9);
    // -- MAP 4-1-2
    this->iLevelType = 1;
    structGND(250, 0, 17, 2);
    structBrick(250, 2, 1, 11);
    structBrick(253, 3, 1, 3);
    structBrick(260, 3, 1, 3);
    structBrick(254, 5, 6, 1);
    structBrick(253, 12, 12, 1);
    structCoins(253, 2, 10, 1);
    structCoins(253, 6, 8, 1);
    structBrick(263, 5, 1, 1);
    lMap[263][5]->setSpawnMushroom(true);
    structPipeVertical(265, 2, 10);
    structPipeHorizontal(263, 2, 1);
    // -- END
    this->iLevelType = 0;
}
void Map::loadLVL_4_2() {
    clearMap();
    this->iMapWidth = 435;
    this->iMapHeight = 25;
    this->iLevelType = 1;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    // ---------- LOAD LISTS ----------
    // ----- MINIONS
    loadMinionsLVL_4_2();
    // ----- PIPEEVENTS
    loadPipeEventsLVL_4_2();
    structGND(0, 0, 11, 2);
    structGND(13, 0, 2, 2);
    structGND(17, 0, 1, 2);
    structGND(21, 0, 36, 2);
    structGND(63, 0, 42, 2);
    structGND(107, 0, 2, 2);
    structGND(111, 0, 2, 2);
    structGND(119, 0, 4, 2);
    structGND(129, 0, 15, 2);
    structGND(146, 0, 9, 2);
    structGND(160, 0, 23, 2);
    structGND(185, 0, 49, 2);
    structBrick(0, 2, 1, 11);
    structBrick(6, 12, 14, 1);
    structBrick(22, 3, 5, 3);
    structBrick(27, 5, 2, 1);
    lMap[28][5]->setSpawnMushroom(true);
    structBrick(30, 2, 18, 4);
    structBrick(20, 9, 28, 4);
    structBrick(48, 12, 9, 1);
    lMap[43][10]->setNumOfUse(8);
    lMap[77][5]->setNumOfUse(6);
    lMap[81][5]->setSpawnStar(true);
    structBrick(65, 9, 2, 1);
    lMap[64][9]->setBlockID(129);
    structBrick(76, 5, 2, 1);
    structBrick(80, 5, 2, 1);
    structBrick(87, 6, 1, 1);
    structBrick(119, 5, 4, 1);
    structBrick(119, 9, 3, 1);
    structBrick(119, 12, 4, 1);
    structBrick(67, 12, 45, 1);
    structBrick(128, 12, 27, 1);
    structBrick(161, 5, 11, 1);
    lMap[161][5]->setSpawnMushroom(true);
    lMap[120][9]->setSpawnMushroom(true);
    structBrick(160, 7, 1, 6);
    structBrick(161, 9, 15, 4);
    structBrick(176, 12, 13, 1);
    structBrick(185, 2, 6, 3);
    structBrick(191, 2, 18, 11);
    structBrick(209, 12, 10, 1);
    structBrick(222, 2, 12, 18);
    struckBlockQ2(63, 6, 1);
    struckBlockQ2(64, 7, 1);
    struckBlockQ2(65, 6, 1);
    struckBlockQ2(66, 5, 1);
    struckBlockQ(50, 5, 2);
    struckBlockQ(54, 5, 3);
    struckBlockQ(50, 9, 2);
    lMap[55][5]->setSpawnMushroom(true);
    structGND2(103, 2, 1, 2);
    structGND2(104, 2, 1, 3);
    structGND2(111, 2, 2, 3);
    structGND2(152, 2, 3, true);
    structGND2(173, 2, 4, true);
    structPipe(72, 2, 2);
    structPipe(78, 2, 6);
    structPipe(84, 2, 2);
    structPipe(89, 2, 2);
    structPipe(107, 2, 3);
    structPipe(131, 2, 1);
    structPipe(138, 2, 2);
    structPipe(142, 2, 3);
    structPipe(180, 2, 6);
    structPipe(214, 2, 2);
    structPipeVertical(189, 5, 7);
    structPipeHorizontal(187, 5, 1);
    structCoins(27, 2, 3, 1);
    structCoins(162, 6, 10, 1);
    vPlatform.push_back(new Platform(6, 0, 58*32 + 16, 58*32 + 16, -32, screen_height + 32, 58*32 + 16, 9*32 + 16, true));
    vPlatform.push_back(new Platform(6, 0, 58*32 + 16, 58*32 + 16, -32, screen_height + 32, 58*32 + 16, 1*32 + 16, true));
    vPlatform.push_back(new Platform(6, 0, 114*32 + 16, 114*32 + 16, -32, screen_height + 32, 114*32 + 16, 9*32 + 16, true));
    vPlatform.push_back(new Platform(6, 0, 114*32 + 16, 114*32 + 16, -32, screen_height + 32, 114*32 + 16, 1*32 + 16, true));
    vPlatform.push_back(new Platform(6, 1, 124*32, 124*32, -32, screen_height + 32, 124*32, 10*32, true));
    vPlatform.push_back(new Platform(6, 1, 124*32, 124*32, -32, screen_height + 32, 124*32, 2*32, true));
    vPlatform.push_back(new Platform(6, 0, 156*32, 156*32, -32, screen_height + 32, 156*32, 9*32 + 16, true));
    vPlatform.push_back(new Platform(6, 0, 156*32, 156*32, -32, screen_height + 32, 156*32, 1*32 + 16, true));
    vLevelText.push_back(new LevelText(210*32, screen_height - 16 - 8*32, "WELCOME TO WARP ZONEz"));
    vLevelText.push_back(new LevelText(214*32 + 16, screen_height - 6*32, "5"));
    // -- MAP 4-2-2
    this->iLevelType = 0;
    structGND(240, 0, 24, 2);
    structCastleSmall(240, 2);
    structCloud(243, 10, 2);
    structCloud(249, 6, 1);
    structCloud(258, 11, 2);
    structPipe(252, 2, 3);
    structPipeHorizontal(250, 2, 1);
    for(int i = 13; i < 19; i++) {
        lMap[285][i]->setBlockID(83);
    }
    // -- MAP 4-2-3
    this->iLevelType = 1;
    structGND(270, 0, 17, 2);
    structBrick(269, 2, 1, 18);
    structBrick(270, 2, 1, 11);
    structBrick(274, 7, 1, 2);
    structBrick(275, 7, 5, 1);
    structBrick(280, 7, 1, 6);
    structBrick(281, 8, 2, 1);
    structBrick(274, 12, 6, 1);
    structBrick(284, 7, 1, 1);
    lMap[284][7]->setNumOfUse(8);
    structCoins(275, 8, 5, 2);
    structPipeVertical(285, 2, 10);
    structPipeHorizontal(283, 2, 1);
    // -- MAP 4-2-4
    this->iLevelType = 0;
    structCloud(308, 2, 1);
    structCloud(313, 10, 2);
    structCloud(319, 6, 1);
    structCloud(328, 11, 2);
    structCloud(338, 2, 1);
    structCloud(345, 6, 1);
    structCloud(348, 7, 1);
    structCloud(361, 10, 2);
    structCloud(367, 6, 1);
    structGND(310, 0, 4, 2);
    structGND(315, 0, 59, 2);
    structTMush(322, 2, 3, 4);
    structTMush(326, 2, 3, 8);
    structTMush(328, 2, 3, 2);
    structTMush(332, 2, 3, 8);
    structTMush(332, 2, 5, 2);
    structTMush(336, 2, 3, 6);
    structTMush(340, 2, 5, 9);
    structTMush(341, 2, 7, 3);
    structGND2(350, 2, 9, true);
    structGND2(359, 10, 11, 1);
    structGND2(372, 2, 2, 11);
    structCoins(322, 6, 3, 1);
    structCoins(326, 10, 3, 1);
    structCoins(332, 10, 3, 1);
    structCoins(336, 8, 3, 1);
    structCoins(340, 11, 5, 1);
    structCoins(345, 5, 2, 1);
    vLevelText.push_back(new LevelText(360*32, screen_height - 16 - 8*32, "WELCOME TO WARP ZONEz"));
    vLevelText.push_back(new LevelText(360*32 + 16, screen_height - 6*32, "8"));
    vLevelText.push_back(new LevelText(364*32 + 16, screen_height - 6*32, "7"));
    vLevelText.push_back(new LevelText(368*32 + 16, screen_height - 6*32, "6"));
    for(int i = 0; i < 19; i++) {
        lMap[374][i]->setBlockID(83);
    }
    this->iLevelType = 5;
    structPipe(360, 2, 2);
    structPipe(364, 2, 2);
    structPipe(368, 2, 2);
    lMap[314][0]->setBlockID(130);
    lMap[314][1]->setBlockID(83);
    // -- MAP 4-2-5
    this->iLevelType = 0;
    for(int i = 0; i < 19; i++) {
        lMap[390][i]->setBlockID(83);
    }
    structCloud(395, 11, 2);
    structCloud(415, 10, 1);
    structCloud(426, 11, 1);
    structGrass(420, 2, 1);
    structGrass(420, 2, 1);
    structBush(417, 2, 2);
    structBush(423, 2, 1);
    structGND(391, 0, 42, 2);
    structPipe(394, 2, 1);
    structGND2(396, 2, 8, true);
    structGND2(404, 2, 1, 8);
    structGND2(413, 2, 1, 1);
    structEnd(413, 3, 9);
    structCastleSmall(417, 2);
    // -- END
    this->iLevelType = 1;
}
void Map::loadLVL_4_3() {
    clearMap();
    this->iMapWidth = 170;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    // ----- MINIONS
    loadMinionsLVL_4_3();
    structGND(0, 0, 15, 2);
    structGND(141, 0, 28, 2);
    structCloud(3, 10, 2);
    structCloud(9, 6, 1);
    structCloud(18, 11, 2);
    structCloud(28, 2, 1);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(46, 2, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(66, 11, 2);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(84, 2, 1);
    structCloud(99, 10, 2);
    structCloud(105, 6, 1);
    structCloud(114, 11, 2);
    structCloud(124, 2, 1);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structTMush(16, 0, 5, 2);
    structTMush(19, 0, 5, 10);
    structTMush(23, 0, 7, 6);
    structTMush(32, 0, 3, 11);
    structTMush(39, 0, 5, 10);
    structTMush(36, 0, 7, 3);
    structTMush(44, 0, 3, 6);
    structTMush(51, 0, 3, 7);
    structTMush(67, 0, 3, 8);
    structTMush(65, 0, 5, 2);
    structTMush(70, 0, 3, 12);
    structTMush(72, 0, 3, 6);
    structTMush(74, 0, 5, 10);
    structTMush(84, 0, 3, 6);
    structTMush(99, 0, 3, 4);
    structTMush(105, 0, 3, 5);
    structTMush(113, 0, 5, 6);
    structTMush(117, 0, 3, 9);
    structTMush(121, 0, 7, 2);
    structTMush(130, 0, 5, 5);
    structCoins(20, 10, 3, 1);
    structCoins(24, 6, 5, 1);
    structCoins(38, 3, 3, 1);
    structCoins(48, 7, 1, 1);
    structCoins(57, 11, 1, 1);
    structCoins(67, 8, 3, 1);
    structCoins(73, 6, 1, 1);
    structCoins(70, 12, 3, 1);
    structCoins(96, 7, 1, 1);
    structCoins(104, 10, 1, 1);
    structCoins(113, 6, 5, 1);
    struckBlockQ(43, 12, 1);
    lMap[43][12]->setSpawnMushroom(true);
    // -- SEESAW
    structSeeSaw(49, 12, 8);
    vPlatform.push_back(new Platform(6, 6, 48*32, 48*32, screen_height - 16, screen_height - 16 - 11*32, 48*32, (float)screen_height - 16 - 9*32, true, 1));
    vPlatform.push_back(new Platform(6, 6, 55*32, 55*32, screen_height - 16, screen_height - 16 - 11*32, 55*32, (float)screen_height - 16 - 6*32, true, 0));
    structSeeSaw(81, 12, 9);
    vPlatform.push_back(new Platform(6, 6, 80*32, 80*32, screen_height - 16, screen_height - 16 - 11*32, 80*32, (float)screen_height - 16 - 9*32, true, 3));
    vPlatform.push_back(new Platform(6, 6, 88*32, 88*32, screen_height - 16, screen_height - 16 - 11*32, 88*32, (float)screen_height - 16 - 5*32, true, 2));
    structSeeSaw(92, 12, 6);
    vPlatform.push_back(new Platform(6, 6, 91*32, 91*32, screen_height - 16, screen_height - 16 - 11*32, 91*32, (float)screen_height - 16 - 9*32, true, 5));
    vPlatform.push_back(new Platform(6, 6, 96*32, 96*32, screen_height - 16, screen_height - 16 - 11*32, 96*32, (float)screen_height - 16 - 5*32, true, 4));
    structSeeSaw(103, 12, 7);
    vPlatform.push_back(new Platform(6, 6, 102*32, 102*32, screen_height - 16, screen_height - 16 - 11*32, 102*32, (float)screen_height - 16 - 9*32, true, 7));
    vPlatform.push_back(new Platform(6, 6, 108*32, 108*32, screen_height - 16, screen_height - 16 - 11*32, 108*32, (float)screen_height - 16 - 5*32, true, 6));
    // -- SEESAW
    vPlatform.push_back(new Platform(6, 2, 58*32, 58*32, screen_height - 16 - 10*32, screen_height - 64, 58*32, (float)screen_height - 64, true));
    vPlatform.push_back(new Platform(6, 2, 62*32, 62*32, screen_height - 12*32, screen_height - 128, 62*32, (float)screen_height - 12*32, false));
    vPlatform.push_back(new Platform(6, 2, 136*32, 136*32, screen_height - 16 - 9*32, screen_height - 48, 136*32, (float)screen_height - 6*32, false));
    structCastleSmall(0, 2);
    structGND2(147, 2, 1, 1);
    structEnd(147, 3, 9);
    structCastleBig(150, 2);
    structCastleWall(159, 2, 11, 6);
}
void Map::loadLVL_4_4() {
    clearMap();
    this->iMapWidth = 200;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    // ----- MINIONS
    loadMinionsLVL_4_4();
    structGND(0, 12, 147, 1);
    structGND(0, 10, 6, 2);
    structGND(0, 0, 7, 5);
    structGND(0, 5, 5, 1);
    structGND(0, 6, 4, 1);
    structGND(0, 5, 3, 1);
    structGND(0, 7, 3, 1);
    structGND(9, 0, 2, 5);
    structGND(13, 0, 3, 5);
    structGND(16, 0, 72, 2);
    structGND(90, 4, 5, 1);
    structGND(91, 4, 4, 1);
    structGND(94, 5, 1, 2);
    structGND(93, 7, 1, 1);
    structGND(95, 7, 1, 2);
    structGND(95, 0, 65, 2);
    structGND(18, 5, 6, 4);
    structGND(25, 5, 1, 4);
    structGND(27, 5, 1, 4);
    structGND(29, 5, 1, 4);
    structGND(31, 5, 1, 4);
    structGND(33, 5, 1, 4);
    structGND(35, 5, 3, 4);
    structGND(38, 8, 6, 1);
    structGND(44, 5, 31, 4);
    structGND(77, 2, 3, 3);
    structGND(86, 4, 2, 1);
    structGND(89, 6, 2, 1);
    structGND(96, 8, 2, 1);
    structGND(96, 4, 48, 1);
    structGND(144, 5, 1, 5);
    structGND(145, 10, 2, 3);
    structGND(77, 9, 3, 3);
    structGND(90, 0, 1, 4);
    structGND(100, 8, 4, 1);
    structGND(105, 5, 1, 4);
    structGND(106, 8, 2, 1);
    structGND(110, 8, 34, 1);
    structGND(152, 2, 4, 3);
    structGND(158, 2, 2, 3);
    structLava(160, 0, 13, 2);
    structBridge(160, 4, 13);
    structGND(152, 12, 41, 1);
    structGND(173, 0, 20, 2);
    structGND(173, 2, 3, 4);
    structGND(174, 9, 2, 3);
    structLava(7, 0, 2, 2);
    structLava(11, 0, 2, 2);
    structLava(88, 0, 2, 2);
    structLava(91, 0, 4, 2);
    this->iLevelType = 4;
    structPipe(40, 2, 2);
    // -- END
    this->iLevelType  = 3;
}
void Map::loadLVL_5_1() {
    clearMap();
    this->iMapWidth = 300;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_5_1();
    loadPipeEventsLVL_5_1();
    structGND(0, 0, 49, 2);
    structGND(51, 0, 41, 2);
    structGND(96, 0, 18, 2);
    structGND(116, 0, 36, 2);
    structGND(155, 0, 70, 2);
    structCloud(0, 10, 2);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structCloud(222, 10, 1);
    structFence(14, 2, 4);
    structFence(38, 2, 2);
    structFence(41, 2, 1);
    structFence(62, 2, 4);
    structFence(86, 2, 2);
    structFence(110, 2, 1);
    structFence(112, 2, 2);
    structFence(134, 2, 2);
    structFence(137, 2, 1);
    structFence(158, 2, 1);
    structFence(160, 2, 2);
    structFence(208, 2, 2);
    structGND2(90, 5, 4, 1);
    structGND2(89, 2, 1, 4);
    structGND2(116, 2, 1, 3);
    structGND2(147, 2, 1, 4);
    structGND2(156, 5, 2, 1);
    structGND2(182, 2, 5, true);
    structGND2(189, 4, 2, 6);
    this->iLevelType = 4;
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(23, 2, 1, false);
    structTree(24, 2, 1, false);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(88, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(107, 2, 1, false);
    structTree(109, 2, 1, false);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(139, 2, 1, true);
    structTree(155, 2, 1, false);
    structTree(157, 2, 1, true);
    structTree(165, 2, 1, true);
    structTree(167, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(187, 2, 1, true);
    structTree(213, 2, 1, true);
    structTree(215, 2, 1, false);
    structTree(216, 2, 1, false);
    structPipe(44, 2, 2);
    structPipe(51, 2, 2);
    structPipe(156, 6, 1);
    structPipe(163, 2, 1);
    structGND2(199, 2, 1, 1);
    structEnd(199, 3, 9);
    this->iLevelType = 0;
    structBulletBill(111, 2, 0);
    structBulletBill(159, 2, 0);
    structBulletBill(170, 2, 0);
    structBrick(90, 9, 3, 1);
    lMap[91][9]->setSpawnStar(true);
    structBrick(148, 5, 3, 1);
    lMap[148][5]->setSpawnMushroom(true);
    structCastleSmall(0, 2);
    structCastleSmall(203, 2);
    // -- MAP 5-1-2
    this->iLevelType = 1;
    structGND(228, 0, 17, 2);
    structBrick(227, 2, 1, 18);
    structBrick(228, 2, 1, 11);
    structBrick(232, 7, 1, 2);
    structBrick(233, 7, 5, 1);
    structBrick(238, 7, 1, 6);
    structBrick(239, 8, 2, 1);
    structBrick(232, 12, 6, 1);
    structBrick(242, 7, 1, 1);
    lMap[242][7]->setNumOfUse(8);
    structCoins(233, 8, 5, 2);
    structPipeVertical(243, 2, 10);
    structPipeHorizontal(241, 2, 1);
    // -- END
    this->iLevelType = 0;
}
void Map::loadLVL_5_2() {
    clearMap();
    this->iMapWidth = 415;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_5_2();
    loadPipeEventsLVL_5_2();
    for(int i = 0; i < 16; i++) {
        lMap[79][i]->setBlockID(83);
    }
    structGND(74, 0, 33, 2);
    structGND(109, 0, 37, 2);
    structGND(148, 0, 24, 2);
    structGND(176, 0, 31, 2);
    structGND(209, 0, 25, 2);
    structGND(231, 0, 19, 2);
    structGND(253, 0, 2, 2);
    structGND(257, 0, 8, 2);
    structGND(266, 0, 2, 2);
    structGND(269, 0, 35, 2);
    structCastleSmall(80, 2);
    structCastleSmall(284, 2);
    structGrass(75, 2, 2);
    structCloud(75, 11, 2);
    structCloud(80, 10, 2);
    structCloud(98, 10, 1);
    structCloud(107, 11, 1);
    structCloud(110, 10, 2);
    structCloud(125, 11, 1);
    structCloud(128, 10, 1);
    structCloud(146, 10, 1);
    structCloud(155, 11, 1);
    structCloud(158, 10, 2);
    structCloud(173, 11, 1);
    structCloud(176, 10, 2);
    structCloud(194, 10, 1);
    structCloud(203, 11, 1);
    structCloud(206, 10, 2);
    structCloud(221, 11, 1);
    structCloud(224, 10, 2);
    structCloud(242, 10, 1);
    structCloud(251, 11, 1);
    structCloud(254, 10, 2);
    structCloud(269, 11, 1);
    structCloud(272, 10, 2);
    structCloud(290, 10, 1);
    structCloud(299, 11, 1);
    addSpring(106*32, 336);
    structCoins(101, 10, 3, 1);
    structCoins(109, 6, 3, 1);
    structCoins(166, 10, 2, 1);
    structCoins(252, 10, 2, 1);
    structBulletBill(97, 6, 0);
    structBulletBill(187, 2, 0);
    this->iLevelType = 4;
    structGND2(280, 2, 1, 1);
    structEnd(280, 3, 9);
    structTree(91, 2, 1, false);
    structTree(93, 2, 1, true);
    structTree(102, 2, 1, true);
    structTree(104, 2, 1, false);
    structTree(105, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(123, 2, 1, true);
    structTree(139, 2, 1, false);
    structTree(141, 2, 1, true);
    structTree(151, 2, 1, false);
    structTree(152, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(171, 2, 1, true);
    structTree(189, 2, 1, true);
    structTree(197, 2, 1, true);
    structTree(199, 2, 1, false);
    structTree(200, 2, 1, false);
    structTree(216, 2, 1, false);
    structTree(219, 2, 1, true);
    structTree(235, 2, 1, false);
    structTree(237, 2, 1, true);
    structTree(245, 2, 1, true);
    structTree(247, 2, 1, false);
    structTree(248, 2, 1, false);
    structTree(283, 2, 1, false);
    structTree(293, 2, 1, true);
    structTree(295, 2, 1, false);
    structTree(296, 2, 1, false);
    structFence(96, 2, 3);
    structFence(118, 2, 2);
    structFence(121, 2, 1);
    structFence(166, 2, 2);
    structFence(190, 2, 4);
    structFence(214, 2, 2);
    structFence(217, 2, 1);
    structFence(238, 2, 4);
    structFence(262, 2, 1);
    structFence(289, 2, 1);
    structGND2(92, 2, 4, true);
    structGND2(96, 5, 3, 1);
    structGND2(124, 2, 4, true);
    structGND2(128, 2, 1, 4);
    structGND2(129, 2, 1, 2);
    structGND2(142, 2, 4, true);
    structGND2(148, 2, 1, 5);
    structGND2(149, 2, 1, 6);
    structGND2(150, 2, 1, 6);
    structGND2(209, 2, 1, 3);
    structGND2(263, 2, 2, true);
    structGND2(266, 2, 1, 4);
    structGND2(267, 2, 1, 5);
    structGND2(269, 2, 1, 7);
    structGND2(270, 2, 2, 8);
    structBrick(109, 5, 6, 1);
    structBrick(109, 9, 6, 1);
    lMap[114][9]->setSpawnMushroom(true);
    structBrick(166, 9, 2, 1);
    structBrick(169, 6, 3, 1);
    structBrick(198, 5, 8, 1);
    structBrick(198, 9, 8, 1);
    lMap[205][9]->setSpawnStar(true);
    structBrick(221, 3, 2, 1);
    lMap[221][3]->setNumOfUse(6);
    lMap[222][3]->setSpawnMushroom(true);
    structBrick(227, 5, 3, 1);
    structBrick(233, 9, 5, 1);
    structBrick(248, 5, 1, 1);
    lMap[248][5]->setSpawnMushroom(true);
    structBrick(252, 9, 4, 1);
    struckBlockQ(158, 5, 5);
    struckBlockQ2(164, 5, 1);
    structPipe(135, 2, 2);
    structPipe(195, 2, 1);
    structPipe(253, 2, 1);
    // -- MAP 5-2-2
    this->iLevelType = 2;
    structWater(0, 0, 65, 13);
    structPipeHorizontal(62, 6, 1);
    structGND(0, 0, 22, 2);
    structGND(26, 0, 2, 5);
    structGND(32, 0, 2, 5);
    structGND(34, 0, 4, 2);
    structGND(40, 0, 2, 2);
    structGND(44, 0, 30, 2);
    structGND(11, 8, 5, 1);
    structGND(26, 10, 2, 3);
    structGND(32, 10, 2, 3);
    structGND(38, 10, 6, 1);
    structGND(50, 5, 4, 1);
    structGND(54, 8, 4, 1);
    structGND(59, 2, 15, 1);
    structGND(60, 3, 14, 1);
    structGND(61, 4, 13, 2);
    structGND(63, 6, 11, 3);
    structGND(61, 9, 13, 4);
    structUW1(12, 2, 1, 3);
    structUW1(15, 9, 1, 2);
    structUW1(20, 2, 1, 4);
    structUW1(38, 6, 1, 4);
    structUW1(43, 6, 1, 4);
    structUW1(46, 2, 1, 2);
    structCoins(22, 7, 10, 1);
    structCoins(40, 2, 2, 1);
    structCoins(50, 6, 4, 1);
    structCoins(54, 9, 4, 1);
    vPlatform.push_back(new Platform(6, 2, 22*32, 22*32, screen_height - 12*32, screen_height - 16 - 3*32, 22*32 + 16, (float)screen_height - 16 - 4*32, true));
    vPlatform.push_back(new Platform(6, 2, 28*32, 28*32, screen_height - 12*32, screen_height - 16 - 3*32, 28*32 + 16, (float)screen_height - 12*32, false));
    this->iLevelType = 0;
    // -- MAP 5-2-3 BONUS
    this->iLevelType = 0;
    lMap[165][9]->setBlockID(128);
    structBonus(325, 1, 4);
    structBonus(330, 1, 57);
    lMap[329][0]->setBlockID(130); // VINE
    lMap[329][1]->setBlockID(83);
    structCoins(340, 8, 16, 1);
    structCoins(357, 10, 3, 1);
    structCoins(361, 9, 16, 1);
    structCoins(378, 10, 3, 1);
    structCoins(394, 2, 3, 1);
    structBonusEnd(387);
    vPlatform.push_back(new Platform(6, 4, 341*32 + 16, 390*32 + 16, screen_height - 16 - 4*32, screen_height - 16 - 4*32, 341*32.0f + 16, screen_height - 16.0f - 4*32, true));
}
void Map::loadLVL_5_3() {
    clearMap();
    this->iMapWidth = 270;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_5_3();
    // ---------- LOAD LISTS ----------
    vPlatform.push_back(new Platform(4, 2, 55*32, 55*32, screen_height - 16 - 9*32, screen_height - 32, 55*32, (float)screen_height - 32, true));
    vPlatform.push_back(new Platform(4, 3, 83*32 - 16, 86*32 + 16, screen_height - 16 - 6*32, screen_height - 16 - 6*32, 83*32 - 16, (float)screen_height - 16 - 6*32, true));
    vPlatform.push_back(new Platform(4, 3, 91*32 - 16, 93*32 + 16, screen_height - 16 - 5*32, screen_height - 16 - 5*32, 93*32 + 16, (float)screen_height - 16 - 5*32, false));
    vPlatform.push_back(new Platform(4, 3, 127*32 - 16, 131*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 127*32 - 16, (float)screen_height - 16 - 8*32, false));
    // ----- CLOUDS
    structCloud(3, 10, 2);
    structCloud(9, 6, 1);
    structCloud(11, 18, 2);
    structCloud(18, 11, 2);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(46, 2, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(66, 11, 2);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(94, 2, 1);
    structCloud(99, 10, 2);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structCloud(142, 2, 1);
    structCloud(147, 10, 2);
    structCloud(153, 6, 1);
    structCloud(162, 11, 2);
    // ----- GND
    structGND(0, 0, 16, 2);
    structGND(129, 0, 47, 2);
    // ----- T
    structT(18, 0, 4, 3);
    structT(24, 0, 8, 6);
    structT(26, 6, 5, 4);
    structT(32, 0, 3, 3);
    structT(35, 0, 5, 7);
    structT(40, 0, 7, 11);
    structT(50, 0, 4, 2);
    structT(59, 0, 5, 2);
    structT(60, 2, 4, 8);
    structT(50, 0, 4, 2);
    structT(65, 0, 5, 2);
    structT(70, 0, 3, 6);
    structT(76, 0, 6, 9);
    structT(98, 0, 4, 4);
    structT(104, 0, 8, 8);
    structT(113, 0, 3, 2);
    structT(116, 0, 4, 6);
    structT(122, 0, 4, 6);
    // ----- GND2
    struckBlockQ(59, 4, 1);
    lMap[59][4]->setSpawnMushroom(true);
    structGND2(138, 2, 2, 4);
    structGND2(140, 2, 2, 6);
    structGND2(142, 2, 2, 8);
    // ----- COINS
    structCoins(27, 10, 3, 1);
    structCoins(33, 3, 1, 1);
    structCoins(37, 12, 2, 1);
    structCoins(50, 8, 2, 1);
    structCoins(60, 10, 4, 1);
    structCoins(85, 9, 2, 1);
    structCoins(93, 10, 2, 1);
    structCoins(97, 10, 2, 1);
    structCoins(113, 2, 3, 1);
    structCoins(120, 9, 2, 1);
    // ----- END
    structGND2(152, 2, 1, 1);
    structEnd(152, 3, 9);
    // ----- CASTLE
    structCastleSmall(0, 2);
    structCastleBig(155, 2);
    structCastleWall(164, 2, 12, 6);
    this->iLevelType = 0;
}
void Map::loadLVL_5_4() {
    clearMap();
    this->iMapWidth = 190;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    // ----- MINIONS
    loadMinionsLVL_5_4();
    vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, screen_height + 32, 85*32 + 24, 9*32 + 16, true));
    vPlatform.push_back(new Platform(3, 1, 85*32 + 24, 85*32 + 16, -32, screen_height + 32, 85*32 + 24, 1*32 + 16, true));
    vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, screen_height + 32, 88*32 + 24, 10*32, true));
    vPlatform.push_back(new Platform(3, 0, 88*32 + 24, 88*32 + 16, -32, screen_height + 32, 88*32 + 24, 2*32, true));
    vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 136*32 - 16, (float)screen_height - 16 - 8*32, false));
    structGND(32, 0, 52, 2);
    structGND(92, 0, 17, 2);
    structGND(115, 0, 13, 2);
    structGND(141, 0, 40, 2);
    structGND(0, 0, 16, 5);
    structGND(0, 5, 5, 1);
    structGND(0, 6, 4, 1);
    structGND(0, 7, 3, 1);
    structGND(18, 5, 2, 1);
    structGND(22, 7, 3, 1);
    structGND(27, 5, 2, 1);
    structGND(32, 2, 2, 3);
    structGND(37, 5, 37, 1);
    structGND(80, 2, 4, 3);
    structGND(92, 2, 7, 3);
    structGND(108, 2, 1, 3);
    structGND(111, 0, 2, 5);
    structGND(115, 2, 5, 3);
    structGND(122, 2, 2, 3);
    structGND(126, 2, 2, 3);
    structGND(141, 2, 3, 4);
    structGND(0, 10, 16, 3);
    structGND(34, 9, 49, 4);
    structGND(93, 10, 6, 3);
    structGND(115, 10, 13, 3);
    structGND(128, 12, 42, 1);
    structGND(142, 9, 2, 3);
    lMap[23][7]->setBlockID(56);
    lMap[43][1]->setBlockID(56);
    lMap[49][5]->setBlockID(56);
    lMap[55][9]->setBlockID(56);
    lMap[55][1]->setBlockID(56);
    lMap[61][5]->setBlockID(56);
    lMap[67][1]->setBlockID(56);
    lMap[73][5]->setBlockID(56);
    lMap[82][8]->setBlockID(56);
    lMap[92][4]->setBlockID(56);
    lMap[103][3]->setBlockID(56);
    structCoins(102, 2, 3, 1);
    structCoins(102, 6, 3, 1);
    struckBlockQ(23, 11, 1);
    lMap[23][11]->setSpawnMushroom(true);
    structBridge(128, 4, 13);
    structBrick(128, 9, 6, 1);
    structLava(16, 0, 16, 2);
    structLava(109, 0, 2, 2);
    structLava(113, 0, 2, 2);
    structLava(128, 0, 13, 2);
    structPlatformLine(86);
    structPlatformLine(89);
    this->iLevelType = 3;
}
void Map::loadLVL_6_1() {
    clearMap();
    this->iMapWidth = 210;
    this->iMapHeight = 25;
    this->iLevelType = 4;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_6_1();
    structGND(0, 0, 20, 2);
    structGND(22, 0, 9, 2);
    structGND(37, 0, 2, 2);
    structGND(41, 0, 16, 2);
    structGND(59, 0, 15, 2);
    structGND(77, 0, 16, 2);
    structGND(96, 0, 31, 2);
    structGND(134, 0, 2, 2);
    structGND(137, 0, 12, 2);
    structGND(155, 0, 9, 2);
    structGND(167, 0, 7, 2);
    structGND(176, 0, 34, 2);
    structCloud(8, 10, 1);
    structCloud(19, 11, 1);
    structCloud(27, 10, 3);
    structCloud(36, 11, 2);
    structCloud(56, 10, 1);
    structCloud(67, 11, 1);
    structCloud(75, 10, 3);
    structCloud(81, 11, 2);
    structCloud(104, 10, 1);
    structCloud(115, 11, 1);
    structCloud(123, 10, 3);
    structCloud(132, 11, 2);
    structCloud(152, 10, 1);
    structCloud(163, 11, 1);
    structCloud(171, 10, 3);
    structCloud(180, 11, 2);
    structCloud(200, 10, 1);
    structCastleBig(-2, 2);
    structBush(16, 2, 1);
    structBush(48, 2, 2);
    structBush(64, 2, 1);
    structBush(96, 2, 2);
    structBush(112, 2, 1);
    structBush(160, 2, 1);
    structGrass(11, 2, 3);
    structGrass(23, 2, 1);
    structGrass(41, 2, 2);
    structGrass(59, 2, 3);
    structGrass(89, 2, 2);
    structGrass(107, 2, 3);
    structGrass(119, 2, 1);
    structGrass(137, 2, 2);
    structGrass(155, 2, 3);
    structGrass(167, 2, 1);
    structGrass(185, 2, 2);
    structGrass(203, 2, 3);
    structCastleSmall(192, 2);
    structGND2(26, 2, 6, 1);
    structGND2(29, 3, 5, 1);
    structGND2(32, 4, 4, 1);
    structGND2(35, 5, 3, 1);
    structGND2(69, 2, 4, true);
    structGND2(73, 2, 1, 4);
    structGND2(84, 3, 2, true);
    structGND2(87, 2, 1, 5);
    structGND2(88, 2, 1, 6);
    structGND2(89, 2, 1, 7);
    structGND2(122, 2, 5, true);
    structGND2(143, 2, 6, true);
    structGND2(169, 2, 5, true);
    structGND2(176, 2, 2, 8);
    struckBlockQ(16, 5, 2);
    structBrick(36, 9, 2, 1);
    lMap[36][9]->setSpawnMushroom(true);
    structBrick(41, 5, 3, 1);
    lMap[43][5]->setNumOfUse(7);
    structBrick(90, 8, 3, 1);
    structBrick(92, 4, 3, 1);
    structBrick(127, 6, 2, 1);
    struckBlockQ(130, 6, 2);
    lMap[130][6]->setSpawnMushroom(true);
    structBrick(130, 2, 5, 1);
    structBrick(149, 7, 2, 1);
    structBrick(151, 5, 2, 1);
    lMap[152][5]->setNumOfUse(8);
    structBrick(152, 3, 2, 1);
    struckBlockQ2(90, 6, 1);
    lMap[90][6]->setSpawnMushroom(true);
    lMap[90][6]->setPowerUP(false);
    struckBlockQ2(113, 5, 1);
    struckBlockQ2(113, 9, 1);
    structCoins(62, 5, 3, 1);
    structCoins(76, 7, 2, 1);
    structCoins(105, 6, 3, 1);
    this->iLevelType = 0;
    structPipe(102, 2, 2);
    structGND2(186, 2, 1, 1);
    structEnd(186, 3, 9);
    this->iLevelType = 4;
}
void Map::loadLVL_6_2() {
    clearMap();
    this->iMapWidth = 495;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_6_2();
    loadPipeEventsLVL_6_2();
    for(int i = 0; i < 16; i++) {
        lMap[84][i]->setBlockID(83);
    }
    structGND(77, 0, 131, 2);
    structGND(214, 0, 12, 2);
    structGND(227, 0, 1, 2);
    structGND(229, 0, 8, 2);
    structGND(238, 0, 82, 2);
    structCloud(82, 11, 1);
    structCloud(93, 10, 1);
    structCloud(104, 11, 1);
    structCloud(112, 10, 3);
    structCloud(121, 11, 2);
    structCloud(141, 10, 1);
    structCloud(152, 11, 1);
    structCloud(160, 10, 3);
    structCloud(169, 11, 2);
    structCloud(189, 10, 1);
    structCloud(200, 11, 1);
    structCloud(208, 10, 3);
    structCloud(217, 11, 2);
    structCloud(237, 10, 1);
    structCloud(248, 11, 1);
    structCloud(256, 10, 3);
    structCloud(265, 11, 2);
    structCloud(285, 10, 1);
    structCloud(296, 11, 1);
    structCloud(304, 10, 3);
    structCloud(313, 11, 2);
    structGrass(80, 2, 2);
    structGrass(96, 2, 3);
    structGrass(108, 2, 1);
    structGrass(126, 2, 2);
    structGrass(144, 2, 3);
    structGrass(156, 2, 1);
    structGrass(174, 2, 2);
    structGrass(192, 2, 3);
    structGrass(204, 2, 1);
    structGrass(222, 2, 2);
    structGrass(240, 2, 3);
    structGrass(252, 2, 1);
    structGrass(270, 2, 2);
    structGrass(300, 2, 1);
    structBush(101, 2, 1);
    structBush(133, 2, 2);
    structBush(149, 2, 1);
    structBush(181, 2, 2);
    structBush(197, 2, 1);
    structBush(229, 2, 2);
    structBush(245, 2, 1);
    structBush(277, 2, 2);
    structBush(293, 2, 1);
    structGND2(117, 6, 2, 1);
    structGND2(147, 5, 2, 1);
    structGND2(196, 5, 2, 1);
    structGND2(234, 2, 3, true);
    structGND2(241, 2, 1, 4);
    structGND2(242, 2, 1, 2);
    structGND2(252, 5, 2, 1);
    structGND2(284, 2, 8, true);
    structGND2(292, 2, 1, 8);
    structBrick(95, 5, 3, 1);
    structBrick(108, 9, 3, 1);
    structBrick(136, 5, 2, 1);
    lMap[137][5]->setSpawnMushroom(true);
    structBrick(152, 9, 5, 1);
    structBrick(162, 9, 4, 1);
    structBrick(195, 5, 1, 1);
    structBrick(198, 5, 1, 1);
    structBrick(200, 9, 1, 1);
    structBrick(204, 9, 9, 1);
    structBrick(223, 6, 2, 1);
    structBrick(225, 9, 2, 1);
    lMap[225][9]->setSpawnStar(true);
    structBrick(229, 5, 3, 1);
    structBrick(230, 9, 2, 1);
    structBrick(245, 5, 3, 1);
    structBrick(245, 9, 3, 1);
    lMap[109][9]->setNumOfUse(8);
    structCastleSmall(85, 2);
    structCastleSmall(305, 2);
    struckBlockQ2(109, 5, 1);
    struckBlockQ2(167, 5, 1);
    this->iLevelType = 0;
    structPipe(104, 2, 3);
    structPipe(113, 2, 3);
    structPipe(117, 7, 1);
    structPipe(120, 2, 1);
    structPipe(122, 2, 1);
    structPipe(131, 2, 3);
    structPipe(141, 2, 4);
    structPipe(147, 6, 1);
    structPipe(152, 2, 1);
    structPipe(159, 2, 1);
    structPipe(165, 2, 2);
    structPipe(169, 2, 1);
    structPipe(172, 2, 5);
    structPipe(179, 2, 2);
    structPipe(187, 2, 3);
    structPipe(190, 2, 1);
    structPipe(196, 6, 2);
    structPipe(200, 2, 1);
    structPipe(216, 2, 1);
    structPipe(220, 2, 1);
    structPipe(238, 2, 2);
    structPipe(252, 6, 2);
    structPipe(259, 2, 3);
    structPipe(264, 2, 1);
    structPipe(266, 2, 2);
    structPipe(268, 2, 3);
    structPipe(274, 2, 2);
    structPipe(286, 2, 3);
    structGND2(301, 2, 1, 1);
    structEnd(301, 3, 9);
    // -- MAP 6-2-2
    this->iLevelType = 2;
    structWater(0, 0, 65, 13);
    structPipeHorizontal(62, 6, 1);
    structGND(0, 0, 22, 2);
    structGND(26, 0, 2, 5);
    structGND(32, 0, 2, 5);
    structGND(34, 0, 4, 2);
    structGND(40, 0, 2, 2);
    structGND(44, 0, 30, 2);
    structGND(11, 8, 5, 1);
    structGND(26, 10, 2, 3);
    structGND(32, 10, 2, 3);
    structGND(38, 10, 6, 1);
    structGND(50, 5, 4, 1);
    structGND(54, 8, 4, 1);
    structGND(59, 2, 15, 1);
    structGND(60, 3, 14, 1);
    structGND(61, 4, 13, 2);
    structGND(63, 6, 11, 3);
    structGND(61, 9, 13, 4);
    structUW1(12, 2, 1, 3);
    structUW1(15, 9, 1, 2);
    structUW1(20, 2, 1, 4);
    structUW1(38, 6, 1, 4);
    structUW1(43, 6, 1, 4);
    structUW1(46, 2, 1, 2);
    structCoins(22, 7, 10, 1);
    structCoins(40, 2, 2, 1);
    structCoins(50, 6, 4, 1);
    structCoins(54, 9, 4, 1);
    vPlatform.push_back(new Platform(4, 2, 22*32, 22*32, screen_height - 12*32, screen_height - 16 - 3*32, 22*32 + 16, (float)screen_height - 16 - 4*32, true));
    vPlatform.push_back(new Platform(4, 2, 28*32, 28*32, screen_height - 12*32, screen_height - 16 - 3*32, 28*32 + 16, (float)screen_height - 12*32, false));
    // -- MAP 6-2-3
    this->iLevelType = 1;
    structGND(330, 0, 17, 2);
    structBrick(329, 2, 1, 18);
    structBrick(330, 2, 1, 11);
    structBrick(334, 7, 1, 2);
    structBrick(335, 7, 5, 1);
    structBrick(340, 7, 1, 6);
    structBrick(341, 8, 2, 1);
    structBrick(334, 12, 6, 1);
    structBrick(344, 7, 1, 1);
    lMap[344][7]->setNumOfUse(8);
    structCoins(335, 8, 5, 2);
    structPipeVertical(345, 2, 10);
    structPipeHorizontal(343, 2, 1);
    // -- MAP 6-2-4
    this->iLevelType = 1;
    structGND(360, 0, 17, 2);
    structBrick(360, 2, 1, 11);
    structBrick(363, 3, 1, 3);
    structBrick(370, 3, 1, 3);
    structBrick(364, 5, 6, 1);
    structBrick(363, 12, 12, 1);
    structCoins(363, 2, 10, 1);
    structCoins(363, 6, 8, 1);
    structBrick(373, 5, 1, 1);
    lMap[373][5]->setSpawnMushroom(true);
    structPipeVertical(375, 2, 10);
    structPipeHorizontal(373, 2, 1);
    // -- MAP 6-2-5 BONUS
    this->iLevelType = 4;
    lMap[166][9]->setBlockID(128);
    for(int i = 0; i < 16; i++) {
        lMap[389][i]->setBlockID(83);
    }
    structBonus(390, 1, 4);
    lMap[394][0]->setBlockID(130); // VINE
    lMap[394][1]->setBlockID(83);
    structBonus(395, 1, 78);
    structBonus(421, 6, 1);
    structBonus(440, 6, 1);
    structBonus(440, 7, 1);
    structBonus(451, 6, 1);
    structBonus(451, 7, 1);
    structBonus(457, 8, 2);
    structBonus(461, 8, 1);
    structBonus(463, 8, 1);
    structBonus(465, 8, 1);
    structBonus(467, 8, 1);
    structBonus(469, 8, 1);
    structCoins(404, 8, 16, 1);
    structCoins(423, 8, 16, 1);
    structCoins(442, 9, 8, 1);
    structCoins(461, 9, 10, 1);
    structCoins(475, 3, 3, 1);
    structBonusEnd(473);
    vPlatform.push_back(new Platform(6, 4, 406*32 + 16, 475*32 + 16, screen_height - 16 - 4*32, screen_height - 16 - 4*32, 406*32.0f + 16, screen_height - 16.0f - 4*32, true));
    // -- END
    this->iLevelType = 4;
}
void Map::loadLVL_6_3() {
    clearMap();
    this->iMapWidth = 200;
    this->iMapHeight = 25;
    this->iLevelType = 6;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_6_3();
    structGND(0, 0, 16, 2);
    structGND(160, 0, 30, 2);
    this->iLevelType = 3;
    structCloud(3, 10, 2);
    structCloud(18, 11, 2);
    structCloud(9, 6, 1);
    structCloud(28, 2, 1);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(46, 2, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(66, 11, 2);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(94, 2, 1);
    structCloud(99, 10, 2);
    structCloud(105, 6, 1);
    structCloud(114, 11, 2);
    structCloud(124, 2, 1);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structCloud(142, 2, 1);
    structCloud(147, 10, 2);
    structCloud(153, 6, 1);
    structT(18, 0, 3, 2);
    structT(21, 0, 3, 6);
    structT(24, 0, 3, 2);
    structT(31, 0, 4, 6);
    structT(37, 0, 3, 2);
    structT(43, 0, 3, 2);
    structT(49, 0, 4, 4);
    structT(65, 0, 5, 6);
    structT(85, 0, 3, 12);
    structT(85, 0, 5, 5);
    structT(90, 0, 3, 7);
    structT(93, 0, 3, 2);
    structT(97, 0, 4, 2);
    structT(103, 0, 3, 2);
    structT(107, 0, 5, 6);
    structT(113, 0, 5, 2);
    structT(123, 0, 3, 2);
    structT(132, 0, 3, 10);
    structT(132, 0, 4, 2);
    structT(135, 0, 4, 6);
    structT(156, 0, 3, 10);
    structCastleBig(170, 2);
    structCastleWall(179, 2, 11, 6);
    structCastleSmall(0, 2);
    // -- SEESAW
    structSeeSaw(71, 12, 5);
    vPlatform.push_back(new Platform(4, 6, 70*32 + 16, 70*32 + 16, screen_height - 16, screen_height - 16 - 11*32, 70*32 + 16, (float)screen_height - 16 - 9*32, true, 1));
    vPlatform.push_back(new Platform(4, 6, 74*32 + 16, 74*32 + 16, screen_height - 16, screen_height - 16 - 11*32, 74*32 + 16, (float)screen_height - 16 - 5*32, true, 0));
    structSeeSaw(79, 12, 4);
    vPlatform.push_back(new Platform(4, 6, 78*32 + 16, 78*32 + 16, screen_height - 16, screen_height - 16 - 11*32, 78*32 + 16, (float)screen_height - 16 - 9*32, true, 3));
    vPlatform.push_back(new Platform(4, 6, 81*32 + 16, 81*32 + 16, screen_height - 16, screen_height - 16 - 11*32, 81*32 + 16, (float)screen_height - 16 - 5*32, true, 2));
    structSeeSaw(127, 12, 4);
    vPlatform.push_back(new Platform(4, 6, 126*32 + 16, 126*32 + 16, screen_height - 16, screen_height - 16 - 11*32, 126*32 + 16, (float)screen_height - 16 - 9*32, true, 5));
    vPlatform.push_back(new Platform(4, 6, 129*32 + 16, 129*32 + 16, screen_height - 16, screen_height - 16 - 11*32, 129*32 + 16, (float)screen_height - 16 - 5*32, true, 4));
    // -- SEESAW
    vPlatform.push_back(new Platform(4, 2, 28*32, 28*32, screen_height - 11*32, screen_height - 16 - 2*32, 28*32, (float)screen_height - 12*32, false));
    vPlatform.push_back(new Platform(4, 3, 39*32 + 16, 45*32 + 16, screen_height - 9*32, screen_height - 16 - 9*32, 39*32 + 16, (float)screen_height - 9*32, false));
    vPlatform.push_back(new Platform(4, 3, 45*32 + 16, 49*32 + 16, screen_height - 7*32, screen_height - 16 - 7*32, 45*32 + 16, (float)screen_height - 7*32, true));
    vPlatform.push_back(new Platform(4, 3, 51*32, 56*32, screen_height - 8*32, screen_height - 16 - 8*32, 51*32 + 16, (float)screen_height - 8*32, false));
    vPlatform.push_back(new Platform(4, 2, 60*32, 60*32, screen_height - 11*32, screen_height - 16 - 2*32, 60*32, (float)screen_height - 12*32, false));
    vPlatform.push_back(new Platform(4, 2, 121*32 + 16, 121*32 + 16, screen_height - 11*32, screen_height - 16 - 2*32, 121*32 + 16, (float)screen_height - 12*32, false));
    vPlatform.push_back(new Platform(4, 5, 141*32, 141*32, screen_height - 16 - 7*32, screen_height - 16 - 7*32, 141*32, (float)screen_height - 16 - 7*32, true));
    vPlatform.push_back(new Platform(4, 5, 145*32, 145*32, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 145*32, (float)screen_height - 16 - 8*32, true));
    vPlatform.push_back(new Platform(4, 5, 149*32, 149*32, screen_height - 16 - 6*32, screen_height - 16 - 6*32, 149*32, (float)screen_height - 16 - 6*32, true));
    vPlatform.push_back(new Platform(4, 5, 153*32, 153*32, screen_height - 16 - 7*32, screen_height - 16 - 7*32, 153*32, (float)screen_height - 16 - 7*32, true));
    addSpring(38*32, 336);
    addSpring(116*32, 336);
    structGND2(167, 2, 1, 1);
    this->iLevelType = 4;
    structEnd(167, 3, 9);
    structCoins(28, 12, 2, 1);
    structCoins(43, 10, 7, 1);
    structCoins(73, 9, 2, 1);
    structCoins(85, 12, 3, 1);
    structCoins(100, 6, 4, 1);
    structCoins(128, 9, 2, 1);
    structCoins(145, 7, 2, 1);
    structCoins(154, 11, 2, 1);
    struckBlockQ(55, 11, 1);
    lMap[55][11]->setSpawnMushroom(true);
    // -- END
    this->iLevelType = 4;
}
void Map::loadLVL_6_4() {
    clearMap();
    this->iMapWidth = 180;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_6_4();
    vPlatform.push_back(new Platform(4, 3, 136*32 - 16, 138*32 + 16, screen_height - 16 - 8*32, screen_height - 16 - 8*32, 136*32 - 16, (float)screen_height - 16 - 8*32, false));
    structGND(0, 0, 3, 8);
    structGND(3, 0, 1, 7);
    structGND(4, 0, 1, 6);
    structGND(5, 0, 8, 5);
    structGND(15, 0, 11, 5);
    structGND(29, 0, 3, 5);
    structGND(35, 0, 37, 6);
    structGND(72, 0, 32, 5);
    structGND(104, 0, 24, 2);
    structGND(116, 2, 4, 3);
    structGND(123, 2, 5, 3);
    structGND(141, 0, 3, 6);
    structGND(144, 0, 19, 2);
    structGND(0, 10, 24, 3);
    structGND(23, 9, 1, 4);
    structGND(24, 12, 13, 1);
    structGND(37, 9, 35, 4);
    structGND(72, 12, 91, 1);
    structGND(80, 11, 1, 1);
    structGND(88, 11, 1, 1);
    structGND(97, 10, 7, 2);
    structGND(123, 10, 5, 2);
    structGND(142, 9, 2, 3);
    structBridge(128, 4, 13);
    structLava(13, 0, 2, 3);
    structLava(26, 0, 3, 2);
    structLava(32, 0, 3, 2);
    structLava(128, 0, 13, 2);
    lMap[23][8]->setBlockID(56);
    lMap[37][8]->setBlockID(56);
    lMap[80][10]->setBlockID(56);
    lMap[92][5]->setBlockID(56);
    lMap[30][4]->setBlockID(56);
    lMap[49][8]->setBlockID(56);
    lMap[60][8]->setBlockID(56);
    lMap[67][8]->setBlockID(56);
    lMap[76][5]->setBlockID(56);
    lMap[84][5]->setBlockID(56);
    lMap[88][10]->setBlockID(56);
    struckBlockQ(30, 8, 1);
    lMap[30][8]->setSpawnMushroom(true);
    struckBlockQ2(106, 5, 1);
    struckBlockQ2(109, 5, 1);
    struckBlockQ2(112, 5, 1);
    struckBlockQ2(107, 9, 1);
    struckBlockQ2(110, 9, 1);
    struckBlockQ2(113, 9, 1);
    this->iLevelType = 3;
}
void Map::loadLVL_7_1() {
    clearMap();
    this->iMapWidth = 300;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_7_1();
    loadPipeEventsLVL_7_1();
    structBulletBill(19, 2, 0);
    structBulletBill(28, 1, 0);
    structBulletBill(28, 3, 0);
    structBulletBill(36, 2, 0);
    structBulletBill(46, 2, 1);
    structBulletBill(56, 1, 0);
    structBulletBill(56, 3, 0);
    structBulletBill(64, 6, 0);
    structBulletBill(68, 2, 0);
    structBulletBill(104, 2, 0);
    structBulletBill(122, 2, 0);
    structBulletBill(146, 1, 0);
    structBulletBill(146, 3, 0);
    structGND(0, 0, 73, 2);
    structGND(75, 0, 77, 2);
    structGND(153, 0, 46, 2);
    structGND2(64, 5, 1, 1);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCastleBig(-2, 2);
    structFence(14, 2, 4);
    structFence(38, 2, 2);
    structFence(41, 2, 1);
    structFence(62, 2, 4);
    structFence(86, 2, 2);
    structFence(89, 2, 1);
    structFence(111, 2, 3);
    structFence(134, 2, 2);
    structFence(137, 2, 1);
    structFence(159, 2, 3);
    structFence(182, 2, 1);
    this->iLevelType = 4;
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(23, 2, 1, false);
    structTree(24, 2, 1, false);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(88, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(107, 2, 1, false);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(139, 2, 1, true);
    structPipe(76, 2, 2);
    structPipe(93, 2, 2);
    structPipe(109, 2, 2);
    structPipe(115, 2, 1);
    structPipe(128, 2, 1);
    structGND2(179, 2, 1, 1);
    structEnd(179, 3, 9);
    this->iLevelType = 0;
    structBrick(27, 9, 3, 1);
    lMap[27][9]->setSpawnMushroom(true);
    structBrick(62, 5, 2, 1);
    structBrick(65, 5, 2, 1);
    lMap[65][5]->setNumOfUse(8);
    structBrick(82, 5, 7, 1);
    structBrick(82, 9, 7, 1);
    structBrick(134, 5, 5, 1);
    structBrick(134, 9, 5, 1);
    structBrick(149, 6, 2, 1);
    structBrick(153, 8, 2, 1);
    structBrick(151, 12, 1, 1);
    lMap[151][12]->setSpawnMushroom(true);
    struckBlockQ2(93, 9, 1);
    lMap[93][9]->setSpawnMushroom(true);
    lMap[93][9]->setPowerUP(false);
    struckBlockQ(39, 5, 4);
    structGND2(141, 2, 1, 3);
    structGND2(153, 2, 6, true);
    structGND2(162, 2, 8, true);
    structGND2(170, 2, 1, 8);
    addSpring(151*32, 336);
    structCastleSmall(183, 2);
    // -- MAP 7-1-2
    this->iLevelType = 1;
    structGND(210, 0, 17, 2);
    structBrick(210, 2, 1, 11);
    structBrick(214, 2, 7, 3);
    structBrick(214, 12, 7, 1);
    structPipeVertical(225, 2, 10);
    structPipeHorizontal(223, 2, 1);
    structCoins(214, 5, 7, 1);
    structCoins(214, 7, 7, 1);
    structCoins(215, 9, 5, 1);
    // -- END
    this->iLevelType = 0;
}
void Map::loadLVL_7_2() {
    clearMap();
    this->iMapWidth = 320;
    this->iMapHeight = 25;
    this->iLevelType = 2;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_7_2();
    loadPipeEventsLVL_7_2();
    this->underWater = true;
    structWater(0, 0, 200, 13);
    structGND(0, 0, 66, 2);
    structGND(71, 0, 60, 2);
    structGND(140, 0, 17, 2);
    structGND(164, 0, 38, 2);
    structGND(64, 2, 1, 3);
    structGND(65, 2, 1, 5);
    structGND(71, 2, 1, 5);
    structGND(72, 2, 1, 3);
    structGND(78, 2, 2, 3);
    structGND(78, 10, 2, 3);
    structGND(82, 9, 3, 1);
    structGND(129, 2, 1, 4);
    structGND(130, 2, 1, 2);
    structGND(141, 2, 1, 4);
    structGND(140, 2, 1, 2);
    structGND(131, 10, 1, 3);
    structGND(132, 10, 8, 1);
    structGND(18, 5, 3, 1);
    structGND(42, 5, 2, 1);
    structGND(102, 5, 2, 1);
    structGND(115, 6, 2, 1);
    structGND(156, 2, 1, 8);
    structGND(157, 9, 2, 1);
    structGND(164, 2, 1, 8);
    structGND(162, 9, 2, 1);
    structGND(172, 5, 5, 1);
    structGND(172, 9, 5, 1);
    structGND(180, 5, 4, 1);
    structGND(180, 9, 4, 1);
    structCoins(14, 2, 2, 1);
    structCoins(27, 9, 3, 1);
    structCoins(36, 2, 3, 1);
    structCoins(67, 4, 3, 1);
    structCoins(101, 3, 3, 1);
    structCoins(113, 8, 3, 1);
    structCoins(133, 3, 1, 1);
    structCoins(134, 2, 3, 1);
    structCoins(137, 3, 1, 1);
    structCoins(159, 5, 3, 1);
    structCoins(159, 2, 3, 1);
    structUW1(11, 2, 1, 3);
    structUW1(33, 2, 1, 5);
    structUW1(42, 6, 1, 2);
    structUW1(50, 2, 1, 4);
    structUW1(83, 10, 1, 2);
    structUW1(89, 2, 1, 3);
    structUW1(102, 6, 1, 4);
    structUW1(120, 2, 1, 4);
    structUW1(147, 2, 1, 2);
    structUW1(149, 2, 1, 3);
    structUW1(173, 10, 1, 2);
    structPipeHorizontal(189, 6, 1);
    structGND(185, 2, 17, 1);
    structGND(186, 3, 16, 1);
    structGND(187, 4, 15, 1);
    structGND(188, 5, 14, 1);
    structGND(190, 6, 12, 3);
    structGND(188, 9, 14, 4);
    // -- MAP 2-2-2
    this->iLevelType = 0;
    structGND(220, 0, 24, 2);
    structCastleSmall(220, 2);
    structCloud(223, 10, 2);
    structCloud(229, 6, 1);
    structCloud(238, 11, 2);
    structPipe(232, 2, 3);
    structPipeHorizontal(230, 2, 1);
    // -- MAP 2-2-3
    structCloud(275, 11, 2);
    structCloud(295, 10, 1);
    structCloud(306, 11, 1);
    structGrass(300, 2, 1);
    structGrass(310, 2, 1);
    structBush(287, 2, 2);
    structBush(303, 2, 1);
    structGND(271, 0, 42, 2);
    structPipe(274, 2, 1);
    structGND2(276, 2, 8, true);
    structGND2(284, 2, 1, 8);
    structGND2(293, 2, 1, 1);
    structEnd(293, 3, 9);
    structCastleSmall(297, 2);
    // -- END
    this->iLevelType = 2;
}
void Map::loadLVL_7_3() {
    clearMap();
    this->iMapWidth = 250;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_7_3();
    structGND(0, 0, 7, 2);
    structGND(207, 0, 43, 2);
    structCloud(3, 10, 2);
    structCloud(9, 6, 1);
    structCloud(18, 11, 2);
    structCloud(28, 2, 1);
    structCloud(35, 6, 1);
    structCloud(38, 7, 1);
    structCloud(46, 2, 1);
    structCloud(51, 10, 2);
    structCloud(57, 6, 1);
    structCloud(66, 11, 2);
    structCloud(76, 2, 1);
    structCloud(83, 6, 1);
    structCloud(86, 7, 1);
    structCloud(94, 2, 1);
    structCloud(99, 10, 2);
    structCloud(105, 6, 1);
    structCloud(114, 11, 2);
    structCloud(124, 2, 1);
    structCloud(131, 6, 1);
    structCloud(134, 7, 1);
    structCloud(142, 2, 1);
    structCloud(147, 10, 2);
    structCloud(153, 6, 1);
    structCloud(162, 11, 2);
    structCloud(172, 2, 1);
    structCloud(179, 6, 1);
    structCloud(182, 7, 1);
    structCloud(190, 2, 1);
    structCloud(195, 10, 2);
    structCloud(201, 6, 1);
    structCloud(210, 11, 2);
    structCloud(220, 2, 1);
    structCloud(227, 6, 1);
    structCloud(239, 9, 2);
    structT(8, 0, 8, 2);
    structGND2(10, 2, 3, true);
    structGND2(13, 2, 2, 3);
    structBridge2(15, 4, 16);
    structBridge2(32, 4, 15);
    structBridge2(48, 4, 15);
    structBridge2(69, 4, 10);
    structBridge2(85, 4, 10);
    structBridge2(100, 5, 5);
    structT(112, 0, 8, 2);
    structBridge2(122, 4, 3);
    structBridge2(128, 4, 15);
    structBridge2(147, 2, 8);
    structBridge2(160, 4, 8);
    structBridge2(171, 4, 2);
    structBridge2(175, 4, 2);
    structBridge2(179, 4, 2);
    structBridge2(184, 4, 9);
    structGND2(31, 0, 1, 5);
    structGND2(47, 0, 1, 5);
    structGND2(63, 0, 1, 5);
    structGND2(68, 0, 1, 5);
    structGND2(79, 0, 1, 5);
    structGND2(84, 0, 1, 5);
    structGND2(95, 0, 1, 5);
    structGND2(99, 0, 1, 6);
    structGND2(105, 0, 1, 6);
    structGND2(127, 0, 1, 5);
    structGND2(143, 0, 1, 5);
    structGND2(146, 0, 1, 3);
    structGND2(155, 0, 1, 3);
    structGND2(159, 0, 1, 5);
    structGND2(168, 0, 1, 5);
    structGND2(183, 0, 1, 5);
    structT(192, 0, 13, 2);
    structGND2(193, 2, 1, 3);
    structGND2(194, 2, 3, false);
    structGND2(208, 2, 8, true);
    structGND2(216, 2, 1, 8);
    structCoins(36, 9, 4, 1);
    structCoins(55, 9, 1, 1);
    structCoins(57, 9, 1, 1);
    structCoins(59, 9, 1, 1);
    structCoins(56, 8, 1, 1);
    structCoins(58, 8, 1, 1);
    structCoins(72, 9, 1, 1);
    structCoins(73, 10, 2, 1);
    structCoins(75, 9, 1, 1);
    structCoins(97, 9, 3, 1);
    structCoins(108, 9, 3, 1);
    structCoins(133, 9, 6, 1);
    structCoins(149, 6, 4, 1);
    structCoins(173, 8, 6, 1);
    struckBlockQ(102, 9, 1);
    lMap[102][9]->setSpawnMushroom(true);
    structCastleSmall(0, 2);
    structGND2(225, 2, 1, 1);
    structEnd(225, 3, 9);
    structCastleBig(228, 2);
    structCastleWall(237, 2, 13, 6);
    // -- END
    this->iLevelType = 0;
}
void Map::loadLVL_7_4() {
    clearMap();
    this->iMapWidth = 300;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_7_4();
    structGND(0, 0, 16, 5);
    structGND(0, 5, 5, 1);
    structGND(0, 6, 4, 1);
    structGND(0, 7, 3, 1);
    structGND(27, 0, 5, 5);
    structGND(32, 0, 61, 2);
    structGND(93, 2, 3, 3);
    structGND(34, 5, 16, 1);
    structGND(36, 6, 14, 1);
    structGND(37, 7, 13, 1);
    structGND(38, 8, 12, 1);
    structGND(54, 4, 13, 1);
    structGND(55, 8, 11, 1);
    structGND(70, 5, 18, 1);
    structGND(70, 6, 19, 1);
    structGND(70, 7, 20, 1);
    structGND(70, 8, 22, 1);
    structGND(96, 0, 68, 2);
    structGND(157, 2, 3, 3);
    structGND(98, 5, 16, 1);
    structGND(100, 6, 14, 1);
    structGND(101, 7, 13, 1);
    structGND(102, 8, 12, 1);
    structGND(118, 4, 13, 1);
    structGND(119, 8, 11, 1);
    structGND(134, 5, 18, 1);
    structGND(134, 6, 19, 1);
    structGND(134, 7, 20, 1);
    structGND(134, 8, 22, 1);
    structGND(157, 2, 3, 3);
    structGND(163, 8, 2, 1);
    structGND(166, 8, 2, 1);
    structGND(93, 0, 3, 2);
    structGND(0, 10, 16, 3);
    structGND(16, 12, 180, 1);
    structGND(28, 10, 4, 2);
    structLava(164, 0, 3, 2);
    structLava(16, 0, 11, 2);
    vPlatform.push_back(new Platform(4, 5, 18*32, 18*32, screen_height - 16 - 7*32, screen_height - 16 - 7*32, 18*32, (float)screen_height - 16 - 7*32, true));
    vPlatform.push_back(new Platform(4, 5, 22*32, 22*32, screen_height - 16 - 6*32, screen_height - 16 - 6*32, 22*32, (float)screen_height - 16 - 6*32, true));
    lMap[167][7]->setBlockID(56);
    // -- TP
    for(int i = 0; i < 3; i++) {
        lMap[60][5 + i]->setBlockID(171);
        lMap[124][5 + i]->setBlockID(171);
    }
    for(int i = 0; i < 3; i++) {
        lMap[78][9 + i]->setBlockID(169);
    }
    for(int i = 0; i < 3; i++) {
        lMap[138][9 + i]->setBlockID(169);
    }
    for(int i = 0; i < 3; i++) {
        lMap[138][2 + i]->setBlockID(170);
    }
    // -- TP
    structGND(167, 0, 1, 2);
    structLava(168, 0, 3, 2);
    structGND(168, 8, 1, 1);
    structGND(170, 8, 4, 1);
    structGND(174, 5, 6, 4);
    structGND(171, 0, 85, 2);
    structGND(182, 8, 3, 1);
    structGND(183, 4, 3, 1);
    structGND(187, 8, 7, 1);
    structGND(188, 4, 7, 1);
    structGND(196, 8, 3, 1);
    structGND(197, 4, 3, 1);
    structGND(201, 5, 1, 4);
    structGND(203, 2, 1, 1);
    structGND(204, 2, 1, 2);
    structGND(205, 2, 19, 3);
    structGND(202, 8, 15, 1);
    structGND(227, 2, 3, 3);
    structGND(232, 2, 8, 3);
    structGND(242, 2, 2, 3);
    structGND(246, 2, 2, 3);
    structGND(250, 2, 6, 3);
    structGND(196, 12, 94, 1);
    structBridge(256, 4, 13);
    structLava(256, 0, 13, 2);
    structGND(269, 0, 3, 6);
    structGND(270, 9, 2, 3);
    structGND(272, 0, 18, 2);
}
void Map::loadLVL_8_1() {
    clearMap();
    this->iMapWidth = 440;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_8_1();
    loadPipeEventsLVL_8_1();
    structGND(0, 0, 46, 2);
    structGND(47, 0, 1, 2);
    structGND(49, 0, 2, 2);
    structGND(52, 0, 2, 2);
    structGND(55, 0, 2, 2);
    structGND(58, 0, 111, 2);
    structGND(170, 0, 1, 2);
    structGND(172, 0, 2, 2);
    structGND(175, 0, 1, 2);
    structGND(177, 0, 2, 2);
    structGND(180, 0, 17, 2);
    structGND(198, 0, 1, 2);
    structGND(200, 0, 1, 2);
    structGND(202, 0, 19, 2);
    structGND(227, 0, 10, 2);
    structGND(238, 0, 2, 2);
    structGND(242, 0, 2, 2);
    structGND(246, 0, 44, 2);
    structGND(293, 0, 2, 2);
    structGND(298, 0, 16, 2);
    structGND(319, 0, 1, 2);
    structGND(324, 0, 35, 2);
    structGND(360, 0, 1, 2);
    structGND(362, 0, 1, 2);
    structGND(364, 0, 1, 2);
    structGND(366, 0, 34, 2);
    structCastleBig(-2, 2);
    structCastleSmall(380, 2);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structCloud(222, 10, 2);
    structCloud(237, 11, 1);
    structCloud(240, 10, 2);
    structCloud(258, 10, 1);
    structCloud(267, 11, 1);
    structCloud(270, 10, 2);
    structCloud(285, 11, 1);
    structCloud(288, 10, 2);
    structCloud(306, 10, 1);
    structCloud(315, 11, 1);
    structCloud(318, 10, 2);
    structCloud(333, 11, 1);
    structCloud(336, 10, 2);
    structCloud(354, 10, 1);
    structCloud(363, 11, 1);
    structCloud(366, 10, 2);
    structCloud(381, 11, 1);
    structCloud(384, 10, 2);
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(23, 2, 1, false);
    structTree(24, 2, 1, false);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(88, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(107, 2, 1, false);
    structTree(109, 2, 1, false);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(139, 2, 1, true);
    structTree(155, 2, 1, false);
    structTree(157, 2, 1, true);
    structTree(165, 2, 1, true);
    structTree(167, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(184, 2, 1, false);
    structTree(187, 2, 1, true);
    structTree(379, 2, 1, true);
    structTree(357, 2, 1, true);
    structTree(349, 2, 1, true);
    structTree(347, 2, 1, false);
    structTree(331, 2, 1, true);
    structTree(328, 2, 1, false);
    structTree(309, 2, 1, true);
    structTree(311, 2, 1, false);
    structTree(312, 2, 1, false);
    structTree(299, 2, 1, false);
    structTree(301, 2, 1, true);
    structTree(283, 2, 1, true);
    structTree(261, 2, 1, true);
    structTree(263, 2, 1, false);
    structTree(264, 2, 1, false);
    structTree(253, 2, 1, true);
    structTree(251, 2, 1, false);
    structTree(232, 2, 1, false);
    structTree(235, 2, 1, true);
    structTree(213, 2, 1, true);
    structTree(215, 2, 1, false);
    structTree(216, 2, 1, false);
    structTree(203, 2, 1, false);
    structTree(205, 2, 1, true);
    structFence(185, 2, 1);
    structFence(182, 2, 2);
    structFence(158, 2, 4);
    structFence(134, 2, 2);
    structFence(137, 2, 1);
    structFence(110, 2, 4);
    structFence(86, 2, 2);
    structFence(89, 2, 1);
    structFence(62, 2, 4);
    structFence(38, 2, 2);
    structFence(41, 2, 1);
    structFence(14, 2, 4);
    structFence(206, 2, 4);
    structFence(230, 2, 2);
    structFence(233, 2, 1);
    structFence(254, 2, 4);
    structFence(281, 2, 1);
    structFence(302, 2, 4);
    structFence(326, 2, 2);
    structFence(329, 2, 1);
    structFence(350, 2, 4);
    structFence(374, 2, 4);
    structPipe(35, 2, 3);
    structPipe(76, 2, 3);
    structPipe(82, 2, 2);
    structPipe(94, 2, 3);
    structPipe(104, 2, 3);
    structPipe(115, 2, 1);
    structPipe(140, 2, 2);
    struckBlockQ2(80, 6, 1);
    lMap[80][6]->setSpawnMushroom(true);
    struckBlockQ2(158, 5, 1);
    structCoins(64, 6, 1, 1);
    structCoins(89, 6, 1, 1);
    structCoins(98, 6, 1, 1);
    structCoins(109, 10, 2, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(184, 6, 8, 1);
    structBrick(154, 9, 8, 1);
    lMap[158][9]->setNumOfUse(8);
    structBrick(184, 6, 8, 1);
    lMap[186][6]->setSpawnStar(true);
    structGND2(153, 2, 1, 4);
    structGND2(163, 2, 1, 4);
    structGND2(303, 2, 1, 2);
    structGND2(307, 2, 1, 2);
    structGND2(210, 2, 1, 2);
    structGND2(360, 2, 1, 2);
    structGND2(362, 2, 1, 4);
    structGND2(364, 2, 1, 6);
    structGND2(366, 2, 2, 8);
    structGND2(275, 2, 6, true);
    structPipe(344, 2, 2);
    structPipe(355, 2, 1);
    structPipe(246, 2, 4);
    structPipe(242, 2, 3);
    structPipe(238, 2, 2);
    structCoins(223, 6, 2, 1);
    structCoins(283, 6, 2, 1);
    structCoins(291, 6, 1, 1);
    structCoins(296, 6, 1, 1);
    structCoins(316, 6, 2, 1);
    structCoins(321, 6, 2, 1);
    structGND2(376, 2, 1, 1);
    structEnd(376, 3, 9);
    this->iLevelType = 1;
    structGND(410, 0, 17, 2);
    structBrick(410, 2, 1, 11);
    structBrick(413, 5, 10, 1);
    structBrick(413, 9, 10, 4);
    structBrick(423, 4, 2, 9);
    lMap[422][5]->setNumOfUse(10);
    structCoins(413, 2, 9, 1);
    structCoins(414, 6, 8, 1);
    structPipeVertical(425, 2, 10);
    structPipeHorizontal(423, 2, 1);
    this->iLevelType = 0;
}
void Map::loadLVL_8_2() {
    clearMap();
    this->iMapWidth = 400;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_8_2();
    loadPipeEventsLVL_8_2();
    structBulletBill(85, 2, 0);
    structBulletBill(93, 3, 0);
    structBulletBill(93, 1, 0);
    structBulletBill(105, 2, 0);
    structBulletBill(115, 1, 0);
    structBulletBill(119, 6, 0);
    structBulletBill(125, 2, 1);
    structBulletBill(175, 2, 0);
    structBulletBill(191, 1, 0);
    structBulletBill(191, 3, 0);
    structGND(0, 0, 15, 2);
    structGND(16, 0, 5, 2);
    structGND(22, 0, 14, 2);
    structGND(37, 0, 8, 2);
    structGND(46, 0, 4, 2);
    structGND(51, 0, 1, 2);
    structGND(53, 0, 3, 2);
    structGND(57, 0, 6, 2);
    structGND(64, 0, 14, 2);
    structGND(80, 0, 4, 2);
    structGND(85, 0, 53, 2);
    structGND(139, 0, 5, 2);
    structGND(145, 0, 1, 2);
    structGND(147, 0, 1, 2);
    structGND(154, 0, 20, 2);
    structGND(175, 0, 1, 2);
    structGND(179, 0, 23, 2);
    structGND(203, 0, 1, 2);
    structGND(206, 0, 31, 2);
    structCloud(0, 10, 2);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 1);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structCloud(222, 10, 2);
    structFence(14, 2, 1);
    structFence(16, 2, 1);
    structFence(38, 2, 4);
    structFence(62, 2, 1);
    structFence(64, 2, 2);
    structFence(86, 2, 4);
    structFence(110, 2, 4);
    structFence(134, 2, 4);
    structFence(158, 2, 4);
    structFence(208, 2, 2);
    structFence(230, 2, 2);
    structCastleSmall(0, 2);
    structCastleSmall(220, 2);
    structTree(232, 2, 1, false);
    structTree(215, 2, 1, false);
    structTree(213, 2, 1, true);
    structTree(187, 2, 1, true);
    structTree(165, 2, 1, true);
    structTree(167, 2, 1, false);
    structTree(168, 2, 1, false);
    structTree(155, 2, 1, false);
    structTree(136, 2, 1, false);
    structTree(139, 2, 1, true);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(107, 2, 1, false);
    structTree(109, 2, 1, true);
    structTree(88, 2, 1, false);
    structTree(91, 2, 1, true);
    structTree(69, 2, 1, true);
    structTree(71, 2, 1, false);
    structTree(72, 2, 1, false);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(40, 2, 1, false);
    structTree(43, 2, 1, true);
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structGND2(206, 2, 2, 8);
    structGND2(203, 2, 1, 5);
    structGND2(199, 2, 3, true);
    structGND2(182, 2, 5, true);
    structPipe(163, 2, 1);
    structPipe(156, 2, 3);
    structPipe(142, 2, 1);
    structPipe(131, 2, 1);
    structBrick(118, 5, 1, 1);
    structBrick(120, 5, 1, 1);
    lMap[120][5]->setNumOfUse(8);
    structGND2(119, 5, 1, 1);
    structBrick(110, 5, 2, 1);
    structBrick(99, 5, 2, 1);
    lMap[100][5]->setSpawnMushroom(true);
    structBrick(77, 5, 2, 1);
    structBrick(43, 9, 33, 1);
    lMap[44][9]->setSpawnMushroom(true);
    lMap[44][9]->setPowerUP(false);
    addSpring(44*32, 336);
    struckBlockQ(29, 5, 4);
    structGND2(17, 2, 4, true);
    structGND2(22, 2, 1, 6);
    structGND2(23, 2, 1, 7);
    structGND2(24, 2, 2, 8);
    structGND2(216, 2, 1, 1);
    structEnd(216, 3, 9);
    // -- MAP 8-2-2
    this->iLevelType = 1;
    structGND(240, 0, 17, 2);
    structBrick(239, 2, 1, 18);
    structBrick(240, 2, 1, 11);
    structBrick(244, 7, 1, 2);
    structBrick(245, 7, 5, 1);
    structBrick(250, 7, 1, 6);
    structBrick(251, 8, 2, 1);
    structBrick(244, 12, 6, 1);
    structBrick(254, 7, 1, 1);
    lMap[254][7]->setNumOfUse(8);
    structCoins(245, 8, 5, 2);
    structPipeVertical(255, 2, 10);
    structPipeHorizontal(253, 2, 1);
    this->iLevelType = 0;
}
void Map::loadLVL_8_3() {
    clearMap();
    this->iMapWidth = 240;
    this->iMapHeight = 25;
    this->iLevelType = 0;
    this->iMapTime = 300;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_8_3();
    structGND(0, 0, 69, 2);
    structGND(71, 0, 4, 2);
    structGND(77, 0, 47, 2);
    structGND(126, 0, 2, 2);
    structGND(130, 0, 67, 2);
    structGND(208, 0, 31, 2);
    structBulletBill(18, 2, 0);
    structBulletBill(34, 2, 1);
    structBulletBill(86, 2, 0);
    structBulletBill(86, 2, 0);
    structBulletBill(86, 2, 0);
    structBulletBill(86, 2, 0);
    structBulletBill(86, 2, 0);
    structCloud(0, 10, 2);
    structCloud(18, 10, 1);
    structCloud(27, 11, 1);
    structCloud(30, 10, 2);
    structCloud(45, 11, 1);
    structCloud(48, 10, 2);
    structCloud(66, 10, 1);
    structCloud(75, 11, 1);
    structCloud(78, 10, 2);
    structCloud(93, 11, 1);
    structCloud(96, 10, 2);
    structCloud(114, 10, 1);
    structCloud(123, 11, 1);
    structCloud(126, 10, 2);
    structCloud(141, 11, 1);
    structCloud(144, 10, 2);
    structCloud(162, 10, 1);
    structCloud(171, 11, 1);
    structCloud(174, 10, 2);
    structCloud(189, 11, 1);
    structCloud(192, 10, 2);
    structCloud(210, 10, 1);
    structCloud(219, 11, 1);
    structCastleSmall(0, 2);
    structTree(11, 2, 1, false);
    structTree(13, 2, 1, true);
    structTree(21, 2, 1, true);
    structTree(23, 2, 1, false);
    structTree(59, 2, 1, false);
    structTree(61, 2, 1, true);
    structTree(107, 2, 1, false);
    structTree(117, 2, 1, true);
    structTree(119, 2, 1, false);
    structTree(120, 2, 1, false);
    structTree(167, 2, 1, false);
    structTree(213, 2, 1, true);
    structTree(215, 2, 1, false);
    structTree(216, 2, 1, false);
    structGND2(195, 2, 1, 2);
    structGND2(198, 3, 1, 1);
    structGND2(200, 5, 1, 1);
    structGND2(202, 7, 1, 1);
    structGND2(204, 9, 2, 1);
    structGND2(95, 2, 1, 3);
    structGND2(109, 2, 2, 4);
    structGND2(71, 2, 4, false);
    structBrick(115, 5, 8, 1);
    structBrick(115, 9, 8, 1);
    lMap[116][9]->setSpawnMushroom(true);
    structBrick(60, 5, 8, 1);
    structBrick(60, 9, 8, 1);
    lMap[66][9]->setSpawnMushroom(true);
    structFence(208, 2, 2);
    structFence(87, 2, 1);
    structFence(62, 2, 4);
    structFence(14, 2, 4);
    structFence(111, 2, 3);
    structPipe(53, 2, 3);
    structPipe(126, 2, 3);
    structPipe(168, 2, 2);
    structCastleWall(24, 2, 8, 6);
    structCastleWall(37, 2, 14, 6);
    structCastleWall(79, 2, 6, 6);
    structCastleWall(88, 2, 6, 6);
    structCastleWall(97, 2, 10, 6);
    structCastleWall(132, 2, 34, 6);
    structCastleWall(172, 2, 20, 6);
    structBrick(190, 5, 1, 1);
    lMap[190][5]->setNumOfUse(10);
    structCastleBig(220, 2);
    structCastleWall(229, 2, 10, 6);
    structGND2(214, 2, 1, 1);
    structEnd(214, 3, 9);
}
void Map::loadLVL_8_4() {
    clearMap();
    this->iMapWidth = 480;
    this->iMapHeight = 25;
    this->iLevelType = 3;
    this->iMapTime = 400;
    // ---------- LOAD LISTS ----------
    createMap();
    loadMinionsLVL_8_4();
    loadPipeEventsLVL_8_4();
    structGND(0, 0, 369, 2);
    structGND(0, 12, 369, 1);
    structGND(0, 0, 6, 5);
    structGND(0, 5, 5, 1);
    structGND(0, 6, 4, 1);
    structGND(0, 7, 3, 1);
    structLava(6, 0, 5, 2);
    structGND(11, 0, 55, 2);
    structLava(66, 0, 9, 2);
    structGND(62, 2, 4, 3);
    structGND(75, 0, 21, 5);
    structGND(76, 8, 4, 1);
    structPipe(19, 0, 3);
    structPipe(51, 0, 3);
    structPipe(81, 0, 6);
    structPipe(90, 0, 7);
    structPipe(126, 0, 3);
    structPipe(133, 0, 4);
    structPipe(143, 0, 3);
    structPipe(153, 0, 4);
    structPipe(215, 0, 3);
    structLava(155, 0, 3, 2);
    struckBlockQ2(161, 5, 1);
    structGND(163, 5, 2, 1);
    structPipe(163, 6, 2);
    vPlatform.push_back(new Platform(4, 3, 68*32 - 16, 71*32 + 16, screen_height - 16 - 32, screen_height - 16 - 32, 68*32 - 16, (float)screen_height - 16 - 30, true));
    for(int i = 0; i < 10; i++) {
        lMap[110][2 + i]->setBlockID(169);
    }
    for(int i = 0; i < 10; i++) {
        lMap[191][2 + i]->setBlockID(170);
    }
    structGND(218, 2, 22, 3);
    structGND(244, 2, 16, 3);
    structLava(240, 0, 4, 2);
    for(int i = 0; i < 10; i++) {
        lMap[274][2 + i]->setBlockID(171);
    }
    structPipe(302, 0, 3);
    structPipe(224, 0, 6);
    structPipe(232, 0, 7);
    structPipe(248, 0, 6);
    structPipe(309, 0, 3);
    structLava(320, 0, 5, 2);
    structGND(325, 2, 6, 3);
    structGND(325, 10, 6, 2);
    structBridge(331, 4, 13);
    structLava(331, 0, 13, 2);
    structGND(344, 2, 3, 4);
    structGND(345, 9, 2, 3);
    this->iLevelType = 2;
    structWater(392, 2, 67, 11);
    structPipeHorizontal(458, 6, 1);
    this->iLevelType = 7;
    structGND(385, 0, 7, 13);
    structGND(392, 0, 79, 2);
    structGND(396, 2, 12, 3);
    structGND(401, 5, 7, 1);
    structGND(396, 10, 12, 3);
    structGND(401, 9, 7, 1);
    structGND(408, 12, 63, 1);
    structGND(429, 2, 3, 3);
    structGND(429, 9, 3, 3);
    structGND(457, 2, 14, 4);
    structGND(459, 6, 12, 3);
    structGND(457, 9, 14, 3);
    structPipe(393, 0, 3);
    this->iLevelType = 3;
}

// ----- POS 0 = TOP, 1 = BOT
bool Map::blockUse(int nX, int nY, int iBlockID, int POS) {
    if(POS == 0) {
        switch(iBlockID) {
            case 8: case 55: // ----- BlockQ
                if(lMap[nX][nY]->getSpawnMushroom()) {
                    if(lMap[nX][nY]->getPowerUP()) {
                        if(oPlayer->getPowerLVL() == 0) {
                            lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, screen_height - 16 - 32 * nY, true, nX, nY));
                        } else {
                            lMinion[getListID(32 * nX)].push_back(new Flower(32 * nX, screen_height - 16 - 32 * nY, nX, nY));
                        }
                    } else {
                        lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, screen_height - 16 - 32 * nY, false, nX, nY));
                    }
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
                } else {
                    lCoin.push_back(new Coin(nX * 32 + 7, screen_height - nY * 32 - 48));
                    oPlayer->setScore(oPlayer->getScore() + 200);
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
                    oPlayer->setCoins(oPlayer->getCoins() + 1);
                }
                if(lMap[nX][nY]->getNumOfUse() > 1) {
                    lMap[nX][nY]->setNumOfUse(lMap[nX][nY]->getNumOfUse() - 1);
                } else {
                    lMap[nX][nY]->setNumOfUse(0);
                    lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 56);
                }
                lMap[nX][nY]->startBlockAnimation();
                checkCollisionOnTopOfTheBlock(nX, nY);
                break;
            case 13: case 28: case 81: // ----- Brick
                if(lMap[nX][nY]->getSpawnStar()) {
                    lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
                    lMinion[getListID(32 * nX)].push_back(new Star(32 * nX, screen_height - 16 - 32 * nY, nX, nY));
                    lMap[nX][nY]->startBlockAnimation();
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
                } else if(lMap[nX][nY]->getSpawnMushroom()) {
                    lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
                    if(lMap[nX][nY]->getPowerUP()) {
                        if(oPlayer->getPowerLVL() == 0) {
                            lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, screen_height - 16 - 32 * nY, true, nX, nY));
                        } else {
                            lMinion[getListID(32 * nX)].push_back(new Flower(32 * nX, screen_height - 16 - 32 * nY, nX, nY));
                        }
                    } else {
                        lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, screen_height - 16 - 32 * nY, false, nX, nY));
                    }
                    lMap[nX][nY]->startBlockAnimation();
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
                } else if(lMap[nX][nY]->getNumOfUse() > 0) {
                    lCoin.push_back(new Coin(nX * 32 + 7, screen_height - nY * 32 - 48));
                    oPlayer->setScore(oPlayer->getScore() + 200);
                    oPlayer->setCoins(oPlayer->getCoins() + 1);
                    lMap[nX][nY]->setNumOfUse(lMap[nX][nY]->getNumOfUse() - 1);
                    if(lMap[nX][nY]->getNumOfUse() == 0) {
                        lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
                    }
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
                    lMap[nX][nY]->startBlockAnimation();
                } else {
                    if(oPlayer->getPowerLVL() > 0) {
                        lMap[nX][nY]->setBlockID(0);
                        lBlockDebris.push_back(new BlockDebris(nX * 32, screen_height - 48 - nY * 32));
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKBREAK);
                    } else {
                        lMap[nX][nY]->startBlockAnimation();
                        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
                    }
                }
                checkCollisionOnTopOfTheBlock(nX, nY);
                break;
            case 24: // ----- BlockQ2
                if(lMap[nX][nY]->getSpawnMushroom()) {
                    if(lMap[nX][nY]->getPowerUP()) {
                        if(oPlayer->getPowerLVL() == 0) {
                            lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, screen_height - 16 - 32 * nY, true, nX, nY));
                        } else {
                            lMinion[getListID(32 * nX)].push_back(new Flower(32 * nX, screen_height - 16 - 32 * nY, nX, nY));
                        }
                    } else {
                        lMinion[getListID(32 * nX)].push_back(new Mushroom(32 * nX, screen_height - 16 - 32 * nY, false, nX, nY));
                    }
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMAPPER);
                } else {
                    lCoin.push_back(new Coin(nX * 32 + 7, screen_height - nY * 32 - 48));
                    oPlayer->setCoins(oPlayer->getCoins() + 1);
                    oPlayer->setScore(oPlayer->getScore() + 200);
                    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
                    lMap[nX][nY]->startBlockAnimation();
                }
                lMap[nX][nY]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 9 : iLevelType == 1 ? 56 : 80);
                checkCollisionOnTopOfTheBlock(nX, nY);
                break;
            case 128: case 129:
                spawnVine(nX, nY, iBlockID);
                lMap[nX][nY]->setBlockID(iBlockID == 128 ? 9 : 56);
                lMap[nX][nY]->startBlockAnimation();
                break;
            default:
                break;
        }
    } else if(POS == 1) {
        switch(iBlockID) {
            case 21: case 23: case 31: case 33: case 98: case 100: case 113: case 115: case 137: case 139: case 177: case 179: // Pipe
                pipeUse();
                break;
            case 40: case 41: case 123: case 124: case 182: // End
                EndUse();
                break;
            case 82:
                EndBoss();
                break;
            default:
                break;
        }
    }
    switch(iBlockID) {
        case 29: case 71: case 72: case 73:// COIN
            lMap[nX][nY]->setBlockID(iLevelType == 2 ? 94 : 0);
            oPlayer->addCoin();
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
            return false;
            break;
        case 36: case 38: case 60: case 62: case 103: case 105: case 118: case 120: // Pipe
            pipeUse();
            break;
        case 127: // BONUS END
            EndBonus();
            break;
        case 169:
            TPUse();
            break;
        case 170:
            TPUse2();
            break;
        case 171:
            TPUse3();
            break;
        default:
            break;
    }
    return true;
}
void Map::pipeUse() {
    for(unsigned int i = 0; i < lPipe.size(); i++) {
        lPipe[i]->checkUse();
    }
}
void Map::EndUse() {
    inEvent = true;
    oEvent->resetData();
    oPlayer->resetJump();
    oPlayer->stopMove();
    oEvent->newUnderWater = false;
    CCFG::getMusic()->StopMusic();
    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cLEVELEND);
    oEvent->eventTypeID = oEvent->eEnd;
    if(oPlayer->getYPos() < screen_height - 16 - 10*32) {
        oFlag->iPoints = 5000;
    } else if(oPlayer->getYPos() < screen_height - 16 - 8*32) {
        oFlag->iPoints = 2000;
    } else if(oPlayer->getYPos() < screen_height - 16 - 6*32) {
        oFlag->iPoints = 500;
    } else if(oPlayer->getYPos() < screen_height - 16 - 4*32) {
        oFlag->iPoints = 200;
    } else {
        oFlag->iPoints = 100;
    }
    oEvent->vOLDDir.push_back(oEvent->eRIGHTEND);
    oEvent->vOLDLength.push_back(oPlayer->getHitBoxX());
    oEvent->vOLDDir.push_back(oEvent->eENDBOT1);
    oEvent->vOLDLength.push_back((screen_height - 16 - 32 * 2) - oPlayer->getYPos() - oPlayer->getHitBoxY() - 2);
    oEvent->vOLDDir.push_back(oEvent->eENDBOT2);
    oEvent->vOLDLength.push_back((screen_height - 16 - 32 * 2) - oPlayer->getYPos() - oPlayer->getHitBoxY() - 2);
    oEvent->vOLDDir.push_back(oEvent->eRIGHTEND);
    oEvent->vOLDLength.push_back(oPlayer->getHitBoxX());
    oEvent->vOLDDir.push_back(oEvent->eBOTRIGHTEND);
    oEvent->vOLDLength.push_back(32);
    oEvent->vOLDDir.push_back(oEvent->eRIGHTEND);
    oEvent->vOLDLength.push_back(132);
    oEvent->iSpeed = 3;
    switch(currentLevelID) {
        case 0: {
            oEvent->newLevelType = 100;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = 1;
            oEvent->inEvent = true;
            oEvent->newMapXPos = -210*32;
            oEvent->newPlayerXPos = 64;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = false;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(204);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(204);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(205);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(205);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 1: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = 2;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(309);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(309);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(310);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(310);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 2: {
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = 3;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oEvent->reDrawX.push_back(159);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(159);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(160);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(160);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 4: {
            oEvent->newLevelType = 100;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = 5;
            oEvent->inEvent = true;
            oEvent->newMapXPos = -220*32;
            oEvent->newPlayerXPos = 64;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = false;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(206);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(206);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(207);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(207);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 5: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = 6;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oEvent->reDrawX.push_back(299);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(299);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(300);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(300);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 6:
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oEvent->reDrawX.push_back(232);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(232);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(234);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(234);
            oEvent->reDrawY.push_back(3);
            break;
        case 8:
            oEvent->newLevelType = 4;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(206);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(206);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(207);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(207);
            oEvent->reDrawY.push_back(3);
            break;
        case 9:
            oEvent->newLevelType = 4;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(215);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(215);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(216);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(216);
            oEvent->reDrawY.push_back(3);
            break;
        case 10:
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(158);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(158);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(159);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(159);
            oEvent->reDrawY.push_back(3);
            break;
        case 12: {
            oEvent->newLevelType = 100;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = true;
            oEvent->newMapXPos = -240*32;
            oEvent->newPlayerXPos = 64;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = false;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(231);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(231);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(232);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(232);
            oEvent->reDrawY.push_back(3);
            break;
        case 13:
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(419);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(419);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(420);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(420);
            oEvent->reDrawY.push_back(3);
            break;
        case 14:
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(154);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(154);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(155);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(155);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 16: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = -80*32;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(205);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(205);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(206);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(206);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 17: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(286);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(286);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(287);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(287);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 18: {
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(159);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(159);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(160);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(160);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 20: {
            oEvent->newLevelType = 4;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = -85*32;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(96);
            oFlag->castleFlagExtraXPos = 96;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(194);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(194);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(195);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(195);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 21: {
            oEvent->newLevelType = 4;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(307);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(307);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(308);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(308);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 22: {
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(174);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(174);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(175);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(175);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 24: {
            oEvent->newLevelType = 100;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = true;
            oEvent->newMapXPos = -220*32;
            oEvent->newPlayerXPos = 64;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = false;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(185);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(185);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(186);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(186);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 25: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oEvent->reDrawX.push_back(299);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(299);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(300);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(300);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 26:
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(32);
            oFlag->castleFlagExtraXPos = 32;
            oEvent->reDrawX.push_back(232);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(232);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(233);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(233);
            oEvent->reDrawY.push_back(3);
            break;
        case 28: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(382);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(382);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(383);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(383);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 29: {
            oEvent->newLevelType = 0;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->reDrawX.push_back(222);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(222);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(223);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(223);
            oEvent->reDrawY.push_back(3);
            break;
        }
        case 30: {
            oEvent->newLevelType = 3;
            oEvent->iDelay = 1500;
            oEvent->newCurrentLevel = currentLevelID + 1;
            oEvent->inEvent = false;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 84;
            oEvent->newPlayerYPos = 150;
            oEvent->newMoveMap = true;
            oPlayer->setMoveDirection(true);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(128);
            oFlag->castleFlagExtraXPos = 128;
            oEvent->reDrawX.push_back(224);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(224);
            oEvent->reDrawY.push_back(3);
            oEvent->reDrawX.push_back(225);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(225);
            oEvent->reDrawY.push_back(3);
            break;
        }
    }
    oEvent->vOLDDir.push_back(oEvent->eENDPOINTS);
    oEvent->vOLDLength.push_back(iMapTime);
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(128);
}
void Map::EndBoss() {
    inEvent = true;
    oEvent->resetData();
    oPlayer->resetJump();
    oPlayer->stopMove();
    oEvent->endGame = false;
    switch(currentLevelID) {
        case 31:
            oEvent->endGame = true;
            break;
        default:
            CCFG::getMusic()->StopMusic();
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCASTLEEND);
            break;
    }
    oEvent->eventTypeID = oEvent->eBossEnd;
    oEvent->iSpeed = 3;
    oEvent->newLevelType = 0;
    oEvent->newCurrentLevel = currentLevelID + 1;
    oEvent->newMoveMap = true;
    oEvent->iDelay = 500;
    oEvent->inEvent = false;
    oEvent->newMapXPos = 0;
    oEvent->newPlayerXPos = 64;
    oEvent->newPlayerYPos = screen_height - 48 - oPlayer->getHitBoxY();
    oEvent->newMoveMap = true;
    switch(currentLevelID) {
        case 7:
            oEvent->newLevelType = 4;
            break;
        case 19:
            oEvent->newLevelType = 4;
            break;
    }
    bool addOne = false;
    if(lMap[getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))][6]->getBlockID() == 82) {
        oEvent->vOLDDir.push_back(oEvent->eBOT);
        oEvent->vOLDLength.push_back(screen_height - 16 - 5*32 - (oPlayer->getYPos() + oPlayer->getHitBoxY()));
    } else {
        oEvent->vOLDDir.push_back(oEvent->eBOTRIGHTEND);
        oEvent->vOLDLength.push_back(screen_height - 16 - 5*32 - (oPlayer->getYPos() + oPlayer->getHitBoxY()));
        oEvent->vOLDDir.push_back(oEvent->eRIGHT);
        oEvent->vOLDLength.push_back(32 - screen_height - 16 - 5*32 - (oPlayer->getYPos() + oPlayer->getHitBoxY()));
        addOne = true;
    }
    oEvent->vOLDDir.push_back(oEvent->eBOSSEND1);
    oEvent->vOLDLength.push_back(1);
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(10);
    oEvent->vOLDDir.push_back(oEvent->eBOSSEND2);
    oEvent->vOLDLength.push_back(1);
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(3);
    for(int i = 0; i < 6; i++) {
        oEvent->vOLDDir.push_back(oEvent->eBOSSEND3);
        oEvent->vOLDLength.push_back(2 + i);
        oEvent->vOLDDir.push_back(oEvent->eNOTHING);
        oEvent->vOLDLength.push_back(3);
    }
    oEvent->vOLDDir.push_back(oEvent->eBOSSEND4);
        oEvent->vOLDLength.push_back(1);
    for(int i = 6; i < 12; i++) {
        oEvent->vOLDDir.push_back(oEvent->eBOSSEND3);
        oEvent->vOLDLength.push_back(2 + i);
        oEvent->vOLDDir.push_back(oEvent->eNOTHING);
        oEvent->vOLDLength.push_back(3);
    }
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(90);
    if(currentLevelID == 31) {
        CCFG::getMusic()->StopMusic();
        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPRINCESSMUSIC);
    }
    oEvent->vOLDDir.push_back(oEvent->eBOTRIGHTBOSS);
    oEvent->vOLDLength.push_back(8*32);
    switch(currentLevelID) {
        case 31:
            oEvent->vOLDDir.push_back(oEvent->eENDGAMEBOSSTEXT1);
            oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 7*32 + (addOne ? 32 : 0));
        break;
        default:
            oEvent->vOLDDir.push_back(oEvent->eBOSSTEXT1);
            oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 7*32 + (addOne ? 32 : 0));
        break;
    }
    oEvent->vOLDDir.push_back(oEvent->eRIGHT);
    oEvent->vOLDLength.push_back(2*32 + 16);
    oEvent->vOLDDir.push_back(oEvent->eMARIOSPRITE1);
    oEvent->vOLDLength.push_back(1);
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(90);
    switch(currentLevelID) {
        case 31:
            oEvent->vOLDDir.push_back(oEvent->eENDGAMEBOSSTEXT2);
            oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 5*32 + (addOne ? 32 : 0) + 28);
            break;
        default:
            oEvent->vOLDDir.push_back(oEvent->eBOSSTEXT2);
            oEvent->vOLDLength.push_back(getBlockIDX((int)(oPlayer->getXPos() + oPlayer->getHitBoxX()/2 - fXPos))*32 + 5*32 + (addOne ? 32 : 0));
            break;
    }
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(300 + (currentLevelID == 31 ? 100 : 0));
    switch(currentLevelID) {
        case 31:
            oEvent->vOLDDir.push_back(oEvent->eNOTHING);
            oEvent->vOLDLength.push_back(90);
        break;
    }
}
void Map::EndBonus() {
    inEvent = true;
    oEvent->resetData();
    oPlayer->resetJump();
    oPlayer->stopMove();
    oEvent->eventTypeID = oEvent->eNormal;
    oEvent->iSpeed = 3;
    oEvent->newLevelType = iLevelType;
    oEvent->newCurrentLevel = currentLevelID;
    oEvent->newMoveMap = true;
    oEvent->iDelay = 0;
    oEvent->inEvent = false;
    oEvent->newMoveMap = true;
    switch(currentLevelID) {
        case 4: {
            oEvent->newMapXPos = -158*32 + 16;
            oEvent->newPlayerXPos = 128;
            oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
            break;
        }
        case 8: {
            oEvent->newMapXPos = -158*32 + 16;
            oEvent->newPlayerXPos = 128;
            oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
            break;
        }
        case 17: {
            oEvent->newMapXPos = -207*32 + 16;
            oEvent->newPlayerXPos = 128;
            oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
            break;
        }
        case 21: {
            oEvent->newMapXPos = -243*32 + 16;
            oEvent->newPlayerXPos = 128;
            oEvent->newPlayerYPos = -oPlayer->getHitBoxY();
            break;
        }
    }
    oEvent->vOLDDir.push_back(oEvent->eNOTHING);
    oEvent->vOLDLength.push_back(1);
}
void Map::playerDeath(bool animation, bool instantDeath) {
    if((oPlayer->getPowerLVL() == 0 && !oPlayer->getUnkillAble()) || instantDeath) {
        inEvent = true;
        oEvent->resetData();
        oPlayer->resetJump();
        oPlayer->stopMove();
        oEvent->iDelay = 150;
        oEvent->newCurrentLevel = currentLevelID;
        oEvent->newMoveMap = bMoveMap;
        oEvent->eventTypeID = oEvent->eNormal;
        oPlayer->resetPowerLVL();
        if(animation) {
            oEvent->iSpeed = 4;
            oEvent->newLevelType = iLevelType;
            oPlayer->setYPos(oPlayer->getYPos() + 4.0f);
            oEvent->vOLDDir.push_back(oEvent->eDEATHNOTHING);
            oEvent->vOLDLength.push_back(30);
            oEvent->vOLDDir.push_back(oEvent->eDEATHTOP);
            oEvent->vOLDLength.push_back(64);
            oEvent->vOLDDir.push_back(oEvent->eDEATHBOT);
            oEvent->vOLDLength.push_back(screen_height - oPlayer->getYPos() + 128);
        } else {
            oEvent->iSpeed = 4;
            oEvent->newLevelType = iLevelType;
            oEvent->vOLDDir.push_back(oEvent->eDEATHTOP);
            oEvent->vOLDLength.push_back(1);
        }
        oEvent->vOLDDir.push_back(oEvent->eNOTHING);
        oEvent->vOLDLength.push_back(64);
        if(oPlayer->getNumOfLives() > 1) {
            oEvent->vOLDDir.push_back(oEvent->eLOADINGMENU);
            oEvent->vOLDLength.push_back(90);
            oPlayer->setNumOfLives(oPlayer->getNumOfLives() - 1);
            CCFG::getMusic()->StopMusic();
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cDEATH);
        } else {
            oEvent->vOLDDir.push_back(oEvent->eGAMEOVER);
            oEvent->vOLDLength.push_back(90);
            oPlayer->setNumOfLives(oPlayer->getNumOfLives() - 1);
            CCFG::getMusic()->StopMusic();
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cDEATH);
        }
    } else if(!oPlayer->getUnkillAble()) {
        oPlayer->setPowerLVL(oPlayer->getPowerLVL() - 1);
    }
}
void Map::startLevelAnimation() {
    oEvent->newUnderWater = false;
    switch(currentLevelID) {
        case 0:
            break;
        case 1:
            oEvent->resetData();
            oPlayer->resetJump();
            oPlayer->stopMove();
            oEvent->iSpeed = 2;
            oEvent->newLevelType = 1;
            oEvent->iDelay = 150;
            oEvent->newCurrentLevel = 1;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 96;
            oEvent->newPlayerYPos = 64;
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(7 * 32 + 4);
            oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
            oEvent->vOLDLength.push_back(1);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(1 * 32 - 2);
            oEvent->vOLDDir.push_back(oEvent->eNOTHING);
            oEvent->vOLDLength.push_back(75);
            oEvent->reDrawX.push_back(220);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(220);
            oEvent->reDrawY.push_back(3);
            break;
        case 5:
            oEvent->resetData();
            oPlayer->resetJump();
            oPlayer->stopMove();
            oEvent->iSpeed = 2;
            oEvent->newLevelType = 2;
            oEvent->iDelay = 150;
            oEvent->newCurrentLevel = 5;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 96;
            oEvent->newPlayerYPos = 64;
            oEvent->newMoveMap = true;
            oEvent->newUnderWater = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(7 * 32 + 4);
            oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
            oEvent->vOLDLength.push_back(1);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(1 * 32 - 2);
            oEvent->vOLDDir.push_back(oEvent->eNOTHING);
            oEvent->vOLDLength.push_back(75);
            oEvent->reDrawX.push_back(230);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(230);
            oEvent->reDrawY.push_back(3);
            break;
        case 13:
            oEvent->resetData();
            oPlayer->resetJump();
            oPlayer->stopMove();
            oEvent->iSpeed = 2;
            oEvent->newLevelType = 1;
            oEvent->iDelay = 150;
            oEvent->newCurrentLevel = currentLevelID;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 96;
            oEvent->newPlayerYPos = 64;
            oEvent->newMoveMap = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(7 * 32 + 4);
            oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
            oEvent->vOLDLength.push_back(1);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(1 * 32 - 2);
            oEvent->vOLDDir.push_back(oEvent->eNOTHING);
            oEvent->vOLDLength.push_back(75);
            oEvent->reDrawX.push_back(250);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(250);
            oEvent->reDrawY.push_back(3);
            break;
        case 25:
            oEvent->resetData();
            oPlayer->resetJump();
            oPlayer->stopMove();
            oEvent->iSpeed = 2;
            oEvent->newLevelType = 2;
            oEvent->iDelay = 150;
            oEvent->newCurrentLevel = 25;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 96;
            oEvent->newPlayerYPos = 64;
            oEvent->newMoveMap = true;
            oEvent->newUnderWater = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(7 * 32 + 4);
            oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
            oEvent->vOLDLength.push_back(1);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(1 * 32 - 2);
            oEvent->vOLDDir.push_back(oEvent->eNOTHING);
            oEvent->vOLDLength.push_back(75);
            oEvent->reDrawX.push_back(230);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(230);
            oEvent->reDrawY.push_back(3);
            break;
        case 26:
            oEvent->resetData();
            oPlayer->resetJump();
            oPlayer->stopMove();
            oEvent->iSpeed = 2;
            oEvent->newLevelType = 2;
            oEvent->iDelay = 150;
            oEvent->newCurrentLevel = 26;
            oEvent->newMapXPos = 0;
            oEvent->newPlayerXPos = 96;
            oEvent->newPlayerYPos = 64;
            oEvent->newMoveMap = true;
            oEvent->newUnderWater = true;
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(7 * 32 + 4);
            oEvent->vOLDDir.push_back(oEvent->ePLAYPIPERIGHT);
            oEvent->vOLDLength.push_back(1);
            oEvent->vOLDDir.push_back(oEvent->eRIGHT);
            oEvent->vOLDLength.push_back(1 * 32 - 2);
            oEvent->vOLDDir.push_back(oEvent->eNOTHING);
            oEvent->vOLDLength.push_back(75);
            oEvent->reDrawX.push_back(230);
            oEvent->reDrawY.push_back(2);
            oEvent->reDrawX.push_back(230);
            oEvent->reDrawY.push_back(3);
            break;
    }
}
void Map::spawnVine(int nX, int nY, int iBlockID) {
    if(iLevelType == 0 || iLevelType == 4) {
        addVine(nX, nY, 0, 34);
    } else {
        addVine(nX, nY, 0, 36);
    }
}
void Map::TPUse() {
    switch(currentLevelID) {
        case 27:
            if(bTP) {
                if(!(-fXPos > 90*32)) {
                    fXPos -= 64*32;
                    bTP = false;
                }
            } else {
                if(!(-fXPos < 90*32)) {
                    fXPos += 64*32;
                }
            }
            break;
        case 31:
            fXPos += 75*32;
            break;
    }
}
void Map::TPUse2() {
    switch(currentLevelID) {
        case 27:
            fXPos += 64*32;
            bTP = false;
            break;
        case 31:
            fXPos += 12*32;
            break;
    }
}
void Map::TPUse3() {
    switch(currentLevelID) {
        case 27:
            bTP = true;
            break;
        case 31:
            fXPos += 79*32;
            break;
    }
}

void Map::structBush(int X, int Y, int iSize) {
    // ----- LEFT & RIGHT
    for(int i = 0; i < iSize; i++) {
        lMap[X + i][Y + i]->setBlockID(5);
        lMap[X + iSize + 1 + i][Y + iSize - 1 - i]->setBlockID(6);
    }
    // ----- CENTER LEFT & RIGHT
    for(int i = 0, k = 1; i < iSize - 1; i++) {
        for(int j = 0; j < k; j++) {
            lMap[X + 1 + i][Y + j]->setBlockID((i%2 == 0 ? 3 : 4));
            lMap[X + iSize * 2 - 1 - i][Y + j]->setBlockID((i%2 == 0 ? 3 : 4));
        }
        ++k;
    }
    // ----- CENTER
    for(int i = 0; i < iSize; i++) {
        lMap[X + iSize][Y + i]->setBlockID((i%2 == 0 && iSize != 1 ? 4 : 3));
    }
    // ----- TOP
    lMap[X + iSize][Y + iSize]->setBlockID(7);
}
void Map::structGrass(int X, int Y, int iSize) {
    lMap[X][Y]->setBlockID(10);
    for(int i = 0; i < iSize; i++) {
        lMap[X + 1 + i][Y]->setBlockID(11);
    }
    lMap[X + iSize + 1][Y]->setBlockID(12);
}
void Map::structCloud(int X, int Y, int iSize) {
    // ----- LEFT
    lMap[X][Y]->setBlockID(iLevelType == 3 ? 148 : 14);
    lMap[X][Y + 1]->setBlockID(15);
    for(int i = 0; i < iSize; i++) {
        lMap[X + 1 + i][Y]->setBlockID(iLevelType == 3 ? 149 : 16);
        lMap[X + 1 + i][Y + 1]->setBlockID(iLevelType == 3 ? 150 : 17);
    }
    lMap[X + iSize + 1][Y]->setBlockID(18);
    lMap[X + iSize + 1][Y + 1]->setBlockID(19);
}
void Map::structGND(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight; j++) {
            lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 1 : iLevelType == 1 ? 26 : iLevelType == 2 ? 92 : iLevelType == 6 ? 166 : iLevelType == 7 ? 181 : 75);
        }
    }
}
void Map::structBonus(int X, int Y, int iWidth)
{
    for(int i = 0; i < iWidth; i++)
    {
        lMap[X + i][Y]->setBlockID(125);
    }
}


void Map::structBonusEnd(int X) {
    for(int i = 0; i < 20; i++) {
        lMap[X + i][0]->setBlockID(127);
    }
}
void Map::structUW1(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight; j++) {
            lMap[X + i][Y + j]->setBlockID(93);
        }
    }
}
// ----- true = LEFT, false = RIGHT -----
void Map::structGND2(int X, int Y, int iSize, bool bDir) {
    if(bDir) {
        for(int i = 0, k = 1; i < iSize; i++) {
            for(int j = 0; j < k; j++) {
                lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 25 : iLevelType == 3 ? 167 : 27);
            }
            ++k;
        }
    } else {
        for(int i = 0, k = 1; i < iSize; i++) {
            for(int j = 0; j < k; j++) {
                lMap[X + iSize - 1 - i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 25 : iLevelType == 3 ? 167 : 27);
            }
            ++k;
        }
    }
}
void Map::structGND2(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight; j++) {
            lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 25 : iLevelType == 3 ? 167 : 27);
        }
    }
}
void Map::structPipe(int X, int Y, int iHeight) {
    for(int i = 0; i < iHeight; i++) {
        lMap[X][Y + i]->setBlockID(iLevelType == 0 ? 20 : iLevelType == 2 ? 97 : iLevelType == 4 ? 112 : iLevelType == 5 ? 136 : iLevelType == 3 ? 176 : iLevelType == 7 ? 172 : 30);
        lMap[X + 1][Y + i]->setBlockID(iLevelType == 0 ? 22 : iLevelType == 2 ? 99 : iLevelType == 4 ? 114 : iLevelType == 5 ? 138 : iLevelType == 3 ? 178 : iLevelType == 7 ? 174 : 32);
    }
    lMap[X][Y + iHeight]->setBlockID(iLevelType == 0 ? 21 : iLevelType == 2 ? 98 : iLevelType == 4 ? 113 : iLevelType == 5 ? 137 : iLevelType == 3 ? 177 : iLevelType == 7 ? 173 : 31);
    lMap[X + 1][Y + iHeight]->setBlockID(iLevelType == 0 ? 23 : iLevelType == 2 ? 100 : iLevelType == 4 ? 115 : iLevelType == 5 ? 139 : iLevelType == 3 ? 179 : iLevelType == 7 ? 175 : 33);
}
void Map::structPipeVertical(int X, int Y, int iHeight) {
    for(int i = 0; i < iHeight + 1; i++) {
        lMap[X][Y + i]->setBlockID(iLevelType == 0 ? 20 : iLevelType == 2 ? 97 : iLevelType == 4 ? 112 : 30);
        lMap[X + 1][Y + i]->setBlockID(iLevelType == 0 ? 22 : iLevelType == 2 ? 99 : iLevelType == 4 ? 114 : 32);
    }
}
void Map::structPipeHorizontal(int X, int Y, int iWidth) {
    lMap[X][Y]->setBlockID(iLevelType == 0 ? 62 : iLevelType == 2 ? 105 : iLevelType == 4 ? 120 : 38);
    lMap[X][Y + 1]->setBlockID(iLevelType == 0 ? 60 : iLevelType == 2 ? 103 : iLevelType == 4 ? 118 : 36);
    for(int i = 0 ; i < iWidth; i++) {
        lMap[X + 1 + i][Y]->setBlockID(iLevelType == 0 ? 61 : iLevelType == 2 ? 104 : iLevelType == 4 ? 119 : 37);
        lMap[X + 1 + i][Y + 1]->setBlockID(iLevelType == 0 ? 59 : iLevelType == 2 ? 102 : iLevelType == 4 ? 117 : 35);
    }
    lMap[X + 1 + iWidth][Y]->setBlockID(iLevelType == 0 ? 58 : iLevelType == 2 ? 101 : iLevelType == 4 ? 116 : 34);
    lMap[X + 1 + iWidth][Y + 1]->setBlockID(iLevelType == 0 ? 63 : iLevelType == 2 ? 106 : iLevelType == 4 ? 121 : 39);
}
void Map::structBrick(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight; j++) {
            lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 13 : iLevelType == 3 ? 81 : 28);
        }
    }
}
void Map::struckBlockQ(int X, int Y, int iWidth) {
    for(int i = 0; i < iWidth; i++) {
        lMap[X + i][Y]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 8 : 55);
    }
}
void Map::struckBlockQ2(int X, int Y, int iWidth) {
    for(int i = 0; i < iWidth; i++) {
        lMap[X + i][Y]->setBlockID(24);
    }
}
void Map::structCoins(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight; j++) {
            lMap[X + i][Y + j]->setBlockID(iLevelType == 0 || iLevelType == 4 ? 71 : iLevelType == 1 ? 29 : iLevelType == 2 ? 73 : 29);
        }
    }
}
void Map::structEnd(int X, int Y, int iHeight) {
    for(int i = 0; i < iHeight; i++) {
        lMap[X][Y + i]->setBlockID(iLevelType == 4 ? 123 : 40);
    }
    oFlag = new Flag(X*32 - 16, Y + iHeight + 72);
    lMap[X][Y + iHeight]->setBlockID(iLevelType == 4 ? 124 : 41);
    for(int i = Y + iHeight + 1; i < Y + iHeight + 4; i++) {
        lMap[X][i]->setBlockID(182);
    }
}
void Map::structCastleSmall(int X, int Y) {
    for(int i = 0; i < 2; i++){
        lMap[X][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
        lMap[X + 1][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
        lMap[X + 3][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
        lMap[X + 4][Y + i]->setBlockID(iLevelType == 3 ? 155 : 43);
        lMap[X + 2][Y + i]->setBlockID(iLevelType == 3 ? 159 : 47);
    }
    lMap[X + 2][Y + 1]->setBlockID(iLevelType == 3 ? 158 : 46);
    for(int i = 0; i < 5; i++) {
        lMap[X + i][Y + 2]->setBlockID(i == 0 || i == 4 ? iLevelType == 3 ? 157 : 45 : iLevelType == 3 ? 156 : 44);
    }
    lMap[X + 1][Y + 3]->setBlockID(iLevelType == 3 ? 160 : 48);
    lMap[X + 2][Y + 3]->setBlockID(iLevelType == 3 ? 155 : 43);
    lMap[X + 3][Y + 3]->setBlockID(iLevelType == 3 ? 161 : 49);
    for(int i = 0; i < 3; i++) {
        lMap[X + i + 1][Y + 4]->setBlockID(iLevelType == 3 ? 157 : 45);
    }
}
void Map::structCastleBig(int X, int Y) {
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 5; j++) {
            setBlockID(X + i, Y + j, iLevelType == 3 ? 155 : 43);
            setBlockID(X + i + 7, Y + j, iLevelType == 3 ? 155 : 43);
        }
    }
    for(int i = 0; i < 3; i++) {
        setBlockID(X + 2 + i*2, Y, iLevelType == 3 ? 159 : 47);
        setBlockID(X + 2 + i*2, Y + 1, iLevelType == 3 ? 158 : 46);
    }
    for(int i = 0; i < 9; i++) {
        setBlockID(X + i, Y + 2, iLevelType == 3 ? 155 : 43);
    }
    for(int i = 0; i < 9; i++) {
        if(i < 2 || i > 6) {
            setBlockID(X + i, Y + 5, iLevelType == 3 ? 157 : 45);
        } else {
            setBlockID(X + i, Y + 5, iLevelType == 3 ? 156 : 44);
        }
    }
    for(int i = 0; i < 2; i++) {
        setBlockID(X + 3 + i*2, Y, iLevelType == 3 ? 155 : 43);
        setBlockID(X + 3 + i*2, Y + 1, iLevelType == 3 ? 155 : 43);
    }
    for(int i = 0; i < 2; i++) {
        setBlockID(X + 3 + i*2, Y + 3, iLevelType == 3 ? 159 : 47);
        setBlockID(X + 3 + i*2, Y + 4, iLevelType == 3 ? 158 : 46);
    }
    for(int i = 0; i < 3; i++) {
        setBlockID(X + 2 + i*2, Y + 3, iLevelType == 3 ? 155 : 43);
        setBlockID(X + 2 + i*2, Y + 4, iLevelType == 3 ? 155 : 43);
    }
    for(int i = 0; i < 2; i++) {
        setBlockID(X + 2, Y + 6 + i, iLevelType == 3 ? 155 : 43);
        setBlockID(X + 3, Y + 6 + i, iLevelType == 3 ? 155 : 43);
        setBlockID(X + 5, Y + 6 + i, iLevelType == 3 ? 155 : 43);
        setBlockID(X + 6, Y + 6 + i, iLevelType == 3 ? 155 : 43);
    }
    setBlockID(X + 4, Y + 6, iLevelType == 3 ? 159 : 47);
    setBlockID(X + 4, Y + 7, iLevelType == 3 ? 158 : 46);
    for(int i = 0; i < 3; i++) {
        setBlockID(X + 3 + i, Y + 8, iLevelType == 3 ? 156 : 44);
    }
    setBlockID(X + 2, Y + 8, iLevelType == 3 ? 157 : 45);
    setBlockID(X + 6, Y + 8, iLevelType == 3 ? 157 : 45);
    setBlockID(X + 2, Y + 8, iLevelType == 3 ? 157 : 45);
    setBlockID(X + 3, Y + 9, iLevelType == 3 ? 160 : 48);
    setBlockID(X + 4, Y + 9, iLevelType == 3 ? 155 : 43);
    setBlockID(X + 5, Y + 9, iLevelType == 3 ? 161 : 49);
    for(int i = 0; i < 3; i++) {
        setBlockID(X + 3 + i, Y + 10, iLevelType == 3 ? 157 : 45);
    }
}
void Map::structCastleWall(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j  < iHeight - 1; j++) {
            lMap[X + i][Y + j]->setBlockID(iLevelType == 3 ? 155 : 43);
        }
    }
    for(int i = 0; i < iWidth; i++) {
        lMap[X + i][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 157 : 45);
    }
}
void Map::structT(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iHeight - 1; i++) {
        for(int j = 1; j < iWidth - 1; j++) {
            lMap[X + j][Y + i]->setBlockID(iLevelType == 3 ? 154 : 70);
        }
    }
    for(int i = 1; i < iWidth - 1; i++) {
        lMap[X + i][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 152 : 68);
    }
    lMap[X][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 151 : 67);
    lMap[X + iWidth - 1][Y + iHeight - 1]->setBlockID(iLevelType == 3 ? 153 : 69);
}
void Map::structTMush(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iHeight - 2; i++) {
        lMap[X + iWidth/2][Y + i]->setBlockID(144);
    }
    lMap[X + iWidth/2][Y + iHeight - 2]->setBlockID(143);
    for(int i = 1; i < iWidth - 1; i++) {
        lMap[X + i][Y + iHeight - 1]->setBlockID(141);
    }
    lMap[X][Y + iHeight - 1]->setBlockID(140);
    lMap[X + iWidth - 1][Y + iHeight - 1]->setBlockID(142);
}
void Map::structWater(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight - 1; j++) {
            lMap[X + i][Y + j]->setBlockID(iLevelType == 2 ? 94 : 110);
        }
        lMap[X + i][Y + iHeight - 1]->setBlockID(iLevelType == 2 ? 95 : 111);
    }
}
void Map::structLava(int X, int Y, int iWidth, int iHeight) {
    for(int i = 0; i < iWidth; i++) {
        for(int j = 0; j < iHeight - 1; j++) {
            lMap[X + i][Y + j]->setBlockID(77);
        }
        lMap[X + i][Y + iHeight - 1]->setBlockID(78);
    }
}
void Map::structBridge(int X, int Y, int iWidth) {
    for(int i = 0; i < iWidth; i++) {
        lMap[X + i][Y]->setBlockID(76);
    }
    lMap[X + iWidth - 1][Y + 1]->setBlockID(79);
    lMap[X + iWidth][6]->setBlockID(82);
    lMap[X + iWidth + 1][6]->setBlockID(83);
    lMap[X + iWidth + 1][7]->setBlockID(83);
    lMap[X + iWidth + 1][8]->setBlockID(83);
}
void Map::structBridge2(int X, int Y, int iWidth) {
    for(int i = 0; i < iWidth; i++) {
        lMap[X + i][Y]->setBlockID(107);
        lMap[X + i][Y + 1]->setBlockID(iLevelType == 4 ? 122 : 108);
    }
}
void Map::structTree(int X, int Y, int iHeight, bool BIG) {
    for(int i = 0; i < iHeight; i++) {
        lMap[X][Y + i]->setBlockID(91);
    }
    if(BIG) {
        lMap[X][Y + iHeight]->setBlockID(iLevelType == 4 ? 88 : 85);
        lMap[X][Y + iHeight + 1]->setBlockID(iLevelType == 4 ? 89 : 86);
    } else {
        lMap[X][Y + iHeight]->setBlockID(iLevelType == 4 ? 87 : 84);
    }
}
void Map::structFence(int X, int Y, int iWidth) {
    for(int i = 0; i < iWidth; i++) {
        lMap[X + i][Y]->setBlockID(90);
    }
}
void Map::structPlatformLine(int X) {
    for(int i = 0; i < iMapHeight; i++) {
        lMap[X][i]->setBlockID(109);
    }
}
void Map::structSeeSaw(int X, int Y, int iWidth) {
    lMap[X][Y]->setBlockID(iLevelType == 3 ? 162 : 132);
    lMap[X + iWidth - 1][Y]->setBlockID(iLevelType == 3 ? 163 : 133);
    for(int i = 1; i < iWidth - 1; i++) {
        lMap[X + i][Y]->setBlockID(iLevelType == 3 ? 164 : 134);
    }
}
void Map::structBulletBill(int X, int Y, int iHieght) {
    lMap[X][Y + iHieght + 1]->setBlockID(145);
    lMap[X][Y + iHieght]->setBlockID(146);
    for(int i = 0; i < iHieght; i++) {
        lMap[X][Y + i]->setBlockID(147);
    }
    addBulletBillSpawner(X, Y + iHieght + 1, 0);
}

void Map::setBlockID(int X, int Y, int iBlockID) {
    if(X >= 0 && X < iMapWidth) {
        lMap[X][Y]->setBlockID(iBlockID);
    }
}

Player* Map::getPlayer() {
    return oPlayer;
}
Platform* Map::getPlatform(int iID) {
    return vPlatform[iID];
}
float Map::getXPos() {
    return fXPos;
}
void Map::setXPos(float iXPos) {
    this->fXPos = iXPos;
}
float Map::getYPos() {
    return fYPos;
}
void Map::setYPos(float iYPos) {
    this->fYPos = iYPos;
}
int Map::getLevelType() {
    return iLevelType;
}
void Map::setLevelType(int iLevelType) {
    this->iLevelType = iLevelType;
}
int Map::getCurrentLevelID() {
    return currentLevelID;
}
void Map::setCurrentLevelID(int currentLevelID) {
    if(this->currentLevelID != currentLevelID) {
        this->currentLevelID = currentLevelID;
        oEvent->resetRedraw();
        loadLVL();
        iSpawnPointID = 0;
    }
    this->currentLevelID = currentLevelID;
}
bool Map::getUnderWater() {
    return underWater;
}
void Map::setUnderWater(bool underWater) {
    this->underWater = underWater;
}
void Map::setSpawnPointID(int iSpawnPointID) {
    this->iSpawnPointID = iSpawnPointID;
}
int Map::getMapTime() {
    return iMapTime;
}
void Map::setMapTime(int iMapTime) {
    this->iMapTime = iMapTime;
}
int Map::getMapWidth() {
    return iMapWidth;
}
bool Map::getMoveMap() {
    return bMoveMap;
}
void Map::setMoveMap(bool bMoveMap) {
    this->bMoveMap = bMoveMap;
}
bool Map::getDrawLines() {
    return drawLines;
}
void Map::setDrawLines(bool drawLines) {
    this->drawLines = drawLines;
}

Event* Map::getEvent() {
    return oEvent;
}
bool Map::getInEvent() {
    return inEvent;
}
void Map::setInEvent(bool inEvent) {
    this->inEvent = inEvent;
}

Block* Map::getBlock(int iID) {
    return vBlock[iID];
}
Block* Map::getMinionBlock(int iID) {
    return vMinion[iID];
}
MapLevel* Map::getMapBlock(int iX, int iY) {
    return lMap[iX][iY];
}
Flag* Map::getFlag() {
    return oFlag;
}

MapLevel::MapLevel(void) { }
MapLevel::MapLevel(int iBlockID) {
    this->iBlockID = iBlockID;
    this->iNumOfUse = 0;
    this->blockAnimation = false;
    this->iYPos = 0;
    this->bYDirection = true;
    this->spawnMushroom = false;
    this->spawnStar = false;
    this->powerUP = true;
}
MapLevel::~MapLevel(void) {
}

void MapLevel::startBlockAnimation() {
    this->blockAnimation = true;
    this->iYPos = 0;
    this->bYDirection = true;
}
int MapLevel::updateYPos() {
    if(blockAnimation) {
        if (bYDirection)
        {
            if (iYPos < 10)
            {
                if (iYPos < 5) {
                    iYPos += 2;
                } else {
                     iYPos += 1;
                }
            }
            else {
                bYDirection = false;
            }
        }
        else {
            if (iYPos > 0) {
                if (iYPos > 5) {
                    iYPos -= 2;
                } else {
                    iYPos -= 1;
                }
            }
            else {
                blockAnimation = false;
            }
        }
    }
    return iYPos;
}

int MapLevel::getBlockID() {
    return iBlockID;
}
void MapLevel::setBlockID(int iBlockID) {
    this->iBlockID = iBlockID;
}
int MapLevel::getYPos() {
    return iYPos;
}
int MapLevel::getNumOfUse() {
    return iNumOfUse;
}
void MapLevel::setNumOfUse(int iNumOfUse) {
    this->iNumOfUse = iNumOfUse;
}
bool MapLevel::getSpawnMushroom() {
    return spawnMushroom;
}
void MapLevel::setSpawnMushroom(bool spawnMushroom) {
    this->spawnMushroom = spawnMushroom;
}
bool MapLevel::getPowerUP() {
    return powerUP;
}
void MapLevel::setPowerUP(bool powerUP) {
    this->powerUP = powerUP;
}
bool MapLevel::getSpawnStar() {
    return spawnStar;
}
void MapLevel::setSpawnStar(bool spawnStar) {
    this->spawnStar = spawnStar;
}

Menu::Menu(void)
{
    this->activeMenuOption = 0;
}
Menu::~Menu(void)
{
}

void Menu::Update()
{
}
void Menu::Draw(SDL_Renderer* rR)
{
    for(unsigned int i = 0; i < lMO.size(); i++)
    {
        CCFG::getText()->Draw(rR, lMO[i]->getText(), lMO[i]->getXPos(), lMO[i]->getYPos());
    }

    CCFG::getMM()->getActiveOption()->Draw(rR, lMO[activeMenuOption]->getXPos() - 32, lMO[activeMenuOption]->getYPos());
}

void Menu::updateActiveButton(int iDir)
{
    switch(iDir)
    {
        case 0:
            if (activeMenuOption - 1 < 0)
            {
                activeMenuOption = numOfMenuOptions - 1;
            }
            else
            {
                --activeMenuOption;
            }
            break;
        case 2:
            if (activeMenuOption + 1 >= numOfMenuOptions)
            {
                activeMenuOption = 0;
            }
            else
            {
                ++activeMenuOption;
            }

            break; default: break;
    }
}

MenuManager::MenuManager(void)
{
    this->currentGameState = eMainMenu;
    this->oMainMenu = new MainMenu();
    this->oLoadingMenu = new LoadingMenu();
    this->oAboutMenu = new AboutMenu();
    //this->oConsole = new Console();
    this->oOptionsMenu = new OptionsMenu();
    //this->oLE = new LevelEditor();
    this->oPauseMenu = new PauseMenu();
}
MenuManager::~MenuManager(void) {
    delete activeOption;
    delete oMainMenu;
    delete oLoadingMenu;
    delete oAboutMenu;
}

void MenuManager::Update() {
    switch(currentGameState) {
        case eMainMenu:
            oMainMenu->Update();
            CCore::getMap()->UpdateBlocks();
            break;
        case eGameLoading:
            oLoadingMenu->Update();
            break;
        case eGame:
            CCore::getMap()->Update();
            CCore::getMap()->UpdateMinionsCollisions();
            //oLE->Update();
            break;
        case eAbout:
            CCore::getMap()->UpdateMinions();
            CCore::getMap()->UpdateMinionBlokcs();
            CCore::getMap()->UpdateBlocks();
            oAboutMenu->Update();
            break;
        case eOptions:
            oOptionsMenu->Update();
            break;
        case ePasue:
            oPauseMenu->Update();
            break;
    }
}
void MenuManager::Draw(SDL_Renderer* rR) {
    switch(currentGameState) {
        case eMainMenu:
            CCore::getMap()->DrawMap(rR);
            CCore::getMap()->getPlayer()->Draw(rR);
            CCore::getMap()->DrawGameLayout(rR);
            oMainMenu->Draw(rR);
            break;
        case eGameLoading:
            oLoadingMenu->Draw(rR);
            break;
        case eGame:
            CCore::getMap()->Draw(rR);
            //oLE->Draw(rR);
            break;
        case eAbout:
            CCore::getMap()->DrawMap(rR);
            //CCore::getMap()->getPlayer()->Draw(rR);
            CCore::getMap()->DrawMinions(rR);
            oAboutMenu->Draw(rR);
            break;
        case eOptions:
            CCore::getMap()->DrawMap(rR);
            CCore::getMap()->DrawMinions(rR);
            CCore::getMap()->getPlayer()->Draw(rR);
            CCore::getMap()->DrawGameLayout(rR);
            oOptionsMenu->Draw(rR);
            break;
        case ePasue:
            CCore::getMap()->DrawMap(rR);
            CCore::getMap()->DrawMinions(rR);
            CCore::getMap()->getPlayer()->Draw(rR);
            CCore::getMap()->DrawGameLayout(rR);
            oPauseMenu->Draw(rR);
            break;
    }
    /* -- CRT EFFECT
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rR, 0, 0, 0, CCFG::getMusic()->getVolume());
    for(int i = 0; i < screen_width; i += 2) {
        SDL_RenderDrawLine(rR, i, 0, i, screen_width);
    }
    for(int i = 0; i < screen_height; i += 2) {
        SDL_RenderDrawLine(rR, 0, i, screen_width, i);
    }*/
    //oConsole->Draw(rR);
}
void MenuManager::setBackgroundColor(SDL_Renderer* rR) {
    switch(currentGameState) {
        case eMainMenu:
            CCore::getMap()->setBackgroundColor(rR);
            break;
        case eGameLoading:
            SDL_SetRenderDrawColor(rR, 0, 0, 0, 255);
            break;
        case eGame:
            CCore::getMap()->setBackgroundColor(rR);
            break;
        case eAbout:
            oAboutMenu->setBackgroundColor(rR);
            break;
    }
}

void MenuManager::enter() {
    switch(currentGameState) {
        case eMainMenu:
            oMainMenu->enter();
            break;
        case eGame:
            CCore::getMap()->setDrawLines(!CCore::getMap()->getDrawLines());
            break;
        case eAbout:
            oAboutMenu->enter();
            break;
        case eOptions:
            oOptionsMenu->enter();
            break;
        case ePasue:
            oPauseMenu->enter();
            break;
    }
}
void MenuManager::escape() {
    switch(currentGameState) {
        case eGame:
            break;
        case eAbout:
            oAboutMenu->enter();
            break;
        case eOptions:
            oOptionsMenu->escape();
            break;
        case ePasue:
            oPauseMenu->escape();
            break;
        case eMainMenu:
            oMainMenu->escape();
            break;
    }
}
void MenuManager::setKey(int keyID) {
    switch(currentGameState) {
        case eOptions:
            oOptionsMenu->setKey(keyID);
            break;
    }
}
void MenuManager::keyPressed(int iDir) {
    switch(currentGameState) {
        case eMainMenu:
            oMainMenu->updateActiveButton(iDir);
            break;
        case eOptions:
            oOptionsMenu->updateActiveButton(iDir);
            break;
        case ePasue:
            oPauseMenu->updateActiveButton(iDir);
            break;
    }
}
void MenuManager::resetActiveOptionID(gameState ID) {
    switch(ID) {
        case eMainMenu:
            oMainMenu->activeMenuOption = 0;
            break;
        case eOptions:
            oOptionsMenu->activeMenuOption = 0;
            break;
        case ePasue:
            oPauseMenu->activeMenuOption = 0;
            break;
    }
}

int MenuManager::getViewID() {
    return currentGameState;
}
void MenuManager::setViewID(gameState viewID) {
    this->currentGameState = viewID;
}
CIMG* MenuManager::getActiveOption() {
    return activeOption;
}
void MenuManager::setActiveOption(SDL_Renderer* rR) {
    activeOption = new CIMG("active_option", rR);
}
LoadingMenu* MenuManager::getLoadingMenu() {
    return oLoadingMenu;
}
AboutMenu* MenuManager::getAboutMenu() {
    return oAboutMenu;
}
/*
Console* MenuManager::getConsole() {
    return oConsole;
}
LevelEditor* MenuManager::getLE() {
    return oLE;
}
*/
OptionsMenu* MenuManager::getOptions() {
    return oOptionsMenu;
}



Minion::Minion(void) {
    this->minionState = 0;
    this->iHitBoxX = this->iHitBoxY = 32;
    this->killOtherUnits = false;
    this->minionSpawned = false;
    this->collisionOnlyWithPlayer = false;
    this->moveDirection = true;
    this->moveSpeed = 1;
    this->jumpState = 0;
    this->startJumpSpeed = 6.65f;
    this->currentFallingSpeed = 2.2f;
    this->currentJumpSpeed = 0;
    this->jumpDistance = 0;
    this->currentJumpDistance = 0;
    this->deadTime = -1;
    this->onAnotherMinion = false;
}
Minion::~Minion(void) {
}

void Minion::Update() { }
void Minion::Draw(SDL_Renderer* rR, CIMG* iIMG) { }
bool Minion::updateMinion() {
    if (!minionSpawned) {
        Spawn();
    } else {
        minionPhysics();
    }
    return minionSpawned;
}
void Minion::minionPhysics() {
    if (jumpState == 1) {
        physicsState1();
    } else {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true) && !onAnotherMinion) {
            physicsState2();
        } else {
            jumpState = 0;
            onAnotherMinion = false;
        }
    }
}
void Minion::physicsState1() {
    updateYPos(-(int)currentJumpSpeed);
    currentJumpDistance += (int)currentJumpSpeed;
    currentJumpSpeed *= (currentJumpDistance / jumpDistance > 0.75f ? 0.972f : 0.986f);
    if (currentJumpSpeed < 2.5f) {
        currentJumpSpeed = 2.5f;
    }
    if (jumpDistance <= currentJumpDistance) {
        jumpState = 2;
    }
}
void Minion::physicsState2() {
    currentFallingSpeed *= 1.06f;
    if (currentFallingSpeed > startJumpSpeed) {
        currentFallingSpeed = startJumpSpeed;
    }
    updateYPos((int)currentFallingSpeed);
    jumpState = 2;
    if (fYPos >= screen_height) {
        minionState = -1;
    }
}
void Minion::updateXPos() {
    // ----- LEFT
    if (moveDirection) {
        if (CCore::getMap()->checkCollisionLB((int)fXPos - moveSpeed, (int)fYPos - 2, iHitBoxY, true) || CCore::getMap()->checkCollisionLT((int)fXPos - moveSpeed, (int)fYPos + 2, true)) {
            moveDirection = !moveDirection;
            if(killOtherUnits && fXPos > -CCore::getMap()->getXPos() - 64 && fXPos < -CCore::getMap()->getXPos() + screen_width + 64 + iHitBoxX) CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
        } else {
            fXPos -= (jumpState == 0 ? moveSpeed : moveSpeed/2.0f);
        }
    }
    // ----- RIGHT
    else {
        if (CCore::getMap()->checkCollisionRB((int)fXPos + moveSpeed, (int)fYPos - 2, iHitBoxX, iHitBoxY, true) || CCore::getMap()->checkCollisionRT((int)fXPos + moveSpeed, (int)fYPos + 2, iHitBoxX, true)) {
            moveDirection = !moveDirection;
            if(killOtherUnits && fXPos > -CCore::getMap()->getXPos() - 64 && fXPos < -CCore::getMap()->getXPos() + screen_width + 64 + iHitBoxX) CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
        } else {
            fXPos += (jumpState == 0 ? moveSpeed : moveSpeed/2.0f);
        }
    }
    if(fXPos < -iHitBoxX) {
        minionState = -1;
    }
}
void Minion::updateYPos(int iN) {
    if (iN > 0) {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + iN, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + iN, iHitBoxX, iHitBoxY, true)) {
            fYPos += iN;
        } else {
            if (jumpState == 1) {
                jumpState = 2;
            }
            updateYPos(iN - 1);
        }
    }
    else if (iN < 0) {
        if (!CCore::getMap()->checkCollisionLT((int)fXPos + 2, (int)fYPos + iN, true) && !CCore::getMap()->checkCollisionRT((int)fXPos - 2, (int)fYPos + iN, iHitBoxX, true)) {
            fYPos += iN;
        } else {
            if (jumpState == 1) {
                jumpState = 2;
            }
            updateYPos(iN + 1);
        }
    }
    if((int)fYPos % 2 == 1) {
        fYPos += 1;
    }
}
void Minion::collisionEffect() {
    if(minionSpawned)
    moveDirection = !moveDirection;
}
void Minion::collisionWithPlayer(bool TOP) { }
void Minion::collisionWithAnotherUnit() { };
void Minion::lockMinion() { }

void Minion::Spawn() {
    if ((fXPos >= -CCore::getMap()->getXPos() && fXPos <= -CCore::getMap()->getXPos() + screen_width) || (fXPos + iHitBoxX >= -CCore::getMap()->getXPos() && fXPos + iHitBoxX <= -CCore::getMap()->getXPos() + screen_width)) {
        minionSpawned = true;
    }
}
void Minion::startJump(int iH) {
    jumpState = 1;
    currentJumpSpeed = startJumpSpeed;
    jumpDistance = 32 * iH + 16.0f;
    currentJumpDistance = 0;
}
void Minion::resetJump() {
    jumpState = 0;
    currentFallingSpeed = 2.7f;
}
void Minion::points(int iPoints) {
    CCore::getMap()->addPoints((int)fXPos + 7, (int)fYPos, ToString(iPoints * CCore::getMap()->getPlayer()->getComboPoints()), 8, 16);
    CCore::getMap()->getPlayer()->setScore(CCore::getMap()->getPlayer()->getScore() + iPoints * CCore::getMap()->getPlayer()->getComboPoints());
    CCore::getMap()->getPlayer()->addComboPoints();
}
void Minion::minionDeathAnimation() {
    fXPos += (moveDirection ? -1.5f : 1.5f);
    fYPos += 2 * (deadTime > 8 ? -1 : deadTime > 2 ? 1 : 4.25f);
    if(deadTime > 0) {
        --deadTime;
    }
    if(fYPos > screen_height) {
        minionState = -1;
    }
}

int Minion::getBloockID() {
    return iBlockID;
}
void Minion::setBlockID(int iBlockID) {
    this->iBlockID = iBlockID;
}
int Minion::getXPos() {
    return (int)fXPos;
}
int Minion::getYPos() {
    return (int)fYPos;
}
void Minion::setYPos(int iYPos) {
    this->fYPos = (float)iYPos;
}
int Minion::getMinionState() {
    return minionState;
}
void Minion::setMinionState(int minionState) {
    this->minionState = minionState;
    if(minionState == -2) {
        deadTime = 16;
        fYPos -= iHitBoxY/4;
        points(200);
        collisionOnlyWithPlayer = true;
        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSHOT);
    }
}
bool Minion::getPowerUP() { return true; }

Mushroom::Mushroom(int iXPos, int fYPos, bool powerUP, int iX, int iY) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)fYPos - 2;
    this->iBlockID = powerUP ? 2 : CCore::getMap()->getLevelType() == 1 ? 25 : 3;
    this->moveSpeed = 2;
    this->inSpawnState = true;
    this->minionSpawned = true;
    this->inSpawnY = 30;
    this->moveDirection = false;
    this->powerUP = powerUP;
    this->collisionOnlyWithPlayer = true;
    this->minionState = 0;
    this->iX = iX;
    this->iY = iY;
}
Mushroom::~Mushroom(void) {
}

void Mushroom::Update() {
    if (inSpawnState) {
        if (inSpawnY <= 0) {
            inSpawnState = false;
        } else {
            if (fYPos > -5) {
                inSpawnY -= 2;
                fYPos -= 2;
            } else {
                inSpawnY -= 1;
                fYPos -= 1;
            }
        }
    } else {
        updateXPos();
    }
}
bool Mushroom::updateMinion() {
    if (!inSpawnState) {
        minionPhysics();
    }
    return minionSpawned;
}
void Mushroom::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState >= 0) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2, false);
        if (inSpawnState) {
            CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 9 : 56)->getSprite()->getTexture()->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + (32 - inSpawnY) - CCore::getMap()->getMapBlock(iX, iY)->getYPos() + 2, false);
        }
    }
}

void Mushroom::collisionWithPlayer(bool TOP) {
    if(!inSpawnState && minionState == 0) {
        if(powerUP) {
            CCore::getMap()->getPlayer()->setPowerLVL(CCore::getMap()->getPlayer()->getPowerLVL() + 1);
        } else {
            CCore::getMap()->getPlayer()->setNumOfLives(CCore::getMap()->getPlayer()->getNumOfLives() + 1);
            CCore::getMap()->addPoints((int)fXPos, (int)fYPos, "1UP", 10, 14);
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cONEUP);
        }
        minionState = -1;
    }
}
void Mushroom::setMinionState(int minionState) {
}

Music::Music(void) {
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    vMusic.push_back(loadMusic("overworld"));
    vMusic.push_back(loadMusic("overworld-fast"));
    vMusic.push_back(loadMusic("underground"));
    vMusic.push_back(loadMusic("underground-fast"));
    vMusic.push_back(loadMusic("underwater"));
    vMusic.push_back(loadMusic("underwater-fast"));
    vMusic.push_back(loadMusic("castle"));
    vMusic.push_back(loadMusic("castle-fast"));
    vMusic.push_back(loadMusic("lowtime"));
    vMusic.push_back(loadMusic("starmusic"));
    vMusic.push_back(loadMusic("starmusic-fast"));
    vMusic.push_back(loadMusic("scorering"));
    vChunk.push_back(loadChunk("coin"));
    vChunk.push_back(loadChunk("blockbreak"));
    vChunk.push_back(loadChunk("blockhit"));
    vChunk.push_back(loadChunk("boom"));
    vChunk.push_back(loadChunk("bowserfall"));
    vChunk.push_back(loadChunk("bridgebreak"));
    vChunk.push_back(loadChunk("bulletbill"));
    vChunk.push_back(loadChunk("death"));
    vChunk.push_back(loadChunk("fire"));
    vChunk.push_back(loadChunk("fireball"));
    vChunk.push_back(loadChunk("gameover"));
    vChunk.push_back(loadChunk("intermission"));
    vChunk.push_back(loadChunk("jump"));
    vChunk.push_back(loadChunk("jumpbig"));
    vChunk.push_back(loadChunk("levelend"));
    vChunk.push_back(loadChunk("lowtime"));
    vChunk.push_back(loadChunk("mushroomappear"));
    vChunk.push_back(loadChunk("mushroomeat"));
    vChunk.push_back(loadChunk("oneup"));
    vChunk.push_back(loadChunk("pause"));
    vChunk.push_back(loadChunk("shrink"));
    vChunk.push_back(loadChunk("rainboom"));
    vChunk.push_back(loadChunk("shot"));
    vChunk.push_back(loadChunk("shrink"));
    vChunk.push_back(loadChunk("stomp"));
    vChunk.push_back(loadChunk("swim"));
    vChunk.push_back(loadChunk("vine"));
    vChunk.push_back(loadChunk("castleend"));
    vChunk.push_back(loadChunk("princessmusic"));
    setVolume(100);
    this->currentMusic = mNOTHING;
}
Music::~Music(void) {
    for(unsigned int i = 0; i < vMusic.size(); i++) {
        Mix_FreeMusic(vMusic[i]);
    }
    vMusic.clear();
    for(unsigned int i = 0; i < vChunk.size(); i++) {
        Mix_FreeChunk(vChunk[i]);
    }
    vChunk.clear();
}

void Music::changeMusic(bool musicByLevel, bool forceChange) {
    eMusic eNew = currentMusic;
    if(musicByLevel) {
        if(CCore::getMap()->getInEvent() && CCore::getMap()->getEvent()->inEvent) {
            eNew = mNOTHING;
            PlayChunk(cINTERMISSION);
        } else {
            switch(CCore::getMap()->getLevelType()) {
                case 0: case 4:
                    eNew = CCore::getMap()->getMapTime() > 90 ? mOVERWORLD : mOVERWORLDFAST;
                    break;
                case 1:
                    eNew = CCore::getMap()->getMapTime() > 90 ? mUNDERWORLD : mUNDERWORLDFAST;
                    break;
                case 2:
                    eNew = CCore::getMap()->getMapTime() > 90 ? mUNDERWATER : mUNDERWATERFAST;
                    break;
                case 3:
                    eNew = CCore::getMap()->getMapTime() > 90 ? mCASTLE : mCASTLEFAST;
                    break;
                case 100:
                    eNew = mNOTHING;
                    PlayChunk(cINTERMISSION);
                    CCore::getMap()->setLevelType(0);
                    break;
                default:
                    eNew = mNOTHING;
                    break;
            }
        }
    }
    if(currentMusic != eNew || forceChange) {
        StopMusic();
        currentMusic = eNew;
        PlayMusic();
    }
}
void Music::PlayMusic() {
    if(currentMusic != mNOTHING) {
        Mix_PlayMusic(vMusic[currentMusic - 1], -1);
        musicStopped = false;
    } else {
        StopMusic();
    }
}
void Music::PlayMusic(eMusic musicID) {
    if(musicID != mNOTHING) {
        Mix_PlayMusic(vMusic[musicID - 1], -1);
        musicStopped = false;
        currentMusic = musicID;
    } else {
        StopMusic();
        currentMusic = mNOTHING;
    }
}
void Music::StopMusic() {
    if(!musicStopped) {
        Mix_HaltMusic();
        musicStopped = true;
    }
}
void Music::PauseMusic() {
    if(Mix_PausedMusic() == 1) {
        Mix_ResumeMusic();
        musicStopped = false;
    } else {
        Mix_PauseMusic();
        musicStopped = true;
    }
}

void Music::PlayChunk(eChunk chunkID) {
    Mix_VolumeChunk(vChunk[chunkID], iVolume);
    Mix_PlayChannel(-1, vChunk[chunkID], 0);
}

Mix_Music* Music::loadMusic(string fileName) {
    fileName = "files/sounds/" + fileName + ".wav";
    return Mix_LoadMUS(fileName.c_str());
}
Mix_Chunk* Music::loadChunk(string fileName) {
    fileName = "files/sounds/" + fileName + ".wav";
    return Mix_LoadWAV(fileName.c_str());
}
int Music::getVolume() {
    return iVolume;
}
void Music::setVolume(int iVolume) {
    this->iVolume = iVolume;
    Mix_VolumeMusic(iVolume);
}
/************************
 * @author £ukasz Jakowski
 * @since  02.05.2014 16:13
 *
 ************************/

MusicManager::MusicManager(void)
{
}
MusicManager::~MusicManager(void) {
}

void MusicManager::PlayMusic() {
    vMusic.push_back(Mix_LoadMUS("files/sounds/overworld.wav"));
    vMusic.push_back(Mix_LoadMUS("files/sounds/overworld.wav"));
    Mix_VolumeMusic(100);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_PlayMusic(vMusic[0], -1);
}

Mix_Music* MusicManager::loadMusic(string fileName) {
    fileName = "files/sounds/" + fileName + ".wav";
    return Mix_LoadMUS("files/sounds/overworld.wav");
}


OptionsMenu::OptionsMenu(void) {
    rRect.x = 58;
    rRect.y = 48;
    rRect.w = 403;
    rRect.h = 324;
    this->lMO.push_back(new MenuOption("VOLUME", 73, 65));
    this->lMO.push_back(new MenuOption("LEFT", 73, 89));
    this->lMO.push_back(new MenuOption("DOWN", 73, 113));
    this->lMO.push_back(new MenuOption("RIGHT", 73, 137));
    this->lMO.push_back(new MenuOption("JUMP", 73, 161));
    this->lMO.push_back(new MenuOption("RUN", 73, 185));
    this->lMO.push_back(new MenuOption("CAN MOVE BACKWARD", 73, 209));
    this->lMO.push_back(new MenuOption("MAIN MENU", 73, 257));
    this->numOfMenuOptions = lMO.size();
    this->inSetKey = this->resetSetKey = false;
    rSetKeyRect.x = 75;
    rSetKeyRect.y = 284;
    rSetKeyRect.w = 369;
    rSetKeyRect.h = 71;
    rVolumeBG.x = 185;
    rVolumeBG.y = 65;
    rVolumeBG.h = 16;
    rVolumeBG.w = 200;
    rVolume.x = 185;
    rVolume.y = 65;
    rVolume.h = 16;
    rVolume.w = 100;
    this->escapeToMainMenu = true;
}
OptionsMenu::~OptionsMenu(void) {
}

void OptionsMenu::Update() {
    if(resetSetKey) {
        inSetKey = false;
        resetSetKey = false;
    }
}
void OptionsMenu::Draw(SDL_Renderer* rR) {
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rR, 4, 4, 4, 235);
    SDL_RenderFillRect(rR, &rRect);
    SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
    rRect.x += 1;
    rRect.y += 1;
    rRect.h -= 2;
    rRect.w -= 2;
    SDL_RenderDrawRect(rR, &rRect);
    rRect.x -= 1;
    rRect.y -= 1;
    rRect.h += 2;
    rRect.w += 2;
    for(unsigned int i = 0; i < lMO.size(); i++) {
        if(i == activeMenuOption) {
            CCFG::getText()->Draw(rR, lMO[i]->getText(), lMO[i]->getXPos(), lMO[i]->getYPos(), 16, 255, 255, 255);
        } else {
            CCFG::getText()->Draw(rR, lMO[i]->getText(), lMO[i]->getXPos(), lMO[i]->getYPos(), 16, 90, 90, 90);
        }
    }
    SDL_SetRenderDrawColor(rR, 4, 4, 4, 245);
    SDL_RenderFillRect(rR, &rVolumeBG);
    SDL_SetRenderDrawColor(rR, activeMenuOption == 0 ? 150 : 90, activeMenuOption == 0 ? 150 : 90, activeMenuOption == 0 ? 150 : 90, 255);
    SDL_RenderFillRect(rR, &rVolume);
    if(activeMenuOption == 0) {
        SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
        SDL_RenderDrawRect(rR, &rVolumeBG);
    } else {
        SDL_SetRenderDrawColor(rR, 160, 160, 160, 55);
        SDL_RenderDrawRect(rR, &rVolumeBG);
    }
    CCFG::getText()->Draw(rR, CCFG::getKeyString(CCFG::keyIDA), 185, 89, 16, activeMenuOption == 1 ? 255 : 90, activeMenuOption == 1 ? 255 : 90, activeMenuOption == 1 ? 255 : 90);
    CCFG::getText()->Draw(rR, CCFG::getKeyString(CCFG::keyIDS), 185, 113, 16, activeMenuOption == 2 ? 255 : 90, activeMenuOption == 2 ? 255 : 90, activeMenuOption == 2 ? 255 : 90);
    CCFG::getText()->Draw(rR, CCFG::getKeyString(CCFG::keyIDD), 185, 137, 16, activeMenuOption == 3 ? 255 : 90, activeMenuOption == 3 ? 255 : 90, activeMenuOption == 3 ? 255 : 90);
    CCFG::getText()->Draw(rR, CCFG::getKeyString(CCFG::keyIDSpace), 185, 161, 16, activeMenuOption == 4 ? 255 : 90, activeMenuOption == 4 ? 255 : 90, activeMenuOption == 4 ? 255 : 90);
    CCFG::getText()->Draw(rR, CCFG::getKeyString(CCFG::keyIDShift), 185, 185, 16, activeMenuOption == 5 ? 255 : 90, activeMenuOption == 5 ? 255 : 90, activeMenuOption == 5 ? 255 : 90);
    CCFG::getText()->Draw(rR, CCFG::canMoveBackward ? "TRUE" : "FALSE", 357, 209, 16, activeMenuOption == 6 ? 255 : 90, activeMenuOption == 6 ? 255 : 90, activeMenuOption == 6 ? 255 : 90);
    if(inSetKey) {
        SDL_SetRenderDrawColor(rR, 20, 20, 20, 245);
        SDL_RenderFillRect(rR, &rSetKeyRect);
        SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
        rSetKeyRect.x += 1;
        rSetKeyRect.y += 1;
        rSetKeyRect.h -= 2;
        rSetKeyRect.w -= 2;
        SDL_RenderDrawRect(rR, &rSetKeyRect);
        rSetKeyRect.x -= 1;
        rSetKeyRect.y -= 1;
        rSetKeyRect.h += 2;
        rSetKeyRect.w += 2;
        CCFG::getText()->Draw(rR, "PRESS KEY FOR " + lMO[activeMenuOption]->getText(), 92, rSetKeyRect.y + 16, 16, 255, 255, 255);
        CCFG::getText()->Draw(rR, "PRESS ESC TO CANCEL", 92, rSetKeyRect.y + 40, 16, 255, 255, 255);
    }
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE);
    CCore::getMap()->setBackgroundColor(rR);
}

void OptionsMenu::enter() {
    switch(activeMenuOption) {
        case 0:
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
            break;
        case 1: case 2: case 3: case 4: case 5:
            inSetKey = true;
            break;
        case 6:
            CCFG::canMoveBackward = !CCFG::canMoveBackward;
            break;
        case 7:
            CCore::getMap()->resetGameData();
            CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
            break;
    }
}
void OptionsMenu::escape() {
    if(inSetKey) {
        resetSetKey = true;
    } else {
        if(escapeToMainMenu) {
            CCore::getMap()->resetGameData();
            CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
        } else {
            CCFG::getMM()->setViewID(CCFG::getMM()->ePasue);
        }
    }
}
void OptionsMenu::setKey(int keyID) {
    if(inSetKey && keyID != SDLK_KP_ENTER && keyID != SDLK_RETURN && keyID != SDLK_ESCAPE) {
        switch(activeMenuOption) {
            case 1:
                CCFG::keyIDA = keyID;
                if(CCFG::keyIDD == keyID) CCFG::keyIDD = 0;
                if(CCFG::keyIDS == keyID) CCFG::keyIDS = 0;
                if(CCFG::keyIDSpace == keyID) CCFG::keyIDSpace = 0;
                if(CCFG::keyIDShift == keyID) CCFG::keyIDShift = 0;
                break;
            case 2:
                CCFG::keyIDS = keyID;
                if(CCFG::keyIDA == keyID) CCFG::keyIDA = 0;
                if(CCFG::keyIDD == keyID) CCFG::keyIDD = 0;
                if(CCFG::keyIDSpace == keyID) CCFG::keyIDSpace = 0;
                if(CCFG::keyIDShift == keyID) CCFG::keyIDShift = 0;
                break;
            case 3:
                CCFG::keyIDD = keyID;
                if(CCFG::keyIDA == keyID) CCFG::keyIDA = 0;
                if(CCFG::keyIDS == keyID) CCFG::keyIDS = 0;
                if(CCFG::keyIDSpace == keyID) CCFG::keyIDSpace = 0;
                if(CCFG::keyIDShift == keyID) CCFG::keyIDShift = 0;
                break;
            case 4:
                CCFG::keyIDSpace = keyID;
                if(CCFG::keyIDA == keyID) CCFG::keyIDA = 0;
                if(CCFG::keyIDS == keyID) CCFG::keyIDS = 0;
                if(CCFG::keyIDD == keyID) CCFG::keyIDD = 0;
                if(CCFG::keyIDShift == keyID) CCFG::keyIDShift = 0;
                break;
            case 5:
                CCFG::keyIDShift = keyID;
                if(CCFG::keyIDA == keyID) CCFG::keyIDA = 0;
                if(CCFG::keyIDS == keyID) CCFG::keyIDS = 0;
                if(CCFG::keyIDD == keyID) CCFG::keyIDD = 0;
                if(CCFG::keyIDSpace == keyID) CCFG::keyIDSpace = 0;
                break;
        }
        resetSetKey = true;
    } else if(keyID == SDLK_ESCAPE) {
        resetSetKey = true;
    }
}
void OptionsMenu::updateActiveButton(int iDir) {
    if(activeMenuOption == 0 && (iDir == 1 || iDir == 3)) {
        switch(iDir) {
            case 1:
                if(CCFG::getMusic()->getVolume() < 100) {
                    CCFG::getMusic()->setVolume(CCFG::getMusic()->getVolume() + 5);
                } else {
                    CCFG::getMusic()->setVolume(100);
                }
                break;
            case 3:
                if(CCFG::getMusic()->getVolume() > 0) {
                    CCFG::getMusic()->setVolume(CCFG::getMusic()->getVolume() - 5);
                } else {
                    CCFG::getMusic()->setVolume(0);
                }
                break;
        }
        updateVolumeRect();
        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cCOIN);
    }
    if(!inSetKey) {
        Menu::updateActiveButton(iDir);
    }
}

void OptionsMenu::updateVolumeRect() {
    rVolume.w = CCFG::getMusic()->getVolume()*2;
}
void OptionsMenu::setEscapeToMainMenu(bool escapeToMainMenu) {
    this->escapeToMainMenu = escapeToMainMenu;
}

PauseMenu::PauseMenu(void) {
    rPause.x = 220;
    rPause.y = 140;
    rPause.w = 360;
    rPause.h = 142;
    this->lMO.push_back(new MenuOption("RESUME", 0, 156));
    this->lMO.push_back(new MenuOption("OPTIONS", 0, 180));
    this->lMO.push_back(new MenuOption("QUIT TO MENU", 0, 228));
    this->lMO.push_back(new MenuOption("QUIT TO DESKTOP", 0, 252));
    this->numOfMenuOptions = lMO.size();
}
PauseMenu::~PauseMenu(void) {
}

void PauseMenu::Update() {
}
void PauseMenu::Draw(SDL_Renderer* rR) {
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rR, 4, 4, 4, 235);
    SDL_RenderFillRect(rR, &rPause);
    SDL_SetRenderDrawColor(rR, 255, 255, 255, 255);
    rPause.x += 1;
    rPause.y += 1;
    rPause.h -= 2;
    rPause.w -= 2;
    SDL_RenderDrawRect(rR, &rPause);
    rPause.x -= 1;
    rPause.y -= 1;
    rPause.h += 2;
    rPause.w += 2;
    for(unsigned int i = 0; i < lMO.size(); i++) {
        if(i == activeMenuOption) {
            CCFG::getText()->DrawCenterX(rR, lMO[i]->getText(), lMO[i]->getYPos(), 16, 255, 255, 255);
        } else {
            CCFG::getText()->DrawCenterX(rR, lMO[i]->getText(), lMO[i]->getYPos(), 16, 90, 90, 90);
        }
    }
    SDL_SetRenderDrawBlendMode(rR, SDL_BLENDMODE_NONE);
    CCore::getMap()->setBackgroundColor(rR);
}

void PauseMenu::enter() {
    switch(activeMenuOption) {
        case 0:
            CCFG::getMM()->setViewID(CCFG::getMM()->eGame);
            CCFG::getMusic()->PlayMusic();
            break;
        case 1:
            CCFG::getMM()->getOptions()->setEscapeToMainMenu(false);
            CCFG::getMM()->resetActiveOptionID(CCFG::getMM()->eOptions);
            CCFG::getMM()->getOptions()->updateVolumeRect();
            CCFG::getMM()->setViewID(CCFG::getMM()->eOptions);
            break;
        case 2:
            CCore::getMap()->resetGameData();
            CCFG::getMM()->setViewID(CCFG::getMM()->eMainMenu);
            break;
        case 3: executando = false; break;
    }
}
void PauseMenu::escape() {
    CCFG::getMM()->setViewID(CCFG::getMM()->eGame);
    CCFG::getMusic()->PauseMusic();
}
void PauseMenu::updateActiveButton(int iDir) {
    Menu::updateActiveButton(iDir);
}

Pipe::Pipe(int iType, int iLX, int iLY, int iRX, int iRY, int newPlayerPosX, int newPlayerPosY, int newCurrentLevel, int newLevelType, bool newMoveMap, int iDelay, int iSpeed, bool newUnderWater) {
    this->iType = iType;
    this->iLX = iLX;
    this->iLY = iLY;
    this->iRX = iRX;
    this->iRY = iRY;
    this->newPlayerPosX = newPlayerPosX;
    this->newPlayerPosY = newPlayerPosY;
    this->newCurrentLevel = newCurrentLevel;
    this->newLevelType = newLevelType;
    this->newMoveMap = newMoveMap;
    this->newUnderWater = newUnderWater;
    this->iDelay = iDelay;
    this->iSpeed = iSpeed;
}
Pipe::~Pipe(void)
{
}

void Pipe::checkUse() {
    if(iType == 0 || iType == 2) {
        if(CCore::getMap()->getPlayer()->getSquat() && -(int)CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getXPos() >= iLX * 32 + 4 && -(int)CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX() < (iRX + 1) * 32 - 4) {
            setEvent();
        }
    } else {
        if(!CCore::getMap()->getPlayer()->getSquat() && CCore::getMap()->getBlockIDX(-(int)CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX() / 2 + 2) == iRX - 1 && CCore::getMap()->getBlockIDY(CCore::getMap()->getPlayer()->getYPos() + CCore::getMap()->getPlayer()->getHitBoxY() + 2) == iRY - 1) {
            setEvent();
        }
    }
}

void Pipe::setEvent() {
    CCore::getMap()->getEvent()->resetData();
    CCore::getMap()->getPlayer()->stopMove();
    CCore::getMap()->getPlayer()->resetJump();
    CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cPIPE);
    CCore::getMap()->getEvent()->eventTypeID = CCore::getMap()->getEvent()->eNormal;
    CCore::getMap()->getEvent()->newCurrentLevel = newCurrentLevel;
    CCore::getMap()->getEvent()->newLevelType = newLevelType;
    CCore::getMap()->getEvent()->newMoveMap = newMoveMap;
    CCore::getMap()->getEvent()->iSpeed = iSpeed;
    CCore::getMap()->getEvent()->iDelay = iDelay;
    CCore::getMap()->getEvent()->inEvent = false;
    CCore::getMap()->getEvent()->newUnderWater = newUnderWater;
    CCore::getMap()->getEvent()->newMapXPos = (newPlayerPosX <= 32 * 2 ? 0 : -(newPlayerPosX - 32 * 2));
    CCore::getMap()->getEvent()->newPlayerXPos = (newPlayerPosX <= 32 * 2 ? newPlayerPosX : 32 * 2);
    CCore::getMap()->getEvent()->newPlayerYPos = newPlayerPosY;
    if(iType == 0) { // VERTICAL -> NONE
        CCore::getMap()->getEvent()->newPlayerYPos -= CCore::getMap()->getPlayer()->getPowerLVL() > 0 ? 32 : 0;
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eBOT);
        CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
        CCore::getMap()->getEvent()->vOLDLength.push_back(35);
        for(int i = 0; i < 3; i++) {
            CCore::getMap()->getEvent()->reDrawX.push_back(iLX);
            CCore::getMap()->getEvent()->reDrawY.push_back(iLY - i);
            CCore::getMap()->getEvent()->reDrawX.push_back(iRX);
            CCore::getMap()->getEvent()->reDrawY.push_back(iRY - i);
        }
    } else if(iType == 1) {
        CCore::getMap()->getEvent()->newPlayerXPos += 32 - CCore::getMap()->getPlayer()->getHitBoxX() / 2;
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eRIGHT);
        CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxX());
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
        CCore::getMap()->getEvent()->vOLDLength.push_back(35);
        CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->ePLAYPIPETOP);
        CCore::getMap()->getEvent()->vNEWLength.push_back(1);
        CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
        CCore::getMap()->getEvent()->vNEWLength.push_back(35);
        CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eTOP);
        CCore::getMap()->getEvent()->vNEWLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());
        for(int i = 0; i < 3; i++) {
            CCore::getMap()->getEvent()->reDrawX.push_back(iLX + i);
            CCore::getMap()->getEvent()->reDrawY.push_back(iLY);
            CCore::getMap()->getEvent()->reDrawX.push_back(iRX + i);
            CCore::getMap()->getEvent()->reDrawY.push_back(iRY);
            CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos));
            CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
            CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos) + 1);
            CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
        }
    } else { // -- VERT -> VERT
        CCore::getMap()->getEvent()->newPlayerXPos -= CCore::getMap()->getPlayer()->getPowerLVL() > 0 ? 32 : 0 - CCore::getMap()->getPlayer()->getHitBoxX()/2;
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eBOT);
        CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
        CCore::getMap()->getEvent()->vOLDLength.push_back(55);
        for(int i = 0; i < 3; i++) {
            CCore::getMap()->getEvent()->reDrawX.push_back(iLX);
            CCore::getMap()->getEvent()->reDrawY.push_back(iLY - i);
            CCore::getMap()->getEvent()->reDrawX.push_back(iRX);
            CCore::getMap()->getEvent()->reDrawY.push_back(iRY - i);
            CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos));
            CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
            CCore::getMap()->getEvent()->reDrawX.push_back(CCore::getMap()->getBlockIDX(CCore::getMap()->getEvent()->newPlayerXPos - CCore::getMap()->getEvent()->newMapXPos) + 1);
            CCore::getMap()->getEvent()->reDrawY.push_back(CCore::getMap()->getBlockIDY(newPlayerPosY) - 1 - i);
        }
        CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->ePLAYPIPETOP);
        CCore::getMap()->getEvent()->vNEWLength.push_back(1);
        CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
        CCore::getMap()->getEvent()->vNEWLength.push_back(35);
        CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eTOP);
        CCore::getMap()->getEvent()->vNEWLength.push_back(CCore::getMap()->getPlayer()->getHitBoxY());
    }
    CCore::getMap()->setInEvent(true);
}

Plant::Plant(int fXPos, int fYPos, int iBlockID) {
    this->fXPos = (float)fXPos + 2;
    this->fYPos = (float)fYPos + 6;
    this->iHitBoxX = 28;
    this->iHitBoxY = 38;
    this->iBlockID = iBlockID;
    this->bWait = true;
    this->iTime = SDL_GetTicks();
    this->lockPlant = false;
    // -- true = TOP, false = BOT
    this->moveDirection = true;
    this->iLength = 50;
    this->X = CCore::getMap()->getBlockIDX((int)fXPos);
    this->Y = CCore::getMap()->getBlockIDY((int)fYPos);
}
Plant::~Plant(void) {
}

void Plant::Update() {
    lockPlant = CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() >= fXPos - 48 && CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() <= fXPos + iHitBoxX + 48;
    if(bWait) {
        if((!lockPlant || !moveDirection) && iTime + (moveDirection ? 1000 : 985) <= SDL_GetTicks()) {
            bWait = false;
        } else if(lockPlant && moveDirection) {
            iTime = SDL_GetTicks();
        }
    } else if(moveDirection) {
        if(iLength > 0) {
            fYPos -= 1;
            iLength -= 1;
        } else {
            moveDirection = false;
            iLength = 50;
            bWait = true;
            iTime = SDL_GetTicks();
        }
    } else {
        if(iLength > 0) {
            fYPos += 1;
            iLength -= 1;
        } else if(!lockPlant) {
            moveDirection = true;
            iLength = 50;
            bWait = true;
            iTime = SDL_GetTicks();
        }
    }
}
void Plant::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    iIMG->Draw(rR, (int)(fXPos + CCore::getMap()->getXPos()) - 2, (int)fYPos - 6, false);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 2; j++) {
            CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(X + j, Y - i)->getBlockID())->Draw(rR, (int)((X + j) * 32 + CCore::getMap()->getXPos()), screen_height - 32 * (Y - i) - 16);
        }
    }
}
void Plant::minionPhysics() {
}

void Plant::collisionWithPlayer(bool onTOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect()) {
        minionState = -1;
        points(200);
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}
void Plant::collisionEffect() { }
void Plant::lockMinion() {
    if(moveDirection) {
        fYPos += 50 - iLength;
    } else {
        fYPos += iLength;
    }
    moveDirection = true;
    iLength = 50;
    lockPlant = true;
    bWait = true;
    iTime = SDL_GetTicks();
}
void Plant::setMinionState(int minionState) {
    if(minionState < 0) {
        this->minionState = -1;
        points(200);
    } else {
        this->minionState = minionState;
    }
}


Platform::Platform(int iSize, int iType, int iXStart, int iXEnd, int iYStart, int iYEnd, float fXPos, float fYPos, bool direction, int seesawPlatformID) {
    this->iSize = iSize;
    this->iType = iType;
    this->iXStart = iXStart;
    this->iXEnd = iXEnd;
    this->iYStart = iYStart;
    this->iYEnd = iYEnd;
    this->fXPos = fXPos;
    this->fYPos = fYPos;
    this->direction = direction;
    this->seesawPlatformID = seesawPlatformID;
    if(iType == 6) {
        this->iXStart = (int)fYPos;
    }
    this->ON = false;
}
Platform::~Platform(void) {
}

void Platform::Update() {
    switch(iType) {
        case 0:
            if(fYPos > iYEnd) {
                fYPos = (float)iYStart;
            } else {
                fYPos += 2;
            }
            break;
        case 1:
            if(fYPos < iYStart) {
                fYPos = (float)iYEnd;
            } else {
                fYPos -= 2;
            }
            break;
        case 2: // -- TOP & BOT
            if(direction) {
                if(fYPos < iYStart) {
                    direction = false;
                } else if(fYPos < iYStart + 32) {
                    fYPos -= 1;
                } else {
                    fYPos -= 2;
                }
            } else {
                if(fYPos > iYEnd) {
                    direction = true;
                } else if(fYPos > iYEnd - 32) {
                    fYPos += 1;
                } else {
                    fYPos += 2;
                }
            }
            break;
        case 3: // -- LEFT & RIGHT
            if(direction) {
                if(fXPos > iXEnd) {
                    direction = false;
                }
                else if(fXPos < iXStart + 16 || fXPos + 16 > iXEnd) {
                    fXPos += 0.5f;
                } else {
                    fXPos += 1;
                }
            } else {
                if(fXPos < iXStart) {
                    direction = true;
                } else if(fXPos - 16 < iXStart || fXPos + 16 > iXEnd) {
                    fXPos -= 0.5f;
                } else {
                    fXPos -= 1;
                }
            }
            break;
        case 4:
            if(fXPos > iXEnd) {
                iSize = 0;
            } else if(ON) {
                fXPos += 3;
            }
            break;
        case 7:
            if(fYPos < screen_height + 32) {
                fYPos += 4;
            }
            break;
    }
}
void Platform::Draw(SDL_Renderer* rR) {
    if(iType == 6 || iType == 7) { // -- iXStart = YPos 2
        for(int i = 1; i < (iXStart - iYEnd)/16 + 2; i++) {
            CCore::getMap()->getBlock(CCore::getMap()->getCurrentLevelID() == 22 ? 165 : 135)->getSprite()->getTexture()->Draw(rR, (int)(iXEnd + (int)CCore::getMap()->getXPos() + (iSize*8)/2 + 16 - (iSize/2%2 == 0 ? 8 : 0)), (int)(iXStart - 16*i));
        }
    }
    for(int i = 0; i < iSize; i++) {
        CCore::getMap()->getBlock(iType != 4 ? 74 : 126)->Draw(rR, (int)fXPos + i*16 + (int)CCore::getMap()->getXPos(), (int)fYPos);
    }
}

float Platform::getMoveX() {
    switch(iType) {
        case 3:
            if(direction) {
                if(fXPos < iXStart + 16 || fXPos + 16 > iXEnd) {
                    return 0.5f;
                } else {
                    return 1;
                }
            } else {
                if(fXPos - 16 < iXStart || fXPos + 16 > iXEnd) {
                    return - 0.5f;
                } else {
                    return -1;
                }
            }
            break;
        case 4:
            return 3;
    }
    return 0;
}
int Platform::getMoveY() {
    switch(iType) {
        case 0:
            return 2;
        case 1:
            return -2;
        case 2:
            if(direction) {
                if(fYPos < iYStart + 32) {
                    return -1;
                } else {
                    return -2;
                }
            } else {
                if(fYPos > iYEnd - 32) {
                    return 1;
                } else {
                    return 2;
                }
            }
        case 5:
            return 2;
        case 6:
            return 2;
        case 7:
            return 4;
    }
    return 0;
}
void Platform::moveY() {
    switch(iType) {
        case 5:
            fYPos += 2;
            break;
        case 6:
            fYPos += 2;
            iXStart = (int)fYPos;
            CCore::getMap()->getPlatform(seesawPlatformID)->moveYReverse();
            if(fYPos < iYEnd) {
                iType = 7;
                CCore::getMap()->getPlatform(seesawPlatformID)->setTypeID(7);
            }
            break;
    }
}
void Platform::moveYReverse() {
    switch(iType) {
        case 6:
            fYPos -= 2;
            iXStart = (int)fYPos;
            if(fYPos < iYEnd) {
                iType = 7;
                CCore::getMap()->getPlatform(seesawPlatformID)->setTypeID(7);
            }
            break;
    }
}

int Platform::getXPos() {
    return (int)fXPos;
}
int Platform::getYPos() {
    return (int)fYPos;
}
int Platform::getSize() {
    return iSize;
}
void Platform::setTypeID(int iType) {
    this->iType = iType;
}
int Platform::getTypeID() {
    return iType;
}
void Platform::turnON() {
    this->ON = true;
}


Player::Player(SDL_Renderer* rR, float fXPos, float fYPos) {
    this->fXPos = fXPos;
    this->fYPos = fYPos;
    this->iNumOfLives = 3;
    this->iSpriteID = 1;
    this->iScore = this->iCoins = 0;
    this->iFrameID = 0, this->iComboPoints = 1;
    this->nextBubbleTime = 0;
    this->nextFallFrameID = 0;
    this->powerLVL = 0;
    this->inLevelAnimation = false;
    this->inLevelAnimationType = false;
    this->unKillAble = false;
    this->starEffect = false;
    this->unKillAbleTimeFrameID = 0;
    this->inLevelDownAnimation = false;
    this->inLevelDownAnimationFrameID = 0;
    this->moveDirection = true;
    this->currentMaxMove = 4;
    this->moveSpeed = 0;
    this->bMove = false;
    this->changeMoveDirection = false;
    this->bSquat = false;
    this->onPlatformID = -1;
    this->springJump = false;
    this->iTimePassed = SDL_GetTicks();
    this->jumpState = 0;
    this->startJumpSpeed = 7.65f;
    this->currentFallingSpeed = 2.7f;
    this->iMoveAnimationTime = 0;
    this->nextFireBallFrameID = 8;
    // ----- LOAD SPRITE
    vector<string> tempS;
    vector<unsigned int> tempI;
    srand((unsigned)time(NULL));
    // ----- 0
    tempS.push_back("mario/mario_death");
    tempI.push_back(0);
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 1
    tempS.push_back("mario/mario");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 2
    tempS.push_back("mario/mario_move0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 3
    tempS.push_back("mario/mario_move1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 4
    tempS.push_back("mario/mario_move2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 5
    tempS.push_back("mario/mario_jump");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 6
    tempS.push_back("mario/mario_st");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 7
    tempS.push_back("mario/mario"); // SQUAT
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 8
    tempS.push_back("mario/mario_underwater0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 9
    tempS.push_back("mario/mario_underwater1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 10
    tempS.push_back("mario/mario_end");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 11
    tempS.push_back("mario/mario_end1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ---------- BIG
    // ----- 12
    tempS.push_back("mario/mario1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 13
    tempS.push_back("mario/mario1_move0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 14
    tempS.push_back("mario/mario1_move1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 15
    tempS.push_back("mario/mario1_move2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 16
    tempS.push_back("mario/mario1_jump");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 17
    tempS.push_back("mario/mario1_st");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 18
    tempS.push_back("mario/mario1_squat"); // SQUAT
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 19
    tempS.push_back("mario/mario1_underwater0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 20
    tempS.push_back("mario/mario1_underwater1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 21
    tempS.push_back("mario/mario1_end");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 22
    tempS.push_back("mario/mario1_end1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 23
    tempS.push_back("mario/mario2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 24
    tempS.push_back("mario/mario2_move0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 25
    tempS.push_back("mario/mario2_move1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 26
    tempS.push_back("mario/mario2_move2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 27
    tempS.push_back("mario/mario2_jump");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 28
    tempS.push_back("mario/mario2_st");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 29
    tempS.push_back("mario/mario2_squat"); // SQUAT
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 30
    tempS.push_back("mario/mario2_underwater0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 31
    tempS.push_back("mario/mario2_underwater1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 32
    tempS.push_back("mario/mario2_end");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 33
    tempS.push_back("mario/mario2_end1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 34
    tempS.push_back("mario/mario2s");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 35
    tempS.push_back("mario/mario2s_move0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 36
    tempS.push_back("mario/mario2s_move1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 37
    tempS.push_back("mario/mario2s_move2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 38
    tempS.push_back("mario/mario2s_jump");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 39
    tempS.push_back("mario/mario2s_st");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 40
    tempS.push_back("mario/mario2s_squat"); // SQUAT
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 41
    tempS.push_back("mario/mario2s_underwater0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 42
    tempS.push_back("mario/mario2s_underwater1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 43
    tempS.push_back("mario/mario2s_end");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 44
    tempS.push_back("mario/mario2s_end1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 45
    tempS.push_back("mario/mario_s0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 46
    tempS.push_back("mario/mario_s0_move0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 47
    tempS.push_back("mario/mario_s0_move1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 48
    tempS.push_back("mario/mario_s0_move2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 49
    tempS.push_back("mario/mario_s0_jump");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 50
    tempS.push_back("mario/mario_s0_st");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 51
    tempS.push_back("mario/mario_s0"); // SQUAT
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 52
    tempS.push_back("mario/mario_s0_underwater0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 53
    tempS.push_back("mario/mario_s0_underwater1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 54
    tempS.push_back("mario/mario_s0_end");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 55
    tempS.push_back("mario/mario_s0_end1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 56
    tempS.push_back("mario/mario_s1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 57
    tempS.push_back("mario/mario_s1_move0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 58
    tempS.push_back("mario/mario_s1_move1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 59
    tempS.push_back("mario/mario_s1_move2");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 60
    tempS.push_back("mario/mario_s1_jump");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 61
    tempS.push_back("mario/mario_s1_st");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 62
    tempS.push_back("mario/mario_s1"); // SQUAT
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 63
    tempS.push_back("mario/mario_s1_underwater0");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 64
    tempS.push_back("mario/mario_s1_underwater1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 65
    tempS.push_back("mario/mario_s1_end");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- 66
    tempS.push_back("mario/mario_s1_end1");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
    // ----- LOAD SPRITE
    // ----- 67
    tempS.push_back("mario/mario_lvlup");
    sMario.push_back(new Sprite(rR,  tempS, tempI, true));
    tempS.clear();
}
Player::~Player(void) {
    for(vector<Sprite*>::iterator i = sMario.begin(); i != sMario.end(); i++) {
        delete (*i);
    }
    delete tMarioLVLUP;
}

void Player::Update() {
    playerPhysics();
    movePlayer();
    if(iFrameID > 0) {
        --iFrameID;
    } else if(iComboPoints > 1) {
        --iComboPoints;
    }
    if(powerLVL == 2) {
        if(nextFireBallFrameID > 0) {
            --nextFireBallFrameID;
        }
    }
    if(inLevelDownAnimation) {
        if(inLevelDownAnimationFrameID > 0) {
            --inLevelDownAnimationFrameID;
        } else {
            unKillAble = false;
        }
    }
}
void Player::playerPhysics() {
    if(!CCore::getMap()->getUnderWater()) {
        if (jumpState == 1) {
            updateYPos(-(int)currentJumpSpeed);
            currentJumpDistance += (int)currentJumpSpeed;
            currentJumpSpeed *= (currentJumpDistance / jumpDistance > 0.75f ? 0.972f : 0.986f);
            if (currentJumpSpeed < 2.5f) {
                currentJumpSpeed = 2.5f;
            }
            if(!CCFG::keySpace && currentJumpDistance > 64 && !springJump) {
                jumpDistance = 16;
                currentJumpDistance = 0;
                currentJumpSpeed = 2.5f;
            }
            if (jumpDistance <= currentJumpDistance) {
                jumpState = 2;
            }
        } else {
            if(onPlatformID == -1) {
                onPlatformID = CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, getHitBoxX(), getHitBoxY());
                if(onPlatformID >= 0) {
                    if (CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) || CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {
                        onPlatformID = -1;
                        resetJump();
                    } else {
                        fYPos = (float)CCore::getMap()->getPlatform(onPlatformID)->getYPos() - getHitBoxY();
                        resetJump();
                        CCore::getMap()->getPlatform(onPlatformID)->turnON();
                    }
                }
            } else {
                onPlatformID = CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, getHitBoxX(), getHitBoxY());
            }
            if(onPlatformID >= 0) {
                if (CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) || CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {
                    onPlatformID = -1;
                    resetJump();
                } else {
                    fYPos += CCore::getMap()->getPlatform(onPlatformID)->getMoveY();
                    CCore::getMap()->getPlatform(onPlatformID)->moveY();
                    //if(moveSpeed == 0)
                    CCore::getMap()->setXPos(CCore::getMap()->getXPos() - CCore::getMap()->getPlatform(onPlatformID)->getMoveX());
                    jumpState = 0;
                }
            }
            else if (!CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) &&
                !CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {
                if(nextFallFrameID > 0) {
                    --nextFallFrameID;
                } else {
                    currentFallingSpeed *= 1.05f;
                    if (currentFallingSpeed > startJumpSpeed) {
                        currentFallingSpeed = startJumpSpeed;
                    }
                    updateYPos((int)currentFallingSpeed);
                }
                jumpState = 2;
                setMarioSpriteID(5);
            } else if(jumpState == 2) {
                resetJump();
            } else {
                checkCollisionBot(0, 0);
            }
        }
    } else {
        if(nextBubbleTime + 685 < SDL_GetTicks()) {
            CCore::getMap()->addBubble((int)(fXPos - CCore::getMap()->getXPos() + (moveDirection ? getHitBoxX() - rand()%8 : rand()%8)), (int)fYPos + 4);
            nextBubbleTime = SDL_GetTicks() + rand()%715;
        }
        if (jumpState == 1) {
            if(fYPos > screen_height - 12*32 - 16) {
                updateYPos(-2);
                currentJumpDistance += 2;
                swimingAnimation();
                if (jumpDistance <= currentJumpDistance) {
                    jumpState = 2;
                    currentJumpDistance = 0;
                    nextFallFrameID = 4;
                }
            } else {
                jumpState = 2;
                nextFallFrameID = 14;
                currentJumpDistance = 0;
            }
        } else {
            if (!CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + 2, getHitBoxY(), true) &&
                !CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + 2, getHitBoxX(), getHitBoxY(), true)) {
                if(nextFallFrameID > 0) {
                    --nextFallFrameID;
                } else {
                    if(currentJumpDistance < 32) {
                        updateYPos(1);
                        ++currentJumpDistance;
                    } else {
                        updateYPos(2);
                    }
                }
                jumpState = 2;
                swimingAnimation();
            } else if(jumpState == 2) {
                resetJump();
            }
        }
    }
}
void Player::movePlayer() {
    if (bMove && !changeMoveDirection && (!bSquat || powerLVL == 0)) {
        if (moveSpeed > currentMaxMove) {
            --moveSpeed;
        }
        else if (SDL_GetTicks() - (100 + 35 * moveSpeed) >= iTimePassed && moveSpeed < currentMaxMove) {
            ++moveSpeed;
            iTimePassed = SDL_GetTicks();
        }
        else if (moveSpeed == 0) {
            moveSpeed = 1;
        }
    } else {
        if (SDL_GetTicks() - (50 + 15 * (currentMaxMove - moveSpeed) * (bSquat && powerLVL > 0 ? 6 : 1)) > iTimePassed && moveSpeed != 0) {
            --moveSpeed;
            iTimePassed = SDL_GetTicks();
            if (jumpState == 0 && !CCore::getMap()->getUnderWater()) setMarioSpriteID(6);
        }
        if (changeMoveDirection && moveSpeed <= 1) {
            moveDirection = newMoveDirection;
            changeMoveDirection = false;
            bMove = true;
        }
    }
    if (moveSpeed > 0) {
        if (moveDirection) {
            updateXPos(moveSpeed);
        }
        else {
            updateXPos(-moveSpeed);
        }
        // ----- SPRITE ANIMATION
        if(CCore::getMap()->getUnderWater()) {
            swimingAnimation();
        } else if (!changeMoveDirection && jumpState == 0 && bMove) {
            moveAnimation();
        }
        // ----- SPRITE ANIMATION
    }
    else if (jumpState == 0) {
        setMarioSpriteID(1);
        updateXPos(0);
    } else {
        updateXPos(0);
    }
    if(bSquat && !CCore::getMap()->getUnderWater() && powerLVL > 0) {
        setMarioSpriteID(7);
    }
}

void Player::powerUPAnimation() {
    if(inLevelDownAnimation) {
        if(inLevelAnimationFrameID%15 < 5) {
            iSpriteID = 12;
            if(inLevelAnimationFrameID != 0 && inLevelAnimationFrameID%15 == 0) {
                fYPos += 16;
                fXPos -= 4;
            }
        } else if(inLevelAnimationFrameID%15 < 10) {
            iSpriteID = 67;
            if(inLevelAnimationFrameID%15 == 5) {
                fYPos += 16;
                fXPos += 1;
            }
        } else {
            iSpriteID = 1;
            if(inLevelAnimationFrameID%15 == 10) {
                fYPos -= 32;
                fXPos += 3;
            }
        }
        ++inLevelAnimationFrameID;
        if(inLevelAnimationFrameID > 59) {
            inLevelAnimation = false;
            fYPos += 32;
            if(jumpState != 0) {
                setMarioSpriteID(5);
            }
        }
    } else if(powerLVL == 1) {
        if(inLevelAnimationFrameID%15 < 5) {
            iSpriteID = 1;
            if(inLevelAnimationFrameID != 0 && inLevelAnimationFrameID%15 == 0) {
                fYPos += 32;
                fXPos += 4;
            }
        } else if(inLevelAnimationFrameID%15 < 10) {
            iSpriteID = 67;
            if(inLevelAnimationFrameID%15 == 5) {
                fYPos -= 16;
                fXPos -= 3;
            }
        } else {
            iSpriteID = 12;
            if(inLevelAnimationFrameID%15 == 10) {
                fYPos -= 16;
                fXPos -= 1;
            }
        }
        ++inLevelAnimationFrameID;
        if(inLevelAnimationFrameID > 59) {
            inLevelAnimation = false;
            if(jumpState != 0) {
                setMarioSpriteID(5);
            }
        }
    } else if(powerLVL == 2) {
        if(inLevelAnimationFrameID%10 < 5) {
            iSpriteID = iSpriteID%11 + 22;
        } else {
            iSpriteID = iSpriteID%11 + 33;
        }
        ++inLevelAnimationFrameID;
        if(inLevelAnimationFrameID > 59) {
            inLevelAnimation = false;
            if(jumpState != 0) {
                setMarioSpriteID(5);
            }
            iSpriteID = iSpriteID%11 + 22;
        }
    } else {
        inLevelAnimation = false;
    }
}
void Player::moveAnimation() {
    if(SDL_GetTicks() - 65 + moveSpeed * 4 > iMoveAnimationTime) {
        iMoveAnimationTime = SDL_GetTicks();
        if (iSpriteID >= 4 + 11 * powerLVL) {
            setMarioSpriteID(2);
        }
        else {
            ++iSpriteID;
        }
    }
}
void Player::swimingAnimation() {
    if(SDL_GetTicks() - 105 > iMoveAnimationTime) {
        iMoveAnimationTime = SDL_GetTicks();
        if(iSpriteID % 11 == 8) {
            setMarioSpriteID(9);
        } else {
            setMarioSpriteID(8);
        }
    }
}
void Player::startMove() {
    iMoveAnimationTime = SDL_GetTicks();
    iTimePassed = SDL_GetTicks();
    moveSpeed = 1;
    bMove = true;
    if(CCore::getMap()->getUnderWater()) {
        setMarioSpriteID(8);
    }
}
void Player::resetMove() {
    --moveSpeed;
    bMove = false;
}
void Player::stopMove() {
    moveSpeed = 0;
    bMove = false;
    changeMoveDirection = false;
    bSquat = false;
    setMarioSpriteID(1);
}
void Player::startRun() {
    currentMaxMove = maxMove + (CCore::getMap()->getUnderWater() ? 0 : 2);
    createFireBall();
}
void Player::resetRun() {
    currentMaxMove = maxMove;
}
void Player::createFireBall() {
    if(powerLVL == 2) {
        if(nextFireBallFrameID <= 0) {
            CCore::getMap()->addPlayerFireBall((int)(fXPos - CCore::getMap()->getXPos() + (moveDirection ? getHitBoxX() : -32)), (int)(fYPos + getHitBoxY()/2), !moveDirection);
            nextFireBallFrameID = 16;
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cFIREBALL);
        }
    }
}

void Player::jump() {
    if(CCore::getMap()->getUnderWater()) {
        startJump(1);
        nextBubbleTime -= 65;
    } else if(jumpState == 0) {
        startJump(4);
    }
}
void Player::startJump(int iH) {
    currentJumpSpeed = startJumpSpeed;
    jumpDistance = 32 * iH + 24.0f;
    currentJumpDistance = 0;
    if(!CCore::getMap()->getUnderWater()) {
        setMarioSpriteID(5);
    } else {
        if(jumpState == 0) {
            iMoveAnimationTime = SDL_GetTicks();
            setMarioSpriteID(8);
            swimingAnimation();
        }
        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSWIM);
    }
    if(iH > 1) {
        if(powerLVL == 0) {
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cJUMP);
        } else {
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cJUMPBIG);
        }
    }
    jumpState = 1;
}
void Player::resetJump() {
    jumpState = 0;
    jumpDistance = 0;
    currentJumpDistance = 0;
    currentFallingSpeed = 2.7f;
    nextFallFrameID = 0;
    springJump = false;
}

void Player::updateXPos(int iN) {
    checkCollisionBot(iN, 0);
    checkCollisionCenter(iN, 0);
    if (iN > 0) {
        if (!CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos - 2, getHitBoxX(), getHitBoxY(), true) && !CCore::getMap()->checkCollisionRT((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos + 2, getHitBoxX(), true) &&
            (powerLVL == 0 ? true : (!CCore::getMap()->checkCollisionRC((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos, getHitBoxX(), getHitBoxY() / 2, true))))
        {
            if (fXPos >= 416 && CCore::getMap()->getMoveMap()) {
                CCore::getMap()->moveMap(-iN, 0);
            }
            else {
                fXPos += iN;
            }
        }
        else {
            updateXPos(iN - 1);
            if (moveSpeed > 1 && jumpState == 0) --moveSpeed;
        }
    } else if (iN < 0) {
        if (!CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos - 2, getHitBoxY(), true) && !CCore::getMap()->checkCollisionLT((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos + 2, true) &&
                (powerLVL == 0 ? true : (!CCore::getMap()->checkCollisionLC((int)(fXPos - CCore::getMap()->getXPos() + iN), (int)fYPos, getHitBoxY() / 2, true))))
            {
            if (fXPos <= 192 && CCore::getMap()->getXPos() && CCore::getMap()->getMoveMap() && CCFG::canMoveBackward) {
                CCore::getMap()->moveMap(-iN, 0);
            }
            else if(fXPos - CCore::getMap()->getXPos() + iN >= 0 && fXPos >= 0) {
                fXPos += iN;
            } else if(CCFG::canMoveBackward && fXPos >= 0) {
                updateXPos(iN + 1);
            }
        }
        else {
            updateXPos(iN + 1);
            if (moveSpeed > 1 && jumpState == 0) --moveSpeed;
        }
    }
}
void Player::updateYPos(int iN)
{
    bool bLEFT, bRIGHT;
    if (iN > 0)
	{
        bLEFT  = CCore::getMap()->checkCollisionLB((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + iN, getHitBoxY(), true);
        bRIGHT = CCore::getMap()->checkCollisionRB((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + iN, getHitBoxX(), getHitBoxY(), true);
        if (!bLEFT && !bRIGHT)
		{
            fYPos += iN;
        }
		else
		{
            if (jumpState == 2)
			{
                jumpState = 0;
            }
            updateYPos(iN - 1);
        }
    }
	else if(iN < 0)
	{
        bLEFT  = CCore::getMap()->checkCollisionLT((int)(fXPos - CCore::getMap()->getXPos() + 2), (int)fYPos + iN, false);
        bRIGHT = CCore::getMap()->checkCollisionRT((int)(fXPos - CCore::getMap()->getXPos() - 2), (int)fYPos + iN, getHitBoxX(), false);
        if(CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, 0, 0) >= 0 || CCore::getMap()->checkCollisionWithPlatform((int)fXPos, (int)fYPos, getHitBoxX(), 0) >= 0) {
            jumpState = 2;
        }
        else if (!bLEFT && !bRIGHT) {
            fYPos += iN;
        } else  {
            if (jumpState == 1) {
                if (!bLEFT && bRIGHT) {
                    Vector2* vRT = getBlockRT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);
                    if(!CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID())->getVisible()) {
                        if(CCore::getMap()->blockUse(vRT->getX(), vRT->getY(), CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID(), 0)) {
                            jumpState = 2;
                        } else {
                            fYPos += iN;
                        }
                    } else if((int)(fXPos + getHitBoxX() - CCore::getMap()->getXPos()) % 32 <= 8) {
                        updateXPos((int)-((int)(fXPos + getHitBoxX() - CCore::getMap()->getXPos()) % 32));
                    } else if(CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID())->getUse()) {
                        if(CCore::getMap()->blockUse(vRT->getX(), vRT->getY(), CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID(), 0)) {
                            jumpState = 2;
                        } else {
                            fYPos += iN;
                        }
                    } else {
                        jumpState = 2;
                    }
                    delete vRT;
                } else if (bLEFT && !bRIGHT) {
                    Vector2* vLT = getBlockLT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);
                    if(!CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getVisible()) {
                        if(CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 0)) {
                            jumpState = 2;
                        } else {
                            fYPos += iN;
                        }
                    } else if ((int)(fXPos - CCore::getMap()->getXPos()) % 32 >= 24) {
                        updateXPos((int)(32 - (int)(fXPos - CCore::getMap()->getXPos()) % 32));
                    } else if(CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
                        if(CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 0)) {
                            jumpState = 2;
                        } else {
                            fYPos += iN;
                        }
                    } else {
                        jumpState = 2;
                    }
                    delete vLT;
                } else {
                    if ((int)(fXPos + getHitBoxX() - CCore::getMap()->getXPos()) % 32 > 32 - (int)(fXPos - CCore::getMap()->getXPos()) % 32) {
                        Vector2* vRT = getBlockRT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);
                        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID())->getUse()) {
                            if(CCore::getMap()->blockUse(vRT->getX(), vRT->getY(), CCore::getMap()->getMapBlock(vRT->getX(), vRT->getY())->getBlockID(), 0)) {
                                jumpState = 2;
                            }
                        } else {
                            jumpState = 2;
                        }
                        delete vRT;
                    } else {
                        Vector2* vLT = getBlockLT(fXPos - CCore::getMap()->getXPos(), fYPos + iN);
                        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
                            if(CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 0)) {
                                jumpState = 2;
                            }
                        } else {
                            jumpState = 2;
                        }
                        delete vLT;
                    }
                }
            }
            updateYPos(iN + 1);
        }
    }
    if((int)fYPos % 2 == 1)
	{
        fYPos += 1;
    }
    if(!CCore::getMap()->getInEvent() && fYPos - getHitBoxY() > screen_height) {
        CCore::getMap()->playerDeath(false, true);
        fYPos = -80;
    }
}

bool Player::checkCollisionBot(int nX, int nY) {
    Vector2* vLT = getBlockLB(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);
    if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
        CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 1);
    }
    delete vLT;
    vLT = getBlockRB(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);
    if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
        CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 1);
    }
    delete vLT;
    return true;
}
bool Player::checkCollisionCenter(int nX, int nY) {
    if(powerLVL == 0) {
        Vector2* vLT = getBlockLC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);
        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
            CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
        }
        delete vLT;
        vLT = getBlockRC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY);
        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
            CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
        }
        delete vLT;
    } else {
        Vector2* vLT = getBlockLC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY + (powerLVL > 0 ? 16 : 0));
        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
            CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
        }
        delete vLT;
        vLT = getBlockRC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY + (powerLVL > 0 ? 16 : 0));
        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
            CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
        }
        delete vLT;
        vLT = getBlockLC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY - (powerLVL > 0 ? 16 : 0));
        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
            CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
        }
        delete vLT;
        vLT = getBlockRC(fXPos - CCore::getMap()->getXPos() + nX, fYPos + nY - (powerLVL > 0 ? 16 : 0));
        if (CCore::getMap()->getBlock(CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID())->getUse()) {
            CCore::getMap()->blockUse(vLT->getX(), vLT->getY(), CCore::getMap()->getMapBlock(vLT->getX(), vLT->getY())->getBlockID(), 2);
        }
        delete vLT;
    }
    return true;
}

Vector2* Player::getBlockLB(float nX, float nY) {
    return CCore::getMap()->getBlockID((int)nX + 1, (int)nY + getHitBoxY() + 2);
}
Vector2* Player::getBlockRB(float nX, float nY) {
    return CCore::getMap()->getBlockID((int)nX + getHitBoxX() - 1, (int)nY + getHitBoxY() + 2);
}
Vector2* Player::getBlockLC(float nX, float nY) {
    return CCore::getMap()->getBlockID((int)nX - 1, (int)nY + getHitBoxY()/2);
}
Vector2* Player::getBlockRC(float nX, float nY) {
    return CCore::getMap()->getBlockID((int)nX + getHitBoxX() + 1, (int)nY + getHitBoxY()/2);
}
Vector2* Player::getBlockLT(float nX, float nY) {
    return CCore::getMap()->getBlockID((int)nX + 1, (int)nY);
}
Vector2* Player::getBlockRT(float nX, float nY) {
    return CCore::getMap()->getBlockID((int)nX + getHitBoxX() - 1, (int)nY);
}

void Player::Draw(SDL_Renderer* rR) {
    if(!inLevelDownAnimation || CCore::getMap()->getInEvent()) {
        sMario[getMarioSpriteID()]->getTexture()->Draw(rR, (int)fXPos, (int)fYPos + (CCore::getMap()->getInEvent() ? 0 : 2), !moveDirection);
    } else {
        if(inLevelDownAnimationFrameID%15 < (inLevelDownAnimationFrameID > 120 ? 7 : inLevelDownAnimationFrameID > 90 ? 9 : inLevelDownAnimationFrameID > 60 ? 11 : inLevelDownAnimationFrameID > 30 ? 13 : 14)) {
            sMario[getMarioSpriteID()]->getTexture()->Draw(rR, (int)fXPos, (int)fYPos + (CCore::getMap()->getInEvent() ? 0 : 2), !moveDirection);
        }
    }
}

int Player::getMarioSpriteID() {
    if(starEffect && !inLevelAnimation && CCFG::getMM()->getViewID() == CCFG::getMM()->eGame) {
        if(unKillAbleTimeFrameID <= 0) {
            starEffect = unKillAble = false;
        }
        if(unKillAbleTimeFrameID == 35) {
            CCFG::getMusic()->changeMusic(true, true);
        }
        ++unKillAbleFrameID;
        --unKillAbleTimeFrameID;
        if(unKillAbleTimeFrameID < 90) {
            if(unKillAbleFrameID < 5) {
                return powerLVL < 1 ? iSpriteID + 44 : powerLVL == 2 ? iSpriteID : iSpriteID + 11;
            } else if(unKillAbleFrameID < 10) {
                return powerLVL < 1 ? iSpriteID + 55 : powerLVL == 2 ? iSpriteID + 11 : iSpriteID + 22;
            } else {
                unKillAbleFrameID = 0;
            }
        } else {
            if(unKillAbleFrameID < 20) {
                return powerLVL < 1 ? iSpriteID + 44 : powerLVL == 2 ? iSpriteID : iSpriteID + 11;
            } else if(unKillAbleFrameID < 40) {
                return powerLVL < 1 ? iSpriteID + 55 : powerLVL == 2 ? iSpriteID + 11 : iSpriteID + 22;
            } else {
                unKillAbleFrameID = 0;
            }
        }
    }
    return iSpriteID;
}
void Player::setMarioSpriteID(int iID) {
    this->iSpriteID = iID + 11 * powerLVL;
}
int Player::getHitBoxX() {
    return powerLVL == 0 ? iSmallX : iBigX;
}
int Player::getHitBoxY() {
    return powerLVL == 0 ? iSmallY : bSquat ? 44 : iBigY;
}

void Player::addCoin()
{
    ++iCoins;
    iScore += 100;
}

void Player::setMoveDirection(bool moveDirection)
{
    this->moveDirection = moveDirection;
}
bool Player::getChangeMoveDirection()
{
    return changeMoveDirection;
}
void Player::setChangeMoveDirection()
{
    this->changeMoveDirection = true;
    this->newMoveDirection = !moveDirection;
}

bool Player::getInLevelAnimation() {
    return inLevelAnimation;
}
void Player::setInLevelAnimation(bool inLevelAnimation) {
    this->inLevelAnimation = inLevelAnimation;
}
int Player::getXPos() {
    return (int)fXPos;
}
void Player::setXPos(float fXPos) {
    this->fXPos = fXPos;
}
int Player::getYPos() {
    return (int)fYPos;
}
int Player::getPowerLVL() {
    return powerLVL;
}
void Player::setPowerLVL(int powerLVL) {
    if(powerLVL <= 2) {
        if(this->powerLVL < powerLVL) {
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cMUSHROOMMEAT);
            setScore(getScore() + 1000);
            CCore::getMap()->addPoints((int)(fXPos - CCore::getMap()->getXPos() + getHitBoxX() / 2), (int)fYPos + 16, "1000", 8, 16);
            inLevelAnimation = true;
            inLevelAnimationFrameID = 0;
            inLevelDownAnimationFrameID = 0;
            inLevelDownAnimation = false;
            this->powerLVL = powerLVL;
        } else if(this->powerLVL > 0) {
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cSHRINK);
            inLevelDownAnimation = true;
            inLevelDownAnimationFrameID = 180;
            inLevelAnimation = true;
            inLevelAnimationFrameID = 0;
            this->powerLVL = 0;
            unKillAble = true;
        }
    } else {
        ++iNumOfLives;
        CCore::getMap()->addPoints((int)(fXPos - CCore::getMap()->getXPos() + getHitBoxX() / 2), (int)fYPos + 16, "1UP", 10, 14);
        CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cONEUP);
    }
}
void Player::resetPowerLVL() {
    this->powerLVL = 0;
    this->iSpriteID = 1;
}
int Player::getNumOfLives() {
    return iNumOfLives;
}
bool Player::getStarEffect() {
    return starEffect;
}
void Player::setStarEffect(bool starEffect) {
    if(starEffect && this->starEffect != starEffect) {
        CCFG::getMusic()->PlayMusic(CCFG::getMusic()->mSTAR);
    }
    this->starEffect = starEffect;
    this->unKillAble = starEffect;
    this->unKillAbleFrameID = 0;
    this->unKillAbleTimeFrameID = 550;
}
bool Player::getUnkillAble() {
    return unKillAble;
}
void Player::setNumOfLives(int iNumOfLives) {
    this->iNumOfLives = iNumOfLives;
}
int Player::getMoveSpeed() {
    return moveSpeed;
}
int Player::getJumpState() {
    return jumpState;
}
bool Player::getMove() {
    return bMove;
}
bool Player::getMoveDirection() {
    return moveDirection;
}
void Player::setNextFallFrameID(int nextFallFrameID) {
    this->nextFallFrameID = nextFallFrameID;
}
void Player::setCurrentJumpSpeed(float currentJumpSpeed) {
    this->currentJumpSpeed = currentJumpSpeed;
}
void Player::setMoveSpeed(int moveSpeed) {
    this->moveSpeed = moveSpeed;
}
void Player::setYPos(float fYPos) {
    this->fYPos = fYPos;
}
bool Player::getSquat() {
    return bSquat;
}
void Player::setSquat(bool bSquat) {
    if(bSquat && this->bSquat != bSquat) {
        if(powerLVL > 0) {
            fYPos += 20;
        }
        this->bSquat = bSquat;
    } else if(this->bSquat != bSquat && !CCore::getMap()->getUnderWater()) {
        if(powerLVL > 0) {
            fYPos -= 20;
        }
        this->bSquat = bSquat;
    }
}
void Player::setSpringJump(bool springJump) {
    this->springJump = springJump;
}
unsigned int Player::getScore() {
    return iScore;
}
void Player::setScore(unsigned int iScore) {
    this->iScore = iScore;
}
void Player::addComboPoints() {
    ++iComboPoints;
    iFrameID = 40;
}
int Player::getComboPoints() {
    return iComboPoints;
}
unsigned int Player::getCoins() {
    return iCoins;
}
void Player::setCoins(unsigned int iCoins) {
    this->iCoins = iCoins;
}
Sprite* Player::getMarioSprite()
{
    return sMario[1 + 11 * powerLVL];
}

CIMG* Player::getMarioLVLUP()
{
    return tMarioLVLUP;
}

PlayerFireBall::PlayerFireBall(int iXPos, int iYPos, bool moveDirection)
{
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos;
    this->moveDirection = moveDirection;
    this->killOtherUnits = true;
    this->minionSpawned = true;
    this->moveSpeed = 14;
    this->iBlockID = 62;
    this->jumpState = 2;
    this->iHitBoxX = 16;
    this->iHitBoxY = 16;
    this->bDestroy = false;
    this->destroyFrameID = 15;
}

PlayerFireBall::~PlayerFireBall(void)
{
}

void PlayerFireBall::Update() {
    if(bDestroy) {
        if(destroyFrameID > 10) {
            this->iBlockID = 63;
        } else if(destroyFrameID > 5) {
            this->iBlockID = 64;
        } else if(destroyFrameID > 0) {
            this->iBlockID = 65;
        } else {
            minionState = -1;
        }
        --destroyFrameID;
    } else {
        if(jumpState == 0) {
            jumpState = 1;
            currentJumpSpeed = startJumpSpeed;
            jumpDistance = 34.0f;
            currentJumpDistance = 0;
        } else if(jumpState == 1) {
            updateYPos(-1);
            currentJumpDistance += 1;
            if (jumpDistance <= currentJumpDistance) {
                jumpState = 2;
            }
        } else {
            if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true) && !onAnotherMinion) {
                updateYPos(1);
                jumpState = 2;
                if (fYPos >= screen_height) {
                    minionState = -1;
                }
            } else {
                jumpState = 0;
                onAnotherMinion = false;
            }
        }
        updateXPos();
    }
}
void PlayerFireBall::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(!bDestroy) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, !moveDirection);
    } else {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - (moveDirection ? 16 : 0), (int)fYPos - 8, !moveDirection);
    }
}
void PlayerFireBall::updateXPos() {
    // ----- LEFT
    if (moveDirection) {
        if (CCore::getMap()->checkCollisionLB((int)fXPos - moveSpeed, (int)fYPos - 2, iHitBoxY, true) || CCore::getMap()->checkCollisionLT((int)fXPos - moveSpeed, (int)fYPos + 2, true)) {
            bDestroy = true;
            collisionOnlyWithPlayer = true;
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
        } else {
            fXPos -= (jumpState == 0 ? moveSpeed : moveSpeed/2.0f);
        }
    }
    // ----- RIGHT
    else {
        if (CCore::getMap()->checkCollisionRB((int)fXPos + moveSpeed, (int)fYPos - 2, iHitBoxX, iHitBoxY, true) || CCore::getMap()->checkCollisionRT((int)fXPos + moveSpeed, (int)fYPos + 2, iHitBoxX, true)) {
            bDestroy = true;
            collisionOnlyWithPlayer = true;
            CCFG::getMusic()->PlayChunk(CCFG::getMusic()->cBLOCKHIT);
        } else {
            fXPos += (jumpState == 0 ? moveSpeed : moveSpeed/2.0f);
        }
    }
    if(fXPos < -iHitBoxX) {
        minionState = -1;
    }
}
void PlayerFireBall::minionPhysics() {
    if(!bDestroy) {
        Minion::minionPhysics();
    }
}

void PlayerFireBall::collisionWithPlayer(bool TOP) { }
void PlayerFireBall::collisionWithAnotherUnit() {
    bDestroy = true;
    collisionOnlyWithPlayer = true;
}
void PlayerFireBall::collisionEffect() {
}
void PlayerFireBall::setMinionState(int minionState) {
}


Points::Points(int iXPos, int iYPos, string sText)
{
    this->iXPos = iXPos;
    this->iYPos = iYPos;
    this->sText = sText;
    this->iYLEFT = 96;
    this->iW = 8;
    this->iH = 16;
    this->bDelete = false;
}
Points::Points(int iXPos, int iYPos, string sText, int iW, int iH) {
    this->iXPos = iXPos;
    this->iYPos = iYPos;
    this->sText = sText;
    this->iYLEFT = 96;
    this->iW = iW;
    this->iH = iH;
    this->bDelete = false;
}
Points::~Points(void) {
}

void Points::Update() {
    if(iYLEFT > 0) {
        iYPos -= 2;
        iYLEFT -= 2;
    } else {
        bDelete = true;
    }
}
void Points::Draw(SDL_Renderer* rR) {
    CCFG::getText()->Draw(rR, sText, iXPos + (int)CCore::getMap()->getXPos(), iYPos, iW, iH);
}

bool Points::getDelete() {
    return bDelete;
}

Spikey::Spikey(int iXPos, int iYPos) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos + 6;
    this->iHitBoxX = 32;
    this->iHitBoxY = 26;
    this->iBlockID = 52;
    this->startJump(1);
    this->moveSpeed = 0;
}
Spikey::~Spikey(void) {
}

void Spikey::Update() {
    if (minionState == 0) {
        updateXPos();
    } else if(minionState == -2) {
        Minion::minionDeathAnimation();
    }
}
void Spikey::Draw(SDL_Renderer* rR, CIMG* iIMG) {
    if(minionState != -2) {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos - 4, !moveDirection);
    } else {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2);
    }
}
void Spikey::minionPhysics() {
    if (jumpState == 1) {
        physicsState1();
    } else {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true) && !onAnotherMinion) {
            physicsState2();
        } else {
            jumpState = 0;
            onAnotherMinion = false;
            if(iBlockID == 52) {
                iBlockID = 51;
                moveDirection = fXPos + iHitBoxX/2 > CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2;
                moveSpeed = 1;
            }
        }
    }
}

void Spikey::collisionWithPlayer(bool TOP) {
    if(CCore::getMap()->getPlayer()->getStarEffect()) {
        setMinionState(-2);
    } else {
        CCore::getMap()->playerDeath(true, false);
    }
}

Spring::Spring(int iXPos, int iYPos) {
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos + 2;
    this->iHitBoxX = 32;
    this->iHitBoxY = 72;
    this->iBlockID = CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 37 : 40;
    this->minionState = 0;
    this->nextFrameID = 4;
    this->inAnimation = false;
}
Spring::~Spring(void) {
}

void Spring::Update() {
    if(inAnimation) {
        if(CCFG::keySpace) {
            extraJump = true;
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->setNextFallFrameID(16);
        } else {
            CCore::getMap()->getPlayer()->stopMove();
        }
        CCore::getMap()->getPlayer()->setMarioSpriteID(5);
        if(nextFrameID <= 0) {
            switch(minionState) {
                case 0:
                    iBlockID = iBlockID == 37 ? 38 : 41;
                    minionState = 1;
                    CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() + 16.0f);
                    break;
                case 1:
                    iBlockID = iBlockID == 38 ? 39 : 42;
                    minionState = 2;
                    CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() + 16.0f);
                    break;
                case 2:
                    iBlockID = iBlockID == 39 ? 38 : 41;
                    minionState = 3;
                    CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() - 16.0f);
                    break;
                case 3:
                    iBlockID = iBlockID == 38 ? 37 : 40;
                    minionState = 0;
                    CCore::getMap()->getPlayer()->setYPos(CCore::getMap()->getPlayer()->getYPos() - 16.0f);
                    CCore::getMap()->getPlayer()->resetJump();
                    CCore::getMap()->getPlayer()->startJump(4 + (extraJump ? 5 : 0));
                    CCore::getMap()->getPlayer()->setSpringJump(true);
                    CCore::getMap()->getPlayer()->startMove();
                    if(extraJump) CCore::getMap()->getPlayer()->setCurrentJumpSpeed(10.5f);
                    inAnimation = false;
                    break;
            }
            nextFrameID = 4;
        } else {
            --nextFrameID;
        }
    }
}
void Spring::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, false);
}

void Spring::minionPhysics() { }

void Spring::collisionWithPlayer(bool TOP) {
    if(!inAnimation) {
        if(TOP && CCore::getMap()->getPlayer()->getJumpState() == 2) {
            CCore::getMap()->getPlayer()->stopMove();
            CCore::getMap()->getPlayer()->resetJump();
            CCore::getMap()->getPlayer()->setNextFallFrameID(16);
            inAnimation = true;
            extraJump = false;
            CCFG::keySpace = false;
        } else {
            if(CCore::getMap()->getPlayer()->getMoveDirection()) {
                CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getPlayer()->getMoveSpeed());
            } else {
                CCore::getMap()->getPlayer()->setXPos((float)CCore::getMap()->getPlayer()->getXPos() + CCore::getMap()->getPlayer()->getMoveSpeed());
            }
        }
    }
}
void Spring::setMinionState(int minionState) { }




Squid::Squid(int iXPos, int iYPos)
{
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos;
    this->iHitBoxX = 32;
    this->iBlockID = 29;
    this->minionState = 0;
    this->moveDirection = false;
    this->moveSpeed = 2;
    this->moveXDistance = 96;
    this->moveYDistance = 32;
    this->collisionOnlyWithPlayer = true;
    srand((unsigned)time(NULL));
}

Squid::~Squid(void)
{
}

void Squid::Update() {
    if(CCore::getMap()->getUnderWater()) {
        if(moveXDistance <= 0) {
            if(moveYDistance > 0) {
                fYPos += 1;
                moveYDistance -= 1;
                if(moveYDistance == 0) {
                    changeBlockID();
                }
            } else {
                if(fYPos + 52 > CCore::getMap()->getPlayer()->getYPos()) {
                    moveDirection = CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos() + CCore::getMap()->getPlayer()->getHitBoxX()/2 > fXPos;
                    moveXDistance = 96 + rand()%32;
                    changeBlockID();
                } else {
                    fYPos += 1;
                }
            }
        } else {
            if(moveXDistance == 64) changeBlockID();
            fXPos += moveDirection ? 2 : -2;
            if(fYPos > screen_height - 12*32 - 4) {
                fYPos -= 2;
            }
            moveXDistance -= 2;
            if(moveXDistance <= 0) {
                changeBlockID();
                moveYDistance = 32;
            }
        }
    }
}
void Squid::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos);
}
void Squid::minionPhysics() { }

void Squid::collisionWithPlayer(bool TOP)
{
    CCore::getMap()->playerDeath(true, false);
}

void Squid::changeBlockID()
{
    switch(iBlockID)
    {
        case 28:
            this->iBlockID = 29;
            this->iHitBoxY = 28;
            break;
        default:
            this->iBlockID = 28;
            this->iHitBoxY = 28;
            break;
    }
}

Star::Star(int iXPos, int iYPos, int iX, int iY)
{
    this->fXPos = (float)iXPos + 2;
    this->fYPos = (float)iYPos;
    this->collisionOnlyWithPlayer = true;
    this->moveDirection = false;
    this->moveSpeed = 3;
    this->inSpawnState = true;
    this->minionSpawned = true;
    this->inSpawnY = 30;
    this->startJumpSpeed = 4;
    this->iX = iX;
    this->iY = iY;
    this->iBlockID = 24;
    this->iHitBoxX = 28;
    this->iHitBoxY = 32;
}

Star::~Star(void)
{
}

void Star::Update()
{
    if (inSpawnState)
    {
        if (inSpawnY <= 0)
        {
            inSpawnState = false;
        }

        else
        {
            if (fYPos > -5)
            {
                inSpawnY -= 2;
                fYPos -= 2;
            }

            else
            {
                inSpawnY -= 1;
                fYPos -= 1;
            }
        }
    }

    else
    {
        if(jumpState == 0)
        {
            startJump(1);
            jumpDistance = 32;
        }

        updateXPos();
    }
}

bool Star::updateMinion()
{
    if (!inSpawnState)
    {
        minionPhysics();
    }
    return minionSpawned;
}

void Star::minionPhysics() {
    if (jumpState == 1) {
        if(minionState == 0) {
            updateYPos(-4 + (currentJumpDistance > 64 ? 2 : 0));
            currentJumpDistance += 2;
            if (jumpDistance <= currentJumpDistance) {
                jumpState = 2;
            }
        }
    } else {
        if (!CCore::getMap()->checkCollisionLB((int)fXPos + 2, (int)fYPos + 2, iHitBoxY, true) && !CCore::getMap()->checkCollisionRB((int)fXPos - 2, (int)fYPos + 2, iHitBoxX, iHitBoxY, true)) {
            Minion::physicsState2();
        } else {
            jumpState = 0;
        }
    }
}
void Star::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos + 2, false);
    if (inSpawnState)
    {
        CCore::getMap()->getBlock(CCore::getMap()->getLevelType() == 0 || CCore::getMap()->getLevelType() == 4 ? 9 : 56)->getSprite()->getTexture()->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos() - 2, (int)fYPos + (32 - inSpawnY) - CCore::getMap()->getMapBlock(iX, iY)->getYPos(), false);
    }
}

void Star::collisionWithPlayer(bool TOP)
{
    if(!inSpawnState)
    {
        CCore::getMap()->getPlayer()->setStarEffect(true);
        minionState = -1;
    }
}
void Star::setMinionState(int minionState) { }



Toad::Toad(int iXPos, int iYPos, bool peach)
{
    this->fXPos = (float)iXPos;
    this->fYPos = (float)iYPos;
    this->moveSpeed = 0;
    this->collisionOnlyWithPlayer = true;
    this->minionSpawned = true;
    this->iBlockID = peach ? 27 : 26;
    this->iHitBoxX = 32;
    this->iHitBoxY = 48;
}

Toad::~Toad(void)
{
}

void Toad::Update() { }
void Toad::minionPhysics() { }

void Toad::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    iIMG->Draw(rR,(int)(fXPos + CCore::getMap()->getXPos()), (int)fYPos, !moveDirection);
}

void Toad::collisionWithPlayer(bool TOP) { }
void Toad::setMinionState(int minionState) { }

UpFire::UpFire(int iXPos, int iYJump)
{
    this->fXPos = (float)iXPos;
    this->fYPos = (float)screen_height + 16;
    this->moveDirection = true;
    this->nextJumpFrameID = 0;
    this->collisionOnlyWithPlayer = true;
    this->iYJump = iYJump + 16;
    this->jumpDistance = (float)iYJump + 16;
    this->iBlockID = 32;
    srand((unsigned)time(NULL));
}

UpFire::~UpFire(void)
{
}

void UpFire::Update()
{
    if(nextJumpFrameID <= 0)
    {
        if(moveDirection)
        {
            if(jumpDistance < 32)
            {
                fYPos -= 2;
                jumpDistance -= 2;
            }

            else if(jumpDistance < 2*32)
            {
                fYPos -= 4;
                jumpDistance -= 4;
            }

            else if(jumpDistance < 4*32)
            {
                fYPos -= 5;
                jumpDistance -= 5;
            }

            else
            {
                fYPos -= 6;
                jumpDistance -= 6;
            }

            if(jumpDistance <= 0)
            {
                moveDirection = !moveDirection;
                jumpDistance = (float)iYJump;
            }
        }

        else
        {
            if(jumpDistance > iYJump - 32)
            {
                fYPos += 2;
                jumpDistance -= 2;
            }

            else if(jumpDistance > iYJump - 2*32)
            {
                fYPos += 4;
                jumpDistance -= 4;
            }

            else if(jumpDistance > iYJump - 4*32)
            {
                fYPos += 5;
                jumpDistance -= 5;
            }

            else
            {
                fYPos += 6;
                jumpDistance -= 6;
            }

            if(jumpDistance <= 0)
            {
                moveDirection = !moveDirection;
                jumpDistance = (float)iYJump;
                nextJumpFrameID = 25 + rand()%355;
            }
        }
    }

    else
    {
        --nextJumpFrameID;
    }
}

void UpFire::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    if(moveDirection)
    {
        iIMG->Draw(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos, false);
    }

    else
    {
        iIMG->DrawVert(rR, (int)fXPos + (int)CCore::getMap()->getXPos(), (int)fYPos);
    }
}
void UpFire::minionPhysics() { }
void UpFire::collisionWithPlayer(bool TOP)
{
    CCore::getMap()->playerDeath(true, false);
}


Vine::Vine(int iXPos, int iYPos, int minionState, int iBlockID)
{
    this->fXPos = (float)iXPos*32 + 4;
    this->fYPos = (float)(screen_height - 16 - iYPos*32);
    this->iX = iXPos;
    this->iY = iYPos;
    this->minionState = minionState;
    this->iBlockID = iBlockID;
    this->moveSpeed = 0;
    this->collisionOnlyWithPlayer = true;
    this->jumpDistance = 0;
    this->minionSpawned = true;
    this->iHitBoxX = 24;
    this->iHitBoxY = -32;
}

Vine::~Vine(void)
{
}

void Vine::Update()
{
    if(minionState == 0)
    {
        if(screen_height + 16 - iY * 32 >= jumpDistance)
        {
            jumpDistance += 2;
            iHitBoxY += 2;
            fYPos -= 2;
        }
    }

    else
    {
        if(jumpDistance < 256)
        {
            jumpDistance += 2;
            iHitBoxY += 2;
            fYPos -= 2;
        }
    }
}

void Vine::Draw(SDL_Renderer* rR, CIMG* iIMG)
{
    if(jumpDistance < 32)
	{
        CCore::getMap()->getMinionBlock(iBlockID - 1)->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(screen_height - 16 - iY*32 - jumpDistance));
    }
	else
	{
        CCore::getMap()->getMinionBlock(iBlockID - 1)->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(screen_height - 16 - iY*32 - jumpDistance));
        for(int i = 0; i < jumpDistance/32 - 1; i++)
		{
            iIMG->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(screen_height + 16 - iY*32 + i*32 - jumpDistance), false);
        }
    }
    CCore::getMap()->getBlock(CCore::getMap()->getMapBlock((int)iX, (int)iY)->getBlockID())->getSprite()->getTexture()->Draw(rR, (int)(iX*32 + CCore::getMap()->getXPos()), (int)(screen_height - iY*32 - 16 - CCore::getMap()->getMapBlock((int)iX, (int)iY)->getYPos()));
}


void Vine::minionPhysics() { }

void Vine::collisionWithPlayer(bool TOP)
{
    if(minionState == 0) {
        CCore::getMap()->setInEvent(true);
        CCore::getMap()->getEvent()->resetData();
        CCore::getMap()->getPlayer()->resetJump();
        CCore::getMap()->getPlayer()->stopMove();
        CCore::getMap()->getEvent()->eventTypeID = CCore::getMap()->getEvent()->eNormal;
        CCore::getMap()->getEvent()->iSpeed = 2;
        CCore::getMap()->getEvent()->newLevelType = CCore::getMap()->getLevelType();
        CCore::getMap()->getEvent()->newCurrentLevel = CCore::getMap()->getCurrentLevelID();
        CCore::getMap()->getEvent()->newMoveMap = true;
        CCore::getMap()->getEvent()->iDelay = 0;
        CCore::getMap()->getEvent()->inEvent = false;
        CCore::getMap()->getEvent()->newMoveMap = true;
        CCore::getMap()->getEvent()->newUnderWater = false;
        if(fXPos + iHitBoxX/2 > CCore::getMap()->getPlayer()->getXPos() - CCore::getMap()->getXPos()) {
            CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eRIGHT);
            CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxX() - 4);
        } else {
            CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eLEFT);
            CCore::getMap()->getEvent()->vOLDLength.push_back(CCore::getMap()->getPlayer()->getHitBoxX() - 4);
        }
        for(int i = 0; i < CCore::getMap()->getPlayer()->getYPos() + CCore::getMap()->getPlayer()->getHitBoxY(); i += 32) {
            CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINE1);
            CCore::getMap()->getEvent()->vOLDLength.push_back(16);
            CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINE2);
            CCore::getMap()->getEvent()->vOLDLength.push_back(16);
        }
        switch(CCore::getMap()->getCurrentLevelID()) {
            case 4: {
                CCore::getMap()->getEvent()->newMapXPos = -270*32;
                CCore::getMap()->getEvent()->newPlayerXPos = 128;
                CCore::getMap()->getEvent()->newPlayerYPos = screen_height - CCore::getMap()->getPlayer()->getHitBoxY();
                CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
                CCore::getMap()->getEvent()->vOLDLength.push_back(274);
                break;
            }
            case 8: {
                CCore::getMap()->getEvent()->newMapXPos = -270*32;
                CCore::getMap()->getEvent()->newPlayerXPos = 128;
                CCore::getMap()->getEvent()->newPlayerYPos = screen_height - CCore::getMap()->getPlayer()->getHitBoxY();
                CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
                CCore::getMap()->getEvent()->vOLDLength.push_back(274);
                break;
            }
            case 13: {
                CCore::getMap()->getEvent()->newMapXPos = -310*32;
                CCore::getMap()->getEvent()->newPlayerXPos = 128;
                CCore::getMap()->getEvent()->newPlayerYPos = screen_height - CCore::getMap()->getPlayer()->getHitBoxY();
                CCore::getMap()->getEvent()->newLevelType = 0;
                CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
                CCore::getMap()->getEvent()->vOLDLength.push_back(314);
                break;
            }
            case 17: {
                CCore::getMap()->getEvent()->newMapXPos = -325*32;
                CCore::getMap()->getEvent()->newPlayerXPos = 128;
                CCore::getMap()->getEvent()->newPlayerYPos = screen_height - CCore::getMap()->getPlayer()->getHitBoxY();
                CCore::getMap()->getEvent()->newLevelType = 0;
                CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
                CCore::getMap()->getEvent()->vOLDLength.push_back(329);
                break;
            }
            case 21: {
                CCore::getMap()->getEvent()->newMapXPos = -390*32;
                CCore::getMap()->getEvent()->newPlayerXPos = 128;
                CCore::getMap()->getEvent()->newPlayerYPos = screen_height - CCore::getMap()->getPlayer()->getHitBoxY();
                CCore::getMap()->getEvent()->newLevelType = 4;
                CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eVINESPAWN);
                CCore::getMap()->getEvent()->vOLDLength.push_back(394);
                break;
            }
        }
        CCore::getMap()->getEvent()->vOLDDir.push_back(CCore::getMap()->getEvent()->eNOTHING);
        CCore::getMap()->getEvent()->vOLDLength.push_back(60);
        for(int i = 0; i < 64; i = i+32)
        {
            CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eVINE1);
            CCore::getMap()->getEvent()->vNEWLength.push_back(16);
            CCore::getMap()->getEvent()->vNEWDir.push_back(CCore::getMap()->getEvent()->eVINE2);
            CCore::getMap()->getEvent()->vNEWLength.push_back(16);
        }
    } else {
    }
}


int main(int argc, char *argv[])
{
SDL_Init(SDL_INIT_EVERYTHING);

CCore oCore;
oCore.mainLoop();

SDL_Quit();
return 0;
}
