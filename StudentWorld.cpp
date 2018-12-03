#include <vector>
#include "StudentWorld.h"
#include "GameWorld.h"
#include "GraphObject.h"
#include "Field.h"
#include <string>
#include "Actor.h"
#include <cmath>
#include <sstream>
#include <iomanip>


using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


StudentWorld::~StudentWorld()
{
	cleanUp();
}


int StudentWorld::init()		//makes field and puts actors in correct starting positions
{
	Field f;
	string fieldFile = getFieldFilename();

	string error;
	if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
	{
		setError(fieldFile + " " + error);
		return false; // something bad happened!
	}
	// otherwise the load was successful and you can access the
	// contents of the field – here’s an example


					  //FIELD IS LOADED

	// get the names of all of the ant program source files
	// we provide the getFilenamesOfAntPrograms() function for
	// you in our GameWorld class.
	std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
	
	numBugFiles = fileNames.size();

	Field::FieldItem fielditemArray[4] =
	{ Field::FieldItem::anthill0 ,Field::FieldItem::anthill1, Field::FieldItem::anthill2, Field::FieldItem::anthill3 };

	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			//adds actors on correct spots	
			Field::FieldItem item = f.getContentsOf(x, y);
			if (item == Field::FieldItem::food)
				field[y][x].push_back(new Food(x, y, this, 6000));
			if (item == Field::FieldItem::rock)
				field[y][x].push_back(new Pebble(x, y, this));
			if (item == Field::FieldItem::grasshopper)
				field[y][x].push_back(new BabyGrasshopper(x, y, this, randInt(2, 10)));
			if (item == Field::FieldItem::water)
				field[y][x].push_back(new WaterPool(x, y, this));
			if (item == Field::FieldItem::poison)
				field[y][x].push_back(new Poison(x, y, this));
			if (item == fielditemArray[0] || item == fielditemArray[1] || item == fielditemArray[2] || item == fielditemArray[3])
			{
				for (int i = 0; i < numBugFiles; i++)
				{
					if (item == fielditemArray[i])
					{
						Compiler *compilerForEntrant;
						Anthill *ah;
						compilerForEntrant = new Compiler;
						std::string error;
						if (fileNames.empty())
							cerr << "EMPTYYYY" << endl;
						if (!compilerForEntrant->compile(fileNames[i], error))
						{
							setError(fileNames[i] + " " + error);
							return GWSTATUS_LEVEL_ERROR;
						}
						ah = new Anthill(IID_ANT_HILL, x, y, i, compilerForEntrant, this);
						field[y][x].push_back(ah);
						filenames[i] = compilerForEntrant->getColonyName();
					}

				}
			}
			
		}
			
	}
	
	return GWSTATUS_CONTINUE_GAME;
}
			
		


int StudentWorld::move()		
{
	updateTickCount();

	if (m_tickCount > 0)	//moves each actor
	{
		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				for (int k = field[i][j].size() - 1; k >= 0 && k<field[i][j].size(); k--)
					field[i][j].at(k)->doSomething();
				
			}
		}
		removeDeadObjects();
		resetAllDidSomething();
	
		setDisplayText();
	return  GWSTATUS_CONTINUE_GAME;
	}
	else
	{
		if(winningCol == -1)
			return GWSTATUS_NO_WINNER;
		else
		{
			setWinner(filenames[winningCol]);
			return GWSTATUS_PLAYER_WON;

		}

	}
		

}


void StudentWorld::cleanUp()	//deletes all actors 
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			for (int k = field[i][j].size() - 1; k >= 0; k--)
			{
				Actor* p = field[i][j].at(k);
				field[i][j].pop_back();
				delete p;
			}
			field[i][j].push_back(nullptr);
		}
	}
}

bool StudentWorld::blocksInsect(int Row, int Col)		//checks for pebble
{
	for (unsigned int i = 0; i < field[Row][Col].size(); i++)
	{
		Pebble* pp = dynamic_cast<Pebble*>(field[Row][Col].at(i));	//searching for a pebble if found, return false immediately
		if (pp != nullptr)
			return false;
	}
	
	return true;
}

