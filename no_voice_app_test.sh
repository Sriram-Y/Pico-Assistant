#!/bin/bash

# 1. Test current temperature
echo "--- Query 1: Current Temperature ---"
sudo ./app "what is the temperature now"
echo "" # Empty line for spacing

# 2. Test current humidity
echo "--- Query 2: Current Humidity ---"
sudo ./app "what is the humidity now"
echo ""

# 3. Test future temperature
echo "--- Query 3: Future Temperature ---"
sudo ./app "what will the temperature be tomorrow"
echo ""

# 4. Test future humidity
echo "--- Query 4: Future Humidity ---"
sudo ./app "what will the humidity be tomorrow"
echo ""

echo "All tests complete."