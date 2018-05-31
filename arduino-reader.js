// Require the serialport node module
const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline')

// Require the socket client module
const socketclient = require('socket.io-client')
const localsocket = socketclient('http://localhost:3000');
const remotesocket = socketclient('https://hidden-garden-72560.herokuapp.com/');
localsocket.on('connect', () => {
	console.log("Connected to local server.");
});
remotesocket.on('connect', () => {
	console.log("Connected to Heroku server.");
});
localsocket.on('disconnect', () => {
	console.log("Disconnected from local server.");
});
remotesocket.on('disconnect', () => {
	console.log("Disconnected from Heroku server.");
});

// Open the port
const port = new SerialPort("COM3");

// Open errors will be emitted as an error event
port.on('error', function(err) {
  console.log('Error: ', err.message);
})

const parser=port.pipe(new Readline({ delimiter: '\r\n' }));
parser.on('data', function(data) {
	//console.log(data);
	localsocket.emit('arduino-data',data);
	remotesocket.emit('arduino-data',data);
})

localsocket.on('update-data', function(data) {
	var stresslevel='a';
	if (data.stress > 60) {
		stresslevel='b';
	}
	else if (data.stress > 30) {
		stresslevel='c';
	}
	else if (data.stress !== null) {
		stresslevel='d';
	}
	//console.log(stresslevel);
	port.write(stresslevel);
});
remotesocket.on('update-data', function(data) {
	var stresslevel='a';
	if (data.stress > 60) {
		stresslevel='b';
	}
	else if (data.stress > 30) {
		stresslevel='c';
	}
	else if (data.stress !== null) {
		stresslevel='d';
	}
	//console.log(stresslevel);
	port.write(stresslevel);
});
// parser.on('data', function(){
// 	socket.emit('arduino-data', );
// });

console.log('Running serial reader.')