import tkinter as tk
from tkinter import messagebox
import pygame

class BaseTicTacToe:
    def __init__(self, root, client):
        self.root = root
        self.client = client
        
        # MQTT topics
        self.MQTT_TOPIC_NFC = "game/nfc/reader"
        self.MQTT_TOPIC_PLAYERS = "game/players"
        self.MQTT_TOPIC_STATUS = "game/status"
        self.MQTT_TOPIC_BOARD = "game/board"
        
        # Common attributes
        self.board = [["" for _ in range(3)] for _ in range(3)]
        self.game_over = False
        self.cells = [[None for _ in range(3)] for _ in range(3)]
        
        # Player configuration
        self.player1_name = ""
        self.player2_name = ""
        self.current_player = "X"
        
        # NFC Tags and Reader positions
        self.X_TAGS = {"8916C201", "16B10F02", "3EA6B001", "2E3EB501", "296DB701"}
        self.READER_POSITIONS = {
            "reader1": (0, 0), "reader2": (0, 1), "reader3": (0, 2),
            "reader4": (1, 0), "reader5": (1, 1), "reader6": (1, 2),
            "reader7": (2, 0), "reader8": (2, 1), "reader9": (2, 2)

        }
        
        # GUI setup
        self.status_label = tk.Label(root, text="", font=("Arial", 20))
        self.status_label.grid(row=3, column=0, columnspan=3, pady=10)
        
        self.setup_board()
        self.root.after(100, self.get_player_names)
        self.reset_game()

    def setup_board(self):
        for row in range(3):
            for col in range(3):
                label = tk.Label(self.root, text="", font=("Arial", 40), width=4, height=2, relief="solid", bg="white")
                label.grid(row=row, column=col, padx=2, pady=2)
                self.cells[row][col] = label
        
        reset_button = tk.Button(self.root, text="Reset game", font=("Arial", 14), command=self.reset_game)
        reset_button.grid(row=4, column=0, columnspan=3, pady=10)

    def send_board_state(self):
        board_state = ""
        for row in self.board:
            for cell in row:
                board_state += cell if cell else "."
        self.client.publish(self.MQTT_TOPIC_BOARD, board_state)

    def update_status(self, msg):
        self.status_label.config(text=msg)
        self.client.publish(self.MQTT_TOPIC_STATUS, msg)

    def highlight_possible_moves(self):
            for row in range(3):
                for col in range(3):
                    if self.board[row][col] == "":
                        if self.current_player == "X":
                            self.cells[row][col].config(bg="#C8DAFF")  # Mörkblå för X
                        elif self.current_player == "O":
                            self.cells[row][col].config(bg="#FFC8C8")  # Röd för O

                    else:
                        self.cells[row][col].config(bg="white")  # Vit för redan spelade celler

    def check_winner(self):
        lines = [
            self.board[0], self.board[1], self.board[2],
            [self.board[0][0], self.board[1][0], self.board[2][0]],
            [self.board[0][1], self.board[1][1], self.board[2][1]],
            [self.board[0][2], self.board[1][2], self.board[2][2]],
            [self.board[0][0], self.board[1][1], self.board[2][2]],
            [self.board[0][2], self.board[1][1], self.board[2][0]],
        ]
        for line in lines:
            if line.count(line[0]) == 3 and line[0] != "":
                return line[0]
        if all(cell != "" for row in self.board for cell in row):
            return "draw"
        return None

    def end_game(self, winner):
        self.game_over = True
        pygame.mixer.music.stop()
        pygame.mixer.Sound("win.mp3").play()

        if winner == "draw":
            self.update_status("It's a draw!")
            # Flash all cells white for draw
            for row in range(3):
                for col in range(3):
                    self.cells[row][col].config(bg="white")
                    self.client.publish("game/led_state", f"{row},{col},flash_white")
        
        else:
            winner_name = self.player1_name if winner == "X" else self.player2_name
            self.update_status(f"{winner_name} won!")
            # Flash winning cells (in their respective colors)
            for row in range(3):
                for col in range(3):
                    if winner == "X":
                        self.cells[row][col].config(bg="#C8DAFF") # light blue
                        self.client.publish("game/led_state", f"{row},{col},flash_blue")
                    else:
                        self.cells[row][col].config(bg="#FFCCCB") # light red
                        self.client.publish("game/led_state", f"{row},{col},flash_red")
        
        # Start the flashing animation
        self.flash_count = 0
        self.flash_winner(winner)
        
    def flash_winner(self, winner):
        if self.flash_count < 10:
            if self.flash_count % 2 == 0:
                # Turn all cells to winner's color
                for row in range(3):
                    for col in range(3):
                        if winner == "draw":  # Add draw case here
                            self.cells[row][col].config(bg="white")
                            self.client.publish("game/led_state", f"{row},{col},flash_white")
                        elif winner == "X":
                            self.cells[row][col].config(bg="#C8DAFF")
                            self.client.publish("game/led_state", f"{row},{col},flash_blue")
                        else:
                            self.cells[row][col].config(bg="#FFC8C8")
                            self.client.publish("game/led_state", f"{row},{col},flash_red")
            else:
                # Turn all cells to white
                for row in range(3):
                    for col in range(3):
                        self.cells[row][col].config(bg="white")
                        self.client.publish("game/led_state", f"{row},{col},flash_white")
            
            self.flash_count += 1
            self.root.after(500, lambda: self.flash_winner(winner))  # Flash every 500ms

            
    def reset_game(self):
        self.board = [["" for _ in range(3)] for _ in range(3)]
        self.current_player = "X"
        self.game_over = False

        # Light up all cells in blue for X's turn
        for row in range(3):
            for col in range(3):
                self.cells[row][col].config(text="", bg="#C8DAFF")  # Light blue
                self.client.publish("game/led_state", f"{row},{col},available_x")  # Blue LEDs

        pygame.mixer.music.load("intro.mp3")
        pygame.mixer.music.play()
        self.update_status(f"{self.player1_name} (X) starts!")
        self.highlight_possible_moves()
        self.send_board_state()

        
