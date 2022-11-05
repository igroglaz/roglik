#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h> // strcmp()

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6

int py, px; // @ coords
int sy, sx; // > coords
int att = 1;
int hp;
int mana = 1;
bool t_placed = 0;
bool p_placed = 0;
int r_placed = 0;
int dlvl = 1;
int turns = 1;
int m_defeated = 0;
char state[5] = {0}; // conf, pois, blee etc

// sneakiness
// stasis
// blow surroundings
// dig a hole in floor?

// traps?

struct monsters
{
	int y;
	int x;
	int lvl;
	int type;
	bool awake;
};

struct monsters monster[20];

int dungeon_draw(int rows, int cols, char (* map)[cols], char (* obj)[cols])
{
	// clear all except 1st row
	move(1, 0);
	clrtobot();

    // draw the dungeon
    for (int y = 0; y <= rows; y++)
    {
        for (int x = 0; x <= cols; x++)
        {
            if (y == rows)
                mvaddch(y,x,' ');
			// objects can be visible only on empty floor
            else if (map[y][x] == ' ')
			{
				if (obj[y][x] == '>')
				{
					attron(A_BOLD);
					mvaddch(y,x,'>');
					attroff(A_BOLD);
				}
				// traps (can be seen only in close distance)
				else if (obj[y][x] == '^' && ((y > py - 5 && x > px - 5) &&
				(y < py + 5 && x < px + 5)))
				{
					attron(A_BOLD | COLOR_PAIR(YELLOW));
					mvaddch(y,x,'^');
					attroff(A_BOLD | COLOR_PAIR(YELLOW));
				}
				else
					mvaddch(y,x,' ');
			}
            else if (map[y][x] == '%' && y != 0)
                mvaddch(y,x,'%');
            else if (map[y][x] == '#')
                mvaddch(y,x,'#');
            else if (map[y][x] == '~')
			{
				attron(COLOR_PAIR(RED));
                mvaddch(y,x,'~');
				standend();
			}
			// monsters - visible only at certain distance
            else if (map[y][x] == 't' && ((y > py - 5 && x > px - 5) &&
				(y < py + 5 && x < px + 5)))
            {
				for (int m = 0; m < 10 + dlvl / 2; m++)
				{
					if (monster[m].y == y && monster[m].x == x)
					{
						if (monster[m].lvl < dlvl / 2 + 2)
						{
							attron(COLOR_PAIR(RED));
								mvaddch(y,x,monster[m].type);
						}
						else if (monster[m].lvl < dlvl + 2)
						{
							attron(COLOR_PAIR(YELLOW));
								mvaddch(y,x,monster[m].type);
						}
						else
							mvaddch(y,x,monster[m].type);
					}
					
					standend();  // turn off above attron()
				}	
			}
        }
    }

	// draw p
	attron(A_BOLD);
		mvaddch(py,px,'@');
	attroff(A_BOLD);
	
	mvprintw(rows, 0, "HP: %d   Att: %d   Mana: %d \t\t Dlvl: %d", hp, att, mana, dlvl);
	
	return 0;
}

