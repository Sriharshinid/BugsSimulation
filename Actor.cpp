#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include "Compiler.h"

using namespace std;

void BabyGrasshopper::doSomething()
{
	if (checkDidSomething())
		return;
	setDidSomething();
	decHitPoints(1);
	if (numHitPoints() <= 0)	//makes sure it is not dead
	{
		setDead();	
		return;
	}
	
	if (checknumStunned() > 0)	//check if stunned if it is dec numStunned
	{
		decStunned();
		return;
	}
	if (ableMove() != 0)
	{
		decAbleMove();	//dec m_ableMove so one less sleep tick
		return;
	}		//either sleeping or stunned

	
	if (numHitPoints() >= 1600)
	{
		getWorld()->addAdultGHop(getX(), getY());				//create adult Grasshopper at same spot
		setDead();		
						//this set dead does same thing (makes dead and puts 100 food)
		return;
	}

	int foodEaten = getWorld()->isFoodOnSpot(getX(), getY(), 200);		//eats food if can
	if( foodEaten != -1)
	{
		addHitPts(foodEaten);	
		if (randInt(0, 1) == 1)
		{
			setAbleMove(2);
			return;
		}

	}
	
	if (getDistance() == 0)
		setDistanceAndDirection();
		
	move();
	setAbleMove(2);

}


void GrassHopper::setDistanceAndDirection()
{
	setDistance(randInt(2, 10));
	setRandDirection();
}

bool Insect::move()	//moves insect one step in desired direction if can if cant returns false
{
	int finalX = getX();
	int finalY = getY();
	convertDirToCoord(getDirection(), getX(), getY(), finalX, finalY);
	if (getWorld()->checkMove(finalX, finalY, this, getX(), getY()))	//cehcks move and changes pointers if can
	{
		moveTo(finalX, finalY);
		m_distance--;
		setStunnedFalse();
		return true;
	}
	else
	{
		setDistance(0);	//sets distance to 0 and returns false
		return false;
	}
}


void Insect::setRandDirection()	//sets a random direction
{
	int direct = randInt(0, 3);
	switch (direct)
	{
	case 0:
		setDirection(up);
		break;
	case 1:
		setDirection(right);
		break;
	case 2:
		setDirection(down);
		break;
	case 3:
		setDirection(left);
		break;
	}
}

void Actor::convertDirToCoord(Direction d, int sX, int sY, int& fX, int& fY)
{	//takes direction and switches it to correct coordinate
	switch (d)
	{
	case right:
		fX = sX + 1;
		break;
	case left:
		fX = sX - 1;
		break;
	case up:
		fY = fY + 1;
		break;
	case down:
		fY = fY - 1;
		break;
	}
}

void WaterPool::doSomething()
{
	if (checkDidSomething())
		return;
	setDidSomething();
	getWorld()->stun(getX(), getY());
	//has to iterate through field 
	//stun everything at the spot with a false member variable
	//switch member variable for each thing to true
}

void Poison::doSomething()
{
	getWorld()->poisonn(getX(), getY());
}


void AdultGrasshopper::doSomething()
{
	if (checkDidSomething())
		return;
	setDidSomething();
	decHitPoints(1);
	if (numHitPoints() <= 0)
	{
		setDead();
		return;
	}
	if (ableMove() != 0)
	{
		decAbleMove();	//dec m_ableMove so one less sleep tick
		return;
	}

	if (randInt(0, 2) == 1)	//choosing if it wants to bite
	{
		int num = getWorld()->insectsAtPos(getX(), getY());
		if (num > 0)
		{
			int posIn = randInt(1, num);
			getWorld()->bite(getX(), getY(), posIn, this, 50);
			setAbleMove(2);
			return;
		}
		//make grasshopper bite one of the other insects on the spot 
		//give them -50 hit points
		
	}
	
	if (randInt(1, 10) == 1)	//choosing if it wants to jump
	{
		int fX = getX();
		int fY = getY();
		getWorld()->jump(fY, fX, this, fY, fX);
		moveTo(fX, fY);
		setAbleMove(2);
		return;
	}
	
	int foodEaten = getWorld()->isFoodOnSpot(getX(), getY(), 200);	//eats food on current spot
	if (foodEaten != -1)	//decides if it wants to sleep after eating
	{
		addHitPts(foodEaten);
		if (randInt(0, 1) == 1)
		{
			setAbleMove(2);
			return;
		}
	}

	if (getDistance() == 0)
		setDistanceAndDirection();

	move();
	setAbleMove(2);


}

