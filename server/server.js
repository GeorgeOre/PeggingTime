// Includes
const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const path = require("path");

// Create a new express application called app
const app = express();
// Format app as an http server
const server = http.createServer(app);
// Hook up socket io to the server
const io = new Server(server);

// Set the port according to the environment (default to local port 3000)
const PORT = process.env.PORT || 3000;

// Serve static frontend files (share all files in the public directory)
app.use(express.static(path.join(__dirname, "../public")));

// Create a dictionary to store game state memory
// This stores the IDs of players (aka sockets) in rooms
let rooms = {}; // key = roomId, value = [socketIds]

// Define socket (aka player) handling on connection
io.on("connection", socket => {
    // Log the new socket
    console.log("New connection:", socket.id);

    // Listen for the socket to emit a joinGame and roomID signal
    socket.on("joinGame", roomId => {
        
        // Use socket.io to put the player in requested room
        socket.join(roomId);
        // Create the room if it doesn't exist yet
        if (!rooms[roomId]) rooms[roomId] = [];
    
        // Prevent duplicate joins
        if (!rooms[roomId].includes(socket.id)) {
            rooms[roomId].push(socket.id);
        }

        // Debugging logs
        console.log(`Socket ${socket.id} joined room ${roomId}`);
        console.log("Current room state:", rooms[roomId]);

        // Inform all clients in the room
        io.to(roomId).emit("roomUpdate", rooms[roomId]);

        // Start game only if exactly 2 players are in
        if (rooms[roomId].length === 2) {
            console.log(`Starting game in room ${roomId}`);
            io.to(roomId).emit("startGame");
        }
    });

    // Listen for the socket to emit a playerAction signal
    socket.on("playerAction", data => {
        // Send player action data to all other sockets in the room
        // data = { roomId, action: { type, payload } }
        socket.to(data.roomId).emit("playerAction", data.action);
    });

    // Listen for the socket to emit a disconnect signal
    socket.on("disconnecting", () => {
        for (const roomId of socket.rooms) {
            if (rooms[roomId]) {
                // Remove player from the room
                rooms[roomId] = rooms[roomId].filter(id => id !== socket.id);
                // Inform other players in the room
                io.to(roomId).emit("roomUpdate", rooms[roomId]);
            }
        }
    });
});

// Start the server
server.listen(PORT, () => {
    console.log("Server running on port", PORT);
});