int monster_turn(int cols, char (* map)[cols])
{
	int dist_y, dist_x;
	
	for (int m = 0; m < 10 + dlvl / 2; m++)
	{
		if (monster[m].lvl < 1)
			continue;
		
		dist_y = abs(monster[m].y - py);
		dist_x = abs(monster[m].x - px);
		
		// check sleep
		if (!(rand() % (dlvl + 1)))
			;
		else if (dist_y < dlvl + 3 && dist_x < dlvl + 3)
			monster[m].awake = 1;
		
		if (monster[m].awake == 0)
			continue;
		
		int dir_y = monster[m].y;
		int dir_x = monster[m].x;

		// test 4-side movement 4 sides
		if (dist_y > dist_x)
		{
			if (dir_y > py)
				dir_y -= 1;
			else
				dir_y += 1;
		}
		else
		{
			if (dir_x > px)
				dir_x -= 1;
			else
				dir_x += 1;
		}

		// if failed - try diagonal (corners)
		if (map[dir_y][dir_x] == '#' || map[dir_y][dir_x] == '%')
		{
			dir_y = monster[m].y;
			dir_x = monster[m].x;
			
			if (dir_y > py)
				dir_y -= 1;
			else
				dir_y += 1;

			if (dir_x > px)
				dir_x -= 1;
			else
				dir_x += 1;
		}
		
		// if still failed - move to closes to player empty tile (invert)
		if (map[dir_y][dir_x] == '#' || map[dir_y][dir_x] == '%')
		{
			dir_y = monster[m].y;
			dir_x = monster[m].x;
			
			if (dist_y > dist_x)
			{
				if (dir_x > px)
					dir_x -= 1;
				else
					dir_x += 1;
			}
			else
			{
				if (dir_y > py)
					dir_y -= 1;
				else
					dir_y += 1;
			}
		}
		
		if (dist_y < 2 && dist_x < 2)
		{
			int dmg = ((monster[m].type - 96) + dlvl) / 2;
			if (rand() % 2)
			{
				hp -= dmg;
				if (dmg > dlvl / 2 + 1)
					mvprintw(0, 0, " The '%c' hits you hard.", monster[m].type);
				else
					mvprintw(0, 0, " The '%c' hits you.", monster[m].type);
				if (hp < 1)
					return monster[m].type;
			}
			else
			{
				if (rand() % 2)
					mvprintw(0, 0, " The '%c' missed you.", monster[m].type);
				else
					mvprintw(0, 0, " You dodge the attack.");
			}
		}
		else if (map[dir_y][dir_x] == ' ' && (dir_y != py && dir_x != px))
		{
			map[monster[m].y][monster[m].x] = ' ';
			monster[m].y = dir_y;
			monster[m].x = dir_x;
			map[monster[m].y][monster[m].x] = 't';
		}
	}
	
	return 0;
}

int battle(int cols, char (* map)[cols], int dir_y, int dir_x)
{
	for (int m = 0; m < 10 + dlvl / 2; m++)
	{
		if (dir_y == monster[m].y && dir_x == monster[m].x)
		{
			bool sleeped = 0;
			// wake up
			if (monster[m].awake == 0)
			{
				monster[m].awake = 1;
				sleeped = 1;
			}

			// hit
			monster[m].lvl -= att;

			// if was asleep and we didn't defeat it yet
			if (sleeped && monster[m].lvl > 0)
			{
				bool was_almost_dead = 0;

				mvprintw(0, cols / 2, ">> You hit sleeping '%c' hard!     ", monster[m].type);
				if (monster[m].lvl < (dlvl * 9) / 8)
					was_almost_dead = 1;
				monster[m].lvl -= att;
				// give monster a chance to survive critical hit
				if (!was_almost_dead && monster[m].lvl < 1 && rand() % (dlvl + 1))
					monster[m].lvl = 1;
			}
			else
				mvprintw(0, cols / 2, ">> You hit '%c'.                   ", monster[m].type);

			// monster defeated
			if (monster[m].lvl < 1)
			{
				m_defeated++;

				if (att == 1 && (!(rand() % 3) || m_defeated > 2))
				{
					att++;
					mana++;
					hp += rand() % 10 + 1;
				}
				else if (rand() %
  				 (((monster[m].type - 96) + dlvl) / 2) )
				{
					hp += rand() % 10 + 1;
					// add race dependancy...
					if (rand() % 2)
						mana++;
					else
						att++;
				}

				if (m_defeated > 9 && !(m_defeated % 10))
					att++;
				
				mvprintw(0, cols / 2, ">> You defeat '%c'.                ", monster[m].type);

				// wipe monster from DB
				map[dir_y][dir_x] = ' ';
				monster[m].y = 0;
				monster[m].x = 0;
			}
		}
	}
	
	return 0;
}