class TicTacToe(BaseTicTacToe):
    def __init__(self, root, client):
        super().__init__(root, client)
        self.O_TAGS = {"77700E02", "7A1CB201", "4A78C901", "91A30D02", "DB97B101"}
        
    def get_player_names(self):
        self.root.withdraw()  # Hide main window
        
        self.player1_name = tk.simpledialog.askstring("Player 1", "Enter name for Player 1 (X):", parent=self.root)
        if self.player1_name is None or self.player1_name.strip() == "":
            self.player1_name = "Player 1"
        
        self.player2_name = tk.simpledialog.askstring("Player 2", "Enter name for Player 2 (O):", parent=self.root)
        if self.player2_name is None or self.player2_name.strip() == "":
            self.player2_name = "Player 2"
        
        # Send player names to LCD ESP32
        self.client.publish(self.MQTT_TOPIC_PLAYERS, f"player1:{self.player1_name}")
        self.client.publish(self.MQTT_TOPIC_PLAYERS, f"player2:{self.player2_name}")
        
        # Send initial board state
        self.send_board_state()
        
        self.root.deiconify() # Show main window again
        self.update_status(f"{self.player1_name} (X) starts!")
        
    # Update the cell when a player makes a move
    def update_cell(self, row, col, player):
        if self.board[row][col] == "" and not self.game_over:
            self.board[row][col] = player
            self.cells[row][col].config(text=player, fg="blue" if player == "X" else "red")
            
            # Update LED state for the taken cell
            self.client.publish("game/led_state", f"{row},{col},taken")
            
            self.send_board_state()  # Send updated board state to LCD esp32

            winner = self.check_winner()
            if winner:
                # Flash LEDs for winner
                for r in range(3):
                    for c in range(3):
                        if winner == "X":
                            self.client.publish("game/led_state", f"{r},{c},flash_blue")
                        elif winner == "O":
                            self.client.publish("game/led_state", f"{r},{c},flash_red")
                        else:  # draw
                            self.client.publish("game/led_state", f"{r},{c},flash_white")
                self.end_game(winner)
            else:
                self.current_player = "O" if self.current_player == "X" else "X"
                current_player_name = self.player1_name if self.current_player == "X" else self.player2_name
                self.update_status(f"Player {current_player_name}'s turn'")
                self.highlight_possible_moves()
                
                # Update LED states for available moves
                for r in range(3):
                    for c in range(3):
                        if self.board[r][c] == "":
                            if self.current_player == "X":
                                self.client.publish("game/led_state", f"{r},{c},available_x")
                            else:
                                self.client.publish("game/led_state", f"{r},{c},available_o")
                
    # Handle incoming MQTT messages
    def handle_message(self, topic, payload):
        if topic == "game/nfc/reader":  # hosseins kod har "rfid/reader" fast min espA kod har "game/nfc/reader"
            try:
                reader, uid = payload.split(":", 1)

                if uid in self.X_TAGS:
                    player = "X"
                elif uid in self.O_TAGS:
                    player = "O"
                else:
                    print("Unknown tag:", uid)
                    return

                # Check if it's the correct player's turn
                if player != self.current_player:
                    print(f"It's {self.current_player}'s turn.")
                    return

                # Check if the game is over
                if self.game_over:
                    print("Game is already over.")
                    messagebox.showwarning("Game Over", "The game is already over. Please reset to play again.")
                    return

                if reader in self.READER_POSITIONS:
                    row, col = self.READER_POSITIONS[reader]
                    # Check if the cell is already occupied
                    if self.board[row][col] != "":
                        print("Cell already occupied")
                       # messagebox.showwarning("Invalid Move", "This cell is already occupied!")
                        return
                    self.update_cell(row, col, player)
                else:
                    print("Unknown reader", reader)
            except Exception as e:
                print("Error in message:", e)
                
