# ----------------------------------------------------
# STAGE 1: Build the Application
# ----------------------------------------------------
FROM alpine:latest AS builder

# Install Build Tools and Development Libraries
# build-base: GCC/G++ and Make
# cmake: Build system
# sqlite-dev & ncurses-dev: Headers for compilation
# linux-headers: Needed for some C++ standard features
RUN apk add --no-cache build-base cmake sqlite-dev ncurses-dev linux-headers

WORKDIR /app
COPY . .

# Compile
RUN mkdir build && cd build && \
    cmake .. && \
    make

# ----------------------------------------------------
# STAGE 2: Run the Application (Minimalist)
# ----------------------------------------------------
FROM alpine:latest

# Install Runtime Libraries only (No headers/compilers needed)
# libstdc++: Standard C++ library
RUN apk add --no-cache libstdc++ sqlite-libs ncurses-libs

WORKDIR /app

# Copy the binary from the "builder" stage
COPY --from=builder /app/build/nmea_app .

# Command to run when container starts
CMD ["./nmea_app"]