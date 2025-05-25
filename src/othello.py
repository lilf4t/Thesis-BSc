import tkinter as tk
from tkinter import messagebox
import pygame
import math

class BaseOthello:

    MQTT_TOPIC_NFC = "game/nfc/reader"
    MQTT_TOPIC_BOARD = "game/board"
    MQTT_TOPIC_RESET = "game/reset"
    MQTT_TOPIC_POSSIBLE_MOVES = "game/possible_moves"

    X_TAGS = {"8916C201", "16B10F02", "3EA6B001", "2E3EB501", "296DB701"}
    O_TAGS = {"77700E02", "7A1CB201", "4A78C901", "91A30D02", "DB97B101"}

    READER_POSITIONS = {
        f"reader{i+1}": (i // 4, i % 4) for i in range(4 * 4)
    }

    def __init__(self, root, client):
        self.root = root
        self.client = client
        self.board = [["" for _ in range(self.BOARD_SIZE)] for _ in range(self.BOARD_SIZE)]
        self.current_player = "X"
        self.game_over = False
        self.cells = [[None for _ in range(self.BOARD_SIZE)] for _ in range(self.BOARD_SIZE)]

        self.status_label = tk.Label(root, text="", font=("Arial", 20))
        self.status_label.grid(row=self.BOARD_SIZE + 1, column=0, columnspan=self.BOARD_SIZE, pady=10)

        self.setup_board()
        self.reset_game()

    def setup_board(self):
        for row in range(self.BOARD_SIZE):
            for col in range(self.BOARD_SIZE):
                label = tk.Label(self.root, text="", font=("Arial", 30), width=4, height=2, relief="solid", bg="white")
                label.grid(row=row, column=col, padx=2, pady=2)
                self.cells[row][col] = label

        reset_button = tk.Button(self.root, text="Reset game", font=("Arial", 14), command=self.reset_game)
        reset_button.grid(row=self.BOARD_SIZE + 2, column=0, columnspan=self.BOARD_SIZE, pady=10)

    def reset_game(self):
        self.client.publish(self.MQTT_TOPIC_RESET, "reset")

        self.board = [["" for _ in range(self.BOARD_SIZE)] for _ in range(self.BOARD_SIZE)]
        self.current_player = "X"
        self.game_over = False

        mid = self.BOARD_SIZE // 2
        self.board[mid-1][mid-1] = "O"
        self.board[mid][mid] = "O"
        self.board[mid-1][mid] = "X"
        self.board[mid][mid-1] = "X"

        for row in range(self.BOARD_SIZE):
            for col in range(self.BOARD_SIZE):
                text = self.board[row][col]
                color = "blue" if text == "X" else "red" if text == "O" else "black"
                self.cells[row][col].config(text=text, fg=color, bg="white")

        try:
            pygame.mixer.music.load("intro.mp3")
            pygame.mixer.music.play()
        except:
            pass

        self.update_status(f"Spelare {self.current_player}s tur")
        self.highlight_possible_moves()

    def update_status(self, msg):
        self.status_label.config(text=msg)

    def highlight_possible_moves(self):
        moves = self.valid_moves(self.current_player)

        if not moves and not self.game_over:
            self.current_player = "O" if self.current_player == "X" else "X"
            self.update_status(f"Spelare {self.current_player}s tur")
            moves = self.valid_moves(self.current_player)

        for row in range(self.BOARD_SIZE):
            for col in range(self.BOARD_SIZE):
                if (row, col) in moves:
                    color = "#C8DAFF" if self.current_player == "X" else "#FFC8C8"
                    self.cells[row][col].config(bg=color)
                    message = f"{row},{col},{self.current_player}"
                    self.client.publish(self.MQTT_TOPIC_POSSIBLE_MOVES, message)
                else:
                    self.cells[row][col].config(bg="white")

    def valid_moves(self, player):
        moves = []
        opponent = "O" if player == "X" else "X"

        for row in range(self.BOARD_SIZE):
            for col in range(self.BOARD_SIZE):
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
        return 0 <= row < self.BOARD_SIZE and 0 <= col < self.BOARD_SIZE

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
        for row in range(self.BOARD_SIZE):
            for col in range(self.BOARD_SIZE):
                symbol = self.board[row][col]
                if symbol in ("X", "O"):
                    message = f"{row},{col},{symbol}"
                    self.client.publish(self.MQTT_TOPIC_BOARD, message)

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
        for row in range(self.BOARD_SIZE):
            for col in range(self.BOARD_SIZE):
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
        try:
            pygame.mixer.music.stop()
            pygame.mixer.Sound("win.mp3").play()
        except:
            pass

        if winner == "draw":
            self.update_status("Oavgjort!")
        else:
            self.update_status(f"Spelare {winner} vann!")

    def handle_message(self, topic, payload):
        if topic == self.MQTT_TOPIC_NFC:
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


class Othello(BaseOthello):
    pass  # Inga extra funktioner behövs för människa-mot-människa


class OthelloAI(BaseOthello):
    def update_cell(self, row, col, player):
        super().update_cell(row, col, player)
        if self.current_player == "O" and not self.game_over:
            self.root.after(500, self.ai_make_move)

    def ai_make_move(self):
        best_score = -math.inf
        best_move = None

        for move in self.valid_moves("O"):
            row, col = move
            temp_board = [r.copy() for r in self.board]
            self.make_move(row, col, "O")
            score = self.minimax(3, False, -math.inf, math.inf)
            self.board = [r.copy() for r in temp_board]

            if score > best_score:
                best_score = score
                best_move = move

        if best_move:
            super().update_cell(best_move[0], best_move[1], "O")

    def minimax(self, depth, maximizing, alpha, beta):
        winner = self.check_winner()
        if depth == 0 or winner:
            return self.evaluate_board()

        if maximizing:
            max_eval = -math.inf
            for move in self.valid_moves("O"):
                row, col = move
                temp_board = [r.copy() for r in self.board]
                self.make_move(row, col, "O")
                eval = self.minimax(depth - 1, False, alpha, beta)
                self.board = [r.copy() for r in temp_board]
                max_eval = max(max_eval, eval)
                alpha = max(alpha, eval)
                if beta <= alpha:
                    break
            return max_eval
        else:
            min_eval = math.inf
            for move in self.valid_moves("X"):
                row, col = move
                temp_board = [r.copy() for r in self.board]
                self.make_move(row, col, "X")
                eval = self.minimax(depth - 1, True, alpha, beta)
                self.board = [r.copy() for r in temp_board]
                min_eval = min(min_eval, eval)
                beta = min(beta, eval)
                if beta <= alpha:
                    break
            return min_eval

    def evaluate_board(self):
        x_count = sum(row.count("X") for row in self.board)
        o_count = sum(row.count("O") for row in self.board)
        return o_count - x_count