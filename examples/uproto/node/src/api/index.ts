import express from 'express';
import bodyParser from 'body-parser';
import devicesApi from './devicesApi';

let router = express.Router();

router.use(bodyParser.urlencoded({ extended: true }));
router.use(bodyParser.json());

router.use('/devices', devicesApi);

router.all('*', (req, res) => {
    res.statusCode = 404;
    res.json({
        status: 'error',
        error: 'Resource does not exist',
    });
});

router.use((error, req, res, next) => {
    console.error((error.stack));

    res.status(500).json({
        status: 'error',
        error: 'Uncaught error',
        details: error.stack,
    });
});

export default router;
