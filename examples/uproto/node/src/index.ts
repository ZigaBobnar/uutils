import dotenv from 'dotenv';
import express from 'express';
import path from 'path';
import { engine } from 'express-handlebars';
import mongoose from 'mongoose';
import api from './api';
import { DeviceModel } from './models/device';

dotenv.config();
const port = process.env.SERVER_PORT;

const app = express();

mongoose.connect(process.env.MONGODB_URI, {}, () => {
    console.info('Connected to the database.');
});
if (process.env.NODE_ENV == 'development') {
    mongoose.set('debug', true);
}

app.set('view engine', 'hbs');
app.set('views', path.join(__dirname, 'views'));
app.engine('hbs', engine({
    layoutsDir: path.join(__dirname, 'views', 'layouts'),
    partialsDir: path.join(__dirname, 'views', 'partials'),
    defaultLayout: 'main',
    extname: '.hbs',
}));

app.use(express.static('public'));

app.get('/', async (req, res) => {
    let items = await DeviceModel.find({}).lean();

    res.render('index', {devices:items});
});

app.use('/api', api);

app.listen(port, () => {
    console.info(`Instance server running at http://localhost:${port}/`);
});
