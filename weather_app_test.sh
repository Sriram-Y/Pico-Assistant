#!/bin/bash

# 1. Test current temperature
echo "--- Query 1: Current Temperature ---"
sudo ./WeatherApp "what is the temperature now"
echo "" # Empty line for spacing

# 2. Test current humidity
echo "--- Query 2: Current Humidity ---"
sudo ./WeatherApp "what is the humidity now"
echo ""

# 3. Test future temperature
echo "--- Query 3: Future Temperature ---"
sudo ./WeatherApp "what will the temperature be tomorrow"
echo ""

# 4. Test future humidity
echo "--- Query 4: Future Humidity ---"
sudo ./WeatherApp "what will the humidity be tomorrow"
echo ""

echo "All tests complete."