#include <ics46/factory/DynamicFactory.hpp>
#include "MyOthelloAI.hpp"


#include <string>
#include <iostream>
#include <vector>



ICS46_DYNAMIC_FACTORY_REGISTER(OthelloAI, kudaimib::BisherAI, "Boothello (Required)");



std::vector<std::pair<int, int>> kudaimib::BisherAI::createMoveVector(const OthelloGameState& state)
{
	std::vector<std::pair<int, int>> move_vector;

	for (int i = 0; i < state.board().width(); i++)
	{
		for (int j = 0; j < state.board().height(); j++)
		{
			if (state.isValidMove(i, j))
			{
				move_vector.push_back(std::pair<int, int>(i, j));
			}
		}
	}

	return move_vector;
}


std::pair<int, int> kudaimib::BisherAI::chooseMove(const OthelloGameState& state)
{
	if (state.isBlackTurn())
	{
		turn = "black";
		opposite_turn = "white";
	}
	else
	{
		turn = "white";
		opposite_turn = "black";
	}

	int score;

	int max_score = -99999;

	std::vector<std::pair<int, int>> available_moves = createMoveVector(state);

	std::pair<int, int> best_move;
	
	for (int move = 0; move < available_moves.size(); move++)
	{	
		std::unique_ptr<OthelloGameState> board_copy = state.clone();
		board_copy->makeMove(available_moves[move].first, available_moves[move].second);

		score = search(board_copy, 2, opposite_turn);

		if (score > max_score)
		{
			max_score = score;
			best_move.first = available_moves[move].first;
			best_move.second = available_moves[move].second;
		}
	}

	return best_move;
}


int kudaimib::BisherAI::search(std::unique_ptr<OthelloGameState>& state, int depth, std::string t)
{	
	int rows = state->board().width();
	int columns = state->board().height();

	board_values = createBoardValueVector(columns, rows);

	int value;
	int my_best_score = -99999;
	int opposite_best_score = 99999;
	
	if (depth == 0 || state->isGameOver())
	{	
        return evaluateBoard(state, t);
	}
    else
    {	
    	std::vector<std::pair<int, int>> move_vector = createMoveVector(*state);

        if (t == turn)
        {
            for (int m = 0; m < move_vector.size(); m++)
            {	
            	std::unique_ptr<OthelloGameState> temp_copy = state->clone();
            	temp_copy->makeMove(move_vector[m].first, move_vector[m].second);

            	value = search(temp_copy, depth - 1, opposite_turn); 

            	if (value > my_best_score)
            	{
            		my_best_score = value;
            	}
            }
                
            return my_best_score;
        }
        else
        {
            for (int m = 0; m < move_vector.size(); m++)
            {	
            	std::unique_ptr<OthelloGameState> temp_copy = state->clone();
            	temp_copy->makeMove(move_vector[m].first, move_vector[m].second);

            	value = search(temp_copy, depth - 1, turn);

            	if (value < opposite_best_score)
            	{
            		opposite_best_score = value;
            	}
            }
                
            return opposite_best_score;
        }
    }
}


std::vector<std::vector<int>> kudaimib::BisherAI::createBoardValueVector(int columns, int rows)
{
	// Initialize as all 1's
	std::vector<std::vector<int>> value_vector;
	value_vector = std::vector<std::vector<int>>(columns, std::vector<int>(rows, 1));
	
	// Set up middle 4
	for (int m = (columns / 2 - 1); m < (columns / 2 + 1); m++)
	{
		for (int n = (rows / 2 - 1); n < (rows / 2 + 1); n++)
		{
			value_vector[m][n] = 0;
		}
	}
	
	// Set up edges to be rows of 2's before changing corners
	for (int t = 0; t < columns; t++)
	{
		value_vector[t][0] = 2;
		value_vector[t][rows - 1] = 2;
	}
	
	for (int l = 0; l < rows; l++)
	{
		value_vector[0][l] = 2;
		value_vector[columns - 1][l] = 2;
	}
	
	// Set up top left corner
	value_vector[0][0] = 100;
	value_vector[0][1] = -1;
	value_vector[1][0] = -1;

	// Set up top right corner
	value_vector[columns - 1][0] = 100;
	value_vector[columns - 2][0] = -1;
	value_vector[columns - 1][1] = -1;
	
	// Set up bottom left corner
	value_vector[0][rows - 1] = 100;
	value_vector[0][rows - 2] = -1;
	value_vector[1][rows - 1] = -1;
	
	// Set up bottom right corner
	value_vector[columns - 1][rows - 1] = 100;
	value_vector[columns - 1][rows - 2] = -1;
	value_vector[columns - 2][rows - 1] = -1;

	// Set up adjacent to adjacent of corners to be 5's
	// and diagonals to corner to be 20's
	if (columns > 4)
	{
		value_vector[2][0] = 5;
		value_vector[columns - 3][0] = 5;

		value_vector[2][rows - 1] = 5;
		value_vector[columns - 3][rows - 1] = 5;		
	}
	
	if (rows > 4)
	{
		value_vector[0][2] = 5;
		value_vector[0][rows - 3] = 5;

		value_vector[columns - 1][2] = 5;
		value_vector[columns - 1][rows - 3] = 5;		
	}
	
	if (columns > 4 && rows > 4)
	{
		value_vector[1][1] = -20;
		value_vector[columns - 2][1] = -20;
		value_vector[1][rows - 2] = -20;
		value_vector[columns - 2][rows - 2] = -20;
	}
	
	return value_vector;
}


int kudaimib::BisherAI::evaluateBoard(std::unique_ptr<OthelloGameState>& state, std::string t)
{
	int b_count = 0;
	int w_count = 0;

	for (int x = 0; x < state->board().width(); x++)
		{
			for (int y = 0; y < state->board().height(); y++)
			{
				if (state->board().cellAt(x, y) == OthelloCell::black)
				{
					b_count += board_values[x][y];
				}
				else if (state->board().cellAt(x, y) == OthelloCell::white)
				{
					w_count += board_values[x][y];
				}
			}
		}

	if (t == "white")
	{	
		return state->blackScore() - state->whiteScore() + b_count;
	}
	else
	{
		return state->whiteScore() - state->blackScore() + w_count;
	}
}