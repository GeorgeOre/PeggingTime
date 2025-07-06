const socket = io();
const roomId = prompt("Enter room name to join:");
socket.emit("joinGame", roomId);

socket.on("startGame", () => {
    console.log("Game starting!");
    // Init Phaser game scene here
});

socket.on("playerAction", (action) => {
    // Handle other player’s actions here
});

function sendAction(type, payload) {
    socket.emit("playerAction", { roomId, action: { type, payload } });
}