int p_action(int c, int rows, int cols, char (* map)[cols], char (* obj)[cols])
{
	if (c == 0)
		return 0;
	
	int dir_y = py, dir_x = px;
	
	// remap macro for movement
	if      (c == 'w' || c == 'k')
		c = KEY_UP;
	else if (c == 's' || c == 'j')
		c = KEY_DOWN;
	else if (c == 'a' || c == 'h')
		c = KEY_LEFT;
	else if (c == 'd' || c == 'l')
		c = KEY_RIGHT;
	
	// confusion
	if (!strcmp(state, "conf"))
	{
		if (c == KEY_UP || c == KEY_DOWN || c == KEY_LEFT || c == KEY_RIGHT)
		{
			int rng = rand() % 4;

			if (rng == 0)
				c = KEY_UP;
			else if (rng == 1)
				c = KEY_DOWN;
			else if (rng == 2)
				c = KEY_LEFT;
			else if (rng == 3)
				c = KEY_RIGHT;
		}
	}
	
	if      (c == KEY_UP)
		dir_y--;
	else if (c == KEY_DOWN)
		dir_y++;
	else if (c == KEY_LEFT)
		dir_x--;
	else if (c == KEY_RIGHT)
		dir_x++;
	// go down
	else if ((c == '>' || c == '\r' || c == '\n' || c == KEY_ENTER || c == ' ') && obj[py][px] == '>')
	{
		t_placed = 0;
		p_placed = 0;
		r_placed = 0;
		return 1;
	}
	// teleport
	else if (mana > 0 && (c == '1' || c == 'q' || c == 't'))
	{
		mana--;

        do
        {
            py = rand() % rows;
            px = rand() % cols;
        }
        while (map[py][px] != ' ' && obj[py][px] == ' ');

		return 2;
	}
	// heal
	else if (mana > 1 && (c == '2' || c == 'e' || c == 'y'))
	{
		mana -= 2;
		hp += rand() % dlvl + 5;
		if (hp > dlvl * 10)
			hp = dlvl * 10;
		mvprintw(0, cols / 2, ">> You heal yourself."); 
		return 0;
	}
	// dig
	else if (hp > 1 && (c == '3' || c == 'r' || c == 'u'))
	{
		c = getch();
		hp -= 1;

		if      (c == KEY_UP || c == 'w' || c == 'k')
			dir_y--;
		else if (c == KEY_DOWN || c == 's' || c == 'j')
			dir_y++;
		else if (c == KEY_LEFT || c == 'a' || c == 'h')
			dir_x--;
		else if (c == KEY_RIGHT || c == 'd' || c == 'l')
			dir_x++;
		
		if (map[dir_y][dir_x] == '#')
			map[dir_y][dir_x] = ' ';
		
		return 0;
	}
	// wall
	else if (mana > 2 && (c == '3' || c == 'z' || c == 'i'))
	{
		c = getch();
		mana -= 3;

		if      (c == KEY_UP || c == 'w' || c == 'k')
			dir_y--;
		else if (c == KEY_DOWN || c == 's' || c == 'j')
			dir_y++;
		else if (c == KEY_LEFT || c == 'a' || c == 'h')
			dir_x--;
		else if (c == KEY_RIGHT || c == 'd' || c == 'l')
			dir_x++;
		
		if (map[dir_y][dir_x] == ' ')
			map[dir_y][dir_x] = '#';
		
		return 0;
	}

	if (map[dir_y][dir_x] == '~')
	{
		// win
		return 3;
	}

	if (map[dir_y][dir_x] == ' ')
	{
		py = dir_y;
		px = dir_x;
	}
	else if (map[dir_y][dir_x] == 't')
		battle (cols, map, dir_y, dir_x);
	
	return 0;
}

