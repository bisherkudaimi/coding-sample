#ifndef MYOTHELLOAPI_HPP
#define MYOTHELLOAPI_HPP



#include "OthelloAI.hpp"



namespace kudaimib
{
    class BisherAI : public OthelloAI
    {
    public:
    	// Implements the recursive algorithm 
        virtual std::pair<int, int> chooseMove(const OthelloGameState& state);


        // Finds the best move based on whoever's turn it is on the current state.
        int search(std::unique_ptr<OthelloGameState>& state, int depth, std::string turn);


        // Creates a vector of the available moves for the current player and th current 
        // state
        std::vector<std::pair<int, int>> createMoveVector(const OthelloGameState& state);

        // Creates a vector of vectors of ints of the different values of positions 
        // on the board. For 8x8, would look like:
        // {100,  -1, 5, 2, 2, 5,  -1, 100}
        // { -1, -20, 1, 1, 1, 1, -20,  -1}
    	// {  5,   1, 1, 1, 1, 1,   1,   5}
        // {  2,   1, 1, 0, 0, 1,   1,   2}
    	// {  2,   1, 1, 0, 0, 1,   1,   2}
    	// {  5,   1, 1, 1, 1, 1,   1,   5}
    	// { -1, -20, 1, 1, 1, 1, -20,  -1}
    	// {100,  -1, 5, 2, 2, 5,  -1, 100}	
        std::vector<std::vector<int>> createBoardValueVector(int width, int height);

        // Goes through the board and evaluates the score for the current player
        int evaluateBoard(std::unique_ptr<OthelloGameState>& state, std::string t);

    private:
    	std::string turn;
    	std::string opposite_turn;
    	std::vector<std::vector<int>> board_values;
    };
}



#endif