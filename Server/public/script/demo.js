var pg = require('pg');
const config = {
    user: 'caixa',
    database: 'db_caixa',
    password: '1234',
    port: 5432
};
const pool = new pg.Pool(config);

function startConnect() {
    // Generate a random client ID
    clientID = "clientID-" + parseInt(Math.random() * 100);

    document.getElementById("time").innerHTML = "<span>Aguardando resposta do servidor </span>";

    // Fetch the hostname/IP address and port number from the form
    host = 'tailor.cloudmqtt.com';
    port = 35684;
    topic = 'esp/nivel';

    // Initialize new Paho client connection
    client = new Paho.MQTT.Client(host, Number(port), clientID);

    // Set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    var options = {
        useSSL: true,
        userName: "wieodasv",
        password: "BC_Vlbj4xd9w",
        onSuccess: onConnect,
        onFailure: doFail
      }

    // Connect the client, if successful, call onConnect function
    client.connect(options);
}

// Called when the client connects
function onConnect() {
    // Subscribe to the requested topic
    client.subscribe(topic);
    publish()
}

// Called when the client loses its connection
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log('Error:' + responseObject.errorMessage);
    }
}

// Called when a message arrives
function onMessageArrived(message) {
    console.log("onMessageArrived: " + message.payloadString);
    showPage(message);
}

// Called when the disconnection button is pressed
function startDisconnect() {
    client.disconnect();
}

function doFail(e){
    console.log(e)
}

function publish(){
    console.log("publishing");
    topic_message = new Paho.MQTT.Message("1");
    topic_message.destinationName = "esp/nivelmessage";
    client.send(topic_message); 
}

function showPage(message) {    
    var messages = message.payloadString.split('-');    
    document.getElementById("nivel").innerHTML = "<span>Nível atual: " + messages[0]+"%</span></br>";
    var today = new Date();
    var date = today.getDate()+'/'+(today.getMonth()+1)+'/'+today.getFullYear();
    var time = today.getHours() + ":" + String(today.getMinutes()).padStart(2, '0') + ":" + String(today.getSeconds()).padStart(2, '0');
    document.getElementById("time").innerHTML = "<span>Última atualização:  "+ date+ " "+ time + "</span>";
    document.getElementById("wait").style.display = "none";
    let imagem = document.getElementById('imagem');
    console.log(messages[0]);
    if(messages[0] === '0.00'){
        imagem.src = "../images/water-tower.png";
    }
    if(messages[0] === '50.00'){
        imagem.src = "../images/water-tower50.png";
    }
    if(messages[0] === '100.00'){
        imagem.src = "../images/water-tower100.png";
    }
    

  }
  