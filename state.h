#include <iostream>
#include <utility>
#include <vector>

using namespace std;

class State {
	public:
		State(int n, int k);
		// Constructor that creates a state with n blocs and k stacks
		State(const State& s);
		// Constructor that creates a copy of s
		void setInitial();
		// Initialise this with the initial state (blocs uniformly distributed on stacks)
		bool isFinal() const;
		// Return true if this is a final state (all blocs on the last stack, from largest to smallest)
		typedef pair<int,int> Move;
		// A move is a pair of stacks: the bloc on top of the first stack is moved to the top of the second stack
		void doMove(Move m);
		// Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(), and !emptyStack(m.first)
		// Postrelation: Apply the move (m.first->m.second) to this
		int heuristic();
		// Return a lower bound on the minimal number of moves necessary to transform this into the final state
		void display() const;
		// Display this
		bool operator<(const State&) const;
		bool operator>(const State&) const;
		bool operator==(const State&) const;
		bool operator!=(const State&) const;
		int getNbStacks() const;
		// Return the number of stacks
		bool emptyStack(int s);
		// Precondition: 0 <= s < getNbStacks()
		// Return true if stack s has no block
		~State();
		// Destructor

		int getNbBlocksNotLastStack() const;
		// Return the number of blocks not on the last stack
		int getNbBlocksNotSortedLastBlock() const;
		// Return the number of blocks b such that b is on the last stack but the list of blocks under b is different from (b + 1, b + 2, ..., n - 1).
		int getNbBlocksNotLastStackAscendingFromTop() const;
		// Return the number of blocks not on the last stack which are ordered in ascending order from top.
	private:
		int nbBlocs, nbStacks;
		int* stack; // For each bloc b: stack[b] = stack that contains b
		int* next;  // For each bloc b: if b is the lowest block of its stack,
					// then next[b]=-1, else next[b]=bloc immediately below b
		int* top;   // For each stack s: if s is empty, then top[s]=-1
					// else top[s]=bloc on top of s
		int compare(const int*, const int*, const int*) const; // function to compare states
};

State::State(int n, int k) {
	// Constructor that creates a state with n blocs and k stacks
	nbBlocs = n;
	nbStacks = k;
	stack = new int[nbBlocs];
	next = new int[nbBlocs];
	top = new int[nbStacks];
}

State::State(const State& state) {
	// Constructor that creates a copy of state
	nbBlocs = state.nbBlocs;
	nbStacks = state.nbStacks;
	stack = new int[nbBlocs];
	next = new int[nbBlocs];
	top = new int[nbStacks];
	for (int s=0; s<nbStacks; s++){
		top[s] = state.top[s];
	}
	for (int b=0; b<nbBlocs; b++){
		next[b] = state.next[b];
		stack[b] = state.stack[b];
	}
}

State::~State() {
	delete[] next;
	delete[] stack;
	delete[] top;
}

void State::setInitial() {
	// Initialise this with the initial state (blocs uniformly distributed on stacks)
	for (int s=0; s<nbStacks; s++){
		top[s] = -1;
	}
	for (int b=0; b<nbBlocs; b++){
		int s = b % nbStacks; // Put bloc b on top of stack s
		stack[b] = s;
		next[b] = top[s];
		top[s] = b;
	}
}

bool State::isFinal() const {
	// Return true if this is a final state (all blocs on the last stack, from largest to smallest)
	for (int s=0; s<nbStacks-1; s++){
		if (top[s] != -1) return false;
	}
	if (top[nbStacks-1] != 0) return false;
	for (int b=0; b<nbBlocs-1; b++){
		if (stack[b] != nbStacks-1) return false;
		if (next[b] != b+1) return false;
	}
	if (next[nbBlocs-1] != -1) return false;
	return true;
}

void State::doMove(Move m) {
	// Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(), and !emptyStack(m.first)
	// Apply the doMove (s1->s2) to this
	int b1 = top[m.first];
	int bb1 = next[b1];
	int b2 = top[m.second];
	next[b1] = b2;
	top[m.second] = b1;
	top[m.first] = bb1;
	stack[b1] = m.second;
}

void displayBlockStack(int b, int* next) {
	// Display all blocs below b (including b), from bottom to top
	if (b >= 0){
		displayBlockStack(next[b], next);
		cout << b << " ";
	}
}

void State::display() const {
	// Display this
	for (int s=0; s<nbStacks; s++){
		cout << "stack "<< s << ": ";
		displayBlockStack(top[s], next);
		cout << endl;
	}
}

int State::getNbStacks() const {
	return nbStacks;
}

bool State::emptyStack(int s) {
	// Precondition: 0 <= s < getNbStacks()
	// Return true if stack s has no block
	return top[s] == -1;
}

bool State::operator<(const State& state) const {
	// Return true if this < state
	return state.compare(stack,top,next) < 0;
}

bool State::operator>(const State& state) const {
	// Return true if this > state
	return state.compare(stack,top,next) > 0;
}

bool State::operator==(const State& state) const {
	// Return true if this == state
	return state.compare(stack,top,next) == 0;
}

bool State::operator!=(const State& state) const {
	// Return true if this != state
	return state.compare(stack,top,next) != 0;
}

int State::compare(const int* stack2, const int* top2, const int* next2) const {
	for (int b=0; b<nbBlocs; b++){
		if (stack[b]<stack2[b]) return -1;
		if (stack[b]>stack2[b]) return 1;
		if (next[b]<next2[b]) return -1;
		if (next[b]>next2[b]) return 1;
	}
	for (int s=0; s<nbStacks; s++){
		if (top[s]<top2[s]) return -1;
		if (top[s]>top2[s]) return 1;
	}
	return 0;
}

int State::getNbBlocksNotLastStack() const {
	int nb = 0;
	for (int i = 0; i < nbBlocs; ++i) {
		if (stack[i] != nbStacks-1) ++nb;
	}
	return nb;
}

int State::getNbBlocksNotSortedLastBlock() const {
	int nb = 0;
	int b = top[nbStacks-1];
	while (b != -1) {
		int c = b; // current block
		while (next[c] != -1 && next[c] == c + 1) {
			c = next[c];
		}
		if (next[c] != -1 || c != nbBlocs-1) {
			++nb;
		}
		b = next[b];
	}
	return nb;
}

int State::getNbBlocksNotLastStackAscendingFromTop() const {
	int nb = 0;
	for (int i = 0; i < nbStacks-1; ++i) {
		int b = top[i];
		while (b != -1) {
			int c = next[b]; // current block
			while (next[c] != -1 && c < b) {
				c = next[c];
			}
			if (next[c] != -1) {
				++nb;
			}
			b = next[b];
		}
	}
	return nb;
}