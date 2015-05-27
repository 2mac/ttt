/*
 *  ttt - Simple arbitrary-size tic-tac-toe game
 *  Copyright (C) 2015 David McMackins II
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, version 3 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>

#ifndef BOARD_SIZE
/**
 * @brief The square dimensions of the game board (best when >=1 and <=26).
 */
# define BOARD_SIZE 3
#endif

/**
 * @brief Data structure for move position.
 */
struct move
{
  uint8_t h; // horizontal position
  uint8_t v; // vertical position
};

/**
 * @brief Storage for the board state.
 */
char (*board)[BOARD_SIZE];

/**
 * @brief Calculates the number of spaces by which to bump the board.
 * @param s The size of the board for which to bump.
 * @return The bump size for the given board.
 */
static uint8_t
get_bump (uint8_t s)
{
  uint8_t i = 1;

  while (s > 10)
    {
      ++i;
      s /= 10;
    }

  return i;
}

/**
 * @brief Draws the board on the screen with some white space above it.
 */
static void
draw_board (void)
{
  uint8_t i, j, bump = get_bump (BOARD_SIZE);

  printf ("\n\n\n ");

  for (i = 0; i < bump; ++i)
    putchar (' ');

  for (i = 0; i < BOARD_SIZE; ++i)
    printf (" %c", i + 'A');

  putchar ('\n');

  for (i = 0; i < BOARD_SIZE; ++i)
    {
      printf ("%d", i);

      // make sure board lines up on all rows
      uint8_t b = bump - get_bump (i+1);
      for (j = 0; j < b; ++j)
	putchar (' ');

      printf (" |");

      for (j = 0; j < BOARD_SIZE; ++j)
	printf ("%c|", board[i][j]);

      putchar ('\n');
    }
}

/**
 * @brief Gets a player's desired move.
 * @param player The player for which to obtain the move.
 * @return Move data based on player input.
 */
static struct move
get_move (char player)
{
  struct move m;
  char v;

  printf ("%c's move: ", player);
  scanf ("%c%hhu", &v, &m.h);

  // discard extra input to prevent cheating
  while (getchar () != '\n')
    ;

  m.v = toupper (v) - 'A';

  return m;
}

/**
 * @brief Checks if a player has won.
 * @param player The player for which to check win status.
 * @return true if the player has BOARD_SIZE in a row.
 */
static bool
is_winner (char player)
{
  int16_t i, j;

  // check rows
  for (i = 0; i < BOARD_SIZE; ++i)
    for (j = 0; j < BOARD_SIZE; ++j)
      {
	if (board[i][j] != player)
	  break;

	if ((BOARD_SIZE - 1) == j)
	  return true;
      }

  // check columns
  for (i = 0; i < BOARD_SIZE; ++i)
    for (j = 0; j < BOARD_SIZE; ++j)
      {
	if (board[j][i] != player)
	  break;

	if ((BOARD_SIZE - 1) == j)
	  return true;
      }

  // check diagonal from top-left to bottom-right
  for (i = 0, j = 0; i < BOARD_SIZE && j < BOARD_SIZE; ++i, ++j)
    {
      if (board[i][j] != player)
	break;

      if ((BOARD_SIZE - 1) == i)
	return true;
    }

  // check diagonal from top-right to bottom-left
  for (i = 0, j = BOARD_SIZE-1; i < BOARD_SIZE && j >= 0; ++i, --j)
    {
      if (board[i][j] != player)
	break;

      if (0 == j)
	return true;
    }

  return false;
}

/**
 * @brief Checks if the board is in a winning state.
 * @return The player who wins or 0 if no winner.
 */
static char
find_winner (void)
{
  char players[] = {'X', 'O'};
  uint8_t i;

  for (i = 0; i < 2; ++i)
    if (is_winner (players[i]))
      return players[i];

  return 0;
}

int
main (void)
{
  char turn = 'X', winner;
  uint16_t num_turns = 0;

  board = malloc (sizeof (char[BOARD_SIZE][BOARD_SIZE]));
  if (NULL == board)
    {
      fputs ("Error allocating board", stderr);
      return 1;
    }

  uint8_t i, j;
  for (i = 0; i < BOARD_SIZE; ++i)
    for (j = 0; j < BOARD_SIZE; ++j)
      board[i][j] = ' ';

  while (!(winner = find_winner ()) && num_turns < BOARD_SIZE * BOARD_SIZE)
    {
      draw_board ();

      struct move m = get_move (turn);

      if (m.h >= BOARD_SIZE || m.v >= BOARD_SIZE)
	{
	  fputs ("That space is not on the board!", stderr);
	  continue;
	}

      if (' ' == board[m.h][m.v])
	{
	  board[m.h][m.v] = turn;
	}
      else
	{
	  fputs ("That space is already taken!", stderr);
	  continue;
	}

      turn = turn == 'X' ? 'O' : 'X';
      ++num_turns;
    }

  draw_board ();
  free (board);

  if (!winner)
    puts ("No one wins. :(");
  else
    printf ("%c wins!\n", winner);

  return 0;
}
