/*
 *  ttt - Simple arbitrary-size tic-tac-toe game
 *  Copyright (C) 2015-2017 David McMackins II
 *
 * Redistributions, modified or unmodified, in whole or in part, must retain
 * applicable copyright or other legal privilege notices, these conditions, and
 * the following license terms and disclaimer.  Subject to these conditions,
 * the holder(s) of copyright or other legal privileges, author(s) or
 * assembler(s), and contributors of this work hereby grant to any person who
 * obtains a copy of this work in any form:
 *
 * 1. Permission to reproduce, modify, distribute, publish, sell, sublicense,
 * use, and/or otherwise deal in the licensed material without restriction.
 *
 * 2. A perpetual, worldwide, non-exclusive, royalty-free, irrevocable patent
 * license to reproduce, modify, distribute, publish, sell, use, and/or
 * otherwise deal in the licensed material without restriction, for any and all
 * patents:
 *
 *     a. Held by each such holder of copyright or other legal privilege,
 *     author or assembler, or contributor, necessarily infringed by the
 *     contributions alone or by combination with the work, of that privilege
 *     holder, author or assembler, or contributor.
 *
 *     b. Necessarily infringed by the work at the time that holder of
 *     copyright or other privilege, author or assembler, or contributor made
 *     any contribution to the work.
 *
 * NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS
 * LICENSE OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE, AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS,
 * ASSEMBLERS, OR HOLDERS OF COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR
 * ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT,
 * OR OTHERWISE ARISING FROM, OUT OF, OR IN CONNECTION WITH THE WORK OR THE USE
 * OF OR OTHER DEALINGS IN THE WORK.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VERSION "1.2.0"
#define LICENSE "Copyright (C) 2015-2017 David McMackins II\n\
License COIL: Copyfree Open Innovation License 0.5\n\
<http://coil.apotheon.org/plaintext/00.5.txt>\n\
\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law."

#define HELP "USAGE: ttt [options]\n\
\n\
OPTIONS:\n\
\t-h:\t\tDisplay this help and exit\n\
\t-s SIZE:\tSet board size to SIZE\n\
\t-v:\t\tDisplay version info and exit\n"

/**
 * @brief Data structure for the board
 */
struct board
{
	unsigned int size;
	char **data;
};

static struct board *
alloc_board(unsigned int board_size)
{
	struct board *board = malloc(sizeof(struct board));
	if (!board)
		return NULL;

	unsigned int i = 0;

	board->data = malloc(board_size * sizeof(char *));
	if (!board->data)
		goto fail;


	for (; i < board_size; ++i)
	{
		board->data[i] = malloc(board_size * sizeof(char));
		if (!board->data[i])
			goto fail;

		memset(board->data[i], ' ', board_size);
	}

	board->size = board_size;
	return board;

fail:
	for (unsigned int j = 0; j < i; ++j)
		free(board->data[j]);

	if (board->data)
		free(board->data);

	free(board);

	return NULL;
}

static void
free_board(struct board *board)
{
	for (unsigned int i = 0; i < board->size; ++i)
		free(board->data[i]);

	free(board->data);
	free(board);
}

/**
 * @brief Data structure for move position.
 */
struct move
{
	unsigned int h; // horizontal position
	unsigned int v; // vertical position
};

/**
 * @brief Calculates the number of spaces by which to bump the board.
 * @param size The size of the board for which to bump.
 * @return The bump size for the given board.
 */
static unsigned int
get_bump(unsigned int size)
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
 * @param num_cells Number of cells per row.
 */
static void
draw_horiz_bar(unsigned int num_cells)
{
	for (unsigned int i = 0; i <= get_bump(num_cells); ++i)
		putchar(' ');

	putchar('+');

	for (unsigned int i = 0; i < num_cells; ++i)
		printf("---+");

	putchar('\n');
}

/**
 * @brief Draws the board on the screen with some white space above it.
 * @param board Reference to the game board.
 */
static void
draw_board(const struct board *board)
{
	unsigned int board_size = board->size;
	unsigned int bump = get_bump(board_size);

	printf("\n\n\n");

	for (unsigned int i = 0; i < bump; ++i)
		putchar(' ');

	for (unsigned int i = 0; i < board_size; ++i)
		printf("   %c", i + 'A');

	putchar('\n');

	draw_horiz_bar(board_size);

	for (unsigned int i = 0; i < board_size; ++i)
	{
		printf("%u", i);

		// make sure board lines up on all rows
		unsigned int b = bump - get_bump(i + 1);
		for (unsigned int j = 0; j < b; ++j)
			putchar(' ');

		printf(" |");

		for (unsigned int j = 0; j < board_size; ++j)
			printf(" %c |", board->data[i][j]);

		putchar('\n');

		draw_horiz_bar(board_size);
	}
}

