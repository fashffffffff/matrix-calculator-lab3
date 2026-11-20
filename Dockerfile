FROM ubuntu:24.04 AS builder
RUN apt-get update && apt-get install -y --no-install-recommends cmake g++ make && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY CMakeLists.txt ./
COPY include/ include/
COPY src/ src/
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF && cmake --build build --parallel

FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends libstdc++6 && rm -rf /var/lib/apt/lists/*
COPY --from=builder /app/build/matrix_calculator /usr/local/bin/matrix_calculator
USER 65532:65532
CMD ["/usr/local/bin/matrix_calculator"]
