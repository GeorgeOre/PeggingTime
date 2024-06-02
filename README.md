# PeggingTime

## Introduction
This document outlines the structure and content expected for the epic PeggingTime game. The game was made with a three week time limit and incorporates a custom C physics engine.

## Organization of Your Design Document

### Section 0: Summaryy
Here is a brief overview of the game design:
- **Working Title**: PeggingTime was chosen because this game is based off of the game peglin.
- **Team Members**: George Ore - Sole Developer.
- **Concept Statement**: You gotta hit your enemy with your balls.

### Section 1: Gameplay
This section details the fundamental aspects of how the game functions:

#### Game Progression
- The game has a single player roguelike adventure mode and a multiplayer versus mode
- The first mode has levels that increase in difficulty and culminate into a boss battle while allowing the player freedom of choice on what levels to fight. Riskier fights lead to more powerful awards which can come in handy when facing the final boss.
- The second mode is meant for single round games. Once a player is hit, the game is over.

#### Win and Loss Conditions
- For the first mode, a victory is when the boss is slain. A loss can be caused by running out of balls or taking too much damage.
- The second mode has a point system that keeps track of who is winning or losing over several rounds.

#### Levels and Points
- The rougelike adventure prompts the player to choose levels through a level tree. The tree indicates how difficult levels are expected to be and may contain paths with treasure to tempt the player. All paths culminate towards the final boss battle.
- Points are only activated in the muliplayer versus mode.

#### Additional Gameplay Details
- **Controls**: The keyboard will be used to move the character in vs mode and the spacebar will be used to shoot projectiles
- **Physics**: The balls will fall and use physics to bounce off pegs. Gravity will be in effect. Powerups will be attainable that will insert more interesting physics.
- **Game Flow**: The game starts very simple and gets more chaotic as the player collects powerups every level and prepares for the final boss. This final culmination is the climax and end.
- **Graphics**: I will use sprites.

### Section 2: Feature Set
Here I will break down the game into individual features:

#### Prioritization
Assign each feature a priority from one to four:
- **Priority 1**: Essential features (game cannot work without this)
- Physics engine
- Keyboard based player movement
- Render text
- Render sprite
- Create custom graphics or sprites
- Implement a scrolling environment
- Develop a networked/multiplayer game
- Render text
- Develop an AI for an enemy
- Implement speed-independent friction ??
- Enhance integration accuracy (e.g., using trapezoid sum)
- **Priority 2**: Important features
- Powerup selection process
- Roguelike random generation
- Menu state machine
- Exit
- **Priority 3**: Useful features
- Create a mouse handler
- In game shop
- Help page
- Setting page
- **Priority 4**: Optional but cool features
- Implement 2D parallax effects
- Add sound effects
- Add music
- Animations

#### Grading Criteria
- Implementing all priority 1 features guarantees a minimum grade of D.
- Implementing all priority 2 features guarantees a minimum grade of C.
- Implementing all priority 3 features guarantees a minimum grade of B.
- Implementing all priority 4 features guarantees a minimum grade of A-.
- Even distribution of work is required to uphold these guarantees.

#### Minimum Requirements
- At least four features per group member (one for each priority level).

### Section 3: Timeline
Assign each feature to a group member and specify the week they will implement it.
I will do everything ez

### Section 4: Disaster Recovery
Outline a plan for how each member will recover if they fall behind schedule. This section is crucial and should be taken seriously.
