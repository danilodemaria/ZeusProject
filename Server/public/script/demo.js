// Called after form input is processed

function startConnect() {
    // Generate a random client ID
    clientID = "clientID-" + parseInt(Math.random() * 100);

    document.getElementById("time").innerHTML = "<span>Aguardando resposta do servidor </span>";

    // Fetch the hostname/IP address and port number from the form
    host = 'soldier.cloudmqtt.com';
    port = 33470;
    topic = 'esp/nivel';

    // Initialize new Paho client connection
    client = new Paho.MQTT.Client(host, Number(port), clientID);

    // Set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    var options = {
        useSSL: true,
        userName: "ivytoyoe",
        password: "EK32RC1rh_bK",
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
    var date = today.getDate()+'-'+(today.getMonth()+1)+'-'+today.getFullYear();
    var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds()
    document.getElementById("time").innerHTML = "<span>Última atualização:  "+ date+ " "+ time + "</span>";
    document.getElementById("wait").style.display = "none";
  }