# roglik
Roglik - minimalistic roguelike (coffeebreak roguelike) game designed to provide 5-7 minute game sessions till win or yasd.

It's pretty raw pre-alpha, but you can already give it a try :)

To compile yourself:
1) install [MinGW](https://osdn.net/projects/mingw/) and [PDCurses](https://pdcurses.org/). How to do it: [video tutorial](https://www.youtube.com/watch?v=2JfKINdMwAo&list=PLm4uLihLZ5WwAVKysXIfENsYLcqxNLjbt&index=27)
2) `gcc -Wall -Wextra main.c -o main.exe -lpdcurses`
(use `-static` if you want to include lib to binary)

Initially Roglik was made for tutorial purposes as part of C language tutorial at my channel: https://youtube.com/ITPhD , but eventually I've decided to make it 'playable'. Check full C language tutorial playlist to understand how it works:

https://www.youtube.com/playlist?list=PLm4uLihLZ5WwAVKysXIfENsYLcqxNLjbt

Cheers!
