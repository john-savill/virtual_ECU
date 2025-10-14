# SOAFEE-Compliant Virtual ECU Dockerfile
FROM ubuntu:22.04

# Metadata
LABEL description="SOAFEE Virtual ECU Container"
LABEL version="1.0"
LABEL maintainer="John"

# Install dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    net-tools \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /app

# Copy source code
COPY vecu/main.c vecu/main.h /app/

# Compile the Virtual ECU
RUN gcc -o soafee_vecu main.c -Wall -Wextra

# Expose port for communication
EXPOSE 8081

# Health check
#HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
#    CMD netstat -an | grep 8081 > /dev/null || exit 1

# Run the Virtual ECU
CMD ["./soafee_vecu"]
