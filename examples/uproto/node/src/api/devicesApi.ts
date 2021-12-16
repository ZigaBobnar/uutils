import express from 'express';
import { DeviceModel } from '../models/device';

let router = express.Router();

router.post('/', async (req, res, next) => {
    const name = req.body.name;

    try {
        let item = await DeviceModel.create({
            name,
        });
    
        let result = await item.save();

        res.json({ result });
    } catch (err) {
        next(err);
    }
});

router.get('/', async (req, res) => {
    let items = await DeviceModel.find({});

    res.json({ items });
});

router.get('/:name', async (req, res) => {
    const name = req.params.name;

    let item = await DeviceModel.findOne({ name });

    res.json({ item });
});

router.put('/:name', async (req, res, next) => {
    const name = req.params.name;
    const newName = req.body.name;

    try {
        let item = await DeviceModel.findOne({ name });

        item.name = newName;
        await item.save();

        res.json({ item });
    } catch (err) {
        next(err);
    }
});

router.delete('/:name', async (req, res, next) => {
    const name = req.params.name;

    try {
        let result = await DeviceModel.deleteOne({ name });
    
        res.json({ status: result.deletedCount == 1 ? 'ok' : 'error' });
    } catch (err) {
        next(err);
    }
});

export default router;
