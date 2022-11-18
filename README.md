# roglik
Roglik - minimalistic roguelike (coffeebreak roguelike) game designed to provide 5-7 minute game sessions till win or yasd. The map surroundings are known to your character (to make gameplay more fast-paced), but creatures and object aren't.

--

How to build:

Windows:
1) install [MinGW](https://osdn.net/projects/mingw/) and [PDCurses](https://pdcurses.org/). How to do it: [video tutorial](https://www.youtube.com/watch?v=2JfKINdMwAo&list=PLm4uLihLZ5WwAVKysXIfENsYLcqxNLjbt&index=27)
2) `gcc -Wall -Wextra main.c -o roglik.exe -lpdcurses -static`

Linux:
1) install NCurses
2) `gcc main.c -o roglik.bin -lcurses`

--

Initially Roglik was made for tutorial purposes as part of C language tutorial at my channel: https://youtube.com/ITPhD , but eventually I've decided to make it 'playable'.. so now it's a tiny game :) I called it 'Roglik' as it's very simple.

Source code (+ binary for Windows/Linux): https://github.com/igroglaz/roglik

Roglik is the game in public domain, no credits, copyrights etc. Enjoy! :D

Now.. There are detailed video lessons - how such game could be written with a full explanation:

0) https://youtu.be/mQy6ajEqEK0 - most basic tutorial.. how to make basic rlg template with Conio.h . Might be useful for beginners who don't know how to begin.. :)

1) https://youtu.be/u4RQSbiCGL8 - we move to NCurses and implement basic movement

2) https://youtu.be/diOuFclVlY0 - we make room and walls

3) https://youtu.be/0TzUzB_S-bs - random generation of character and room

4) https://youtu.be/EECRMExOhxQ - generate NPC and random rooms

5) https://youtu.be/htNYvlz6aVc - switching to PDCurses and make proper room generation

6) https://youtu.be/cjcK1-y628g - create corridors in dungeon

7) https://youtu.be/1Eev0pNhmpE - C language structures for monsters

8) https://youtu.be/cLMOhvdS8cU - staircases and event messages

9) https://youtu.be/lD1oVG96j44 - monsters diversity (adding moar monster's types)

10) https://youtu.be/9c5b0ypewkg - simple AI of monsters. Movement and attack

Playlist with all these vids: https://www.youtube.com/playlist?list=PLm4uLihLZ5Ww6jW7CebZpLppLSGS_IZhO

In case if you need basics of C language (as roguelike tutorial assumes that you know some minimum), there is also a full C language playlist where I explain stuff from scratch: https://www.youtube.com/playlist?list=PLm4uLihLZ5WwAVKysXIfENsYLcqxNLjbt

Cheers!