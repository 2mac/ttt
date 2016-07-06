/*
 *  ttt - Simple arbitrary-size tic-tac-toe game
 *  Copyright (C) 2015-2016 David McMackins II
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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  unsigned int h; // horizontal position
  unsigned int v; // vertical position
};

/**
 * @brief Storage for the board state.
 */
static char **board = NULL;

/**
 * @brief Calculates the number of spaces by which to bump the board.
 * @param size The size of the board for which to bump.
 * @return The bump size for the given board.
 */
static unsigned int
get_bump (unsigned int size)
{
  unsigned int i = 1;

  while (size > 10)
    {
      ++i;
      size /= 10;
    }

  return i;
}

/**
 * @brief Draws horizontal bar.
 */
static void
draw_horiz_bar ()
{
  for (unsigned int i = 0; i <= get_bump (BOARD_SIZE); ++i)
    putchar (' ');
  
  putchar ('+');

  for (unsigned int i = 0; i < BOARD_SIZE; ++i)
    printf ("---+");

  putchar ('\n');
}

/**
 * @brief Draws the board on the screen with some white space above it.
 */
static void
draw_board ()
{
  unsigned int bump = get_bump (BOARD_SIZE);

  printf ("\n\n\n");

  for (unsigned int i = 0; i < bump; ++i)
    putchar (' ');

  for (unsigned int i = 0; i < BOARD_SIZE; ++i)
    printf ("   %c", i + 'A');

  putchar ('\n');
  
  draw_horiz_bar ();

  for (unsigned int i = 0; i < BOARD_SIZE; ++i)
    {
      printf ("%u", i);

      // make sure board lines up on all rows
      unsigned int b = bump - get_bump (i+1);
      for (unsigned int j = 0; j < b; ++j)
        putchar (' ');

      printf (" |");

      for (unsigned int j = 0; j < BOARD_SIZE; ++j)
        printf (" %c |", board[i][j]);

      putchar ('\n');

      draw_horiz_bar ();
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
  char h;
  struct move m =
    {
      .h = BOARD_SIZE,
      .v = BOARD_SIZE
    };

  printf ("%c's move: ", player);
  scanf ("%c%u", &h, &m.v);

  // discard extra input to prevent cheating
  while (getchar () != '\n')
    ;

  m.h = toupper (h) - 'A';

  return m;
}

/**
 * @brief Checks if a player has won.
 * @param player The player for which to check win status.
 * @return true if the player's moves make a winning tic-tac-toe combination.
 */
static bool
is_winner (char player)
{
  unsigned int i, j;

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
  for (i = 0, j = BOARD_SIZE - 1; i < BOARD_SIZE && j >= 0; ++i, --j)
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
find_winner ()
{
  static const char players[] = { 'X', 'O' };

  for (unsigned int i = 0; i < 2; ++i)
    if (is_winner (players[i]))
      return players[i];

  return 0;
}

static void *
alloc_board ()
{
  char **board = malloc (BOARD_SIZE * sizeof (char *));
  if (!board)
    return NULL;

  unsigned int i;
  for (i = 0; i < BOARD_SIZE; ++i)
    {
      board[i] = malloc (BOARD_SIZE * sizeof (char));
      if (!board[i])
        goto fail;

      memset (board[i], ' ', BOARD_SIZE);
    }

  return board;

 fail:
  for (unsigned int j = 0; j < i; ++j)
    free (board[j]);

  free (board);

  return NULL;
}

static void
free_board ()
{
  for (unsigned int i = 0; i < BOARD_SIZE; ++i)
    free (board[i]);

  free (board);
}

int
main (void)
{
  board = alloc_board ();
  if (!board)
    {
      fputs ("Error allocating board", stderr);
      return 1;
    }

  char turn = 'X', winner;
  unsigned int num_turns = 0;
  do
    {
      draw_board ();

      struct move m = get_move (turn);

      if (m.h >= BOARD_SIZE || m.v >= BOARD_SIZE)
        {
          fputs ("That space is not on the board!", stderr);
          continue;
        }

      if (' ' != board[m.v][m.h])
        {
          fputs ("That space is already taken!", stderr);
          continue;
        }

      board[m.v][m.h] = turn;

      turn = turn == 'X' ? 'O' : 'X';
    }
  while (!(winner = find_winner ()) && num_turns++ < BOARD_SIZE * BOARD_SIZE);

  draw_board ();
  free_board ();

  if (!winner)
    puts ("No one wins. :(");
  else
    printf ("%c wins!\n", winner);

  return 0;
}
