#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <utility>
#include <list>
#include <functional>
#include <cmath>
#include <algorithm>
#include <utility>
#include <iomanip>
#include <limits>
#include <chrono>

#include "state.h"

using namespace std;

typedef function<int(const State& pos)> Heuristic;

int h0(const State& state) {
	return 0;
}

int h1(const State& state) {
	return state.getNbBlocksNotLastStack();
}

int h2(const State& state) {
	return state.getNbBlocksNotLastStack() + 2*state.getNbBlocksNotSortedLastBlock();
}

int h3(const State& state) {
	return state.getNbBlocksNotLastStack() + 2*state.getNbBlocksNotSortedLastBlock() + state.getNbBlocksNotLastStackAscendingFromTop();
}

void addNeighbor(State &currentState, const State::Move &move, vector<pair<State::Move,int>> &neighbors, list<State>& path, Heuristic h) {
	currentState.doMove(move);
	if (find(path.begin(), path.end(), currentState) == path.end()) {
		neighbors.push_back(make_pair(move, h(currentState)));
	}
	currentState.doMove(make_pair(move.second, move.first)); // undo move
}

void search(State& currentState,
				int ub, // upper bound over which exploration must stop
				int& nub,
				list<State>& path,
				list<State>& bestPath,
				Heuristic h,
				int& nbVisitedState) {
	nbVisitedState++;

	int f; // under-estimation of optimal length
	int g = path.size() - 1; // size of the current path to currentState

	if (currentState.isFinal()) {
		bestPath = path;
		return;
	}

	// generate the neighbors
	vector<pair<State::Move,int>> neighbors;
	neighbors.clear();

	for (int i = 0; i < currentState.getNbStacks(); ++i) {
		if (!currentState.emptyStack(i)) {
			for (int j = 0; j < currentState.getNbStacks(); ++j) {
				if (i != j) {
					addNeighbor(currentState, make_pair(i, j), neighbors, path, h);
				}
			}
		}
	}

	// // sort the neighbors by heuristic value

	// sort(neighbors.begin(), neighbors.end(),
	// 		[](const pair<State::Move,int> &left, const pair<State::Move,int> &right) {
	// 			return left.second < right.second;
	// 		} );

	for (const pair<State::Move,int> &p : neighbors) {
		f = g + 1 + p.second;
		if (f > ub) {
			if (f < nub) {
				nub = f; // update the next upper bound
			}
		}
		else {
			currentState.doMove(p.first);
			path.push_back(currentState);
			search(currentState, ub, nub, path, bestPath, h, nbVisitedState);
			path.pop_back();
			currentState.doMove(make_pair(p.first.second, p.first.first)); // undo move
			if(!bestPath.empty()) return;
		}
	}
}

void ida(State&	initialState, Heuristic	h, list<State>&	bestPath, int& nbVisitedState) { // bestPath = path from source to destination
	int ub;	// current upper bound
	int nub = h(initialState); // next upper bound
	list<State> path;
	path.push_back(initialState); // the path to the target starts with the source

	while (bestPath.empty() && nub != numeric_limits<int>::max()) {
		ub = nub;
		nub = numeric_limits<int>::max();

		cout << "upper bound: " << ub;
		search(initialState, ub, nub, path, bestPath, h, nbVisitedState);
		cout << " ; nbVisitedState: " << nbVisitedState << endl;
	}
}

int main() {
	int nbStacks = 4;
	int nbBlocs = 20;
	State state = State(nbBlocs, nbStacks);
	state.setInitial();
	list<State> bestPath;
	int nbVisitedState = 0;

	auto start = std::chrono::high_resolution_clock::now();
	ida(state, h3, bestPath, nbVisitedState);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Elapsed time: " << elapsed.count() << " s\n";
	cout << "nb moves: " << bestPath.size()-1 << endl;
	cout << "nb visited states: " << nbVisitedState << endl;

	return 0;
}