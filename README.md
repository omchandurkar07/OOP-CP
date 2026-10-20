# Smart Electricity Management System

The dashboard uses an Express/EJS web interface and a C++ OOP application as its backend. Express does not implement customer or billing rules itself: it calls the C++ executable, which uses the `Customer`, `Meter`, `Bill`/`ElectricityBill`, and `ElectricityApplication` classes and persists records to the existing `customers.dat` and `bills.dat` files.

## Run the web dashboard

1. Install Node.js 20 or newer and MinGW g++ with C++17 support.
2. Install the Node dependencies and start the app:

   ```powershell
   npm install
   npm start
   ```

   `npm start` compiles `main.cpp` into `sems_cpp.exe` before launching Express.
3. Open `http://127.0.0.1:3000`.

The web app loads the same local `.dat` files as the C++ terminal program, so changes made through either interface are visible to the other. Keep both programs running from the project directory to share these files. To store the C++ data files in another existing directory, set `SEMS_DATA_DIR` in `.env`. Generated executables, local data files, `.env`, and `node_modules` are ignored by Git.

## C++ terminal app

Run `main.exe` (or compile `main.cpp` with g++) to use the original menu-driven C++ program. It continues to use the same customer and bill files.

## MongoDB

MongoDB is not part of this architecture: persistence is handled by the existing C++ file-backed application. A MongoDB URI in `.env` will not be read or used. The web server binds to localhost by default and has no authentication; do not expose it to the internet.