/**
 * @brief Gets a player's desired move.
 * @param m Target move structure to store the result in.
 * @param player The player for which to obtain the move.
 * @param board_size Size of the game board.
 */
static void
get_move(struct move *m, char player, unsigned int board_size)
{
	char h;
	m->h = board_size;
	m->v = board_size;

	printf("%c's move: ", player);
	scanf("%c%u", &h, &m->v);

	// discard extra input to prevent cheating
	while (getchar() != '\n')
		;

	m->h = toupper(h) - 'A';
}

/**
 * @brief Checks if a player has won.
 * @param board Reference to the game board.
 * @param player The player for which to check win status.
 * @return true if the player's moves make a winning tic-tac-toe combination.
 */
static bool
is_winner(const struct board *board, char player)
{
	unsigned int i, j, board_size = board->size;;

	// check rows
	for (i = 0; i < board_size; ++i)
		for (j = 0; j < board_size; ++j)
		{
			if (board->data[i][j] != player)
				break;

			if ((board_size - 1) == j)
				return true;
		}

	// check columns
	for (i = 0; i < board_size; ++i)
		for (j = 0; j < board_size; ++j)
		{
			if (board->data[j][i] != player)
				break;

			if ((board_size - 1) == j)
				return true;
		}

	// check diagonal from top-left to bottom-right
	for (i = 0, j = 0; i < board_size && j < board_size; ++i, ++j)
	{
		if (board->data[i][j] != player)
			break;

		if ((board_size - 1) == i)
			return true;
	}

	// check diagonal from top-right to bottom-left
	for (i = 0, j = board_size - 1; i < board_size; ++i, --j)
	{
		if (board->data[i][j] != player)
			break;

		if (0 == j)
			return true;
	}

	return false;
}

/**
 * @brief Checks if the board is in a winning state.
 * @param board Reference to the game board.
 * @return The player who wins or 0 if no winner.
 */
static char
find_winner(const struct board *board)
{
	static const char players[] = { 'X', 'O' };

	for (unsigned int i = 0; i < 2; ++i)
		if (is_winner(board, players[i]))
			return players[i];

	return 0;
}

int
main(int argc, char *argv[])
{
	unsigned int board_size = 3;

	printf("ttt v%s\n", VERSION);

	int c;
	while ((c = getopt(argc, argv, "hs:v")) != -1)
	{
		int rc;
		unsigned int temp;

		switch (c)
		{
		case 'h':
			printf("%s\n", HELP);
			return 0;

		case 's':
			rc = sscanf(optarg, "%u", &temp);
			if (rc != 1)
			{
				fprintf(stderr,
					"Board size must be a number\n");
				return 1;
			}

			if (temp < 3)
			{
				fprintf(stderr, "A board of size %u wouldn't be much of a game, would it?\n",
					temp);
				return 1;
			}

			board_size = temp;
			break;

		case 'v':
			printf("%s\n", LICENSE); // already printed version
			return 0;

		case '?':
			return 1;

		case ':':
			fprintf(stderr, "Option %c requires an argument\n",
				optopt);
			return 1;
		}
	}

	struct board *board = alloc_board(board_size);
	if (!board)
	{
		fprintf(stderr, "Error allocating board\n");
		return 1;
	}

	printf("Select moves like <letter><number> (i.e. a0).\n");

	char turn = 'X', winner;
	unsigned int num_turns = 0;
	const unsigned int max_turns = board_size * board_size;
	do
	{
		draw_board(board);

		struct move m;
		get_move(&m, turn, board_size);

		if (m.h >= board_size || m.v >= board_size)
		{
			printf("That space is not on the board!\n");
			continue;
		}

		if (' ' != board->data[m.v][m.h])
		{
			printf("That space is already taken!\n");
			continue;
		}

		board->data[m.v][m.h] = turn;

		turn = turn == 'X' ? 'O' : 'X';
		++num_turns;
	} while (!(winner = find_winner(board))
		&& num_turns < max_turns);

	draw_board(board);
	free_board(board);

	if (!winner)
		printf("No one wins. :(\n");
	else
		printf("%c wins!\n", winner);

	return 0;
}
