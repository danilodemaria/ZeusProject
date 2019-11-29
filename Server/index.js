const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const app = express();
var pg = require('pg');
const cors = require('cors');

const config = {
    user: 'caixa',
    database: 'db_caixa',
    password: '1234',
    port: 5432
};
const pool = new pg.Pool(config);

app.use(express.static(path.join(__dirname, 'public')));

app.use(bodyParser.json());

app.use(bodyParser.urlencoded({ extended: false }));

app.use(cors());

app.get('/', (req, res) => {
    res.sendFile('mainWindow.html', {
        root: path.join(__dirname, './public/html')
    })
});



app.get('/relatorio', (req, res) => {
    pool.connect(function (err, client, done) {
        if (err) {
            console.log("Can not connect to the DB" + err);
        }
        client.query('select to_char(data, \'YYYY-MM-DD hh:mm:ss\') as "x",a.nivel as "y" from nivel a order by a.data', function (err, result) {
            done();
            if (err) {
                console.log(err);
                res.status(400).send(err);
            }
            res.status(200).send(result.rows);
        })
    })
});

app.get('/relatorio1', (req, res) => {
    pool.connect(function (err, client, done) {
        if (err) {
            console.log("Can not connect to the DB" + err);
        }
        client.query('select a.data as "x", a.nivel as "y" from nivel a order by a.data', function (err, result) {
            done();
            if (err) {
                console.log(err);
                res.status(400).send(err);
            }
            res.status(200).send(result.rows);
        })
    })
});

app.get('/busca', (req, res) => {
    pool.connect(function (err, client, done) {
        if (err) {
            console.log("Can not connect to the DB" + err);
        }
        client.query('SELECT nivel FROM nivel ORDER BY id DESC LIMIT 1', function (err, result) {
            done();
            if (err) {
                console.log(err);
                res.status(400).send(err);
            }
            res.status(200).send(result.rows);
        })
    })
});

app.get('/grafico', (req, res) => {
    
    

    res.sendFile('report.html', {
        root: path.join(__dirname, './public/html')
    })
});

app.get('/insere', (req, res) => {
    pool.connect(function (err, client, done) {
        var d = new Date();
        var datestring = d.getFullYear() + "-" + (d.getMonth() + 1) + "-" + d.getDate() + " " +
            d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds();
        let aux = req.query.nivel;
        let query = "insert into nivel (nivel,data) values (" + aux + ",'" + datestring + "');";        
        if (err) {
            console.log("Erro ao conectar ao banco de dados." + err);
        }
        client.query(query, function (err, result) {
            done();
            if (err) {
                console.log(err);                
            }
            res.status(200).send("Inserido");
        })
    })   
});

app.listen(3000, '0.0.0.0');