const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const app = express();

app.use(express.static(path.join(__dirname,'public')));

app.use(bodyParser.json());

app.use(bodyParser.urlencoded( {extended:false} ));

app.get('/',(req,res) => {
    res.sendFile('mainWindow.html',{
        root: path.join(__dirname,'./public/html')
    })
});

app.get('/busca',(req,res) => {
    // retorna json pro app
});


app.listen(3000,'0.0.0.0');