int spawn_creatures(int rows, int cols, char (* map)[cols])
{
    if (!t_placed) // aka: t_placed == 0
    {
		int my, mx;
		int boss = 0;
		
		for (int m = 0; m < 10 + dlvl / 2; m++)
		{
			do
			{
				my = rand() % rows;
				mx = rand() % cols;
			}
			while (map[my][mx] != ' ');
			
			monster[m].y = my;
			monster[m].x = mx;

			// lvl
			monster[m].lvl = rand() % dlvl + 2;
			
			if (dlvl == 1 && !(rand() % 5))
				monster[m].lvl = 1;
			
			if (rand() % 2)
				monster[m].lvl = dlvl + 2;

			// type
			if ((dlvl == 13 || dlvl == 14) && boss != 9)
			{
				monster[m].type = 'Z';
				monster[m].lvl = 666;
				monster[m].awake = 1;
				boss++;
			}
			else
			{
				monster[m].type = rand() % (dlvl + 1) + 97;
				if (rand() % 2 && dlvl != 1)
					monster[m].type += 1;
				monster[m].awake = 0;
			}

			map[monster[m].y][monster[m].x] = 't';
		}			

        t_placed = 1;
    }

    if (!p_placed)
    {
		int dist_x;
		int dist_y;
		
        do
        {
            py = rand() % rows;
            px = rand() % cols;
			
			// distance from stairs
			dist_y = abs(py - sy);
			dist_x = abs(px - sx);
			
			if (map[py][px] == ' ' && dist_y > 7 && dist_x > 7)
				break;
        }
        while (1);
		
        p_placed = 1;
    }
	
	return 0;
}

int spawn_objects(int rows, int cols, char (* map)[cols], char (* obj)[cols], int new_lvl)
{
	if (new_lvl == 1 || turns == 1)
	{
        // fill dungeon with empty obj
        for (int y = 0; y <= rows; y++)
        {
            for (int x = 0; x <= cols; x++)
            {              
                obj[y][x] = ' ';
            }
        }
		
		// staircase gen
		int final_lvl = 13 + rand() % 2;
		
		if (dlvl != final_lvl)
		{
			do
			{
				sy = rand() % rows;
				sx = rand() % cols;
			}
			while (map[sy][sx] != ' ');
			obj[sy][sx] = '>';
		}
		// lava (not obj, but terrain.. anyway, lets put it there :)
		else
		{
			int ly, lx;
			do
			{
				ly = rand() % rows;
				lx = rand() % cols;
			}
			while (map[ly][lx] != ' ');
			map[ly][lx] = '~';
			
			for (int i = 0; i < 33; i++)
			{
				// if lava not crossed regular terrain
				if (map[ly][lx] == '%')
					i = 32;

				if (rand() % 2)
					map[ly--][lx] = '~';
				else
					map[ly][lx++] = '~';
			}
		}
	}

	// each turn there is a chance to spawn trap (the faster you run away - the better)
	if ((rand() % (16 - dlvl)))
	{
		int y, x;

		do
		{
			y = rand() % rows;
			x = rand() % cols;
		}
		while (map[y][x] != ' ' && obj[y][x] != ' ');
		
		if (rand() % 2)
			obj[y][x] = '^';
	}
	
	return 0;
}

