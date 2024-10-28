# 2048-Game

A console-based 2048 game developed in C with ncurses, featuring intuitive controls, real-time scoring, automatic moves, recent command history, and live date and time display.

## Overview

This game follows the classic 2048 structure, enhanced with responsive menu navigation, real-time updates, and additional interactive features. Each function was carefully implemented and individually tested to ensure smooth gameplay.

## Features

- **Main Menu**: Set up in the "menu" function with a title created by "titlu." The menu includes four options navigable by arrow keys or W/S keys, selectable with Enter. The current selection is highlighted.
  
- **New Game**: Initiates a new game in the "new_game" function, initializing two cells with values from {2, 4}. The "show_game" function displays the game board, score, valid moves, and recent commands, alongside date and time via the “afisare_data” function. 
  - **Auto-Move**: After 5 seconds of inactivity, the “mutare” function executes the optimal move.
  - **Directional Movement**: Implemented with “key_up,” “key_down,” “key_left,” and “key_right” functions for move validation and execution.
  - **Return to Menu**: Pressing q/Q at any time returns to the main menu.

- **Resume**: Continues an active game if one exists. If no game is in progress, displays a message and returns to the main menu.

- **Rules**: The "rules" function provides a window with essential game rules, accessible via q/Q.

- **Quit**: Ends the session and closes the game.

## Project Structure

- **ipair**: Handles color initialization for the game.
- **Data Structure**: Designed to store essential game-related data for easier management.

## Development Notes

The implementation and library documentation review took approximately 12-15 hours. Each function was tested individually to ensure accuracy and functionality.

