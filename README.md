# 2048-Game
  A console-based 2048 game developed in C with ncurses, featuring intuitive controls, real-time scoring, and enhanced functionality like automatic moves, recent command history, and live date and time display.

  I implemented the functions one by one, testing each individually to ensure they worked correctly.

  *Main Menu*: The menu setup is done in the "menu" function, while the title is created in the "titlu" function. The menu has four options, navigable using the up/down arrow keys or the W/S keys, and options can be selected by pressing Enter. The currently selected option is highlighted.

  *New Game*: The first option is New Game. The "new_game" function starts a new game, initializing two cells with values from the set {2, 4}, with the rest set to 0. For displaying the game, I implemented the "show_game" function, which shows the game menu, randomly generates a 2 or 4 in an available position, and handles win/loss conditions. The game menu includes the game board, score, possible valid moves, date and time, and, as an added feature, a list of recent commands. I also implemented the “afisare_data” function to format and display the date and time. The New Game option continues into the "resume" function, where the game itself is played. This can be done using either the arrow keys or the WASD keys. If no command is entered within 5 seconds, the “mutare” function automatically performs the most efficient move in terms of freeing cells. If multiple moves free the same number of cells, they are prioritized based on the score achieved. This function also checks for move validity. To handle movement on the game board, I implemented four functions, “key_up,” “key_down,” “key_left,” and “key_right,” which are called whenever a move is made or its validity is tested. Throughout the game, pressing the q/Q key returns to the menu.

  *Resume*: The second option is Resume. If an unfinished game exists, it resumes according to the rules described above. If no such game exists, a window with an explanatory message appears, then returns to the menu.

  *Rules*: The third option is Rules. The “rules” function displays a window with the essential game rules. You can return to the menu by pressing q/Q.

  *Quit*: The final option is Quit, which ends the session and closes the program.

  Color initialization is handled in the "ipair" function. To facilitate the implementation, I created a structure containing the main data related to the game.

  Both the documentation review for the library and the game implementation spanned several days, totaling approximately 12-15 hours to complete the application.