class TicTacToeAI(BaseTicTacToe):
    def __init__(self, root, client):
        super().__init__(root, client)
        self.player2_name = "AI"
        self.human_player = "X"
        self.ai_player = "O"
        
    def get_player_names(self):
        self.root.withdraw()  # Hide main window
        
        self.player1_name = tk.simpledialog.askstring("Player 1", "Enter name for Player 1 (X):", parent=self.root)
        if self.player1_name is None or self.player1_name.strip() == "":
            self.player1_name = "Player 1"
        
        # Send player names to LCD ESP32
        self.client.publish(self.MQTT_TOPIC_PLAYERS, f"player1:{self.player1_name}")
        self.client.publish(self.MQTT_TOPIC_PLAYERS, f"player2:{self.player2_name}")
        
        # Send initial board state
        self.send_board_state()
        
        self.root.deiconify() # Show main window again
        self.update_status(f"{self.player1_name} (X) starts!")      
        
    def update_cell(self, row, col, player):
        if self.board[row][col] == "" and not self.game_over:
            self.board[row][col] = player
            self.cells[row][col].config(text=player, fg="blue" if player == "X" else "red")
            
            # Update LED state for the taken cell
            self.client.publish("game/led_state", f"{row},{col},taken")
            
            self.send_board_state()  # Send updated board state to LCD esp32

            winner = self.check_winner()
            if winner:
                # Flash LEDs for winner
                for r in range(3):
                    for c in range(3):
                        if winner == "X":
                            self.client.publish("game/led_state", f"{r},{c},flash_blue")
                        elif winner == "O":
                            self.client.publish("game/led_state", f"{r},{c},flash_red")
                        else:  # draw
                            self.client.publish("game/led_state", f"{r},{c},flash_white")
                self.end_game(winner)
            else:
                self.current_player = self.human_player if self.current_player == self.ai_player else self.ai_player
                if self.current_player == self.ai_player:
                    self.root.after(500, self.ai_move)
                else:
                    self.update_status(f"{self.player1_name}'s turn!")
                    self.highlight_possible_moves() 
                    
                    # Update LED states for available moves
                    for r in range(3):
                        for c in range(3):
                            if self.board[r][c] == "":
                                if self.current_player == self.human_player:
                                    self.client.publish("game/led_state", f"{r},{c},available_x")
                                else:
                                    self.client.publish("game/led_state", f"{r},{c},available_o")

        
    def ai_move(self):
        if self.game_over:
            return

        best_score = float('-inf')
        best_move = None

        for row in range(3):
            for col in range(3):
                if self.board[row][col] == "":
                    self.board[row][col] = self.ai_player
                    score = self.minimax(0, False, float('-inf'), float('inf'))
                    self.board[row][col] = ""
                    if score > best_score:
                        best_score = score
                        best_move = (row, col)

        if best_move:
            row, col = best_move
            self.update_cell(row, col, self.ai_player)
            self.send_board_state()  # Send updated board state after AI move

    def minimax(self, depth, is_maximizing, alpha, beta):
        winner = self.check_winner()
        if winner == self.ai_player:
            return 1
        elif winner == self.human_player:
            return -1
        elif winner == "draw":
            return 0

        if is_maximizing:
            max_eval = float('-inf')
            for row in range(3):
                for col in range(3):
                    if self.board[row][col] == "":
                        self.board[row][col] = self.ai_player
                        eval = self.minimax(depth + 1, False, alpha, beta)
                        self.board[row][col] = ""
                        max_eval = max(max_eval, eval)
                        alpha = max(alpha, eval)
                        if beta <= alpha:
                            break
            return max_eval
        else:
            min_eval = float('inf')
            for row in range(3):
                for col in range(3):
                    if self.board[row][col] == "":
                        self.board[row][col] = self.human_player
                        eval = self.minimax(depth + 1, True, alpha, beta)
                        self.board[row][col] = ""
                        min_eval = min(min_eval, eval)
                        beta = min(beta, eval)
                        if beta <= alpha:
                            break
            return min_eval

    def handle_message(self, topic, payload):
        if topic == self.MQTT_TOPIC_NFC:
            try:
                reader, uid = payload.split(":", 1)

                if uid in self.X_TAGS:
                    player = self.human_player
                else:
                    print("Unknown tag:", uid)
                    return

                if player != self.current_player:
                    print(f"It's {self.current_player}'s turn.")
                    return
                
                if self.game_over:
                    print("Game is already over.")
                    messagebox.showwarning("Game Over", "The game is already over. Please reset to play again.")
                    return

                if reader in self.READER_POSITIONS:
                    row, col = self.READER_POSITIONS[reader]
                    # Check if the cell is already occupied
                    if self.board[row][col] != "":
                        print("Cell already occupied")
                      #  messagebox.showwarning("Invalid Move", "This cell is already occupied!")
                        return

                    self.update_cell(row, col, player)
                else:
                    print("Unknown reader:", reader)
            except Exception as e:
                print("Error in message:", e)