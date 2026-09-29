# CS3113: Game Programming

This repository collects my NYU CS3113 game programming coursework in C++ with SDL2 and OpenGL. The assignments progress from drawing and transforming sprites to collision handling, enemy behavior, and games with multiple scenes.

Each assignment is a separate project with its own source code, assets, and Xcode configuration. The collection makes it possible to follow how the architecture develops as the gameplay becomes more complex.

## Start here

For a quick review, start with one of these projects:

- [Farm Ninja](hw/sl9820_hw06/SDLProject/main.cpp) brings together scene management, collectibles, timers, animation, and progression through three farm challenges. Its standalone repository is [farm_ninja](https://github.com/stevloc/farm_ninja).
- [The platformer](hw/sl9820_hw05/SDLProject/main.cpp) uses multiple levels, a menu, jumping, audio, and win or loss states.
- [Rise of the AI](hw/sl9820_hw04/SDLProject/Entity.cpp) shows patrol, jump, and player-following enemy behaviors.
- [The Pong clone](hw/sl9820_hw02/SDLProject/main.cpp) provides a smaller example of input handling, movement, and collision response.

## Project guide

| Directory | Project | What to look for |
| --- | --- | --- |
| [`hw/sl9820_hw01/`](hw/sl9820_hw01/) | Simple 2D Scene | The scene uses textured sprites with translation, rotation, and scaling. |
| [`hw/sl9820_hw02/`](hw/sl9820_hw02/) | Pong Clone | The game handles two paddles, ball collisions, and a toggle for automatic paddle movement. |
| [`hw/sl9820_hw03/`](hw/sl9820_hw03/) | Lunar Lander | The game introduces entity-based movement, a fixed timestep, and landing outcomes. |
| [`hw/sl9820_hw04/`](hw/sl9820_hw04/) | Rise of the AI | The project separates scene, map, and entity logic and adds several enemy behaviors. |
| [`hw/sl9820_hw05/`](hw/sl9820_hw05/) | Platformer | The game combines three levels with scene transitions, audio, and menu states. |
| [`hw/sl9820_hw06/`](hw/sl9820_hw06/) | Farm Ninja | The final farm game adds a home area, timed objectives, collectibles, and progression. |
| [`hw/Game/`](hw/Game/) | Additional platformer project | This folder preserves another platformer project alongside the numbered assignments. |
| [`exercises/player_input/`](exercises/player_input/) | User input exercise | This folder contains the course exercise instructions, starter code, and a supplied solution. |

## Repository structure

```text
cs3113/
├── exercises/
│   └── player_input/
│       ├── README.md
│       ├── SDLProject.xcodeproj/
│       └── SDLProject/
└── hw/
    ├── sl9820_hw01/
    ├── sl9820_hw02/
    ├── sl9820_hw03/
    ├── sl9820_hw04/
    ├── sl9820_hw05/
    ├── sl9820_hw06/
    └── Game/
```

Each homework folder contains an `.xcodeproj` project and an `SDLProject/` source directory. Earlier assignments keep most gameplay in `main.cpp`. Later assignments split responsibilities across `Entity`, `Map`, `Scene`, individual level classes, and rendering or audio helpers.

The projects include local copies of GLM, image-loading helpers, shaders, and their assets. They are intended to be opened separately rather than built as one application.

## Technical focus

- The rendering code combines SDL windows, OpenGL shaders, texture coordinates, and matrix transformations.
- The input code distinguishes between key events and held keys to control movement and game actions.
- The later games use fixed timestep updates to advance gameplay independently of individual rendered frames.
- Entity and map classes separate movement and collision behavior from level setup.
- Scene classes organize menus, gameplay levels, and end states as the projects grow.

## Build an assignment

The supplied projects target Xcode on macOS. They use SDL2 and OpenGL, and the later games also use SDL_mixer for audio. Check the selected project's framework list for its SDL2_image and SDL2_mixer dependencies.

For example, open the platformer with:

```bash
git clone https://github.com/stevloc/cs3113.git
cd cs3113
open hw/sl9820_hw05/sl9820_hw05.xcodeproj
```

1. Install the SDL2 frameworks referenced by the selected project.
2. Check its framework references and header search paths in Xcode. The original settings may require local path adjustments.
3. Set the scheme's Run working directory to that assignment's `SDLProject` folder so its relative asset and shader paths resolve.
4. Build and run that project on its own.

Controls vary by assignment. The Pong clone uses W and S for the first paddle, the up and down arrows for the second paddle, and T to toggle automatic movement. The platformer uses the left and right arrows, Space to jump, Enter to start, P to pause, and Q to quit. Farm Ninja uses all four arrow keys and a separate Space interaction in its third challenge.

## Coursework context

The homework source files identify my assignments. The `exercises/player_input/` folder also contains instructor-provided learning material and a supplied solution, with its original attribution retained. That exercise material should be read as course context alongside my homework projects.

This repository preserves the original project organization. It does not include a shared build script or packaged executables for the collection.