bool StudentWorld::checkMove(int col, int row, Actor* obj, int oldCol, int oldRow)	//if actor is able to move moves them and deletes their old ptr
{
	if (!blocksInsect(row, col))
		return false;
	
	field[row][col].push_back(obj);					//place actor in its new position
	//NEED TO REMOVE POINTER TO OBJECT FROM PREVIOUS LOCATION!!!

	for (unsigned int i = 0; i < field[oldRow][oldCol].size(); i++)		//deleting pointer from old position on field
	{
		if (obj == field[oldRow][oldCol].at(i))
		{
			field[oldRow][oldCol].at(i) = field[oldRow][oldCol].back();
			field[oldRow][oldCol].pop_back();
		}
	}
	return true;		//return true to show that it has moved to a new location
}


void StudentWorld::updateTickCount()
{
	m_tickCount--;
}


void StudentWorld::resetAllDidSomething()
{
	
	for (int a = 0; a < 64; a++)
		for (int b = 0; b < 64; b++)
			for (unsigned int c = 0; c < field[a][b].size(); c++)
				field[a][b][c]->resetDidSomething();
				
}


void StudentWorld::stun(int col, int row)		//stuns all actors on a spot that can be stunned
{
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Insect* in = dynamic_cast<Insect*>(field[row][col].at(i));		
		if (in != nullptr)
		{
			if (in->checkCanbeStunned() == true && in->checkStunned() == false)
			{
				in->setStunnedtrue();
				in->setnumStunned();
			}
		}
		
	}
}

void StudentWorld::poisonn(int col, int row)	//poisons all actors on a spot that can be poisoned
{
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Insect* in = dynamic_cast<Insect*>(field[row][col].at(i));		
		if (in != nullptr && in->checkCanbeStunned() == true && in->checkStunned() == false)
		{
			in->decHitPoints(150);
			in->setStunnedtrue();
			if(in->numHitPoints() <= 0)
				in->setDead();
		}
		
	}

}

void StudentWorld::removeDeadObjects()		//deletes all dead actors
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			for (int k = field[i][j].size() - 1; k >= 0 && k < field[i][j].size(); k--)
			{
				if (field[i][j][k]->isDead())
				{
					Actor* p = field[i][j][k];
					Insect* in = dynamic_cast<Insect*>(p);
					if (in != nullptr)
					{
						addFoodOnSpot(j, i, 100);
					}
					field[i][j][k] = field[i][j].back();
					delete p;
					field[i][j].pop_back();
					
				}
			}
		}
	}

}

int StudentWorld::isFoodOnSpot(int col, int row, int units)		//checks if food is on spot if there is allows actor to take it
{
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Food* yum = dynamic_cast<Food*>(field[row][col].at(i));

		if (yum != nullptr && !(yum->isDead()))
		{
			if (yum->numHitPoints() > units)
			{
				yum->decHitPoints(units);
				return units;
			}
			if (yum->numHitPoints() <= units)
			{
				int foood = yum->numHitPoints();
				yum->decHitPoints(foood);
				yum->setDead();
				return foood;
			}
		}
	}

	return -1;
}

void StudentWorld::addFoodOnSpot(int col, int row, int units)	//adds set number of food on spot
{
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Food* yum = dynamic_cast<Food*>(field[row][col].at(i));
		if (yum != nullptr)
		{
			yum->addHitPts(units);
			return;
		}
	}
	field[row][col].push_back(new Food(col, row, this, units));
	return;
	
}

void StudentWorld::addAdultGHop(int col, int row)
{
	field[row][col].push_back(new AdultGrasshopper(col, row, this, randInt(2, 10)));
}


void StudentWorld::jump(int Row, int Col, Actor* obj, int& finalR, int& finalC)		//calculates a random spot within a radius for a grasshopper to jump
{
	int angle = randInt(0, 44 / 7);
	double radius = randInt(100, 999) / 100;
	int col = Col + radius*sin(angle);
	int row = Row + radius*cos(angle);
	if ((Row >= 64 || Col < 0 || Row < 0 || Col >= 64) || !checkMove(Col, Row, obj, Col, Row))
		return jump(obj->getY(), obj->getX(), obj, row, col);

}

