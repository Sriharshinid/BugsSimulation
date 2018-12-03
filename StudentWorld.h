#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_


#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
class Compiler;
class Actor;
class Ant;
class Insect;
class Food;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
		m_tickCount = 2000;
		winningCol = -1;
	}

	~StudentWorld();
	virtual int init();
	
	virtual int move();

	virtual void cleanUp();

	void removeDeadObjects();
	void resetAllDidSomething();

	void setDisplayText();
	void getWinningAntNumber();
	std::string Format(int ticks, int ant0, int ant1, int ant2, int ant3);
	void setAstForLeader(std::string& zero, std::string& one , std::string& two, std::string& three);

	bool blocksInsect(int Row, int Col);
	bool checkMove(int col, int row, Actor* obj, int oldCol, int oldRow);
	
	int isFoodOnSpot(int col, int row, int units);
	void addFoodOnSpot(int col, int row, int units);

	void stun(int col, int row);
	void poisonn(int col, int row);

	void addAdultGHop(int col, int row);
	void jump(int Row, int Col, Actor* obj, int& finalR, int& finalC);

	int insectsAtPos(int col, int row);
	void bite(int col, int row, int pos, Actor* obj, int minusPts);


	void addNewAnt(int col, int row, int colNum, Compiler* pntr);


	void updateTickCount();
	void incNumAntsinCol(int colNum);
	int getColonyID(int colNum);
	int getPherID(int colNum);
	void AntWasBitten(Insect* insecttt);

	bool emitPheromone(int col, int row, int Colnum, Ant* obj);
	bool AntBite(int col, int row, int Colnum, Ant* obj);
	void biteBack(Insect* insectPtr, int col, int row);
	int enemyOfAnt(int col, int row, int colNum, Ant* obj);

	template <typename T>
	bool onSpot(T* obj, int col, int row)
	{
		for (int i = 0; i < field[row][col].size(); i++)
		{
			T* newObj = dynamic_cast<T*>(field[row][col][i]);
			if (newObj != nullptr)
				return true;
		}
		return false;
	}

private:
	
	std::vector<Actor*> field[64][64];
	std::string filenames[4];
	int m_tickCount;
	int m_Col_0_Ants;
	int m_Col_1_Ants;
	int m_Col_2_Ants;
	int m_Col_3_Ants;
	int winningCol;
	int numBugFiles;

	

};

#endif // STUDENTWORLD_H_