int dungeon_gen(int rows, int cols, char (* map)[cols])
{
    if (!r_placed)
    {
        int ry, rx; // room coords
        int r_size_y, r_size_x; // room size
        int r_center_y, r_center_x;
        int r_old_center_y, r_old_center_x;
        int room_num = rand() % 5 + 5;
        bool collision;

        // fill dungeon with walls and borders
        for (int y = 0; y <= rows; y++)
        {
            for (int x = 0; x <= cols; x++)
            {          
                // borders
                if (y == 0 || y == 1 || y == rows - 1 || x == 0 ||
					x == cols || y == rows)
                    map[y][x] = '%';
                // walls
                else            
                    map[y][x] = '#';
            }
        }

        while (r_placed < room_num)
        {
            int try_counter = 0; // number of tries for prototyping
            
            // prototyping the room until no collisions
            do
            {
                collision = 0;
                
                // room coords
                ry = rand() % (rows - 4) + 1;
                rx = rand() % (cols - 4) + 1;
                
                // room sizes
                r_size_y = rand() % 5 + 4;
                r_size_x = rand() % 10 + 8;
                
                try_counter++;
                if (try_counter > 100)
                {
                    ry = rx = 3;
                    r_size_y = r_size_x = 3;
                    break;
                }
                
                // fill DB map with rooms
                for (int y = ry; y <= ry + r_size_y; y++)
                {
                    for (int x = rx; x <= rx + r_size_x; x++)
                    {
                        if (map[y][x] == '%' || map[y][x] == ' ' ||
                            map[y + 2][x] == ' ' || map[y - 2][x] == ' ' ||
                            map[y][x + 2] == ' ' || map[y][x - 2] == ' ')
                        {
                            collision = 1;
                            y = ry + r_size_y; // exit upper loop..
                            break; // ..exit from current loop
                        }
                    }
                }
            }
            while (collision == 1);
            
            // fill DB map with rooms
            for (int y = ry; y <= ry + r_size_y; y++)
            {
                for (int x = rx; x <= rx + r_size_x; x++)
                {
                    if (map[y][x] == '%')
                    {
                        y = ry + r_size_y; // exit upper loop..
                        break; // ..exit from current loop
                    }
                    else
                        map[y][x] = ' ';
                }
            }
            
            r_placed++;
            
            // corridors
            if (r_placed > 1)
            {
                r_old_center_y = r_center_y;
                r_old_center_x = r_center_x;
            }

            r_center_y = ry + (r_size_y / 2);
            r_center_x = rx + (r_size_x / 2);
            
            if (r_placed > 1)
            {
                int path_y;

                for (path_y = r_old_center_y; path_y != r_center_y; )
                {
                    map[path_y][r_old_center_x] = ' ';
                    
                    if (r_old_center_y < r_center_y)
                        path_y++;
                    else if (r_old_center_y > r_center_y)
                        path_y--;
                }
                
                for (int x = r_old_center_x; x != r_center_x; )
                {
                    map[path_y][x] = ' ';
                    
                    if (r_old_center_x < r_center_x)
                        x++;
                    else if (r_old_center_x > r_center_x)
                        x--;
                }
            }
        }
    }
	
	return 0;
}

int game_loop(int c, int rows, int cols, char (* map)[cols], char (* obj)[cols])
{
	int new_lvl = 0;
	int killer = 0;
    srand(time(NULL));
	move(0,0); clrtoeol(); // clear 1st line for messages

	new_lvl = p_action(c, rows, cols, map, obj); // +battle()
	
	killer = monster_turn(cols, map);
    
	dungeon_gen(rows, cols, map);
	
	spawn_objects(rows, cols, map, obj, new_lvl);

	spawn_creatures(rows, cols, map);

	dungeon_draw(rows, cols, map, obj);

	// new lvl
	if (new_lvl == 1)
	{
		dlvl++;
		hp += dlvl;
		dungeon_gen(rows, cols, map);
		spawn_creatures(rows, cols, map);
		spawn_objects(rows, cols, map, obj, new_lvl);
		dungeon_draw(rows, cols, map, obj);
	}
	// teleport
	else if (new_lvl == 2)
	{
		mvprintw(0, 0, " You teleported away.");
		new_lvl = 0;
	}
	// win
	else if (new_lvl == 3)
	{
		while (1)
		{
			clear();
			printw("\n\n\n\n \t\t\t\tThe Ring is destroyed!\n\n"
			"\tEagles came and brought you away from the fury of the Flame River!\n\n\n");
			attron(A_BOLD | COLOR_PAIR(CYAN));
			printw("\t\t\t\t     YOU WIN!\n\n");
			attroff(A_BOLD | COLOR_PAIR(CYAN));
			printw("\tLevel reached: %d\n"
			"\tMonsters defeated: %d\n"
			"\tTurns: %d\n\n"
			"\tAttack: %d\n"
			"\tMana: %d\n"
			"\n\n\tPress 'n' to start a new game or 'ESC' to exit.", dlvl, m_defeated, turns, att, mana);
			c = getch();
			if (c == 'n')
			{
				new_lvl = 1;
				break;
			}
			else if (c == 27)
				return c;
		}
	}
	
	// player stept over trap
	if (obj[py][px] == '^')
	{
		attron(A_BOLD | COLOR_PAIR(YELLOW));
		mvprintw(rows, cols - 10, "[Trap]");
		attroff(A_BOLD | COLOR_PAIR(YELLOW));
		
		if (rand() % 2)
		{
			mvprintw(0, 0, " You've stepped into a trap... dart hits you!");
			hp -= dlvl / 2 + 1;
			if (hp < 1)
				killer = '^';
		}
		else
		{
			mvprintw(0, 0, " You've stepped into a trap... you are confused!");
			strncpy(state, "conf\0", 5);
		}
	}
	
	// process state
	if (!strcmp(state, "conf"))
	{
		if (rand() % 3)
		{
			strncpy(state, "\0\0\0\0\0", 5);
			mvprintw(rows, cols - 20, "    ");
		}
		else
			mvprintw(rows, cols - 20, "conf");
	}
	
	// RIP
	if (hp < 1)
	{
		while (1)
		{
			clear();
			attron(A_BOLD | COLOR_PAIR(RED));
			mvprintw(rows / 2 - 3, cols / 2 - 10, "You were captured by %c\n\n\n", killer);
			attroff(A_BOLD | COLOR_PAIR(RED));
			printw("\tLevel reached: %d\n"
			"\tMonsters defeated: %d\n"
			"\tTurns: %d\n\n"
			"\tAttack: %d\n"
			"\tMana: %d\n"
			"\n\n\tPress 'n' to start a new game or 'ESC' to exit.", dlvl, m_defeated, turns, att, mana);
			c = getch();
			if (c == 'n')
				break;
			else if (c == 27)
				return c;
		}
	}
	else
	// regular player input
		c = getch();

	// exit game (ESC)
	if (c == 27)
		return c;

	// turn count
	turns++;
	
	// hunger
	if (!(turns % 50 - (dlvl * 2)) && hp > 1)
		hp--;

	// start over by demand
	if (c == 'n')
	{
		att = 1;
		hp = 10 + rand() % 2;
		mana = 1;
		t_placed = 0;
		p_placed = 0;
		r_placed = 0;
		turns = 1;
		m_defeated = 0;
		c = 0;
		dlvl = 1;
	}

    return c;
}

