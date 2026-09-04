#!/usr/bin/env python3
"""Clean live Pygame visualization for the Codexion simulation."""

import math
import os
import queue
import re
import shutil
import subprocess
import sys
import threading
from collections import deque

try:
    os.environ.setdefault("PYGAME_HIDE_SUPPORT_PROMPT", "1")
    import pygame
except ImportError:
    print("Pygame is required: python3 -m pip install pygame", file=sys.stderr)
    raise SystemExit(1)

WIDTH, HEIGHT = 1200, 800
BACKGROUND = (13, 18, 28)
PANEL = (23, 30, 44)
TABLE = (31, 42, 58)
TEXT = (235, 240, 248)
MUTED = (139, 151, 171)
FREE = (91, 103, 123)
ACCENT = (74, 222, 128)
STATE_COLORS = {
    "waiting": (112, 124, 145), "acquiring": (250, 184, 65),
    "compiling": (74, 222, 128), "debugging": (69, 190, 230),
    "refactoring": (183, 126, 245), "burned out": (248, 82, 82),
    "done": (91, 141, 239),
}
EVENT = re.compile(r"^(\d+)\s+(\d+)\s+(.+)$")


def usage():
    print("Usage: ./visualize.py number_of_coders time_to_burnout "
          "time_to_compile time_to_debug time_to_refactor "
          "number_of_compiles dongle_cooldown fifo|edf")


def update_state(coders, dongles, coder_id, message):
    coder = coders[coder_id]
    count = len(coders) - 1
    left, right = coder_id, coder_id % count + 1
    if message == "has taken a dongle":
        pair = (left, right)
        if coder["taken"] < 2:
            dongles[pair[coder["taken"]]] = coder_id
            coder["taken"] += 1
        coder["state"] = "acquiring"
    elif message == "is compiling":
        coder["state"] = "compiling"
        coder["compiles"] += 1
        dongles[left], dongles[right] = coder_id, coder_id
    elif message == "is debugging":
        coder["state"] = "debugging"
        coder["taken"] = 0
        for index, owner in enumerate(dongles):
            if owner == coder_id:
                dongles[index] = None
    elif message == "is refactoring":
        coder["state"] = "refactoring"
    elif message == "burned out":
        coder["state"] = "burned out"


def polar_positions(count, center, radius_x, radius_y):
    return [(int(center[0] + math.cos(-math.pi / 2 + 2 * math.pi * i / count)
                 * radius_x),
             int(center[1] + math.sin(-math.pi / 2 + 2 * math.pi * i / count)
                 * radius_y)) for i in range(count)]


def dongle_position(first, second, center):
    x, y = (first[0] + second[0]) / 2, (first[1] + second[1]) / 2
    return int(x + (center[0] - x) * .23), int(y + (center[1] - y) * .23)


def rounded_panel(surface, rect, color, radius=18, border=None):
    pygame.draw.rect(surface, color, rect, border_radius=radius)
    if border:
        pygame.draw.rect(surface, border, rect, width=2, border_radius=radius)


def centered(surface, font, value, color, center):
    image = font.render(value, True, color)
    surface.blit(image, image.get_rect(center=center))


