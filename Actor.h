#ifndef ACTOR_H_
#define ACTOR_H_
#include "StudentWorld.h"
#include "GraphObject.h"
#include "Compiler.h"
#include <iostream>
using namespace std;

class Actor : public GraphObject
{
public:
	Actor(int iID, int sX, int sY, StudentWorld* sW, Direction d = right, int dep = 0)
		:GraphObject(iID, sX, sY, d, dep)
	{
		
		m_dead = false;
		stuWrld = sW;
		m_didSomething = false;
	}

	virtual ~Actor() {}
	virtual void doSomething() = 0;
	StudentWorld* getWorld() const { return stuWrld; }

	bool isDead() { return m_dead; }
	void setDead() { m_dead = true; }

	void convertDirToCoord(Direction d, int sX, int sY, int& fX, int& fY);

	void setDidSomething() { m_didSomething = true;	}
	void resetDidSomething() { m_didSomething = false; }
	bool checkDidSomething() { return m_didSomething; }

private:
	bool m_dead;
	bool m_didSomething;
	StudentWorld* stuWrld;

};

class EnergyHolder : public Actor
{
public:
	EnergyHolder(int ID, int startX, int startY, StudentWorld* SW, int hitPts,  Direction dir, int depth)
		:Actor(ID, startX, startY, SW, dir, depth)
	{
		m_hitPoints = hitPts;
	}
	~EnergyHolder() {}
	virtual void doSomething() = 0;


	void decHitPoints(int numMinus) { m_hitPoints = m_hitPoints - numMinus; }
	int numHitPoints() const { return m_hitPoints; }
	void addHitPts(int num) { m_hitPoints += num; }


private:
	int m_hitPoints;
};


class Insect :public EnergyHolder
{
public: 
	Insect(int ID, int StartX, int StartY, StudentWorld* SW, int hitpts, int dist, bool stunabble)
		:EnergyHolder(ID, StartX, StartY, SW, hitpts, right, 1) 
	{
		
		m_ableMove = 0;
		m_stunned = 0;
		stunned = false;
		m_canBeStunned = stunabble;
		
	}
	virtual ~Insect() {}
	bool move();
	virtual void doSomething() = 0;
	
	void setRandDirection();
	int ableMove() const { return m_ableMove; }
	void decAbleMove() { m_ableMove--; }
	void setAbleMove(int add ) { m_ableMove += add; }

	int getDistance() const { return m_distance; }
	void setDistance(int d) { m_distance = d; }

	void setnumStunned() { m_stunned += 2; }
	void setStunnedtrue() { stunned = true; }
	void setStunnedFalse() { stunned = false; }
	void decStunned() { m_stunned--; }
	int checknumStunned() { return m_stunned; }
	bool checkStunned() { return stunned; }

	bool checkCanbeStunned() { return m_canBeStunned; }
	

private:
	bool m_canBeStunned;
	int m_ableMove;
	int m_stunned;
	bool stunned;
	int m_distance;
	
};

class GrassHopper : public Insect
{
public:
	GrassHopper(int ID, int startX, int startY, StudentWorld* SW, int hitPts, int distance, bool stunn)
		:Insect(ID, startX, startY, SW, hitPts, distance, stunn)
	{
		setRandDirection();
	}
	virtual ~GrassHopper() {}
	virtual void doSomething() = 0;
	void setDistanceAndDirection();
private:
};

class BabyGrasshopper : public GrassHopper		
{
public:
	BabyGrasshopper(int X, int Y, StudentWorld* sw, int dist)
		:GrassHopper(IID_BABY_GRASSHOPPER, X, Y, sw, 500, dist, true)
	{}
	virtual void doSomething();
	~BabyGrasshopper() {}


};


class AdultGrasshopper : public GrassHopper
{
public:
	AdultGrasshopper(int X, int Y, StudentWorld* sw, int dist)
		:GrassHopper(IID_ADULT_GRASSHOPPER, X, Y, sw, 1600, dist, false)
	{}
	void doSomething();
	~AdultGrasshopper() {}
private:
};



class Pebble :public Actor
{
public:
	Pebble(int startX, int startY, StudentWorld* SW)
		:Actor(IID_ROCK, startX, startY, SW, right, 1) {}
	void doSomething() {}
	~Pebble() {}
};




class WaterPool :public Actor
{
public:
	WaterPool(int startX, int startY, StudentWorld* SW)
		:Actor(IID_WATER_POOL, startX, startY, SW, right, 2) {}
	~WaterPool() {}
	void doSomething();
};

class Poison :public Actor
{
public:
	Poison(int startX, int startY, StudentWorld* SW)
		:Actor(IID_POISON, startX, startY, SW, right, 2) {}
	~Poison() {}
	void doSomething();
};

class Food :public EnergyHolder
{
public:
	Food(int startX, int startY, StudentWorld* SW, int units)
		:EnergyHolder(IID_FOOD, startX, startY, SW, units, right, 2){}
	~Food() {}
	void doSomething() {}

private:
};



class Ant :public Insect
{
public:
	Ant(int ID, int cNum, Compiler* obj, int sX, int sY, StudentWorld* Sw)
		:Insect(ID, sX, sY, Sw, 1500, 1, true)
	{
		setRandDirection();
		m_colonyNumber = cNum;
		m_foodHeld = 0;
		m_lastRandNum = 0;
		m_instructionCounter = 0;
		m_cmdsExecuted = 0;
		m_compilerPtr = obj;
		m_blocked = false;
		m_bitten = false;
		string error;
		if (!(m_compilerPtr->compile(obj->getColonyName(), error)))
			cout << "FALSEEEE" << endl;

	}
	~Ant() {};
	void doSomething();
	bool executeUpto10Commands();
	bool EatFood();
	int getColNum() { return m_colonyNumber; }
	void rotateClockwise();
	void rotateCounterclockwise();
	bool executeCorrectIf(Compiler::Command command);
	void setBitten() { m_bitten = true; }
	void setUnbitten() { m_bitten = false; }

private:
	int m_colonyNumber;
	int m_foodHeld;
	int m_lastRandNum;
	int m_instructionCounter;
	int m_cmdsExecuted;
	bool m_bitten;
	Compiler* m_compilerPtr;
	bool m_blocked;

};

class Anthill :public EnergyHolder
{
public:
	Anthill(int ID, int stX, int stY, int cNum, Compiler* comp, StudentWorld* SW)
		:EnergyHolder(ID, stX, stY, SW,  8999, right, 2)
	{
		m_compiler = comp;
		m_colNum = cNum;
	}
	~Anthill() {
		Compiler* falseptr = m_compiler;
		delete falseptr; 
		m_compiler = nullptr;
	}
	void doSomething();
	//Compiler* getPointertoCompiler() {return &m_compiler;}
	
private:
	Compiler* m_compiler;
	int m_colNum;

};

class Pheromone :public EnergyHolder
{
public:
	Pheromone(int ID, int sX, int sY, StudentWorld* sw, int colNum)
		:EnergyHolder(ID, sX, sY, sw, 256, right, 2 )
	{
		m_colonyNumber = colNum;
	}
	~Pheromone() {} 
	void doSomething()
	{ decHitPoints(1); 
		if (numHitPoints() <= 0)
		setDead();
	}

	int getColNum() { return m_colonyNumber; }
private:
	int m_colonyNumber;
};


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