int main(void)
{
    int c = 0; // input
    int rows, cols;
  
    initscr(); // init curses
	start_color();
	use_default_colors();

	init_pair(RED, COLOR_RED, COLOR_BLACK);
	init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
	
    keypad(stdscr, 1); // allow ARROWS, F1-F12
    
    noecho(); // don't show user input
    curs_set(0); // hide blinking cursor
    
    // adaptive way of the screen size:
	//getmaxyx(stdscr,rows,cols);
	// but this particular game designed to be played at tiny term:
	rows = 23;
	cols = 80;
    
    char map[rows][cols]; // 0..119
    char obj[rows][cols]; // 0..119

	printw("\n\n\n\n");
	attron(A_BOLD);
	printw("\t\t\t\t       Roglik");
	attroff(A_BOLD);
	
	printw("\n\n\n\tBring the Ring to the ");

	attron(A_BOLD | COLOR_PAIR(RED));
	printw("River of Flame");
	attroff(A_BOLD | COLOR_PAIR(RED));

	printw(" and try to get rid of it...");
	
	printw("\n\n\n\tControls:\n\n"
	"\tArrows/wasd/hjkl - move and attack\n"
	"\tSpace/Enter/> - go to the next level \n"
	"\t1/q/t - teleport \n"
	"\t2/e/y - heal \n"
	"\t\'n\' - start a new game\n"
	"\t\'ESC\' - exit the game\n\n\n"
	"\tYou level up HP and Attack by defeating monsters.\n"
	"\tYour loose satiation (HP) and gain Mana after some time.\n");
	
	c = getch();
	
	// char creation
	hp = 10 + rand() % 2;

	if (c == 27) // 27 'ESC'
	{
		endwin();
		return 0;
	}

    do
    {
		// rows-1 cause map[][] 0.. 119
        c = game_loop(c, rows - 1, cols - 1, map, obj);
		if (c == 27)
		{
			endwin();
			return 0;
		}
    }
    while (1);
}