int StudentWorld::insectsAtPos(int col, int row)	//checks if there are other alive in sects at a position and returns number
{
	int numIns = -1;	//so doesnt count itself
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Insect* in = dynamic_cast<Insect*>(field[row][col][i]);
		if (in != nullptr && !(in->isDead()))
			numIns++;
	}

	return numIns;
}

void StudentWorld::bite(int col, int row, int pos, Actor* obj, int minusPts)	//bites insect and see if wants to bite back
{
	int numEnct = 0;
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Insect* in = dynamic_cast<Insect*>(field[row][col][i]);
		if (in != nullptr && !(in->isDead()) && in != obj)
			numEnct++;
		if (pos == numEnct)
		{
			in->decHitPoints(minusPts);
			AntWasBitten(in);
			if (in->numHitPoints() <= 0)
				in->setDead();
			else
			{
				return biteBack(in, col, row);
			}
		}
	}

}

void StudentWorld::incNumAntsinCol(int colNum)	//adds 1 to the number of ants made in a colony
{
	switch (colNum)
	{
	case 0: m_Col_0_Ants++; break;
	case 1: m_Col_1_Ants++; break;
	case 2: m_Col_2_Ants++; break;
	case 3: m_Col_3_Ants++; break;
	default: cerr << "wrong" << endl;
	}
}



int StudentWorld::getColonyID(int colNum)	//returns colony ID based on colony number
{
	switch (colNum)
	{
	case 0:return IID_ANT_TYPE0;
	case 1:return IID_ANT_TYPE1;
	case 2:return IID_ANT_TYPE2;
	case 3:return IID_ANT_TYPE3;
	default: return -1;
	}
}

void StudentWorld::addNewAnt(int col, int row, int colNum, Compiler* pntr)	
{
	int Id = getColonyID(colNum);
	field[row][col].push_back(new Ant(Id, colNum, pntr, col, row, this));
}

bool StudentWorld::emitPheromone(int col, int row, int colNum, Ant* obj)	//adds to existing pheromone object or makes new one
{
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Pheromone* ph = dynamic_cast<Pheromone*>(field[row][col].at(i));
		if (ph != nullptr && ph->getColNum() == obj->getColNum() )
		{
			if (ph->numHitPoints() == 768)
				return false;
			else if (ph->numHitPoints() > 212)
			{
				ph->addHitPts(768 - ph->numHitPoints());
				return true;
			}
			else
			{
				ph->addHitPts(256);
				return true;
			}
		}
	}
	field[row][col].push_back(new Pheromone(getPherID(colNum), obj->getX(), obj->getY(), this, colNum));
	return true;

}

int StudentWorld::getPherID(int colNum)	//return pheromone ID given the colony number
{
	switch (colNum)
	{
	case 0:return IID_PHEROMONE_TYPE0;
	case 1:return IID_PHEROMONE_TYPE1;
	case 2:return IID_PHEROMONE_TYPE2;
	case 3:return IID_PHEROMONE_TYPE3;
	default: return -1;
	}
}

int StudentWorld::enemyOfAnt(int col, int row, int Colnum, Ant* obj)	//check if grasshopper or ant from another colony is present on spot
{
	int numIns = -1;	//so doesnt count itself
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Insect* in = dynamic_cast<Insect*>(field[row][col][i]);
		if (in != nullptr && !(in->isDead()))
		{
			numIns++;
			Ant* an = dynamic_cast<Ant*>(field[row][col][i]);
			if (an != nullptr && an->getColNum() == obj->getColNum())
				numIns--;
		}
	}
	return numIns;
}