void Ant::doSomething()		
{
	if (checkDidSomething())
		return;
	setDidSomething();
	decHitPoints(1);
	if (numHitPoints() <= 0)		//making sure it can move
	{
		setDead();
		return;
	}
	if (checknumStunned() > 0)		//making sure it isn't stunned
	{
		decStunned();
		return;
	}
	
	if (!executeUpto10Commands())		//executes up to 10 commands, if there is a failure in reading commands, sets ant to dead
	{
		setDead();
		return;
	}
	m_cmdsExecuted = 0;	//resets commands executed to 0
	
	//interpret between 1 and 10 commands
		//get next command from compiler object
			//if error set ant to dead and return
	//execute command until changes state of simulation or 10 commands have been executed

}

void Anthill::doSomething()
{
	decHitPoints(1);
	setDidSomething();
	if (numHitPoints() <= 0)	//if dead sets dead
	{
		setDead();
		return;
	}
	int food = getWorld()->isFoodOnSpot(getX(), getY(), 10000);	//gets amount of food on the spot 
	if (food > 0)				//adds according hitpoints
	{
		addHitPts(food);
		return;
	}

	if (numHitPoints() >= 2000)		//if have enough hit points add new ant
	{
		getWorld()->addNewAnt(getX(), getY(), m_colNum, m_compiler ); //add ant new in field
		decHitPoints(1500);
		getWorld()->incNumAntsinCol(m_colNum);		//inc amount of ants produced
		getWorld()->getWinningAntNumber();			//determine if there is a new leader
	}
	
}


bool Ant::executeUpto10Commands()
{
	//if 10 commands are completed or one of the commands changes the field
		//return ^^ base case
	if (m_cmdsExecuted >= 10)		//if 10 or more commands have been executed return 
		return true;
	int sX = getX();
	int sY = getY();
	Compiler::Command cmd;
	if ( m_compilerPtr == nullptr || !(m_compilerPtr->getCommand(m_instructionCounter, cmd)))	//if there is an error in getting command return false
		return false;


	switch (cmd.opcode)			//calls correct funcetion/ does correct thing based on opcode given
	{
	case Compiler::moveForward:
		m_instructionCounter++;		
		if (!move())
			m_blocked = true;
			
		else
		{
			m_blocked = false;
			setUnbitten();
			setStunnedFalse();
		}
		return true;
	case Compiler::eatFood:
		m_instructionCounter++;
		EatFood();
		return true;
	case Compiler::dropFood:
		m_instructionCounter++;
		getWorld()->addFoodOnSpot(getX(), getY(), m_foodHeld);
		m_foodHeld = 0;
		return true;
	case Compiler::bite:		
		m_instructionCounter++;
		if (getWorld()->insectsAtPos(getX(), getY()) > 0)
		{
			getWorld()->AntBite(getX(), getY(), m_colonyNumber, this);
		}
		return true;
	case Compiler::pickupFood:
		m_instructionCounter++;
		if (1800 - m_foodHeld < 400)
		{
			int foodPickedUp = getWorld()->isFoodOnSpot(getX(), getY(), 1800 - m_foodHeld);
			m_foodHeld += foodPickedUp;
		}
		else
		{
			int foodPickedUp = getWorld()->isFoodOnSpot(getX(), getY(), 400);
			m_foodHeld += foodPickedUp;
		}
		return true;
	case Compiler::emitPheromone:
		m_instructionCounter++;
		if (getWorld()->emitPheromone(getX(), getY(), getColNum(), this))
		return true;
	case Compiler::faceRandomDirection:
		m_instructionCounter++;
		setRandDirection();
		return true;
	case Compiler::generateRandomNumber:
		m_instructionCounter++;
		m_lastRandNum = randInt(0, stoi(cmd.operand1) - 1);
		m_cmdsExecuted++;
		return executeUpto10Commands();
		break;
	case Compiler::goto_command:
		m_instructionCounter = stoi(cmd.operand1);
		m_cmdsExecuted++;
		return executeUpto10Commands();
		break;
	case Compiler::rotateClockwise:
		m_instructionCounter++;
		rotateClockwise();
		return true;
	case Compiler::rotateCounterClockwise:
		m_instructionCounter++;
		rotateCounterclockwise();
		return true;
	case Compiler::if_command:
		m_cmdsExecuted++;
		if (executeCorrectIf(cmd))
			m_instructionCounter = stoi(cmd.operand2);
		else
			m_instructionCounter++;
		return executeUpto10Commands();
		break;
	default:
		return false;
	}

}
		



