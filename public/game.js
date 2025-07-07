// Configuration and setup

// Create a player socket
// Connects the browser to the server through socket.io
const socket = io();
// Ask the player for a room to join
const roomId = prompt("Enter room name to join:");
// Let the server know which room to put the player into
socket.emit("joinGame", roomId);


// Local socket variables //

// UniquePlayer ID
let playerId = null;
// Player information dictionary
let players = {};
// Game start indicator
let gameStarted = false;


// Define socket events

// Define playerId when connected and log
socket.on("connect", () => {
    playerId = socket.id;
    console.log("Connected as", playerId);
});

// Log room members when roomUpdate occurs
socket.on("roomUpdate", (members) => {
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

// Update player data on playerAction
socket.on("playerAction", (action) => {
    if (action.type === "move") {
        const { id, x } = action.payload;
        if (players[id]) {
            players[id].x = x;
        }
    }
});

// Send action to server
function sendAction(type, payload) {
    socket.emit("playerAction", { roomId, action: { type, payload } });
}

// Begin game
function startPhaserGame() {
    // Phaser config
    const config = {
        type: Phaser.AUTO, // Autoselect WebGL or Canvas
        width: 500,        // Game width
        height: 600,       // Game height
        backgroundColor: "#222", // color
        // Define ????
        physics: {
            default: 'arcade',
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
        // Create your player and remote player
        for (let i = 0; i < 2; i++) {
            let id = i === 0 ? socket.id : "other";
            players[id] = this.add.rectangle(100 + i * 600, 500, 50, 50, i === 0 ? 0x3498db : 0xe74c3c);
        }

        // Define keyboard input handlers
        this.input.keyboard.on('keydown', (event) => {
            // Calculate movement
            let dx = 0;
            if (event.key === 'a' || event.key === 'ArrowLeft') dx = -10;
            if (event.key === 'd' || event.key === 'ArrowRight') dx = 10;

            // Send movement action if movement was non-zero and update player position
            if (dx !== 0 && players[playerId]) {
                players[playerId].x += dx;
                sendAction("move", { id: playerId, x: players[playerId].x });
            }

            
        });
    }

    function update() {
        // could animate here
    }
}
