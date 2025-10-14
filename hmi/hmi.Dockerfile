# SOAFEE HMI Dashboard Dockerfile
FROM nginx:alpine

# Metadata
LABEL description="HMI Dashboard Container"
LABEL version="1.0"
LABEL maintainer="John"

# Install for health check
RUN apk update && apk add wget

# Remove default nginx website
RUN rm -rf /usr/share/nginx/html/*

# Copy HTML dashboard
COPY hmi/index.html /usr/share/nginx/html/

# Copy custom nginx configuration
COPY hmi/nginx.conf /etc/nginx/conf.d/default.conf

# Expose port
EXPOSE 80

# Only health check in docker compose
##Health check
#HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
#    CMD wget --quiet --tries=1 --spider http://localhost:80/health || exit 1

# Start nginx
CMD ["nginx", "-g", "daemon off;"]
