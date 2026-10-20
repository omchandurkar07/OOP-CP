# Smart Electricity Management System

The web dashboard stores customer and bill records in MongoDB Atlas. Its Express API calls the C++ OOP domain service for customer validation, meter readings, bill calculations, and payment-state changes before persisting those results to Atlas.

## Configure MongoDB Atlas

1. Create an Atlas database user and allow your development machine's IP in the Atlas Network Access settings.
2. Copy `.env.example` to `.env`.
3. Put your Atlas connection string in `.env` as `MONGODB_URI`. Replace its database name with `sems` and URL-encode any special characters in the database password. Never commit or share `.env`.
4. Install dependencies and run the app:

   ```powershell
   npm install
   npm start
   ```

   `npm start` compiles `main.cpp` into `sems_cpp.exe` before starting the web server.
5. Open `http://127.0.0.1:3000`. `/api/health` reports whether the database connection is available.

MongoDB driver connection-pool settings are left at their defaults; the app currently has no measured concurrency or workload to justify custom values. If the application is deployed, monitor Atlas **Metrics → Connections** and adjust only if observed traffic or pool wait times warrant it.

The web dashboard starts with empty Atlas collections. Existing `customers.dat` and `bills.dat` files are not uploaded automatically. The original menu-driven C++ terminal program continues using those local files and is separate from the Atlas-backed web dashboard.

## Architecture

- `main.cpp`: C++ OOP domain service, including `Customer`, `Meter`, abstract `Bill`, and polymorphic `ElectricityBill`.
- `server.js`: Express API, Atlas connection, MongoDB persistence, and calls into the compiled C++ service.
- `views/index.ejs`: EJS page wrapper for the existing dashboard.

The server binds to `127.0.0.1` by default and has no authentication. Keep it private; do not expose it to the public internet.
