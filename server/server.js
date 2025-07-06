const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const path = require("path");

const app = express();
const server = http.createServer(app);
const io = new Server(server);

const PORT = process.env.PORT || 3000;

app.use(express.static(path.join(__dirname, "../public")));

let rooms = {}; // key = roomId, value = [socketIds]

io.on("connection", socket => {
    console.log("New connection:", socket.id);

    socket.on("joinGame", roomId => {
        socket.join(roomId);
        if (!rooms[roomId]) rooms[roomId] = [];
        rooms[roomId].push(socket.id);
        console.log(`Socket ${socket.id} joined room ${roomId}`);

        // Send the current room info
        io.to(roomId).emit("roomUpdate", rooms[roomId]);

        if (rooms[roomId].length === 2) {
            io.to(roomId).emit("startGame");
        }
    });

    socket.on("playerAction", data => {
        // data = { roomId, action: { type, payload } }
        socket.to(data.roomId).emit("playerAction", data.action);
    });

    socket.on("disconnecting", () => {
        for (const roomId of socket.rooms) {
            if (rooms[roomId]) {
                rooms[roomId] = rooms[roomId].filter(id => id !== socket.id);
                io.to(roomId).emit("roomUpdate", rooms[roomId]);
            }
        }
    });
});

server.listen(PORT, () => {
    console.log("Server running on port", PORT);
});
