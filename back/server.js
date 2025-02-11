const { SerialPort } = require('serialport'); // Updated import
const { ReadlineParser } = require('@serialport/parser-readline'); // Updated import for Readline
const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
app.use(cors());

const PORT = 3000; // Porta do servidor web
const SERIAL_PORT = '/dev/ttyUSB0'; // Porta USB do receptor (ajuste conforme necessário)

// Inicia o servidor WebSocket
const wss = new WebSocket.Server({ port: 8081 }); // Change 8080 to another port

// Configura a leitura da porta serial
const serialPort = new SerialPort({
    path: SERIAL_PORT,
    baudRate: 115200
});
const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\n' }));

serialPort.on('open', () => console.log('Porta serial aberta'));
serialPort.on('error', (err) => console.error('Erro na porta serial:', err));

parser.on('data', (data) => {
    console.log('Dados recebidos:', data);

    // Enviar os dados recebidos para todos os clientes WebSocket conectados
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
});

// Servidor HTTP para a aplicação web
app.get('/', (req, res) => res.send('Servidor rodando!'));
app.listen(PORT, () => console.log(`Servidor rodando na porta ${PORT}`));
