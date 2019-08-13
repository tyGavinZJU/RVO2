/*
 * Roadmap.cpp
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
 * obstacles. There is a roadmap to guide the agents around the obstacles.
 */

#ifndef RVO_OUTPUT_TIME_AND_POSITIONS
#define RVO_OUTPUT_TIME_AND_POSITIONS 1
#endif

#ifndef RVO_SEED_RANDOM_NUMBER_GENERATOR
#define RVO_SEED_RANDOM_NUMBER_GENERATOR 1
#endif

#include <cmath>
#include <cstdlib>
#include <map>
#include <vector>
#include <fstream>

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

using namespace std;
class RoadmapVertex {
public:
	RVO::Vector2 position;
	std::vector<int> neighbors;
	std::vector<float> distToGoal;
};

/* Store the roadmap. */
std::vector<RoadmapVertex> roadmap;

/* Store the goals of the agents. */
std::vector<int> goals;




void setupScenario(RVO::RVOSimulator *sim)
{
#if RVO_SEED_RANDOM_NUMBER_GENERATOR
	std::srand(static_cast<unsigned int>(std::time(NULL)));
#endif

	/* Specify the global time step of the simulation. */
	sim->setTimeStep(2.25f);

	/*
	 * Add (polygonal) obstacles, specifying their vertices in counterclockwise
	 * order.
	 */
	std::vector<RVO::Vector2> obstacle1, obstacle2, obstacle3, obstacle4;

	obstacle1.push_back(RVO::Vector2(-10.0f, 40.0f));
	obstacle1.push_back(RVO::Vector2(-40.0f, 40.0f));
	obstacle1.push_back(RVO::Vector2(-40.0f, 10.0f));
	obstacle1.push_back(RVO::Vector2(-10.0f, 10.0f));

	obstacle2.push_back(RVO::Vector2(10.0f, 40.0f));
	obstacle2.push_back(RVO::Vector2(10.0f, 10.0f));
	obstacle2.push_back(RVO::Vector2(40.0f, 10.0f));
	obstacle2.push_back(RVO::Vector2(40.0f, 40.0f));

	obstacle3.push_back(RVO::Vector2(10.0f, -40.0f));
	obstacle3.push_back(RVO::Vector2(40.0f, -40.0f));
	obstacle3.push_back(RVO::Vector2(40.0f, -10.0f));
	obstacle3.push_back(RVO::Vector2(10.0f, -10.0f));

	obstacle4.push_back(RVO::Vector2(-10.0f, -40.0f));
	obstacle4.push_back(RVO::Vector2(-10.0f, -10.0f));
	obstacle4.push_back(RVO::Vector2(-40.0f, -10.0f));
	obstacle4.push_back(RVO::Vector2(-40.0f, -40.0f));

	sim->addObstacle(obstacle1);
	sim->addObstacle(obstacle2);
	sim->addObstacle(obstacle3);
	sim->addObstacle(obstacle4);

	/* Process the obstacles so that they are accounted for in the simulation. */
	sim->processObstacles();

	/* Add roadmap vertices. */
	RoadmapVertex v;

	/* Add the goal positions of agents. */
	v.position = RVO::Vector2(-75.0f, -75.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(75.0f, -75.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-75.0f, 75.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(75.0f, 75.0f);
	roadmap.push_back(v);

	/* Add roadmap vertices around the obstacles. */
	v.position = RVO::Vector2(-42.0f, -42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-42.0f, -8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-42.0f, 8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-42.0f, 42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-8.0f, -42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-8.0f, -8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-8.0f, 8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(-8.0f, 42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(8.0f, -42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(8.0f, -8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(8.0f, 8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(8.0f, 42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(42.0f, -42.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(42.0f, -8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(42.0f, 8.0f);
	roadmap.push_back(v);
	v.position = RVO::Vector2(42.0f, 42.0f);
	roadmap.push_back(v);

	/* Specify the default parameters for agents that are subsequently added. */
	sim->setAgentDefaults(15.0f, 10, 5.0f, 5.0f, 2.0f, 2.0f);

	/*
	 * Add agents, specifying their start position, and store goals on the
	 * opposite side of the environment (roadmap vertices).
	 */
	for (size_t i = 0; i < 5; ++i) {
		for (size_t j = 0; j < 5; ++j) {
			sim->addAgent(RVO::Vector2(55.0f + i * 10.0f,  55.0f + j * 10.0f));
			goals.push_back(0);

			sim->addAgent(RVO::Vector2(-55.0f - i * 10.0f,  55.0f + j * 10.0f));
			goals.push_back(1);

			sim->addAgent(RVO::Vector2(55.0f + i * 10.0f, -55.0f - j * 10.0f));
			goals.push_back(2);

			sim->addAgent(RVO::Vector2(-55.0f - i * 10.0f, -55.0f - j * 10.0f));
			goals.push_back(3);
		}
	}
}

#if RVO_OUTPUT_TIME_AND_POSITIONS
void updateVisualization(RVO::RVOSimulator *sim)
{
	ofstream outfile;
	outfile.open("out.txt");
    if(!outfile)
    {
        std::cout << "没有打开文件！"<< endl;
    }
    
	/* Output the current global time. */
	outfile << sim->getGlobalTime() << endl;
	std::cout << sim->getGlobalTime();

	/* Output the current position of all the agents. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		outfile << i << ":" <<sim->getAgentPosition(i) << endl;
		std::cout << " " << sim->getAgentPosition(i);
	}

	std::cout << std::endl;
	outfile.close();
}
#endif

void buildRoadmap(RVO::RVOSimulator *sim)
{
	/* Connect the roadmap vertices by edges if mutually visible. */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(roadmap.size()); ++i) {
		for (int j = 0; j < static_cast<int>(roadmap.size()); ++j) {
			if (sim->queryVisibility(roadmap[i].position, roadmap[j].position, sim->getAgentRadius(0))) {
				roadmap[i].neighbors.push_back(j);
			}
		}

		/*
		 * Initialize the distance to each of the four goal vertices at infinity
		 * (9e9f).
		 */
		roadmap[i].distToGoal.resize(4, 9e9f);
	}

	/*
	 * Compute the distance to each of the four goals (the first four vertices)
	 * for all vertices using Dijkstra's algorithm.
	 */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < 4; ++i) {
		std::multimap<float, int> Q;
		std::vector<std::multimap<float, int>::iterator> posInQ(roadmap.size(), Q.end());

		roadmap[i].distToGoal[i] = 0.0f;
		posInQ[i] = Q.insert(std::make_pair(0.0f, i));

		while (!Q.empty()) {
			const int u = Q.begin()->second;
			Q.erase(Q.begin());
			posInQ[u] = Q.end();

			for (int j = 0; j < static_cast<int>(roadmap[u].neighbors.size()); ++j) {
				const int v = roadmap[u].neighbors[j];
				const float dist_uv = RVO::abs(roadmap[v].position - roadmap[u].position);

				if (roadmap[v].distToGoal[i] > roadmap[u].distToGoal[i] + dist_uv) {
					roadmap[v].distToGoal[i] = roadmap[u].distToGoal[i] + dist_uv;

					if (posInQ[v] == Q.end()) {
						posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
					}
					else {
						Q.erase(posInQ[v]);
						posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
					}
				}
			}
		}
	}
}

void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the visible roadmap vertex that is on the shortest path to the
	 * goal.
	 */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i) {
		float minDist = 9e9f;
		int minVertex = -1;

		for (int j = 0; j < static_cast<int>(roadmap.size()); ++j) {
			if (RVO::abs(roadmap[j].position - sim->getAgentPosition(i)) + roadmap[j].distToGoal[goals[i]] < minDist &&
				sim->queryVisibility(sim->getAgentPosition(i), roadmap[j].position, sim->getAgentRadius(i))) {

				minDist = RVO::abs(roadmap[j].position - sim->getAgentPosition(i)) + roadmap[j].distToGoal[goals[i]];
				minVertex = j;
			}
		}

		if (minVertex == -1) {
			/* No roadmap vertex is visible; should not happen. */
			sim->setAgentPrefVelocity(i, RVO::Vector2(0, 0));
		}
		else {
			if (RVO::absSq(roadmap[minVertex].position -
			               sim->getAgentPosition(i)) == 0.0f) {
				if (minVertex == goals[i]) {
					sim->setAgentPrefVelocity(i, RVO::Vector2());
				}
				else {
					sim->setAgentPrefVelocity(i, RVO::normalize(roadmap[goals[i]].position - sim->getAgentPosition(i)));
				}
			}
			else {
				sim->setAgentPrefVelocity(i, RVO::normalize(roadmap[minVertex].position - sim->getAgentPosition(i)));
			}
		}

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
	/* Check if all agents have reached their goals. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		if (RVO::absSq(sim->getAgentPosition(i) - roadmap[goals[i]].position) > 20.0f * 20.0f) {
			return false;
		}
	}

	return true;
}

int main()
{

	
	/* Create a new simulator instance. */
	RVO::RVOSimulator *sim = new RVO::RVOSimulator();

	/* Set up the scenario. */
	setupScenario(sim);

	/* Build the roadmap. */
	buildRoadmap(sim);

	/* Perform (and manipulate) the simulation. */
	do {
#if RVO_OUTPUT_TIME_AND_POSITIONS
		updateVisualization(sim);
#endif
		setPreferredVelocities(sim);
		sim->doStep();
	}
	while (!reachedGoal(sim));

	delete sim;

	return 0;
}