def draw_dashboard(screen, fonts, coders, dongles, events, now, scheduler,
                   final=False):
    screen.fill(BACKGROUND)
    title, body, small, tiny = fonts
    screen.blit(title.render("CODEXION", True, TEXT), (42, 28))
    subtitle = "FINAL STATE" if final else "LIVE SIMULATION"
    screen.blit(small.render(subtitle, True, ACCENT), (44, 72))
    time_text = body.render(f"{now} ms   •   {scheduler.upper()}", True, MUTED)
    screen.blit(time_text, (WIDTH - time_text.get_width() - 42, 42))

    arena = pygame.Rect(30, 108, 820, 660)
    rounded_panel(screen, arena, PANEL)
    center = arena.centerx, arena.centery + 5
    pygame.draw.ellipse(screen, TABLE,
                        (center[0] - 245, center[1] - 185, 490, 370))
    pygame.draw.ellipse(screen, (47, 62, 82),
                        (center[0] - 245, center[1] - 185, 490, 370), 3)
    centered(screen, body, "SHARED WORK TABLE", MUTED, center)

    count = len(coders) - 1
    coder_pos = polar_positions(count, center, 315, 255)
    dongle_pos = [dongle_position(coder_pos[i], coder_pos[(i + 1) % count],
                                  center) for i in range(count)]

    # Coder i owns the two adjacent resources: Di and D(i + 1).
    for i in range(1, count + 1):
        for dongle_id in (i, i % count + 1):
            if dongles[dongle_id] == i:
                pygame.draw.line(screen, ACCENT, coder_pos[i - 1],
                                 dongle_pos[dongle_id - 1], 7)
                pygame.draw.line(screen, (190, 255, 214), coder_pos[i - 1],
                                 dongle_pos[dongle_id - 1], 2)

    for i, position in enumerate(dongle_pos, 1):
        owner = dongles[i]
        color = ACCENT if owner is not None else FREE
        pygame.draw.circle(screen, BACKGROUND, position, 22)
        pygame.draw.circle(screen, color, position, 17)
        pygame.draw.circle(screen, (205, 255, 222) if owner else MUTED,
                           position, 17, 2)
        centered(screen, tiny, f"D{i}", TEXT, position)

    for i, coder in enumerate(coders[1:], 1):
        rect = pygame.Rect(0, 0, 188, 100)
        rect.center = coder_pos[i - 1]
        color = STATE_COLORS.get(coder["state"], MUTED)
        rounded_panel(screen, rect, (27, 36, 51), 16, color)
        pygame.draw.circle(screen, color, (rect.left + 24, rect.top + 25), 7)
        screen.blit(body.render(f"CODER {i}", True, TEXT),
                    (rect.left + 39, rect.top + 12))
        centered(screen, small, coder["state"].upper(), color,
                 (rect.centerx, rect.centery + 8))
        centered(screen, tiny, f"{coder['compiles']} compiles", MUTED,
                 (rect.centerx, rect.bottom - 15))

    side = pygame.Rect(875, 108, 295, 660)
    rounded_panel(screen, side, PANEL)
    screen.blit(body.render("ACTIVITY", True, TEXT), (side.left + 22, side.top + 22))
    pygame.draw.line(screen, (48, 60, 79), (side.left + 22, side.top + 60),
                     (side.right - 22, side.top + 60), 1)
    y = side.top + 82
    for timestamp, label in events:
        screen.blit(tiny.render(f"{timestamp:>5} ms", True, MUTED),
                    (side.left + 20, y))
        screen.blit(tiny.render(label, True, TEXT), (side.left + 92, y))
        y += 31
    legend_y = side.bottom - 92
    pygame.draw.circle(screen, ACCENT, (side.left + 29, legend_y), 7)
    screen.blit(tiny.render("held / grabbed", True, MUTED),
                (side.left + 45, legend_y - 8))
    pygame.draw.circle(screen, FREE, (side.left + 29, legend_y + 34), 7)
    screen.blit(tiny.render("released / free", True, MUTED),
                (side.left + 45, legend_y + 26))
    pygame.display.flip()


def read_output(process, output_queue):
    for line in process.stdout:
        output_queue.put(line.rstrip())
    process.wait()
    output_queue.put(None)


def main():
    if len(sys.argv) != 9:
        usage()
        return 1
    try:
        count = int(sys.argv[1])
    except ValueError:
        usage()
        return 1
    if count < 2:
        print("The visualizer needs at least 2 coders.", file=sys.stderr)
        return 1
    binary = os.path.join(os.path.dirname(os.path.abspath(__file__)), "codexion")
    if not os.path.isfile(binary):
        print("codexion is not built. Run `make` first.", file=sys.stderr)
        return 1
    command = [binary] + sys.argv[1:]
    if shutil.which("stdbuf"):
        command = ["stdbuf", "-oL"] + command
    process = subprocess.Popen(command, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT, text=True, bufsize=1)
    output_queue = queue.Queue()
    threading.Thread(target=read_output, args=(process, output_queue),
                     daemon=True).start()

    pygame.init()
    pygame.display.set_caption("Codexion — Live Simulation")
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    fonts = (pygame.font.SysFont("arial", 31, bold=True),
             pygame.font.SysFont("arial", 20, bold=True),
             pygame.font.SysFont("arial", 15, bold=True),
             pygame.font.SysFont("arial", 13))
    clock = pygame.time.Clock()
    coders = [None] + [{"state": "waiting", "compiles": 0, "taken": 0}
                       for _ in range(count)]
    dongles = [None] * (count + 1)
    events = deque(maxlen=15)
    now, finished, running = 0, False, True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT or (event.type == pygame.KEYDOWN
                                              and event.key == pygame.K_ESCAPE):
                running = False
        while True:
            try:
                line = output_queue.get_nowait()
            except queue.Empty:
                break
            if line is None:
                finished = True
                if process.returncode == 0:
                    for coder in coders[1:]:
                        if coder["state"] != "burned out":
                            coder["state"] = "done"
                continue
            match = EVENT.match(line)
            if match:
                now, coder_id, message = int(match[1]), int(match[2]), match[3]
                update_state(coders, dongles, coder_id, message)
                events.append((now, f"C{coder_id}  {message}"))
            elif line:
                events.append((now, line))
        draw_dashboard(screen, fonts, coders, dongles, events, now,
                       sys.argv[8], finished)
        clock.tick(60)
    if process.poll() is None:
        process.terminate()
        process.wait(timeout=2)
    pygame.quit()
    return process.returncode or 0


if __name__ == "__main__":
    raise SystemExit(main())
