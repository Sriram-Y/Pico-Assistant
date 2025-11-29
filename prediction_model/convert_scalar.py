import joblib
import pandas as pd

scaler = joblib.load('scaler_x.pkl')

print(f"const float SCALER_MEAN = {scaler.mean_[0]}f;")
print(f"const float SCALER_SCALE = {scaler.scale_[0]}f;")