bool StudentWorld::AntBite(int col, int row, int Colnum, Ant* obj)	//bites the specified enemy insect
{
	int num = enemyOfAnt(col, row, Colnum, obj);
	
	if (num <= 0)
		return false;
	int pos = randInt(1, num);
	int numEnct = 0;
	for (int i = 0; i < field[row][col].size(); i++)
	{
		Insect* in = dynamic_cast<Insect*>(field[row][col][i]);
		if (in != nullptr && !(in->isDead()))
		{
			numEnct++;
			Ant* an = dynamic_cast<Ant*>(field[row][col][i]);
			if (an != nullptr && an->getColNum() == obj->getColNum())
				numEnct--;
		}
		if (pos == numEnct)
		{
			in->decHitPoints(15);
			AntWasBitten(in);
			if (in->numHitPoints() <= 0)
				in->setDead();
			else
				biteBack(in, col, row);
		}
		
	}
	return true;


}

void StudentWorld::biteBack(Insect* insectPtr, int col, int row)	//gives adult grasshopper chance to bite back
{
	AdultGrasshopper* ag = dynamic_cast<AdultGrasshopper*>(insectPtr);		//if the insect bitten was an adult grasshopper
	if (ag != nullptr)
	{
		if (randInt(0, 1) == 1)
		{
			int Num = insectsAtPos(col, row);
			if (Num > 0)
			{
				int posI = randInt(1, Num);
				return bite(col, row, posI, ag, 50);
			}
		}
	}
}

void StudentWorld::AntWasBitten(Insect* insecttt)	//sets an ant to bitten
{
	Ant* an = dynamic_cast<Ant*>(insecttt);
	if (an != nullptr)
	{
		an->setBitten();
	}
}

void StudentWorld::setDisplayText()	//sets the string o ftext to display
{
	int ticks = m_tickCount;
	int antsAnt0, antsAnt1, antsAnt2, antsAnt3;
	antsAnt0 = m_Col_0_Ants;
	antsAnt1 = m_Col_1_Ants;
	antsAnt2 = m_Col_2_Ants;
	antsAnt3 = m_Col_3_Ants;
	// Create a string from your statistics, of the form:
	// Ticks: 1134 - AmyAnt: 32 BillyAnt: 33 SuzieAnt*: 77 IgorAnt: 05

	
	string s = Format(ticks, antsAnt0, antsAnt1, antsAnt2, antsAnt3);
	
	
	// Finally, update the display text at the top of the screen with your
	// newly created stats
	setGameStatText(s); // calls our provided GameWorld::setGameStatText
}


string StudentWorld::Format(int ticks, int ant0, int ant1, int ant2, int ant3)
{//formats string of text
	string a0 = ": ";
	string a1 = ": ";
	string a2 = ": ";
	string a3 = ": ";
	setAstForLeader(a0, a1, a2, a3);
	ostringstream oss;
	oss << "Ticks:" << setw(5) << ticks << " -";
	oss.fill('0');
	if (numBugFiles > 0)
		oss << " " << filenames[0] << a0 << setw(2) << ant0;
	if (numBugFiles > 1)
		oss << "  " << filenames[1] << a1 << setw(2) << ant1;
	if (numBugFiles > 2)
		oss << "  " << filenames[2] << a2 << setw(2) << ant2;
	if(numBugFiles > 3)
		oss<< "  " << filenames[3] << a3 << setw(2) << ant3 << endl;
	
	return oss.str();
}

void StudentWorld::setAstForLeader(string& zero, string& one, string& two, string& three)
{//ads asterisk if there is a current leader
	string winnn = "*: ";
	switch (winningCol)
	{
	case 0:
		zero = winnn;
		break;
	case 1:
		one = winnn;
		break;
	case 2:
		two = winnn;
		break;
	case 3:
		three = winnn;
		break;
	default:
		break;
	}

}

void StudentWorld::getWinningAntNumber()	//only call when an ant makes a colony/takes the lead
{//calculates which ant is in the lead
	int arr[4] = { m_Col_0_Ants, m_Col_1_Ants, m_Col_2_Ants, m_Col_3_Ants };
	int winner;
	if (winningCol != -1)
		winner = winningCol;
	else
		winner = 0;

	for (int i = 0; i < 4; i++)
	{
		if (arr[i] > arr[winner])
			winner = i;
	}
	if (arr[winner] == 5)
		winningCol = -1;
	else
		winningCol = winner;

}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp 
