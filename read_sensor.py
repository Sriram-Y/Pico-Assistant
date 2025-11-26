import sys
import time
import board
import adafruit_dht

# ---- Check Argument ----
if len(sys.argv) != 2 or sys.argv[1] not in ["0", "1"]:
    print("Usage: python3 read_sensor.py <0=temp | 1=humidity>")
    sys.exit(1)

mode = sys.argv[1]   # "0" or "1"

# ---- Initialize sensor ----
dhtDevice = adafruit_dht.DHT22(board.D4)

# ---- Read Once ----
try:
    # Try reading a few times (DHT can be unstable)
    for _ in range(3):
        try:
            temperature_c = dhtDevice.temperature
            humidity = dhtDevice.humidity

            if temperature_c is not None and humidity is not None:
                break
        except RuntimeError:
            time.sleep(0.5)
            continue

    # ---- Output based on mode ----
    if mode == "0":       # Temperature
        if temperature_c is None:
            print("Error: Could not read temperature")
        else:
            print(f"{temperature_c:.1f}")

    elif mode == "1":     # Humidity
        if humidity is None:
            print("Error: Could not read humidity")
        else:
            print(f"{humidity:.1f}")

except Exception as e:
    print(f"Fatal error: {e}")

finally:
    dhtDevice.exit()
