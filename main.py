import pygame
import random
import sys
import math
import time

pygame.init()

# Screen setup
WIDTH, HEIGHT = 600, 800
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("PEGGING TIME")

# Constants and colors
WHITE = (255, 255, 255)
BLUE = (50, 100, 255)
RED = (255, 50, 50)
GREEN = (0, 255, 0)
BLACK = (0, 0, 0)
GRAY = (200, 200, 200)

PLAYER_WIDTH, PLAYER_HEIGHT = 60, 20
BALL_RADIUS = 10
PEG_RADIUS = 8
BALL_SPEED = 5
FONT = pygame.font.SysFont(None, 40)
BIGFONT = pygame.font.SysFont(None, 60)

# Load background image
menu_bg = pygame.image.load("assets/mainmenu.png")
menu_bg = pygame.transform.scale(menu_bg, (WIDTH, HEIGHT))

# Load victory images
blue_victory_img = pygame.image.load("assets/blue_victory.png")
red_victory_img = pygame.image.load("assets/red_victory.png")

clock = pygame.time.Clock()


class Player:
    def __init__(self, x, y, controls, color):
        self.rect = pygame.Rect(x, y, PLAYER_WIDTH, PLAYER_HEIGHT)
        self.controls = controls
        self.color = color
        self.name = ""
        self.score = 0

    def move(self, keys):
        if keys[self.controls['left']] and self.rect.left > 0:
            self.rect.x -= 5
        if keys[self.controls['right']] and self.rect.right < WIDTH:
            self.rect.x += 5

    def draw(self, screen):
        pygame.draw.rect(screen, self.color, self.rect)


class Ball:
    def __init__(self, x, y, direction, color):
        self.x = x
        self.y = y
        self.dx = 0
        self.dy = BALL_SPEED * direction
        self.color = color

    def update(self, pegs):
        for peg in pegs:
            dist = math.hypot(peg[0] - self.x, peg[1] - self.y)
            if dist < PEG_RADIUS + BALL_RADIUS:
                angle = math.atan2(self.y - peg[1], self.x - peg[0])
                self.dx += math.cos(angle) * 3
                self.dy += math.sin(angle) * 3

        if self.x <= BALL_RADIUS or self.x >= WIDTH - BALL_RADIUS:
            self.dx *= -1
            self.x = max(BALL_RADIUS, min(self.x, WIDTH - BALL_RADIUS))

        self.x += self.dx
        self.y += self.dy

    def draw(self, screen):
        pygame.draw.circle(screen, self.color, (int(self.x), int(self.y)), BALL_RADIUS)

    def hits(self, player):
        return player.rect.collidepoint(self.x, self.y)


def generate_pegs():
    return [(random.randint(50, WIDTH - 50), random.randint(HEIGHT // 3, HEIGHT * 2 // 3)) for _ in range(30)]


def draw_text_center(text, size, y, color=BLACK):
    font = pygame.font.SysFont(None, size)
    surface = font.render(text, True, color)
    rect = surface.get_rect(center=(WIDTH // 2, y))
    screen.blit(surface, rect)


def main_menu():
    # Load and play menu music
    pygame.mixer.music.load("assets/music_mainmenu.mp3")
    pygame.mixer.music.play(-1)  # loop indefinitely

    while True:
        screen.blit(menu_bg, (0, 0))
        draw_text_center("PEGGING TIME", 60, 200, WHITE)
        mx, my = pygame.mouse.get_pos()

        play_button = pygame.Rect(WIDTH // 2 - 75, 400, 150, 60)
        pygame.draw.rect(screen, GRAY, play_button)
        draw_text_center("PLAY", 40, 430)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if play_button.collidepoint(mx, my):
                    return

        pygame.display.flip()
        clock.tick(60)


def get_player_name(index, control_text):
    name = ""
    input_active = True

    while input_active:
        screen.fill(WHITE)
        draw_text_center(f"Player {index + 1}, enter your name:", 40, 200)
        draw_text_center(control_text, 30, 250)

        box = pygame.Rect(WIDTH // 2 - 150, 300, 300, 50)
        pygame.draw.rect(screen, BLACK, box, 2)
        text_surface = FONT.render(name, True, BLACK)
        screen.blit(text_surface, (box.x + 10, box.y + 10))

        pygame.display.flip()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    return name or f"Player{index + 1}"
                elif event.key == pygame.K_BACKSPACE:
                    name = name[:-1]
                elif len(name) < 15:
                    name += event.unicode


def countdown(winner):
    # Determine which image to show
    if winner.color == BLUE:
        victory_img = blue_victory_img
    else:
        victory_img = red_victory_img

    # Resize image to fill screen if needed
    victory_img_scaled = pygame.transform.scale(victory_img, (WIDTH, HEIGHT))

    # Display for 3 seconds
    start_time = time.time()
    while time.time() - start_time < 3:
        screen.fill(WHITE)
        screen.blit(victory_img_scaled, (0, 0))

        # Victory text
        win_text = BIGFONT.render(f"{winner.name} WINS!", True, WHITE)
        win_rect = win_text.get_rect(center=(WIDTH // 2, 100))
        screen.blit(win_text, win_rect)

        pygame.display.flip()
        clock.tick(60)


def play_game():
    # Start game music
    pygame.mixer.music.load("assets/music_game_edited.mp3")
    pygame.mixer.music.play(-1)

    player1 = Player(WIDTH // 2 - 30, 20, {'left': pygame.K_a, 'right': pygame.K_d, 'drop': pygame.K_SPACE}, BLUE)
    player2 = Player(WIDTH // 2 - 30, HEIGHT - 40, {'left': pygame.K_LEFT, 'right': pygame.K_RIGHT, 'drop': pygame.K_RETURN}, RED)

    player1.name = get_player_name(0, "Use W A S D + SPACE to drop")
    player2.name = get_player_name(1, "Use Arrow Keys + ENTER to drop")

    pegs = generate_pegs()
    balls = []
    running = True

    while running:
        screen.fill(WHITE)
        keys = pygame.key.get_pressed()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                if event.key == player1.controls['drop']:
                    balls.append(Ball(player1.rect.centerx, player1.rect.bottom, 1, BLUE))
                if event.key == player2.controls['drop']:
                    balls.append(Ball(player2.rect.centerx, player2.rect.top, -1, RED))

        player1.move(keys)
        player2.move(keys)

        winner = None
        for ball in balls:
            ball.update(pegs)
            if ball.color == BLUE and ball.hits(player2):
                player1.score += 1
                winner = player1
            if ball.color == RED and ball.hits(player1):
                player2.score += 1
                winner = player2

        for peg in pegs:
            pygame.draw.circle(screen, GREEN, peg, PEG_RADIUS)

        player1.draw(screen)
        player2.draw(screen)
        for ball in balls:
            ball.draw(screen)

        # Display scores and names
        screen.blit(FONT.render(f"{player1.name}: {player1.score}", True, BLUE), (10, 10))
        screen.blit(FONT.render(f"{player2.name}: {player2.score}", True, RED), (10, HEIGHT - 40))

        pygame.display.flip()
        clock.tick(60)

        if winner:
            countdown(winner)
            balls = []
            pegs = generate_pegs()
            player1.rect.x = WIDTH // 2 - 30
            player2.rect.x = WIDTH // 2 - 30


if __name__ == "__main__":
    main_menu()
    play_game()
