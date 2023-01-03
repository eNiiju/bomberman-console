# BomberTTY

BomberTTY is a terminal based simple bomberman clone entirely written in C, with
help of the ncurses library.

You can play with your friends with up to 4 players, each on a different computer
and connected via SSH.

You can choose between 3 different maps, but you can also create your own with ease.

![Screenshot](./assets/screenshot.png)

# Context of the project

This project was made as a school project, which goal was to create a terminal
game using the system programming notions learnt during the lessons :

This game uses threads, mutexes, semaphores and message queues.

# How to play

1. Copy the repository on your computer

```
git clone https://github.com/eNiiju/Bombertty.git
```

2. Compile the project

```
make
```

3. Run the server with the number of players and the map you want to play on

```
cd server/
./server <number_of_players> <path_to_map>
```

4. Run the client on each computer you want to play on

```
cd client/
./client
```

If you are on a different computer, first ssh into the computer running the server, then run the client

```
ssh <username>@<server_ip>
cd <path_to_folder>/client/
./client
```

5. Enjoy !

# Create your own map

To create your own map, you only need a file.
You can name it anything you like.

Inside this file, you'll need to write a character for each tile of the map :
- `#` for a wall
- ` ` for an empty tile (space)
- `/` for a breakable wall
- `1` for a player spawn point (You must set 4 spawnpoints)

The map **must** be rectangle and not bigger than 50x50 !

*Note: The player spawnpoints will be empty tiles during the game.*

## Example

```
###############
#1    // /   2#
# # # # # # # #
# / /    / /  #
# # # # # # # #
#   ///     //#
# # # #/#/#/# #
#//  //  //  /#
# # # # #/# # #
#  /        //#
# # # # # # # #
# ///      /  #
# # # # # # # #
#3  // /  /  4#
###############
```
> bbtty_default
