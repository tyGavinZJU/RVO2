/*
 * Blocks.cpp
 * RVO2 Library
 *
 * Copyright 2008 University of North Carolina at Chapel Hill
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jur van den Berg, Stephen J. Guy, Jamie Snape, Ming C. Lin, Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <http://gamma.cs.unc.edu/RVO2/>
 */

/*
 * Example file showing a demo with 100 agents split in four groups initially
 * positioned in four corners of the environment. Each agent attempts to move to
 * other side of the environment through a narrow passage generated by four
 * obstacles. There is no roadmap to guide the agents around the obstacles.
 */

#ifndef RVO_OUTPUT_TIME_AND_POSITIONS
#define RVO_OUTPUT_TIME_AND_POSITIONS 1
#endif

#ifndef RVO_SEED_RANDOM_NUMBER_GENERATOR
#define RVO_SEED_RANDOM_NUMBER_GENERATOR 1
#endif

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <vector>

#if RVO_OUTPUT_TIME_AND_POSITIONS
#include <iostream>
#endif

#if RVO_SEED_RANDOM_NUMBER_GENERATOR
#include <ctime>
#endif

#if _OPENMP
#include <omp.h>
#endif

#include <RVO.h>

#ifndef M_PI
const float M_PI = 3.14159265358979323846f;
#endif



/* Store the goals of the agents. */
std::vector<RVO::Vector2> goals;
using namespace std;

ofstream outfile;

#include <stdlib.h>
#include  <time.h>  


typedef struct point{
	float x;
	float y;
}Point;

bool lineIntersectSide(Point A, Point B, Point C, Point D){
	// A(x1, y1), B(x2, y2)的直线方程为：    
    // f(x, y) =  (y - y1) * (x1 - x2) - (x - x1) * (y1 - y2) = 0 
	float fC = (C.y - A.y) * (A.x - B.x) - (C.x - A.x) * (A.y - B.y);    
    float fD = (D.y - A.y) * (A.x - B.x) - (D.x - A.x) * (A.y - B.y);  
    if(fC * fD > 0)    
        return false; 
    return true;
}

bool sideIntersectSide(Point A, Point B, Point C, Point D){
	if(!lineIntersectSide(A, B, C, D))    
        return false;    
    if(!lineIntersectSide(C, D, A, B))    
        return false;    
    return true;  
}

double getRandFloat(int min,int max)
{
	
	double m1 = (double)(rand()%101)/101.0;
	min++;
	double m2 = (double)((rand()%(max-min+1))+min);
	m2 = m2 -1;
	return m1+m2;
}

const int RANGE = 100; 

Point getRandCoord(int min = -RANGE, int max = RANGE)
{
	Point ans;
	ans.x = getRandFloat(min, max);

	ans.y = getRandFloat(min, max);

	//cout<<"rand:"<<ans.x<<" "<<ans.y<<endl;
	return ans;
}

bool targetTooClose(RVO::Vector2 A, RVO::Vector2 B, float Radius){
	if (RVO::absSq(A - B) > 2* Radius) {
			return false;
	}
	return true;
}

bool pathTooShort(Point A, Point B){
	if (sqrt(pow(A.x-B.x, 2) + pow(A.y-B.y, 2)) < 150)
		return true;
	return false;
}

