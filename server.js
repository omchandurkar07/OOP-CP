require('dotenv').config();

const path = require('node:path');
const { execFile } = require('node:child_process');
const { promisify } = require('node:util');
const express = require('express');
const mongoose = require('mongoose');

const execFileAsync = promisify(execFile);
const app = express();
const port = Number(process.env.PORT || 3000);
const host = process.env.HOST || '127.0.0.1';
const cppExecutable = process.env.SEMS_CPP_EXECUTABLE ||
  path.join(__dirname, process.platform === 'win32' ? 'sems_cpp.exe' : 'sems_cpp');

const customerSchema = new mongoose.Schema({
  id: { type: Number, required: true, unique: true, min: 1 },
  name: { type: String, required: true, trim: true, minlength: 2, maxlength: 120 },
  address: { type: String, required: true, trim: true, minlength: 3, maxlength: 250 },
  meterNumber: { type: String, required: true, trim: true, maxlength: 80 },
  meterNumberKey: { type: String, required: true, unique: true, select: false },
  type: { type: String, required: true, enum: ['Residential', 'Commercial'] }
}, { timestamps: true, versionKey: false });

const billSchema = new mongoose.Schema({
  customerId: { type: Number, required: true, min: 1 },
  customerName: { type: String, required: true },
  meterNumber: { type: String, required: true },
  meterNumberKey: { type: String, required: true },
  customerType: { type: String, required: true, enum: ['Residential', 'Commercial'] },
  monthYear: { type: String, required: true, trim: true, maxlength: 80 },
  monthYearKey: { type: String, required: true },
  previousReading: { type: Number, required: true, min: 0 },
  currentReading: { type: Number, required: true, min: 0 },
  units: { type: Number, required: true, min: 0 },
  billAmount: { type: Number, required: true, min: 0 },
  paymentStatus: { type: String, required: true, enum: ['PAID', 'UNPAID'], default: 'UNPAID' }
}, { timestamps: true, versionKey: false });

billSchema.index({ meterNumberKey: 1, monthYearKey: 1 }, { unique: true });

const Customer = mongoose.model('Customer', customerSchema);
const Bill = mongoose.model('Bill', billSchema);

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
      { cwd: __dirname, windowsHide: true, maxBuffer: 10 * 1024 * 1024 }
    );
    return JSON.parse(stdout);
  } catch (error) {
    if (typeof error.code === 'number') {
      const status = { 2: 400, 3: 404, 4: 409, 5: 500 }[error.code] || 500;
      throw new ApiError(status, error.stderr || 'The C++ OOP service could not complete the request.');
    }
    if (error.code === 'ENOENT') {
      throw new ApiError(503, 'C++ backend is not built. Run npm start to build it.');
    }
    throw error;
  }
}

function serializeCustomer(customer) {
  return {
    id: customer.id,
    name: customer.name,
    address: customer.address,
    meterNumber: customer.meterNumber,
    type: customer.type
  };
}

function serializeBill(bill) {
  return {
    customerId: bill.customerId,
    customerName: bill.customerName,
    meterNumber: bill.meterNumber,
    customerType: bill.customerType,
    monthYear: bill.monthYear,
    previousReading: bill.previousReading,
    currentReading: bill.currentReading,
    units: bill.units,
    billAmount: bill.billAmount,
    paymentStatus: bill.paymentStatus
  };
}

app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));
app.use(express.json({ limit: '1mb' }));
app.use(express.static(path.join(__dirname, 'public')));

app.get('/', (req, res) => {
  res.render('index');
});

app.get('/api/health', (req, res) => {
  const connected = mongoose.connection.readyState === 1;
  res.status(connected ? 200 : 503).json({
    status: connected ? 'ok' : 'database unavailable',
    backend: 'C++ OOP + MongoDB Atlas'
  });
});

app.get('/api/data', async (req, res) => {
  const [customers, bills] = await Promise.all([
    Customer.find().sort({ createdAt: 1 }).lean(),
    Bill.find().sort({ createdAt: 1 }).lean()
  ]);
  res.json({
    customers: customers.map(serializeCustomer),
    bills: bills.map(serializeBill)
  });
});

