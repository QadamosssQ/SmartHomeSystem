const express = require('express');
const cors = require('cors');
express();
const port = 4040;

app.use(express.json());
app.use(cors({
  origin: '*'
}));

let data = {
  r: 0,
  g: 0,
  b: 0,
  state: 0
};

function validateData(req, res, next) {
  const { r, g, b, state } = req.body;

  if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255 || state < 0 || state > 1) {
    return res.status(400).json({ error: 'Wrong data! Colors: 0-255, State: 0-1' });
  }

  next();
}

app.get('/api', (req, res) => {
  res.json(data);
});

app.put('/api', validateData, (req, res) => {
  data = req.body;
  res.json({ message: 'Done!' });
});

app.listen(port, () => {
  console.log(`Serwer API działa na porcie ${port}.`);
});