void setupScenario(RVO::RVOSimulator *sim)
{


	/* Specify the global time step of the simulation. */
	sim->setTimeStep(0.05f);

	/* Specify the default parameters for agents that are subsequently added. */
	//sim->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, 2.0f, 2.0f);
	sim->setAgentDefaults(200.0f, 10, 50.0f, 50.0f, 15.0f, 20.0f);
/*
	void setAgentDefaults(float neighborDist, size_t maxNeighbors,
							  float timeHorizon, float timeHorizonObst,
							  float radius, float maxSpeed,
							  const Vector2 &velocity = Vector2());
						  */
	
	Point A,B,C,D;
	do{
		A = getRandCoord();
		B = getRandCoord();
		C = getRandCoord();
		D = getRandCoord();
	}
	while(!sideIntersectSide(A, B, C, D) || targetTooClose(RVO::Vector2(B.x, B.y), RVO::Vector2(D.x, D.y), 15.0f) ||
		targetTooClose(RVO::Vector2(A.x, A.y), RVO::Vector2(C.x, C.y), 15.0f)
		|| pathTooShort(A, B) || pathTooShort(C, D)); 

	cout<<A.x<<" "<<A.y<<" "<<B.x<<" "<<B.y<<" "<<C.x<<" "<<C.y<<" "<<D.x<<" "<<D.y<<endl;
	sim->addAgent(RVO::Vector2(A.x , A.y));
	goals.push_back(RVO::Vector2(B.x , B.y));

	sim->addAgent(RVO::Vector2(C.x , C.y));
	goals.push_back(RVO::Vector2(D.x , D.y));
	


	/*
	//Block Demo
	sim->addAgent(RVO::Vector2(50.0f , 50.0f));
	goals.push_back(RVO::Vector2(-50.0f , -50.0f));

	sim->addAgent(RVO::Vector2(-50.0f , -50.0f));
	goals.push_back(RVO::Vector2(50.0f , 50.0f));
	*/

	/*
	// circle demo
	sim->setTimeStep(0.25f);
	sim->setAgentDefaults(15.0f, 10, 10.0f, 10.0f, 1.5f, 2.0f);
	for (size_t i = 0; i < 250; ++i) {
		sim->addAgent(200.0f *
		              RVO::Vector2(std::cos(i * 2.0f * M_PI / 250.0f),
		                           std::sin(i * 2.0f * M_PI / 250.0f)));
		goals.push_back(-sim->getAgentPosition(i));
	}
	*/

	/*
	obstacle1.push_back(RVO::Vector2(-10.0f, 40.0f));
	obstacle1.push_back(RVO::Vector2(-40.0f, 40.0f));
	obstacle1.push_back(RVO::Vector2(-40.0f, 10.0f));
	obstacle1.push_back(RVO::Vector2(-10.0f, 10.0f));


	sim->addObstacle(obstacle1);
	*/
	//sim->processObstacles();

}

#if RVO_OUTPUT_TIME_AND_POSITIONS
void updateVisualization(RVO::RVOSimulator *sim)
{
	
	
    
	/* Output the current global time. */
	outfile <<"t:"<<sim->getGlobalTime()<<std::endl;
	//std::cout << sim->getGlobalTime()<<std::endl;

	/* Output the current position of all the agents. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		outfile << i << ":" << sim->getAgentPosition(i)<< std::endl;
		//std::cout << i << ":" << sim->getAgentPosition(i)<< std::endl;
	}

	//std::cout << std::endl;
	
}
#endif

void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the goal.
	 */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i) {
		RVO::Vector2 goalVector = goals[i] - sim->getAgentPosition(i);

		if (RVO::absSq(goalVector) > 1.0f) {
			goalVector = RVO::normalize(goalVector);
		}

		sim->setAgentPrefVelocity(i, goalVector);

		/*
		 * Perturb a little to avoid deadlocks due to perfect symmetry.
		 */
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;

		sim->setAgentPrefVelocity(i, sim->getAgentPrefVelocity(i) +
		                          dist * RVO::Vector2(std::cos(angle), std::sin(angle)));
	}
}

bool reachedGoal(RVO::RVOSimulator *sim)
{
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		if (RVO::absSq(sim->getAgentPosition(i) - goals[i]) > 5.0f * 5.0f) {
			return false;
		}
	}

	return true;
}

/*
bool reachedGoal(RVO::RVOSimulator *sim)
{
	
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		if (RVO::absSq(sim->getAgentPosition(i) - goals[i]) > sim->getAgentRadius(i) * sim->getAgentRadius(i)) {
			return false;
		}
	}

	return true;
}
*/
int main()
{
	outfile.open("/Users/Gavin/Desktop/github/CADRL/out.txt");
	if(!outfile)
    {
        std::cout << "没有打开文件！"<< endl;
    }
    #if RVO_SEED_RANDOM_NUMBER_GENERATOR
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	#endif
    int count = 0;
    bool flag = false;
    while (count<50){
    	/* Create a new simulator instance. */
		RVO::RVOSimulator *sim = new RVO::RVOSimulator();
		flag = false;
		/* Set up the scenario. */
		setupScenario(sim);

		/* Perform (and manipulate) the simulation. */
		do {
//#if RVO_OUTPUT_TIME_AND_POSITIONS
			if (flag){
				updateVisualization(sim);
				
			}
//#endif	
			flag = true;
			setPreferredVelocities(sim);
			sim->doStep();
		}
		while (!reachedGoal(sim));

		delete sim;
		count++;
		outfile<<"#"<<endl;
		goals.clear();
    }
    /*
    while (count< 100 ){
    	count++;
    	cout<<getRandFloat(-150, 150)<<endl;
    }
	*/	


	outfile.close();
	return 0;
    

}
