require('dotenv').config();

const path = require('node:path');
const { execFile } = require('node:child_process');
const { promisify } = require('node:util');
const express = require('express');

const execFileAsync = promisify(execFile);
const app = express();
const port = Number(process.env.PORT || 3000);
const host = process.env.HOST || '127.0.0.1';
const dataDirectory = path.resolve(process.env.SEMS_DATA_DIR || __dirname);
const cppExecutable = process.env.SEMS_CPP_EXECUTABLE ||
  path.join(__dirname, process.platform === 'win32' ? 'sems_cpp.exe' : 'sems_cpp');

class ApiError extends Error {
  constructor(status, message) {
    super(message);
    this.status = status;
  }
}

async function runCpp(command, ...args) {
  try {
    const { stdout } = await execFileAsync(
      cppExecutable,
      ['--api', command, ...args.map(String)],
      { cwd: dataDirectory, windowsHide: true, maxBuffer: 10 * 1024 * 1024 }
    );
    return JSON.parse(stdout);
  } catch (error) {
    if (typeof error.code === 'number') {
      const status = { 2: 400, 3: 404, 4: 409, 5: 500 }[error.code] || 500;
      const details = (error.stderr || '').split(/\r?\n/)
        .filter((line) => line && !line.startsWith('[Loaded '))
        .join(' ');
      throw new ApiError(status, details || 'The C++ application could not complete the request.');
    }
    if (error.code === 'ENOENT') {
      throw new ApiError(503, 'C++ backend is not built. Run npm start to build it.');
    }
    throw error;
  }
}

app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));
app.use(express.json({ limit: '1mb' }));
app.use(express.static(path.join(__dirname, 'public')));

app.get('/', (req, res) => {
  res.render('index');
});

app.get('/api/health', async (req, res) => {
  await runCpp('data');
  res.json({ status: 'ok', backend: 'C++ OOP' });
});

app.get('/api/data', async (req, res) => {
  res.json(await runCpp('data'));
});

app.post('/api/customers', async (req, res) => {
  const { id, name, address, meterNumber, type } = req.body;
  const customer = await runCpp(
    'add-customer',
    id,
    name,
    address,
    meterNumber,
    type
  );
  res.status(201).json(customer);
});

app.delete('/api/customers/:id', async (req, res) => {
  await runCpp('delete-customer', req.params.id);
  res.status(204).end();
});

app.post('/api/bills', async (req, res) => {
  const { customerId, previousReading, currentReading, monthYear } = req.body;
  const bill = await runCpp(
    'generate-bill',
    customerId,
    previousReading,
    currentReading,
    monthYear
  );
  res.status(201).json(bill);
});

app.patch('/api/bills/payment', async (req, res) => {
  const { meterNumber, monthYear } = req.body;
  res.json(await runCpp('pay', meterNumber, monthYear));
});

app.delete('/api/data', async (req, res) => {
  await runCpp('reset');
  res.status(204).end();
});

app.use((error, req, res, next) => {
  if (res.headersSent) return next(error);
  if (error.type === 'entity.parse.failed') {
    return res.status(400).json({ error: 'Request body must contain valid JSON.' });
  }
  if (error instanceof ApiError) {
    return res.status(error.status).json({ error: error.message });
  }
  console.error('Request failed:', error);
  return res.status(500).json({ error: 'An unexpected server error occurred.' });
});

app.listen(port, host, () => {
  console.log(`Smart Electricity Management System listening at http://${host}:${port}`);
  console.log(`C++ OOP backend: ${cppExecutable}`);
});
