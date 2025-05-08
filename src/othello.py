import tkinter as tk
from tkinter import messagebox
import pygame

class Othello:
    def __init__(self, root, client):
        self.root = root
        self.client = client
        self.board = [["" for _ in range(4)] for _ in range(4)]
        self.current_player = "X"
        self.game_over = False
        self.cells = [[None for _ in range(4)] for _ in range(4)]

        self.X_TAGS = {"8916C201", "16B10F02", "3EA6B001", "2E3EB501", "296DB701"}
        self.O_TAGS = {"77700E02", "7A1CB201", "4A78C901", "91A30D02", "DB97B101"}
        self.READER_POSITIONS = {
            "reader1": (0, 0), "reader2": (0, 1), "reader3": (0, 2), "reader4": (0, 3),
            "reader5": (1, 0), "reader6": (1, 1), "reader7": (1, 2), "reader8": (1, 3),
            "reader9": (2, 0), "reader10": (2, 1), "reader11": (2, 2), "reader12": (2, 3),
            "reader13": (3, 0), "reader14": (3, 1), "reader15": (3, 2), "reader16": (3, 3)
        }

        self.status_label = tk.Label(root, text="", font=("Arial", 20))
        self.status_label.grid(row=5, column=0, columnspan=4, pady=10)

        self.setup_board()
        self.reset_game()

    def setup_board(self):
        for row in range(4):
            for col in range(4):
                label = tk.Label(self.root, text="", font=("Arial", 30), width=4, height=2, relief="solid", bg="white")
                label.grid(row=row, column=col, padx=2, pady=2)
                self.cells[row][col] = label

        reset_button = tk.Button(self.root, text="Reset game", font=("Arial", 14), command=self.reset_game)
        reset_button.grid(row=6, column=0, columnspan=4, pady=10)

    def reset_game(self):
        self.client.publish("game/reset", "reset")

        self.board = [["" for _ in range(4)] for _ in range(4)]
        self.current_player = "X"
        self.game_over = False

        # Startposition (mitten)
        self.board[1][1] = "O"
        self.board[2][2] = "O"
        self.board[1][2] = "X"
        self.board[2][1] = "X"

        for row in range(4):
            for col in range(4):
                text = self.board[row][col]
                color = "blue" if text == "X" else "red" if text == "O" else "black"
                self.cells[row][col].config(text=text, fg=color, bg="white")

        pygame.mixer.music.load("intro.mp3")
        pygame.mixer.music.play()
        self.update_status(f"Spelare {self.current_player}s tur")
        self.highlight_possible_moves()

    def update_status(self, msg):
        self.status_label.config(text=msg)

    def highlight_possible_moves(self):
        moves = self.valid_moves(self.current_player)

        if not moves and not self.game_over:
            # Visa popup om inga drag är möjliga
            messagebox.showinfo("Ingen möjlighet", f"Spelare {self.current_player} har inga möjliga drag.\nTuren går vidare.")
            
            # Byt spelare
            self.current_player = "O" if self.current_player == "X" else "X"
            self.update_status(f"Spelare {self.current_player}s tur")

            # Försök igen med nästa spelare
            moves = self.valid_moves(self.current_player)

            if not moves:
                # Om ingen har drag – avsluta spelet
                winner = self.check_winner()
                self.end_game(winner)
                return

        # Markera möjliga drag visuellt och via MQTT
        for row in range(4):
            for col in range(4):
                if (row, col) in moves:
                    if self.current_player == "X":
                        self.cells[row][col].config(bg="#C8DAFF")
                        self.client.publish("game/possible_moves", f"{row},{col},X")
                    elif self.current_player == "O":
                        self.cells[row][col].config(bg="#FFC8C8")
                        self.client.publish("game/possible_moves", f"{row},{col},O")
                else:
                    self.cells[row][col].config(bg="white")



    def valid_moves(self, player):
        moves = []
        opponent = "O" if player == "X" else "X"

        for row in range(4):
            for col in range(4):
                if self.board[row][col] != "":
                    continue
                for dr, dc in [(-1, -1), (-1, 0), (-1, 1), (0, -1),
                               (0, 1), (1, -1), (1, 0), (1, 1)]:
                    r, c = row + dr, col + dc
                    if self.is_on_board(r, c) and self.board[r][c] == opponent:
                        while self.is_on_board(r, c) and self.board[r][c] == opponent:
                            r += dr
                            c += dc
                        if self.is_on_board(r, c) and self.board[r][c] == player:
                            moves.append((row, col))
                            break
        return moves

    def is_on_board(self, row, col):
        return 0 <= row < 4 and 0 <= col < 4

    def make_move(self, row, col, player):
        opponent = "O" if player == "X" else "X"
        tiles_to_flip = []

        for dr, dc in [(-1, -1), (-1, 0), (-1, 1), (0, -1),
                       (0, 1), (1, -1), (1, 0), (1, 1)]:
            r, c = row + dr, col + dc
            captured = []
            while self.is_on_board(r, c) and self.board[r][c] == opponent:
                captured.append((r, c))
                r += dr
                c += dc
            if self.is_on_board(r, c) and self.board[r][c] == player:
                tiles_to_flip.extend(captured)

        if not tiles_to_flip:
            return False

        self.board[row][col] = player
        for r, c in tiles_to_flip:
            self.board[r][c] = player
        return True
    
    def send_board_state(self):
        for row in range(4):
            for col in range(4):
                symbol = self.board[row][col]
                if symbol in ("X", "O"):
                    message = f"{row},{col},{symbol}"
                    self.client.publish("game/board", message)


    def update_cell(self, row, col, player):
        if self.board[row][col] == "" and not self.game_over:
            if self.make_move(row, col, player):
                self.update_board_display()
                self.send_board_state()

                winner = self.check_winner()
                if winner:
                    self.end_game(winner)
                else:
                    self.current_player = "O" if self.current_player == "X" else "X"
                    self.update_status(f"Spelare {self.current_player}s tur")
                    self.highlight_possible_moves()

    def update_board_display(self):
        for row in range(4):
            for col in range(4):
                text = self.board[row][col]
                color = "blue" if text == "X" else "red" if text == "O" else "black"
                self.cells[row][col].config(text=text, fg=color, bg="white")

    def check_winner(self):
        x_count = sum(row.count("X") for row in self.board)
        o_count = sum(row.count("O") for row in self.board)
        if all(cell != "" for row in self.board for cell in row):
            if x_count > o_count:
                return "X"
            elif o_count > x_count:
                return "O"
            else:
                return "draw"
        return None

    def end_game(self, winner):
        self.game_over = True
        pygame.mixer.music.stop()
        pygame.mixer.Sound("win.mp3").play()

        if winner == "draw":
            self.update_status("Oavgjort!")
        else:
            self.update_status(f"Spelare {winner} vann!")

    def handle_message(self, topic, payload):
        if topic == "game/nfc/reader":
            try:
                reader, uid = payload.split(":", 1)

                if uid in self.X_TAGS:
                    player = "X"
                elif uid in self.O_TAGS:
                    player = "O"
                else:
                    print("Okänd tagg:", uid)
                    return

                if player != self.current_player:
                    print(f"Fel tur. Det är {self.current_player}s tur.")
                    return

                if reader in self.READER_POSITIONS:
                    row, col = self.READER_POSITIONS[reader]
                    self.update_cell(row, col, player)
                else:
                    print("Okänd läsare:", reader)
            except Exception as e:
                print("Fel i meddelande:", e)