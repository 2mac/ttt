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

#define BOARD_SIZE 3

struct move
{
  unsigned int h;
  unsigned int v;
};

char (*board)[BOARD_SIZE];

static int
get_bump (void)
{
  int i = 1, s = BOARD_SIZE;

  while (s > 10)
    {
      ++i;
      s /= 10;
    }

  return i;
}

static void
draw_board (void)
{
  int i, j;

  printf ("\n\n\n ");

  for (i = 0; i < get_bump (); ++i)
    putchar (' ');

  for (i = 0; i < BOARD_SIZE; ++i)
    printf (" %c", (int) i + 'A');

  putchar ('\n');

  for (i = 0; i < BOARD_SIZE; ++i)
    {
      printf ("%d |", i);

      for (j = 0; j < BOARD_SIZE; ++j)
	printf ("%c|", board[i][j]);

      putchar ('\n');
    }
}

static struct move
get_move (char player)
{
  struct move m;
  char v;

  printf ("%c's move: ", player);
  scanf ("%c%u", &v, &m.h);

  getchar (); /* eat enter */

  m.v = (char) toupper (v) - 'A';

  return m;
}

static bool
is_winner (char player)
{
  int i, j;

  for (i = 0; i < BOARD_SIZE; ++i)
    for (j = 0; j < BOARD_SIZE; ++j)
      {
	if (board[i][j] != player)
	  break;

	if ((BOARD_SIZE - 1) == j)
	  return true;
      }

  for (i = 0; i < BOARD_SIZE; ++i)
    for (j = 0; j < BOARD_SIZE; ++j)
      {
	if (board[j][i] != player)
	  break;

	if ((BOARD_SIZE - 1) == j)
	  return true;
      }

  for (i = 0, j = 0; i < BOARD_SIZE && j < BOARD_SIZE; ++i, ++j)
    {
      if (board[i][j] != player)
	break;

      if ((BOARD_SIZE - 1) == i)
	return true;
    }

  for (i = 0, j = BOARD_SIZE-1; i < BOARD_SIZE && j >= 0; ++i, --j)
    {
      if (board[i][j] != player)
	break;

      if (0 == j)
	return true;
    }

  return false;
}

static int
find_winner (void)
{
  char players[] = {'X', 'O'};
  unsigned int i;

  for (i = 0; i < 2; ++i)
    if (is_winner (players[i]))
      return players[i];

  return 0;
}

int
main (void)
{
  char turn = 'X';
  int winner, i, j, num_turns = 0;

  board = malloc (sizeof (char[BOARD_SIZE][BOARD_SIZE]));
  if (NULL == board)
    {
      fputs ("Error allocating board", stderr);
      return 1;
    }

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
    printf ("%c wins!\n", (char) winner);

  return 0;
}
