// Configuration and setup

// Create a player socket
// Connects the browser to the server through socket.io
const socket = io();
// Ask the player for a room to join
const roomId = prompt("Enter room name to join:");
// Let the server know which room to put the player into
socket.emit("joinGame", roomId);


// Local socket variables //

// Unique Player ID
let playerId = null;
// Opponent Player ID
let otherId = null;
// Player role
let role = null;
// Player information dictionary
let players = {};
// Game start indicator
let gameStarted = false;
// Global handle to the active Phaser scene
let currentScene = null;  


// Define socket events

// Define playerId when connected and log
socket.on("connect", () => {
    playerId = socket.id;
    console.log("Connected as", playerId);
});

// Assign role to new player
socket.on("assignRole", (data) => {
    playerId = data.playerId;
    otherId = data.otherId;
    role = data.role;

    console.log("Assigned role:", role, "Player ID:", playerId);
});

// Log room members when roomUpdate occurs
socket.on("roomUpdate", (members) => {
    console.log("Room updated!");
    console.log("Room members:", members);
});

// Set gameStarted and start game on startGame signal
socket.on("startGame", () => {
    console.log("Starting game!");
    if (!gameStarted) {
        gameStarted = true;
        startPhaserGame();
    }
});

// Update player and scene data on playerAction
socket.on("playerAction", (action) => {
    if (action.type === "move") {
        const { id, x } = action.payload;
        if (id !== playerId && players[id]) {
            players[id].x = x;
        }
    }

    if (action.type === "dropBall") {
        const { x, y, velocityY } = action.payload;
        console.log("Other player dropped ball at ", x, y);
        if (gameStarted) {
            spawnBall(x, y, velocityY);
        }
    }
});


// Send action to server
function sendAction(type, payload) {
    console.log("Sending action ", type);
    socket.emit("playerAction", { roomId, action: { type, payload } });
}

// Begin game
function startPhaserGame() {
    // Phaser config
    const config = {
        type: Phaser.AUTO, // Autoselect WebGL or Canvas
        width: 700,        // Game width
        height: 600,       // Game height
        backgroundColor: "#222", // color
        // Define physics
        physics: {
            default: 'arcade',
            arcade: {
                gravity: { y: 0 },
                debug: false
            }
        },
        // Define scene functions
        scene: {
            create,  // Setup
            update   // Mainloop
        }
    };

    // Creates the game defined above
    new Phaser.Game(config);

    // Initalization function
    function create() {
        // 0) Connect the created scene to the global variable
        currentScene = this;

        // 1) Make the world bounds bounce on left/right but not top/bottom
        //    dynamic bodies (like balls) will bounce off these edges.
        this.physics.world.setBounds(100, 0, 350, 600);
        this.physics.world.setBoundsCollision(true, true, false, false);

        // 2) Generate a small green circle texture to use for pegs
        const g = this.make.graphics({ x: 0, y: 0, add: false });
        g.fillStyle(0x00ff00, 1);
        g.fillCircle(8, 8, 8);
        g.generateTexture("peg", 16, 16);
        g.destroy();

        // 3) Create a static group of pegs in the middle third of the screen
        this.pegs = this.physics.add.staticGroup();
        for (let row = 0; row < 5; row++) {
            for (let col = 0; col < 9; col++){
                const offset = row % 2 === 0 ? 17.5 : 0;
                const x = 125 + 35 * col + offset;
                const y = 200 + 40*row;
                this.pegs.create(x, y, "peg");
            }

        }

        // 4) Generate lines to define the left/right boundaries.
        const wallGraphics = this.add.graphics();
        wallGraphics.lineStyle(5, 0xffffff);
        // Draw visual left/right boundaries at x = 10 and x = 410
        wallGraphics.lineBetween(100, 0, 100, 600);  // Left wall
        wallGraphics.lineBetween(450, 0, 450, 600);  // Right wall


        // 5) Player creation

        // Assign player color and position based on roles
        // Blue = top; Red = bottom
        const isBlue = role === "blue";
        const myY = isBlue ? 100 : 500;
        const otherY = isBlue ? 500 : 100;
        const myColor = isBlue ? 0x3498db : 0xe74c3c;
        const otherColor = isBlue ? 0xe74c3c : 0x3498db;

        // Create player rectangles
        players[playerId] = this.add.rectangle(175, myY, 50, 50, myColor);
        players[otherId] = this.add.rectangle(175, otherY, 50, 50, otherColor);

        // Add role info and controls as UI text
        this.add.text(500, 50, `You are ${role.toUpperCase()}`, { font: "20px Arial", fill: "#fff" });

        this.add.text(540, 90, "Controls:", { font: "18px Arial", fill: "#ccc" });
        this.add.text(540, 120, "← →   Move", { font: "16px Arial", fill: "#aaa" });
        this.add.text(540, 150, "SPACE  Drop", { font: "16px Arial", fill: "#aaa" });

        // Controls (same for both players and unchangable for now)
        this.input.keyboard.on('keydown', (event) => {
            let dx = 0;
            if (event.key === 'ArrowLeft') dx = -5;
            if (event.key === 'ArrowRight') dx = 5;

            if (dx !== 0 && players[playerId]) {
                players[playerId].x = Phaser.Math.Clamp(players[playerId].x + dx, 125, 425);
                sendAction("move", { id: playerId, x: players[playerId].x });
            }
    
            if (event.key === ' ') {
                console.log("Pressed SPACEBAR");
                dropBall();

            }
        });

        
        // 5) Balls

        // Create "ball" texture
        const ballGfx = this.make.graphics({ x: 0, y: 0, add: false });
        ballGfx.fillStyle(0xffffff, 1);
        ballGfx.fillCircle(4, 4, 4);
        ballGfx.generateTexture("ball", 8, 8);
        ballGfx.destroy();
        
        // Create balls physics group
        this.balls = this.physics.add.group();

        // Define the function for dropping balls
        const dropBall = () => {
            const me = players[playerId];
            if (!me) return;

            // Direction based on role
            const velocityY = role === "blue" ? 200 : -200;

            // Create ball
            spawnBall(me.x, me.y, velocityY);

            // Sync to other player
            sendAction("dropBall", {
                x: me.x,
                y: me.y,
                velocityY
            });
        };

        // function spawnBall(x, y, velocityY) {
        //     const ball = this.balls.create(x, y, "ball");
        //     ball.setCircle(8);
        //     ball.setBounce(1);
        //     ball.setCollideWorldBounds(true);
        //     ball.setVelocityY(velocityY);

        //     this.physics.add.collider(ball, this.pegs, () => {
        //         // Add slight random X velocity change
        //         const tweak = Phaser.Math.Between(-50, 50);
        //         ball.setVelocityX(ball.body.velocity.x + tweak);
        //     });

        // }

    }


    function update() {
        // could animate here
    }

}


function spawnBall(x, y, velocityY) {
    const ball = currentScene.balls.create(x, y, "ball");
    ball.setCircle(4);
    ball.setBounce(1);
    ball.setCollideWorldBounds(true);
    ball.setVelocityY(velocityY);

    currentScene.physics.add.collider(ball, currentScene.pegs, () => {
        const tweak = Phaser.Math.Between(-50, 50);
        ball.setVelocityX(ball.body.velocity.x + tweak);
    });
}