app.post('/api/customers', async (req, res) => {
  const validated = await runCpp(
    'validate-customer',
    req.body.id,
    req.body.name,
    req.body.address,
    req.body.meterNumber,
    req.body.type
  );
  const customer = await Customer.create({
    ...validated,
    meterNumberKey: validated.meterNumber.toLowerCase()
  });
  res.status(201).json(serializeCustomer(customer));
});

app.delete('/api/customers/:id', async (req, res) => {
  const id = Number(req.params.id);
  if (!Number.isInteger(id) || id <= 0) throw new ApiError(400, 'Invalid customer ID.');

  const customer = await Customer.findOne({ id }).lean();
  if (!customer) throw new ApiError(404, 'Customer not found.');
  await Bill.deleteMany({ customerId: id });
  await Customer.deleteOne({ id });
  res.status(204).end();
});

app.post('/api/bills', async (req, res) => {
  const customerId = Number(req.body.customerId);
  if (!Number.isInteger(customerId) || customerId <= 0) throw new ApiError(400, 'Invalid customer ID.');
  const customer = await Customer.findOne({ id: customerId }).lean();
  if (!customer) throw new ApiError(404, 'Customer not found.');

  const bill = await runCpp(
    'generate-bill',
    customer.id,
    customer.name,
    customer.meterNumber,
    customer.type,
    req.body.monthYear,
    req.body.previousReading,
    req.body.currentReading
  );
  const savedBill = await Bill.create({
    ...bill,
    meterNumberKey: bill.meterNumber.toLowerCase(),
    monthYearKey: bill.monthYear.toLowerCase()
  });
  res.status(201).json(serializeBill(savedBill));
});

app.patch('/api/bills/payment', async (req, res) => {
  const meterNumber = typeof req.body.meterNumber === 'string' ? req.body.meterNumber.trim() : '';
  const monthYear = typeof req.body.monthYear === 'string' ? req.body.monthYear.trim() : '';
  if (!meterNumber || !monthYear) throw new ApiError(400, 'Meter number and billing month are required.');

  const filter = {
    meterNumberKey: meterNumber.toLowerCase(),
    monthYearKey: monthYear.toLowerCase()
  };
  const bill = await Bill.findOne(filter).lean();
  if (!bill) throw new ApiError(404, 'Bill not found.');

  const paidBill = await runCpp(
    'pay-bill',
    bill.customerId,
    bill.customerName,
    bill.meterNumber,
    bill.customerType,
    bill.monthYear,
    bill.previousReading,
    bill.currentReading
  );
  await Bill.updateOne(filter, { $set: { paymentStatus: paidBill.paymentStatus } });
  res.json(serializeBill({ ...bill, paymentStatus: paidBill.paymentStatus }));
});

app.delete('/api/data', async (req, res) => {
  await Promise.all([Customer.deleteMany({}), Bill.deleteMany({})]);
  res.status(204).end();
});

app.use((error, req, res, next) => {
  if (res.headersSent) return next(error);
  if (error.type === 'entity.parse.failed') {
    return res.status(400).json({ error: 'Request body must contain valid JSON.' });
  }
  if (error.code === 11000) {
    return res.status(409).json({ error: 'A customer ID, meter number, or bill for that meter and month already exists.' });
  }
  if (error.name === 'ValidationError' || error.name === 'CastError') {
    return res.status(400).json({ error: 'The submitted data is invalid.' });
  }
  if (error instanceof ApiError) return res.status(error.status).json({ error: error.message });
  console.error('Request failed:', error);
  return res.status(500).json({ error: 'An unexpected server error occurred.' });
});

async function start() {
  if (!process.env.MONGODB_URI) {
    throw new Error('MONGODB_URI is not set. Copy .env.example to .env and add your MongoDB Atlas connection string.');
  }

  await mongoose.connect(process.env.MONGODB_URI);
  await Promise.all([Customer.createIndexes(), Bill.createIndexes()]);
  app.listen(port, host, () => {
    console.log(`Smart Electricity Management System listening at http://${host}:${port}`);
    console.log('Backend: C++ OOP + MongoDB Atlas');
  });
}

start().catch((error) => {
  console.error('Unable to start the application:', error.message);
  process.exitCode = 1;
});
