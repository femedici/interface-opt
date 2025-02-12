const express = require("express");
const http = require("http");
const fs = require("fs");
const SerialPort = require("serialport");
const { Server } = require("socket.io");

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: "*", // 🔥 Permite qualquer origem (ajuste isso em produção)
    methods: ["GET", "POST"],
  },
});

// Servindo o arquivo index.html
app.get("/", (req, res) => {
  res.sendFile(__dirname + "/index.html");
});

// Configuração da porta Serial
const parsers = SerialPort.parsers;
const parser = new parsers.Readline({ delimiter: "\r\n" });

const port = new SerialPort("/dev/ttyUSB0", {
  baudRate: 115200,
  dataBits: 8,
  parity: "none",
  stopBits: 1,
  flowControl: false,
});

port.pipe(parser);

// Conexão Socket.IO
io.on("connection", (socket) => {
  console.log("Cliente conectado via WebSocket");
  
  socket.on("disconnect", () => {
    console.log("Cliente desconectado");
  });
});

// Enviando dados recebidos da serial para os clientes conectados
parser.on("data", (data) => {
  console.log("Recebido da Serial:", data);
  io.emit("data", data);
});

// Iniciando o servidor
server.listen(3000, () => {
  console.log("Servidor rodando em http://localhost:3000");
});