bool Ant::EatFood()		//ants eats the food it has stored
{
	if (m_foodHeld > 0)
	{
		if (m_foodHeld >= 100)
		{
			addHitPts(100);
			m_foodHeld -= 100;
		}
		else
		{
			addHitPts(m_foodHeld);
			m_foodHeld = 0;
		}
		return true;
	}
	else
		return false;
}

void Ant::rotateClockwise()		//sets direction to one move in the clockwise direction
{
	Direction dir = getDirection();
	
		switch (dir)
		{
		case GraphObject::up:
			setDirection(right);
			break;
		case GraphObject::right:
			setDirection(down);
			break;
		case GraphObject::down:
			setDirection(left);
			break;
		case GraphObject::left:
			setDirection(up);
			break;
		}
	
}

void Ant::rotateCounterclockwise()		//sets the direction to one move in the counter clockwise direction
{
	Direction dir = getDirection();
	switch (dir)
	{
	case GraphObject::up:
		setDirection(left);
		break;
	case GraphObject::right:
		setDirection(up);
		break;
	case GraphObject::down:
		setDirection(right);
		break;
	case GraphObject::left:
		setDirection(up);
		break;
	}
}

bool Ant::executeCorrectIf(Compiler::Command command)		//checks the if commands, if true returns true else returns false
{		
	Food* a = nullptr;
	Anthill* b = nullptr;
	Pheromone* f = nullptr;
	
	int FX = getX();
	int FY = getY();
	convertDirToCoord(getDirection(), getX(), getY(), FX, FY);


	switch (stoi(command.operand1))
	{
	case Compiler::i_smell_danger_in_front_of_me:
		return((getWorld()->enemyOfAnt(FX, FY, getColNum(), this) > 0));
		break;
	case Compiler::i_smell_pheromone_in_front_of_me:
		return (getWorld()->onSpot(f, FX, FY));
		break;
	case Compiler::i_was_bit:
		return (m_bitten == true);
		break;
	case Compiler::i_am_carrying_food:
		return (m_foodHeld >= 0);
		break;
	case Compiler::i_am_hungry:
		return (numHitPoints() <= 25);
		break;
	case Compiler::i_am_standing_on_my_anthill:
		return(getWorld()->onSpot(b, getX(), getY()));
		break;
	case Compiler::i_am_standing_on_food:
		return (getWorld()->onSpot(a, getX(), getY()));
		break;
	case Compiler::i_am_standing_with_an_enemy:
		return((getWorld()->enemyOfAnt(getX(), getY(), getColNum(), this) > 0));
		break;
	case Compiler::i_was_blocked_from_moving:
		return (m_blocked == true);
		break;
	case Compiler::last_random_number_was_zero:
		return (m_lastRandNum == 0);
		break;
	default:
		return false;
	}
}

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
