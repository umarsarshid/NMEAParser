# ----------------------------------------------------
# STAGE 1: Build the React Frontend (Node.js)
# ----------------------------------------------------
FROM node:20-alpine AS node-builder
WORKDIR /frontend_build
COPY frontend/package*.json ./
COPY frontend/ ./
RUN npm ci && npm run build

# ----------------------------------------------------
# STAGE 2: Build the C++ Backend (GCC/CMake)
# ----------------------------------------------------
FROM alpine:latest AS cpp-builder
RUN apk add --no-cache build-base cmake sqlite-dev ncurses-dev linux-headers git
WORKDIR /cpp_build
COPY . .
RUN mkdir build && cd build && cmake .. && make

# ----------------------------------------------------
# STAGE 3: Final Runtime Image
# ----------------------------------------------------
FROM alpine:latest

# Runtime Libs
RUN apk add --no-cache libstdc++ sqlite-libs ncurses-libs

# 1. Setup Data Directory (This is where the DB will live)
WORKDIR /data

# 2. Copy Binary to global bin (Safe from volume mounts)
COPY --from=cpp-builder /cpp_build/build/nmea_app /usr/local/bin/nmea_app

# 3. Copy Frontend to root (Safe from volume mounts)
# The app will look for "../frontend/dist" relative to /data
# Parent of /data is /, so it looks in /frontend/dist. Perfect.
COPY --from=node-builder /frontend_build/dist /frontend/dist

EXPOSE 8080

# Run the binary (It's in the PATH now)
CMD ["nmea_app"]
